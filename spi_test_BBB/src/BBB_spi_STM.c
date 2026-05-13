/*
 ============================================================================
 Name        : BBB_spi_STM.c
 Author      : Bharath Kumar Reddy Busireddy
 Description : BBB SPI Master -> STM32 SPI Slave (Ping Packet v1)
 ============================================================================
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <string.h>

#define SPI_DEV "/dev/spidev1.0"

/* Protocol Definitions */
#define SOF         0xA5
#define VERSION     0x01
#define MSG_REQ     0x01
#define CMD_PING    0x10

#define PACKET_SIZE 6

/*--------------------------------------------------*/
/* Simple XOR CRC                                   */
/*--------------------------------------------------*/
uint8_t calc_crc(uint8_t *buf, uint8_t len)
{
    uint8_t crc = 0;

    for(int i = 0; i < len; i++)
    {
        crc ^= buf[i];
    }

    return crc;
}

/*--------------------------------------------------*/
/* Build Ping Packet                                */
/*--------------------------------------------------*/
void build_ping_packet(uint8_t *tx)
{
    tx[0] = SOF;       // Start Of Frame
    tx[1] = VERSION;   // Protocol Version
    tx[2] = MSG_REQ;   // Request
    tx[3] = CMD_PING;  // Ping Command
    tx[4] = 0x00;      // Payload Length = 0
    tx[5] = calc_crc(tx, 5);
}

/*--------------------------------------------------*/
/* Print Buffer                                     */
/*--------------------------------------------------*/
void print_buffer(char *name, uint8_t *buf, uint8_t len)
{
    printf("%s : ", name);

    for(int i = 0; i < len; i++)
    {
        printf("%02X ", buf[i]);
    }

    printf("\n");
}

/*--------------------------------------------------*/
/* Main                                             */
/*--------------------------------------------------*/
int main()
{
    int fd = open(SPI_DEV, O_RDWR);

    if(fd < 0)
    {
        perror("SPI open failed");
        return -1;
    }

    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = 500000;

    ioctl(fd, SPI_IOC_WR_MODE, &mode);
    ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    uint8_t tx[PACKET_SIZE];
    uint8_t rx[PACKET_SIZE];

    struct spi_ioc_transfer tr =
    {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = PACKET_SIZE,
        .speed_hz = speed,
        .bits_per_word = 8,
    };

    while(1)
    {
        memset(rx, 0, sizeof(rx));

        build_ping_packet(tx);

        if(ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 0)
        {
            perror("SPI transfer failed");
            break;
        }

        print_buffer("TX", tx, PACKET_SIZE);
        print_buffer("RX", rx, PACKET_SIZE);

        printf("----------------------------------\n");

        usleep(500000);
    }

    close(fd);
    return 0;
}