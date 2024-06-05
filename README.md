# GrowBox
Growbox software raspberry pi. Read temperature sensors, control fans and lights.

# Temperature Sensors
2 x Bosch BME280
i2C Adresses 0x76 / 0x77 -> connect solder bridge + cut default bridge!!

**Bring Up:**
- enable i2c raspberry pi
- `sudo apt-get install libi2c-dev`
- `sudo apt install i2c-tools`
- `i2cdetect -y 1` on raspi command line to verify

default baudrate 100Kbps -> increase with `gpio load i2c 400` to 400Kbps

# Fan Speed Control
- `gpio readall` to print wPi <-> BCM pin mapping

**PWM Pins**
| BCM | PWM |
|--------|------|
| 12 | PWM0 |
| 18 | PWM0 |
| 13 | PWM1 |
| 19 | PWM1 |

Use BCM pins `12` and `13` -> wPi pins `GPIO26` and `GPIO23`

# Display
SSD1306 based 128x64 pixel OLED screen. I2c address `0x3c`

# Button / Light Control

GPIO21 (wPi) -> BCM 5
GPIO22 (wPi) -> BCM 6