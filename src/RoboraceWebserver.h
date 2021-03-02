#pragma once


#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <value/TimingValue.h>
#include <value/ParamHolder.h>
#include "Roborace.h"


class RoboraceWebserver : public Roborace {

private:


    AsyncWebServer server = AsyncWebServer(80);
    AsyncWebSocket ws = AsyncWebSocket("/ws");

    DynamicJsonDocument doc = DynamicJsonDocument(256);

    IntervalValue *wsInterval = new IntervalValue(new Param(500, "ws-interval", "main"));

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

        // Route for root / web page
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
        paramHolder->add(mainLoopChange->getParam());
        paramHolder->add(debugInterval->getParam());

        paramHolder->add(forward->speed);
        paramHolder->add(forward->turboModeDist);
        paramHolder->add(forward->distWall);
        paramHolder->add(forward->distPersecution);
        paramHolder->add(forward->side45SensorsKoef);
        paramHolder->add(forward->sideSensorsKoef);
        paramHolder->add(forward->maxSum);

        paramHolder->add(turbo->turboSpeed);
        paramHolder->add(turbo->acceleration);
        paramHolder->add(turbo->turboMaxTurn);
        paramHolder->add(turbo->turboModeDisableDist);

        paramHolder->add(mechanics->servoEnabled);
        paramHolder->add(mechanics->turnCentralPosition);
        paramHolder->add(mechanics->turnMaxAngle);
        paramHolder->add(mechanics->powerEnabled);
        paramHolder->add(mechanics->engine->speedCorrector->correctionFactor);
        paramHolder->add(mechanics->engine->speedCorrector->maxCorrectionRun);
        paramHolder->add(mechanics->engine->speedCorrector->maxCorrectionBrake);

        paramHolder->readAllEeprom();
    }

    void loopWs() {
        if (wsInterval->isReady()) {
            String message;
            serializeJson(createMessage(), message);
            ws.textAll(message);
        }
        if (isNeedUpdateParams) {
            sendParams();
            isNeedUpdateParams = false;
        }
    }

    DynamicJsonDocument createMessage() {
        doc.clear();
        doc["t"] = "d";
        doc["fl"] = sensors->forwardLeftDistance;
        doc["fr"] = sensors->forwardRightDistance;
        doc["fc"] = sensors->forwardCenterDistance;
        doc["l"] = sensors->leftDistance;
        doc["r"] = sensors->rightDistance;
        doc["l45"] = sensors->left45Distance;
        doc["r45"] = sensors->right45Distance;
        doc["a"] = activeStrategy->angle;
        doc["p"] = activeStrategy->power;
        doc["s"] = mechanics->engine->getSpeed();
        doc["f"] = fpsLastValue;
        doc["v"] = mechanics->battery.readFloatKalman();
        return doc;
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
            deserializeJson(doc, requestString);
            isNeedUpdateParams = paramHolder->apply(doc);
        }
    }


private:


};
