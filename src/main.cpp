/**
 * @author Valery Akhrymchuk
 */

#include <Arduino.h>
#include <WiFiMulti.h>
#include <Wire.h>

#define DEBUG true

#include "RoboraceWebserver.h"

RoboraceWebserver *roborace;

WiFiMulti *wiFiMulti;

void connectToAp() {
    IPAddress local_IP(192, 168, 1, 215);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.config(local_IP, gateway, subnet);
    WiFi.setHostname("subaru");
    WiFiClass::mode(WIFI_STA);

    wiFiMulti = new WiFiMulti();
    wiFiMulti->addAP("smarthata", "82514211");
    wiFiMulti->addAP("honor-20", "82514211");
    Serial.print("Wait for WiFi... ");
    wiFiMulti->run();

    while (WiFiClass::status() != WL_CONNECTED) {
        wiFiMulti->run();
        delay(100);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void startAp() {
    // Start access point
    WiFiClass::mode(WIFI_AP);
    WiFi.softAP("subaru", "82514211");
    delay(100);

//        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

//        const byte DNS_PORT = 53;
//        dnsServer.start(DNS_PORT, server_name, apIP);

    IPAddress ip_address = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(ip_address);
}

void setup() {


#ifdef VL53
    Wire.begin();
    Wire.setClock(400000);
#endif

#ifdef DEBUG
    Serial.begin(115200);
    Serial.println("start");
#endif

    roborace = new RoboraceWebserver();


    connectToAp();

    roborace->initServer();

//    while (millis() < 5000) {
//        delay(1);
//        wiFiMulti->run(10);
//    }

}

void loop() {
    roborace->loop();
    wiFiMulti->run(10);
}
