//   Bosch BMI088 範例程式 , 火箭鳥創客倉庫

#include <SPI.h>
#include "BMI088_SPI.h"

// XIAO RP2350 / RP2354A Pin Definitions
#define PIN_SCK    2
#define PIN_MOSI   3
#define PIN_MISO   4
#define CS_ACCEL   5
#define CS_GYRO    6

// Global Calibration Variables
float gyroBiasX = 0, gyroBiasY = 0, gyroBiasZ = 0;
bool isCalibrated = false;

// Initialize using SPI0 (standard 'SPI' object on RP2350)
BMI088 imu(SPI, CS_ACCEL, CS_GYRO);

void setup() {
    Serial.begin(115200);
    // Wait for USB Serial to connect
    while (!Serial && millis() < 5000); 

    Serial.println("\n========================================");
    Serial.println("   BMI088 HARDENED DRIVER - STARTUP     ");
    Serial.println("========================================");

    // 1. Assign Hardware SPI0 Pins
    SPI.setSCK(PIN_SCK);
    SPI.setTX(PIN_MOSI);
    SPI.setRX(PIN_MISO);
    
    // 2. Initialize IMU
    if (imu.begin()) {
        Serial.println("[SUCCESS] BMI088 Communication Established.");
    } else {
        Serial.println("[ERROR] BMI088 Not Found! Check Wiring/Solder.");
        while (1) {
            Serial.print(".");
            delay(500);
        }
    }

    // 3. Perform Gyro Calibration (The "Zeroing" Step)
    calibrateSensors();
}

void loop() {
    float ax, ay, az;
    float gx_raw, gy_raw, gz_raw;

    // Read raw data from the library
    imu.readAccel(ax, ay, az);
    imu.readGyro(gx_raw, gy_raw, gz_raw);

    // Apply Calibration Offsets to Gyro
    float gx = gx_raw - gyroBiasX;
    float gy = gy_raw - gyroBiasY;
    float gz = gz_raw - gyroBiasZ;

    // Output formatted for debugging and visualization
    printData(ax, ay, az, gx, gy, gz);

    delay(10); // 100Hz Loop
}

void calibrateSensors() {
    const int numSamples = 500;
    float tx, ty, tz;
    
    Serial.println(">> CALIBRATING GYRO... DO NOT MOVE SENSOR <<");
    
    float sumX = 0, sumY = 0, sumZ = 0;
    
    for (int i = 0; i < numSamples; i++) {
        imu.readGyro(tx, ty, tz);
        sumX += tx;
        sumY += ty;
        sumZ += tz;
        
        if (i % 100 == 0) Serial.print(".");
        delay(3); // Small delay between samples
    }
    
    gyroBiasX = sumX / numSamples;
    gyroBiasY = sumY / numSamples;
    gyroBiasZ = sumZ / numSamples;
    
    Serial.println("\n[DONE] Offsets Calculated:");
    Serial.print("X: "); Serial.print(gyroBiasX);
    Serial.print(" | Y: "); Serial.print(gyroBiasY);
    Serial.print(" | Z: "); Serial.println(gyroBiasZ);
    Serial.println("----------------------------------------");
}

void printData(float ax, float ay, float az, float gx, float gy, float gz) {
    // Accelerometer (g)
    Serial.print("ACC:");
    Serial.print(ax, 3); Serial.print(",");
    Serial.print(ay, 3); Serial.print(",");
    Serial.print(az, 3);
    
    // Gyroscope (deg/s) - Now centered around zero
    Serial.print(" | GYRO:");
    Serial.print(gx, 2); Serial.print(",");
    Serial.print(gy, 2); Serial.print(",");
    Serial.println(gz, 2);
}
