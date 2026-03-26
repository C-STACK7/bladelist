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
#include <bladeRF2.h>

#define ARRAY_SIZE(n) (sizeof(n) / sizeof(n[0]))



static int print_device_radio(struct bladerf *);
static char const *_rfic_tx_portstr(uint32_t);
static char const *_rfic_rx_portstr(uint32_t);
static char const *_rfswitch_portstr(uint32_t);

const char *channel2str(bladerf_channel);
char const *_txfir_to_str(bladerf_rfic_txfir);
char const *_rxfir_to_str(bladerf_rfic_rxfir);

struct bladerf *dev;
struct bladerf_devinfo *devinfo;
struct bladerf_version devversion;
const struct bladerf_range *samprange;
struct bladerf_rational_rate *rsamp;

bladerf_frequency freq;
bladerf_sample_rate samp;
bladerf_bandwidth bw;
const struct bladerf_range *bwrange;
bladerf_gain gain;
bladerf_gain_mode gainmode;
const struct bladerf_gain_modes *gainmodes;
const struct bladerf_range *gainrange;

int setdevnum = -1;
int devcount = 0;


uint16_t dac_trim;
bladerf_fpga_source devfpgasrc;
bladerf_fpga_size fpga_size = 0;
uint32_t flash_size = 1;
bool is_guess;

bladerf_channel ch = BLADERF_CHANNEL_INVALID;
const struct bladerf_range *freqrange;

float rfic_temperature = 0.0;
bladerf_power_sources power_source;
bladerf_clock_select clock_select;
int devspeed = -1;
uint16_t pmicconf = 0;       /**< Configuration register (uint16_t) */
float pmicvshunt = 0.0;     /**< Shunt voltage (float) */
float pmicvbus = 0.0;       /**< Bus voltage (float) */
float pmicpower = 0.0;      /**< Load power (float) */
float pmiccurrent = 0.0;    /**< Load current (float) */
uint16_t pmiccal = 0.0;


bladerf_rf_switch_config config;
bladerf_tuning_mode modetuning;
bladerf_gain_mode modegain= BLADERF_GAIN_DEFAULT;

int main(int argc, char *argv[])
{
    int status = 0;


    const char *mux_str;
    bladerf_rx_mux mux_setting;
    bladerf_rfic_rxfir rxfir;
    bladerf_rfic_txfir txfir;

    bladerf_set_usb_reset_on_open(true);
    devcount = bladerf_get_device_list(&devinfo);
    if(devcount <= 0)
        printf("no connected devices\n\n");
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
            if (setdevnum > (devcount-1) || setdevnum < 0)
                printf("***Error enter device number!***\n\n");

        /*
         *открытие свободного выбранного устройства
         */

        char serial[BLADERF_SERIAL_LENGTH -1 + 9] = ("*:serial=");
        strcat(serial, devinfo[setdevnum].serial);

        //освобождение списка
        bladerf_free_device_list(devinfo);

        //открытие устройства по серийному номеру


        status = bladerf_open(&dev, serial);
        if (status < 0) {
            printf("Not open serial device: %s\n",bladerf_strerror(status));
            return status;
        }
        else
            printf("Open serial device success!!!\n");
        }

        bladerf_close(dev);
        printf("\nDevice close!!!\n");

    return status;
    }
}
