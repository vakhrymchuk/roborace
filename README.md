# Roborace

Author: Valery Akhrymchuk

uint8_t TCA9548A::read()
{
    this->myWire->requestFrom((uint8_t)this->_address, (uint8_t)1, (uint8_t)true);
