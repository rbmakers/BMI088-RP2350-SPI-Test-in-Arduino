BMI088 SPI 驅動開發與測試 (Arduino - RP2350/RP2354A)
=============================================

本專案提供針對 **Bosch BMI088** 高性能慣性測量單元 (IMU) 的 Arduino 驅動實作，特別針對 **Raspberry Pi RP2350/RP2354A** 系列微控制器進行了通訊穩定化處理。

專案目標
----

BMI088 由獨立的加速度計 (Accelerometer) 與陀螺儀 (Gyroscope) 晶片組成，兩者在同一條 SPI 總線上具有不同的通訊協議特性。本專案旨在解決以下硬體開發難點：

*   **總線衝突處理**：優化 MISO 訊號切換邏輯，防止兩個感測器同時驅動總線導致的數據丟失或 $0.000$ 異常讀值。
    
*   **硬體初始化序列**：實作 BMI088 特有的電源啟動握手協議（Power-on Sequence），確保感測器能從待機模式正確喚醒。
    
*   **動態偏移校準**：提供啟動時的自動零偏校準（Zero-bias Calibration），消除陀螺儀初始漂移。
    

硬體配置與接線
-------

測試環境使用 **XIAO RP2350** 與自研 **RP2354A** 測試板，採用硬體 **SPI0** 或 **SPI1** 進行通訊。以下為建議的 GPIO 配置：

**訊號名稱引腳 (GPIO)功能描述SCK**GPIO 2SPI Serial Clock**MOSI (TX)**GPIO 3Master Out Slave In**MISO (RX)**GPIO 4Master In Slave Out**CS\_ACCEL**GPIO 5加速度計片選 (Active Low)**CS\_GYRO**GPIO 6陀螺儀片選 (Active Low)

關鍵技術特點
------

1.  **硬體介面靈活性**：
    
    *   專為 RP2350 架構設計，支援透過 SPI.setRX()、SPI.setTX() 與 SPI.setSCK() 自定義引腳。
        
    *   具備高度相容性，僅需更改對象聲明即可在 SPI (SPI0) 與 SPI1 之間切換。
        
2.  **通訊協定優化**：
    
    *   **Dummy Byte 處理**：針對 BMI088 加速度計在 SPI 讀取模式下需要額外一個虛擬位元組（Dummy Byte）的特性進行封裝，確保數據對齊。
        
    *   **時序補償**：在 CS 引腳切換間加入微秒級延遲，確保 MISO 線路完全釋放，避免訊號干擾。
        
3.  **自動校準功能**：
    
    *   內建啟動校準函式，自動採樣 500 組靜態數據進行均值計算，補償感測器的零點偏移（Zero-rate offset）。
        

快速開始
----

1.  **檔案準備**：將 BMI088\_SPI.h 與 BMI088\_SPI.cpp 置於您的 Arduino 專案目錄下。
    
2.  **引腳定義**：根據您的實際電路佈線修改 .ino 檔案中的 GPIO 定義。
    
3.  **編譯上傳**：使用 Arduino IDE 編譯並上傳至 RP2350/RP2354 開發板。
    
4.  **數據監控**：開啟序列號監控視窗 (115200 baud)，即可觀察校準後的六軸數值。