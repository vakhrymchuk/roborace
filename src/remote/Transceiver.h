#ifndef ROBORACE_TRANSCEIVER_H
#define ROBORACE_TRANSCEIVER_H

#include <Arduino.h>
#include <NRF24.h>
#include <SPI.h>
#include "Message.h"

class Transceiver {
public:

    static const byte CHIP_ENABLE_PIN = 3;
    static const byte CHIP_SELECT_PIN = 4;
    static const byte CHANNEL = 5;

    Transceiver::Transceiver(Print &print) : print(print) {}

    Message message;

    void init(const char *name);

    void send(const char *name);

    bool waitAvailableAndReceive() {
        nrf24.waitAvailable();
        return receive();
    }

    bool receiveWithTimeout(uint16_t timeout) {
        return nrf24.waitAvailableTimeout(timeout) && receive();
    }

private:

    NRF24 nrf24 = NRF24(CHIP_ENABLE_PIN, CHIP_SELECT_PIN);

    Print &print;

    bool receive();
};

void Transceiver::send(const char *name) {

    if (!nrf24.setTransmitAddress((uint8_t *) name, strlen(name)))
        print.println(F("setTransmitAddress failed"));
    if (!nrf24.send((uint8_t *) &message, sizeof(Message)))
        print.println(F("send failed"));
    if (!nrf24.waitPacketSent()) {
        print.println(F("waitPacketSent failed"));
    }

}

bool Transceiver::receive() {

    byte buflen = sizeof(Message);
    if (!nrf24.recv((uint8_t *) &message, &buflen)) {
        print.println(F("read failed"));
        return false;
    }

    return true;
}

void Transceiver::init(const char *name) {
    if (!nrf24.init())
        print.println(F("NRF24 init failed"));
    if (!nrf24.setChannel(CHANNEL))
        print.println(F("setChannel failed"));
    if (!nrf24.setThisAddress((uint8_t *) name, strlen(name)))
        print.println(F("setThisAddress failed"));
    if (!nrf24.setPayloadSize(sizeof(Message)))
        print.println(F("setPayloadSize failed"));
    if (!nrf24.setRF(NRF24::NRF24DataRate2Mbps, NRF24::NRF24TransmitPower0dBm))
        print.println(F("setRF failed"));

    print.println(F("NRF24 initialised"));
}

#endif
