#pragma once


#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <value/TimingValue.h>
#include <DNSServer.h>
#include "Roborace.h"




class RoboraceWebserver : public Roborace {

private:



    AsyncWebServer server = AsyncWebServer(80);
    AsyncWebSocket ws = AsyncWebSocket("/ws");

    DynamicJsonDocument doc = DynamicJsonDocument(256);

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
            const DynamicJsonDocument &document = createMessage();
            String message;
            serializeJson(document, message);
            ws.textAll(message);
        }
    }

    DynamicJsonDocument createMessage() {
        doc.clear();
        doc["fl"] = sensors->forwardLeftDistance;
        doc["fr"] = sensors->forwardRightDistance;
        doc["fc"] = sensors->forwardCenterDistance;
        doc["l"] = sensors->leftDistance;
        doc["r"] = sensors->rightDistance;
        doc["l45"] = sensors->left45Distance;
        doc["r45"] = sensors->right45Distance;
        doc["a"] = activeStrategy->angle;
        doc["p"] = activeStrategy->power;
        return doc;
    }

    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                 void *arg, uint8_t *data, size_t len) {
        switch (type) {
            case WS_EVT_CONNECT:
                Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                              client->remoteIP().toString().c_str());
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

    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
        AwsFrameInfo *info = (AwsFrameInfo *) arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            data[len] = 0;

            const String &request = String((char *) data);
            Serial.print("ws request=");
            Serial.println(request);
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
