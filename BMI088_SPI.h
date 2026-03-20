//   Bosch BMI088 範例程式 , 火箭鳥創客倉庫


#ifndef BMI088_SPI_H
#define BMI088_SPI_H

#include <Arduino.h>
#include <SPI.h>

class BMI088 {
public:
    BMI088(SPIClass &bus, uint8_t accCS, uint8_t gyroCS);
    bool begin();
    void readAccel(float &x, float &y, float &z);
    void readGyro(float &x, float &y, float &z);

private:
    SPIClass *_spi;
    uint8_t _accCS, _gyroCS;
    const float ACC_SCALE = 6.0 / 32768.0;
    const float GYRO_SCALE = 2000.0 / 32768.0;

    void writeReg(uint8_t cs, uint8_t reg, uint8_t data);
    void readRegs(uint8_t cs, uint8_t reg, uint8_t *data, uint8_t len, bool isAccel);
    int16_t combine(uint8_t msb, uint8_t lsb);
};

#endif
