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


static int print_device_interface(struct bladerf *);
static int print_device_power(struct bladerf *);
static int print_device_radio(struct bladerf *);

static int set_device_freq(struct bladerf *);
static int set_device_samplerate(struct bladerf *);
static int set_device_bandwidth(struct bladerf *);
static int set_device_agc(struct bladerf *);
static int set_device_fir(struct bladerf *);





int main(int argc, char *argv[])
{
    int status = 0;
    struct bladerf *dev;
    struct bladerf_devinfo *devinfo;

    int setdevnum = -1;
    int devcount = 0;

    devcount = bladerf_get_device_list(&devinfo);
    if(devcount <= 0)
        printf("no connect devices\n\n");
    else{

        //выбор номера устройства

        while ((setdevnum > (devcount-1) || setdevnum < 0)) {

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

            printf("\nPlease set DEVICE N or free devices USB connect:");
            scanf("%d", &setdevnum);
            if (setdevnum > (devcount-1) || setdevnum < 0) printf("***Error enter device number!***\n\n");
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

//вывод основного меню
            int setmenu = 0;
             while (setmenu != 9) {
                //menu level 2
                switch (setmenu) {
                case 1:
                    /*
                    * menu 1 - info
                    */
                    while(setmenu !=0){
                        printf("\t\t**submenu info**\n"
                               "1 - info interface\n"
                               "2 - info power\n"
                               "3 - info radio\n"
                               "0 - return menu\n\n");
                        scanf("%d", &setmenu);
                    }

                    break;

                case 2:
                    /*
                    * menu 2 - set parametrs
                    */
                    while (setmenu != 0) {
                        printf("\t\t**submenu set parametrs**\n"
                               "1 - set freq\n"
                               "2 - set samplerate\n"
                               "3 - set bandwidth\n"
                               "4 - set AGC"
                               "5 - set FIR"
                               "0 - return menu\n\n");

                        scanf("%d", &setmenu);

                    }

                    break;

                case 3:
                    /*
                    * submenu 3 - read radiodata
                    */
                    while(setmenu != 0){
                        printf("\t\t**submenu read radiodata**\n"
                               "1 - read metadata\n"
                               "2 - read sync\n"
                               "0 - return menu\n\n");
                        scanf("%d", &setmenu);
                    }
                    break;

                case 4:
                    /*
                    * submenu 4 - transmit data
                    */
                    while(setmenu != 0){
                        printf("\t\t**submenu transmit data**\n"
                               "1 - transmit metadata\n"
                               "2 - transmit sync\n"
                               "0 - return menu\n\n");
                        scanf("%d", &setmenu);
                    }

                    break;

                    case 5:
                        /*
                        * submenu 1 - info
                        */

                        break;

                    case 6:
                        /*
                        * submenu 1 - info
                        */

                        break;

                    case 7:
                        /*
                        * submenu 1 - info
                        */

                        break;

                    case 8:
                        /*
                        * submenu 1 - info
                        */

                        break;

                    case 9:
                        /*
                        * submenu 1 - info
                        */

                        break;

                    default:{
                        //menu level 1
                        printf("\t\t**Software control bladeRF 2.0**\n"
                               "1 - info\n"
                               "2 - set parametrs\n"
                               "3 - read radiodata\n"
                               "4 - transmit data\n"
                               "5 - help\n"
                               "9 - exit\n\n");
                        scanf("%d", &setmenu);
                    }
                    break;
            }
             }
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

/*
 * меню приложения
 */


static int print_device_radio(struct bladerf *dev)
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

static int print_device_interface(struct bladerf *dev)
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
