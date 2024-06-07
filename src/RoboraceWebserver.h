#pragma once

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "param/InteravalValue.h"
#include "param/ParamHolder.h"
#include "Roborace2.h"
#include "web.h"

class RoboraceWebserver : public Roborace
{

private:
    AsyncWebServer server = AsyncWebServer(80);
    AsyncWebSocket ws = AsyncWebSocket("/ws");

    JsonDocument doc = JsonDocument();

    IntervalValue *wsInterval = new IntervalValue(new Param(300, "ws-interval", "main"));
    IntervalValue *wsParamInterval = new IntervalValue(new Param(10000, "ws-param-interval", "main"));

    ParamHolder *paramHolder = new ParamHolder();

    bool isNeedUpdateParams = false;

public:
    RoboraceWebserver()
    {
        initServer();
    }

    void loop() final
    {
        loopWs();
        Roborace::loop();
    }

    void initServer()
    {
        WiFi.mode(WIFI_AP);
        WiFi.softAP("esp-server", "82514211");

        server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request)
                  { 
                    DEBUGRR("get root");
                    request->send(200, "text/html", index_html); });
        server.on(index_html_name, HTTP_GET, [&](AsyncWebServerRequest *request)
                  { 
                    DEBUGRR("get index.html");
                    request->send(200, "text/html", index_html); });
        server.on(style_css_name, HTTP_GET, [&](AsyncWebServerRequest *request)
                  {
                    DEBUGRR("get style.css");
                    request->send(200, "text/css", style_css); });
        server.on(script_js_name, HTTP_GET, [&](AsyncWebServerRequest *request)
                  {
                    DEBUGRR("get script.js");
                    request->send(200, "application/javascript", script_js); });

        server.on("/data.js", HTTP_GET, [&](AsyncWebServerRequest *request)
                  {
            const JsonDocument &document = createMessage();
            String message;
            serializeJson(document, message);
            request->send(200, "application/json", message); });

        ws.onEvent([&](AsyncWebSocket *srv, AsyncWebSocketClient *client, AwsEventType type,
                       void *arg, uint8_t *data, size_t len)
                   { onEvent(srv, client, type, arg, data, len); });
        server.addHandler(&ws);

        server.begin();

        createParams();
    }

    void createParams()
    {
        paramHolder->add(wsInterval->getParam());
        paramHolder->add(solution.speedParam);
        paramHolder->add(solution.speedTurboParam);

        // paramHolder->add(mechanics->servoEnabled);
        // paramHolder->add(mechanics->turnCentralPosition);

        // paramHolder->add(mechanics->powerEnabled);

        // paramHolder->readAllEeprom();
    }

    void loopWs()
    {
        if (wsInterval->isReady() && ws.count() > 0)
        {
            String message;
            serializeJson(createMessage(), message);
            ws.textAll(message);
        }
        if (/*wsParamInterval->isReady() ||*/ isNeedUpdateParams)
        {
            sendParams();
            isNeedUpdateParams = false;
        }
    }

    JsonDocument createMessage()
    {
        doc.clear();
        doc["t"] = "data";
        doc["ms"] = millis();
        doc["fps"] = fps;

        auto data = createPhysicalData();

        doc["speed"] = data.speed;
        doc["yaw"] = data.yaw;
        doc["pitch"] = data.pitch;
        doc["roll"] = data.roll;
        doc["absoluteAngle"] = data.absoluteAngle;

        const JsonObject &scan = doc["scan"].to<JsonObject>();
        scan["f"] = data.scan.findDistanceAtDegree(180);
        scan["r45"] = data.scan.findDistanceAtDegree(180 + 45);
        scan["l45"] = data.scan.findDistanceAtDegree(180 - 45);

        const JsonObject &solution = doc["solution"].to<JsonObject>();
        solution["speed"] = speed;
        solution["angle"] = turn;

        return doc;
    }

    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                 void *arg, uint8_t *data, size_t len)
    {
        switch (type)
        {
        case WS_EVT_CONNECT:
            DEBUGRRF("WebSocket client #%u connected from %s\n", client->id(),
                     client->remoteIP().toString().c_str());
            sendParams(client->id());
            break;
        case WS_EVT_DISCONNECT:
            DEBUGRRF("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
        }
    }

    String createMessageWithParams()
    {
        String message;
        serializeJson(paramHolder->createMessageWithParams(), message);
        DEBUGRR(message);
        return message;
    }

    void sendParams(int clientId)
    {
        ws.text(clientId, createMessageWithParams());
    }

    void sendParams()
    {
        ws.textAll(createMessageWithParams());
    }

    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
        {
            data[len] = 0;

            const String &requestString = String((char *)data);
            DEBUGRRF("ws requestString=%s\n", requestString.c_str());
            JsonDocument jsonDoc = JsonDocument();
            deserializeJson(jsonDoc, requestString);
            isNeedUpdateParams = paramHolder->apply(jsonDoc);
        }
    }

private:
};
