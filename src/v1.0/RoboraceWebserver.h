#pragma once


#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "v1.0/value/TimingValue.h"
#include "v1.0/value/ParamHolder.h"
#include "Roborace.h"


class RoboraceWebserver : public Roborace {

private:


    AsyncWebServer server = AsyncWebServer(80);
    AsyncWebSocket ws = AsyncWebSocket("/ws");

    DynamicJsonDocument doc = DynamicJsonDocument(256);

    IntervalValue *wsInterval = new IntervalValue(new Param(500, "ws-interval", "main"));
    IntervalValue *wsParamInterval = new IntervalValue(new Param(20000, "ws-param-interval", "main"));

    ParamHolder *paramHolder = new ParamHolder();

    bool isNeedUpdateParams = false;


public:

    RoboraceWebserver() {
//        initServer();
    }

    void loop() final {
        loopWs();
        Roborace::loop();
    }


    void initServer() {

        server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
            const DynamicJsonDocument &document = createMessage();
            String message;
            serializeJson(document, message);
            request->send(200, "application/json", message);
        });


        ws.onEvent([&](AsyncWebSocket *srv, AsyncWebSocketClient *client, AwsEventType type,
                       void *arg, uint8_t *data, size_t len) {
            onEvent(srv, client, type, arg, data, len);
        });
        server.addHandler(&ws);

        server.begin();

        createParams();

    }

    void createParams() {
        paramHolder->add(wsInterval->getParam());
        paramHolder->add(wsParamInterval->getParam());
        paramHolder->add(mainLoopChange->getParam());
        paramHolder->add(debugInterval->getParam());
        paramHolder->add(forward->stuckCheckEnabled);
        paramHolder->add(forward->rotationCheckEnabled);
        paramHolder->add(forward->rotationHelper->rotationThreshold);

        paramHolder->add(forward->speed);
        paramHolder->add(forward->distWall);
        paramHolder->add(forward->distPersecution);
        paramHolder->add(forward->f30k);
        paramHolder->add(forward->f60k);
        paramHolder->add(forward->f90k);
        paramHolder->add(forward->maxSum);
        paramHolder->add(forward->runCorrectionSide);

        paramHolder->add(forward->turboModeDist);
        paramHolder->add(forward->turboSpeed);
        paramHolder->add(forward->turboMaxTurn);
        paramHolder->add(forward->t30k);
        paramHolder->add(forward->t60k);
        paramHolder->add(forward->t90k);


        paramHolder->add(mechanics->servoEnabled);
        paramHolder->add(mechanics->turnCentralPosition);
        paramHolder->add(mechanics->turnMaxAngle);
        paramHolder->add(mechanics->servoTurnDelta);

        paramHolder->add(mechanics->powerEnabled);
        paramHolder->add(mechanics->engine->speedCorrector->correctionFactor);
        paramHolder->add(mechanics->engine->speedCorrector->maxCorrectionRun);
        paramHolder->add(mechanics->engine->speedCorrector->maxCorrectionBrake);

        paramHolder->add(forward->minSpeed);
        paramHolder->add(forward->maxSpeed);

        paramHolder->readAllEeprom();
    }

    void loopWs() {
        if (wsInterval->isReady() && ws.count() > 0) {
            String message;
            serializeJson(createMessage(), message);
            ws.textAll(message);
        }
        if (/*wsParamInterval->isReady() ||*/ isNeedUpdateParams) {
            sendParams();
            isNeedUpdateParams = false;
        }
    }

    DynamicJsonDocument createMessage() {
        doc.clear();
        doc["t"] = "d";
        const JsonArray &array = doc.createNestedArray("d");
        array.add(sensors->l90d);
        array.add(sensors->l60d);
        array.add(sensors->l30d);
        array.add(sensors->f00d);
        array.add(sensors->r30d);
        array.add(sensors->r60d);
        array.add(sensors->r90d);

//        doc["l90"] = sensors->l90d;
//        doc["l60"] = sensors->l60d;
//        doc["l30"] = sensors->l30d;
//        doc["f00"] = sensors->f00d;
//        doc["r30"] = sensors->r30d;
//        doc["r60"] = sensors->r60d;
//        doc["r90"] = sensors->r90d;
        doc["a"] = activeStrategy->angle;
        doc["p"] = activeStrategy->power;
        doc["s"] = mechanics->engine->getSpeed();
        doc["f"] = fpsLastValue;
        doc["v"] = mechanics->battery.readFloatKalman();
        doc["st"] = getStrategy();
        return doc;
    }

    String getStrategy() {
        if (activeStrategy == forward) {
            return "forward";
        }/* else if (activeStrategy == backward) {
            return "backward";
//        } else if (activeStrategy == turbo) {
//            return "turbo";
        } else if (activeStrategy == rotate) {
            return "rotate";
        }*/
        return "unknown";
    }

    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                 void *arg, uint8_t *data, size_t len) {
        switch (type) {
            case WS_EVT_CONNECT:
                Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                              client->remoteIP().toString().c_str());
                sendParams(client->id());
                break;
            case WS_EVT_DISCONNECT:
                Serial.printf("WebSocket client #%u disconnected\n", client->id());
                break;
            case WS_EVT_DATA:
                handleWebSocketMessage(arg, data, len);
                break;
            case WS_EVT_PONG:
            case WS_EVT_ERROR:
                break;
        }
    }

    String createMessageWithParams() {
        String message;
        serializeJson(paramHolder->createMessageWithParams(), message);
        Serial.println(message);
        return message;
    }

    void sendParams(int clientId) {
        ws.text(clientId, createMessageWithParams());
    }

    void sendParams() {
        ws.textAll(createMessageWithParams());
    }

    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
        AwsFrameInfo *info = (AwsFrameInfo *) arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            data[len] = 0;

            const String &requestString = String((char *) data);
            Serial.printf("ws requestString=%s\n", requestString.c_str());
            DynamicJsonDocument jsonDoc = DynamicJsonDocument(100);
            deserializeJson(jsonDoc, requestString);
            isNeedUpdateParams = paramHolder->apply(jsonDoc);
        }
    }


private:


};
