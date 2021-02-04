# NFC-PN532-ESP32
PN532 NFC reader for ESP32 WIFI DEVKIT 1 

This example is for communicating with the PN532 chip using I2C. Wiring should be as follows:
 - PN532 SDA -> EFM32 D22 Pin (with 4.7K resistor already present)
 - PN532 SCL -> EFM32 D21 Pin(with 4.7K resistor already present)
 - PN532 IRQ -> EFM32 D4 Pin
 - PN32 RST0 -> EFM32 D5 Pin
 - PN532 3.3v -> 3.3v
 - PN532 GND -> GND

Based on readMifareClassicIrq.pde by Adafruit
