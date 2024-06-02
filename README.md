# GrowBox
Growbox software raspberry pi. Read temperature sensors, control fans and lights.

# Temperature sensors

2 x Bosch BME280
i2C Adresses 0x76 / 0x77 -> solder bridge + cut default bridge!!

## Bring up:
- enable i2c raspberry pi
- `sudo apt install i2c-tools`
- `i2cdetect -y 1` on raspi command line to verify