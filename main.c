/*
 * Example of how to open a device with the specified serial number
 *
 */

#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <inttypes.h>
#include <libbladeRF.h>

static int print_device_state(struct bladerf *);

int main(int argc, char *argv[])
{
    int status = 0;
    struct bladerf *dev;
    struct bladerf_devinfo *devinfo;

    int setdevnum = -1;
    int devcount = 0;

    devcount = bladerf_get_device_list(&devinfo);
    if(devcount <= 0)
        printf("no devices\n");
    else{
        // Вывод информации о подключенных устройствах к  usb шине
        printf("Search all devices BLADERF USB connect: %d\n", devcount);
        for (unsigned char i = 0; i < devcount; i++){
            printf("Device %d "
                   "sn:%s, "
                   "product : %s,\t"
                   "usb_bus:%d, "
                   "usb_addr: %d, "
                   "instance: %d,"
                   "\n",
                   i,
                   devinfo[i].serial,
                   devinfo[i].product,
                   devinfo[i].usb_bus,
                   devinfo[i].usb_addr,
                   devinfo[i].instance
                );
        }

        //выбор номера устройства
        enternumdev:
        printf("\nPlease set num or free devices USB connect:");
        scanf("%d", &setdevnum);
        if (setdevnum > (devcount-1) || setdevnum < 0){
            printf("Error enter device number!\n");
            goto enternumdev;
        }

        /*
         *открытие свободного выбранного устройства
         */

        char serial[BLADERF_SERIAL_LENGTH -1 + 9] = ("*:serial=");
        strcat(serial, devinfo[setdevnum].serial);

        //освобождение списка
        bladerf_free_device_list(devinfo);

        //открытие устройства по серийному номеру
        bladerf_set_usb_reset_on_open(true);
        bladerf_open(&dev, serial);
        int status = bladerf_get_devinfo(dev,devinfo);
        if (status ==BLADERF_ERR_NOT_INIT) {
            printf("Open device: %s\n",bladerf_strerror(status));
            return status;
        }
        else{
            printf("Open device: %s, %s\n",
                   devinfo->serial,
                   bladerf_strerror(status));
            //печать информации об устройстве.
            status = print_device_state(dev);
            if (status != 0) {
                fprintf(stderr, "Error: %s\n", bladerf_strerror(status));
            }

        }

        bladerf_free_device_list(devinfo);
        bladerf_close(dev);
        printf("\nDevice close!!!\n");
    }


    return status;
}


int setmenu(void)
{
    int selectmenu = 0;
    printf("\t\t**Software control bladeRF 2.0**\n"
           "1 - print hardware parametrs\n"
           "2 - print software parametrs\n"
           "3 - print power parameters\n"
           "4 - print radio parameters\n");
    scanf("%d", &selectmenu);
    return selectmenu;
}
static int print_device_state(struct bladerf *dev)
{
    int status;
    uint64_t frequency;
    unsigned int bandwidth;
    struct bladerf_rational_rate rate;

    const bladerf_channel rx_ch = BLADERF_CHANNEL_RX(0);
    const bladerf_channel tx_ch = BLADERF_CHANNEL_TX(0);

    status = bladerf_get_frequency(dev, rx_ch, &frequency);
    if (status != 0) {
        return status;
    }

    printf("  RX frequency: %" PRIu64 " Hz\n", frequency);

    status = bladerf_get_frequency(dev, tx_ch, &frequency);
    if (status != 0) {
        return status;
    }

    printf("  TX frequency: %" PRIu64 " Hz\n", frequency);

    status = bladerf_get_bandwidth(dev, rx_ch, &bandwidth);
    if (status != 0) {
        return status;
    }

    printf("  RX bandwidth: %u Hz\n", bandwidth);

    status = bladerf_get_bandwidth(dev, tx_ch, &bandwidth);
    if (status != 0) {
        return status;
    }

    printf("  TX bandwidth: %u Hz\n", bandwidth);

    status = bladerf_get_rational_sample_rate(dev, rx_ch, &rate);
    if (status != 0) {
        return status;
    }

    printf("  RX sample rate: %" PRIu64 " %" PRIu64 "/%" PRIu64 " sps\n",
           rate.integer, rate.num, rate.den);

    status = bladerf_get_rational_sample_rate(dev, tx_ch, &rate);
    if (status != 0) {
        return status;
    }

    printf("  TX sample rate: %" PRIu64 " %" PRIu64 "/%" PRIu64 " sps\n",
           rate.integer, rate.num, rate.den);

    return 0;
}
