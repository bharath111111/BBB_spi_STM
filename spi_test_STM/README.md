# STM32F4 SPI Slave with BeagleBone Black Master

## Overview
This project demonstrates SPI communication between an STM32F410RB (slave) and a BeagleBone Black (master). The STM32 is configured as an SPI slave using HAL libraries, while the BeagleBone Black uses the spidev interface in Linux to act as the SPI master.

---

## Hardware Connections

| STM32F410RB Pin | SPI Signal | BeagleBone Black Pin | Notes                |
|-----------------|------------|---------------------|----------------------|
| PB12           | SCK        | SPI1_SCLK (P9_31)   | Clock from BB        |
| PC2             | MISO       | SPI1_MISO (P9_29)   | Data to BB           |
| PC3             | MOSI       | SPI1_MOSI (P9_30)   | Data from BB         |
| GND             | GND        | GND (P9_1, P9_2)    | Common ground        |
| (Optional)      | NSS        | SPI1_CS0 (P9_28)    | Use for hardware NSS |

- Ensure all grounds are connected.
- If using software NSS, connect as needed or tie NSS low on STM32.

---

## STM32 Project Configuration
- **Mode:** SPI2 as Slave
- **Data Size:** 8-bit
- **CPOL/CPHA:** Mode 0 (CPOL=0, CPHA=0)
- **First Bit:** MSB
- **HAL Library:** STM32Cube HAL
- **CubeMX:** Used for code generation

### Key Files
- `Core/Src/main.c` — Main application and SPI init
- `Core/Inc/main.h` — Main header
- `spi_test.ioc` — CubeMX configuration

---

## BeagleBone Black Configuration
- **SPI Devices:** spidev1.0 (or spidev0.0, as wired)
- **Python Module:** spidev
- **Permissions:** Run as root or adjust device permissions

### Example Python Code (Master)
```python
import spidev
spi = spidev.SpiDev()
spi.open(1, 0)  # Bus 1, Device 0 (spidev1.0)
spi.max_speed_hz = 1000000  # 1 MHz (adjust as needed)
spi.mode = 0b00  # SPI mode 0
resp = spi.xfer2([0x01, 0x02, 0x03])
print('Received:', resp)
spi.close()
```

---

## How to Build and Run

### STM32 Side
1. Open the project in STM32CubeIDE or your preferred tool.
2. Generate code if you changed the .ioc file.
3. Build and flash the firmware to the STM32F410RB board.
4. Connect the hardware as described above.

### BeagleBone Black Side
1. Boot into Linux and ensure SPI is enabled (`/dev/spidev1.0` exists).
2. Install Python spidev if not present: `sudo apt-get install python3-spidev`
3. Run the example Python script or your own SPI master code.

---

## Troubleshooting
- Double-check wiring and pin numbers.
- Ensure both devices use the same SPI mode and data size.
- Use an oscilloscope or logic analyzer to debug signals if needed.
- Check permissions for `/dev/spidev*` on BeagleBone Black.

---

## License
This project is for educational purposes.
