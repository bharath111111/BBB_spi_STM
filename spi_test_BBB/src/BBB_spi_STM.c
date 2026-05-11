/*
 ============================================================================
 Name        : BBB_spi_STM.c
 Author      : Bharath Kumar Reddy Busireddy
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <stdint.h>

#define SPI_DEV "/dev/spidev1.0"

int main()
{
    int fd = open(SPI_DEV, O_RDWR);
    if (fd < 0) {
        perror("SPI open failed");
        return -1;
    }

    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = 500000;

    ioctl(fd, SPI_IOC_WR_MODE, &mode);
    ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    uint8_t tx = 0xA5;
    uint8_t rx = 0x00;

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)&tx,
        .rx_buf = (unsigned long)&rx,
        .len = 1,
        .speed_hz = speed,
        .bits_per_word = 8,
    };

    while (1)
    {
        if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
            perror("SPI transfer failed");
            break;
        }

        printf("Sent 0xA5\n");
        usleep(500000);
    }

    close(fd);
    return 0;
}
