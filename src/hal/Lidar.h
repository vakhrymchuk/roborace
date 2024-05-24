#pragma once

#include <LDS_RPLIDAR_A1.h>
#include <map>

class Lidar
{
public:
    static LDS_RPLIDAR_A1 *lds;
    static HardwareSerial *LdSerial;

private:
    static std::map<int, int> *angles;
    static std::map<int, int> *angles_new;

    static bool scan_completed;

    Lidar() {}

public:
    static void init()
    {

        lds->setScanPointCallback(lds_scan_point_callback);
        lds->setPacketCallback(lds_packet_callback);
        lds->setSerialWriteCallback(lds_serial_write_callback);
        lds->setSerialReadCallback(lds_serial_read_callback);
        lds->setMotorPinCallback(lds_motor_pin_callback);
        lds->init();

        // default 128 hw + 256 sw
        DEBUGF("LDS RX buffer size %d\n", LdSerial->setRxBufferSize(1024)); // must be before .begin()
        uint32_t baud_rate = lds->getSerialBaudRate();
        DEBUGF("LDS baud rate %d\n", baud_rate);

        LdSerial->begin(baud_rate);
        while (LdSerial->read() >= 0)
            DEBUGF(".");

        LDS::result_t result = lds->start();
        DEBUGF("LDS init() result: %d\n", lds->resultCodeToString(result));

        if (result < 0)
            DEBUGF("WARNING: is LDS connected to ESP32?");

        DEBUGF("LDS isActive(): %d\n", lds->isActive());
    }

    static bool getData(std::map<int, int> &data)
    {
        lds->loop();
        if (!scan_completed)
            return false;
        scan_completed = false;
        data.insert(angles->begin(), angles->end());
        angles->clear();
        return true;
    }

    static void lds_scan_point_callback(float angle_deg, float distance_mm, float quality, bool scan_completed)
    {
        if (distance_mm > 0 /*&& quality > 0.5*/)
            angles_new->insert({(int)angle_deg, distance_mm / 10});

        if (scan_completed)
        {
            angles->insert(angles_new->begin(), angles_new->end());
            angles_new->clear();
            Lidar::scan_completed = true;
        }
    }

    static void lds_motor_pin_callback(float value, LDS::lds_pin_t lds_pin) {}

    static int lds_serial_read_callback()
    {
        return LdSerial->read();
    }

    static size_t lds_serial_write_callback(const uint8_t *buffer, size_t length)
    {
        return LdSerial->write(buffer, length);
    }

    static void lds_info_callback(LDS::info_t code, String info)
    {
        DEBUGF("LDS info %s: %s\n", lds->infoCodeToString(code), info);
    }

    static void lds_error_callback(LDS::result_t code, String aux_info)
    {
        DEBUGF("LDS error %s: %s\n", lds->resultCodeToString(code), aux_info);
    }

    static void lds_packet_callback(uint8_t *packet, uint16_t length, bool scan_completed)
    {
        return;
    }

private:
};

bool Lidar::scan_completed = false;
LDS_RPLIDAR_A1 *Lidar::lds = new LDS_RPLIDAR_A1();
HardwareSerial *Lidar::LdSerial = new HardwareSerial(2); // TX 17, RX 16

std::map<int, int> *Lidar::angles = new std::map<int, int>();
std::map<int, int> *Lidar::angles_new = new std::map<int, int>();
