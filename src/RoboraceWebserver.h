#pragma once


#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <value/TimingValue.h>
#include "Roborace.h"


class RoboraceWebserver : public Roborace {

private:


    AsyncWebServer server = AsyncWebServer(80);
    AsyncWebSocket ws = AsyncWebSocket("/ws");

    DynamicJsonDocument doc = DynamicJsonDocument(512);

    IntervalValue *wsInterval = new IntervalValue(new ValueInt(500));


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

    }

    void loopWs() {
        if (wsInterval->isReady()) {
            String message;
            serializeJson(createMessage(), message);
            ws.textAll(message);
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

    void sendParams(int clientId) {
        String message;
        serializeJson(createMessageWithParams(), message);
        Serial.println(message);
        ws.text(clientId, message);
    }

    DynamicJsonDocument createMessageWithParams() {
        doc.clear();
        doc["t"] = "p";
        const JsonObject &main = doc.createNestedObject("main");
        main["ws-interval"] = wsInterval->getValueInt()->value;
        main["main-interval"] = mainLoopChange->getValueInt()->value;
        main["max-angle-turn"] = mechanics->turnMaxAngle->value;
        main["servo-center"] = mechanics->turnCentralPosition->value;

        const JsonObject &forwardObject = doc.createNestedObject("forward");
        forwardObject["forward-speed"] = forward->forwardSpeed->valueInt->value;
        forwardObject["start-turn-dist"] = forward->distStartTurn->value;
        forwardObject["full-turn-dist"] = forward->distFullTurn->value;

        const JsonObject &turboObject = doc.createNestedObject("turbo");
        turboObject["turbo-speed"] = turbo->speed->value;
        turboObject["turbo-max-angle-turn"] = turbo->turboMaxTurn->value;

        const JsonObject &engine = doc.createNestedObject("engine");
        engine["correction-factor"] = mechanics->engine->engineHelper->correctionFactor->value;
        engine["correction-run"] = mechanics->engine->engineHelper->maxCorrectionRun->value;
        engine["correction-brake"] = mechanics->engine->engineHelper->maxCorrectionBrake->value;

        return doc;
    }

    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
        AwsFrameInfo *info = (AwsFrameInfo *) arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            data[len] = 0;

            const String &request = String((char *) data);
            Serial.printf("ws request=%s\n", request.c_str());
            deserializeJson(doc, request);
            if (doc.containsKey("ws-interval")) {
                wsInterval->getValueInt()->value = doc["ws-interval"];
            }
            if (doc.containsKey("main-interval")) {
                mainLoopChange->getValueInt()->value = doc["main-interval"];
            }
            if (doc.containsKey("forward-speed")) {
                forward->forwardSpeed->resetValue(doc["forward-speed"]);
            }
            if (doc.containsKey("start-turn-dist")) {
                forward->distStartTurn->value = doc["start-turn-dist"];
            }
            if (doc.containsKey("full-turn-dist")) {
                forward->distFullTurn->value = doc["full-turn-dist"];
            }
            if (doc.containsKey("turbo-mode-dist")) {
                forward->turboModeDist->value = doc["turbo-mode-dist"];
            }
            if (doc.containsKey("wall-dist")) {
                forward->distWall->value = doc["wall-dist"];
            }
            if (doc.containsKey("persecution-dist")) {
                forward->distPersecution->value = doc["persecution-dist"];
            }

        }
    }


private:


};
