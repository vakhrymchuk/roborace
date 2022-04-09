#pragma once


class PrintFake : public Print {
public:
    size_t write(uint8_t uint8) override {
        return 0;
    }
};
