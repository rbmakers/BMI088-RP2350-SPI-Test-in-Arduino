//   Bosch BMI088 範例程式 , 火箭鳥創客倉庫


#include "BMI088_SPI.h"

BMI088::BMI088(SPIClass &bus, uint8_t accCS, uint8_t gyroCS) {
    _spi = &bus;
    _accCS = accCS;
    _gyroCS = gyroCS;
}

bool BMI088::begin() {
    pinMode(_accCS, OUTPUT);
    pinMode(_gyroCS, OUTPUT);
    digitalWrite(_accCS, HIGH);
    digitalWrite(_gyroCS, HIGH);
    
    _spi->begin();
    delay(100);

    // --- ACCEL INIT ---
    writeReg(_accCS, 0x7E, 0xB6); // Soft Reset
    delay(50);
    
    uint8_t dummy;
    readRegs(_accCS, 0x00, &dummy, 1, true); // Dummy read to wake SPI
    
    writeReg(_accCS, 0x7C, 0x00); // Active Mode
    delay(10);
    writeReg(_accCS, 0x7D, 0x04); // Accel On
    delay(50);
    
    writeReg(_accCS, 0x40, 0x88); // 100Hz ODR
    writeReg(_accCS, 0x41, 0x01); // +/- 6G

    // --- GYRO INIT ---
    writeReg(_gyroCS, 0x14, 0xB6); // Soft Reset
    delay(100);
    
    writeReg(_gyroCS, 0x0F, 0x00); // 2000 dps
    writeReg(_gyroCS, 0x10, 0x07); // 100Hz ODR
    writeReg(_gyroCS, 0x11, 0x00); // Normal Mode
    delay(50);

    // --- VERIFY ---
    uint8_t accID, gyroID;
    readRegs(_accCS, 0x00, &accID, 1, true);
    readRegs(_gyroCS, 0x00, &gyroID, 1, false);

    if (accID == 0x1E && gyroID == 0x0F) return true;
    return false;
}

void BMI088::writeReg(uint8_t cs, uint8_t reg, uint8_t data) {
    _spi->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(cs, LOW);
    _spi->transfer(reg & 0x7F);
    _spi->transfer(data);
    digitalWrite(cs, HIGH);
    _spi->endTransaction();
    delayMicroseconds(10);
}

void BMI088::readRegs(uint8_t cs, uint8_t reg, uint8_t *data, uint8_t len, bool isAccel) {
    _spi->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(cs, LOW);
    _spi->transfer(reg | 0x80);
    
    if (isAccel) _spi->transfer(0x00); // The "Secret" Accel Dummy Byte
    
    for (uint8_t i = 0; i < len; i++) {
        data[i] = _spi->transfer(0x00);
    }
    digitalWrite(cs, HIGH);
    _spi->endTransaction();
    delayMicroseconds(10);
}

void BMI088::readAccel(float &x, float &y, float &z) {
    uint8_t buf[6];
    readRegs(_accCS, 0x12, buf, 6, true);
    x = combine(buf[1], buf[0]) * ACC_SCALE;
    y = combine(buf[3], buf[2]) * ACC_SCALE;
    z = combine(buf[5], buf[4]) * ACC_SCALE;
}

void BMI088::readGyro(float &x, float &y, float &z) {
    uint8_t buf[6];
    readRegs(_gyroCS, 0x02, buf, 6, false);
    x = combine(buf[1], buf[0]) * GYRO_SCALE;
    y = combine(buf[3], buf[2]) * GYRO_SCALE;
    z = combine(buf[5], buf[4]) * GYRO_SCALE;
}

int16_t BMI088::combine(uint8_t msb, uint8_t lsb) {
    return (int16_t)((msb << 8) | lsb);
}
