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



static int print_device_interface(struct bladerf *);
static int print_device_power(struct bladerf *);
static int print_device_radio(struct bladerf *);

static int set_device_freq(struct bladerf *);
static int set_device_samplerate(struct bladerf *);
static int set_device_bandwidth(struct bladerf *);
static int set_device_agc(struct bladerf *);
static int set_device_fir(struct bladerf *);

static char const *_rfic_tx_portstr(uint32_t);
static char const *_rfic_rx_portstr(uint32_t);
static char const *_rfswitch_portstr(uint32_t);

const char *channel2str(bladerf_channel);
char const *_txfir_to_str(bladerf_rfic_txfir);
char const *_rxfir_to_str(bladerf_rfic_rxfir);


int main(int argc, char *argv[])
{
    int status = 0;
    struct bladerf *dev;
    struct bladerf_devinfo *devinfo;
    struct bladerf_version devversion;
    int setdevnum = -1;
    int devcount = 0;


    uint16_t dac_trim;
    bladerf_fpga_source devfpgasrc;
    bladerf_fpga_size fpga_size = 0;
    uint32_t flash_size = 1;
    bool is_guess;



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


        status = bladerf_open(&dev, serial);
        if (status < 0) {
            printf("Not open serial device: %s\n",bladerf_strerror(status));
            return status;
        }
        else{
            printf("Open serial device success!!!\n");



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


                        switch (setmenu) {
                            /*
                             * info interface
                             */
                        case 1:{
                            status = bladerf_get_devinfo(dev,devinfo);
                            if (status < 0) {
                                printf("Info get device: %s\n",bladerf_strerror(status));
                                return status;
                            }
                            else{
                            printf("\n"
                                   "Manufacturer:\t%s \n"
                                   "Product :\t%s\n"
                                   "Serial:\t\t%s \n"
                                   "usb_bus:\t%d \n"
                                   "usb_addr:\t%d \n"
                                   "instance:\t%d\n",
                                   devinfo->manufacturer,
                                   devinfo->product,
                                   devinfo->serial,
                                   devinfo->usb_bus,
                                   devinfo->usb_addr,
                                   devinfo->instance
                                   );
                            }

                            printf("Board name:\t%s\n", bladerf_get_board_name(dev));

                            status = bladerf_get_vctcxo_trim(dev, &dac_trim);
                            if (status < 0) {
                                 printf("%s\n",bladerf_strerror(status));
                            }
                            else
                                printf("VCTCXO DAC cal:\t0x%.4X\n", dac_trim);


                            if(bladerf_get_fpga_size(dev, &fpga_size))
                                printf("%s\n",bladerf_strerror(status));
                            else
                                printf("FPGA size:\t%d KLE\n", fpga_size);

                            printf(bladerf_is_fpga_configured (dev) ? "FPGA configured yes\n" : "FPGA configured no\n");

                            if (bladerf_fpga_version (dev, &devversion))
                                printf("%s\n",bladerf_strerror(status));
                            else {
                                printf("FPGA version:\t%d.%d.%d\n",
                                       devversion.major,
                                       devversion.minor,
                                       devversion.patch
                                       );
                            }
                            if(bladerf_get_fpga_source(dev, &devfpgasrc))
                                printf("FPGA source:\tUNKNOWN %s\n",bladerf_strerror(status));
                            else {
                                if(devfpgasrc == 1)
                                    printf("FPGA source:\tFLASH \n");
                                else
                                    printf("FPGA source:\tHOST \n");
                            }

                            status = bladerf_get_flash_size(dev, &flash_size, &is_guess);
                            if (flash_size == 0 || status < 0)
                                    printf("Flash size:\tUNKNOWN %s\n",bladerf_strerror(status));
                            else{
                                    printf("Flash size:\t%u Mbit", (flash_size >> 17));
                                    printf( is_guess ? " (assumed)\n" : "\n");
                            }



                            devspeed = bladerf_device_speed(dev);
                            switch (devspeed) {
                            case 1:
                                printf("Device speed:\tHIGH\n");
                                break;
                            case 2:
                                printf("Device speed:\tSUPER\n");
                                break;
                            default:
                                printf("Device speed:\tUNKNOWN\n");
                                break;
                            }

                            printf("\n");
                        }
                        break;
                             /*
                              *  info power
                              */
                        case 2:{
                            status = bladerf_get_rfic_temperature(dev, &rfic_temperature);
                            if(status < 0)
                                printf("Rfic temperature:\tERROR READ %s\n",bladerf_strerror(status));
                            else
                                printf("Rfic temperature:\t%.2f °C\n", rfic_temperature);

                            status = bladerf_get_power_source(dev, &power_source);
                            if(status < 0)
                                printf("Power source:\tERROR READ %s\n",bladerf_strerror(status));
                            else{
                                if (power_source == BLADERF_UNKNOWN) printf("Power source:\t\tUNKNOWN\n");
                                if (power_source == BLADERF_PS_DC) printf("Power source:\t\tDC\n");
                                if (power_source == BLADERF_PS_USB_VBUS) printf("Power source:\t\tUSB VBUS\n");
                            }

                            status = bladerf_get_clock_select(dev, &clock_select);
                            if(status < 0)
                                printf("Clock select:\tERROR READ %s\n",bladerf_strerror(status));
                            else{
                                if (clock_select == CLOCK_SELECT_ONBOARD) printf("Clock select::\t\tONBOARD\n");
                                if (clock_select == CLOCK_SELECT_EXTERNAL) printf("Clock select::\t\tEXTERNAL\n");
                            }

                            /**< Configuration register (uint16_t) */
                            status = bladerf_get_pmic_register(dev, BLADERF_PMIC_CONFIGURATION, &pmicconf);
                            if(status < 0)
                                printf("PMIC config :\tERROR READ %s\n",bladerf_strerror(status));
                            else
                                printf("PMIC config :\t%#X\n",pmicconf);

                            /**< Shunt voltage (float) */
                            status = bladerf_get_pmic_register(dev, BLADERF_PMIC_VOLTAGE_SHUNT, &pmicvshunt);
                            if(status < 0)
                                printf("PMIC voltage shunt :\tERROR READ %s\n",bladerf_strerror(status));
                            else
                                printf("PMIC voltage shunt :\t%f V\n",pmicvshunt);

                            /**< Bus voltage (float) */
                            status = bladerf_get_pmic_register(dev, BLADERF_PMIC_VOLTAGE_BUS, &pmicvbus);
                            if(status < 0)
                                printf("PMIC voltage BUS :\tERROR READ %s\n",bladerf_strerror(status));
                            else
                                printf("PMIC voltage BUS :\t%.2f V\n",pmicvbus);

                            /**< Load power (float) */
                            status = bladerf_get_pmic_register(dev, BLADERF_PMIC_POWER, &pmicpower);
                            if(status < 0)
                                printf("PMIC power :\t\tERROR READ %s\n",bladerf_strerror(status));
                            else
                                printf("PMIC power :\t\t%.2f W\n",pmicpower);

                            /**< Load current (float) */
                            status = bladerf_get_pmic_register(dev, BLADERF_PMIC_CURRENT, &pmiccurrent);
                            if(status < 0)
                                printf("PMIC current :\t\tERROR READ %s\n",bladerf_strerror(status));
                            else
                                printf("PMIC current :\t\t%.2f A\n",pmiccurrent);

                            /**< Calibration (uint16_t) */
                            status = bladerf_get_pmic_register(dev, BLADERF_PMIC_CALIBRATION, &pmiccal);
                            if(status < 0)
                                printf("PMIC cal :\tERROR READ %s\n",bladerf_strerror(status));
                            else
                                printf("PMIC cal :\t%u\n", pmiccal);

                        }
                        break;
                            /*
                             * info radio
                             */
                        case 3:{

                            status = bladerf_get_tuning_mode(dev, &modetuning);
                            if (status < 0) printf("No get tuning mode: %s\n",bladerf_strerror(status));
                            else
                                printf("Tuning Mode: %s\n", modetuning == BLADERF_TUNING_MODE_HOST ? "Host" : "FPGA");

                            printf("AGC:\n");

                            status = bladerf_get_gain_mode(dev, BLADERF_CHANNEL_RX(0), &modegain);
                            if (status < 0)
                                printf("No get mode AGC: %s\n",bladerf_strerror(status));
                            else
                                printf("  %s AGC: %-10s\n", channel2str(BLADERF_CHANNEL_RX(0)),
                                       modegain == BLADERF_GAIN_MANUAL ? "Disabled" : "Enabled");

                            status = bladerf_get_gain_mode(dev, BLADERF_CHANNEL_RX(1), &modegain);
                            if (status < 0)
                                printf("No get mode AGC: %s\n",bladerf_strerror(status));
                            else
                                printf("  %s AGC: %-10s\n", channel2str(BLADERF_CHANNEL_RX(1)),
                                       modegain == BLADERF_GAIN_MANUAL ? "Disabled" : "Enabled");
                            printf("\n");

                            status = bladerf_get_rx_mux(dev, &mux_setting);
                            if (status < 0) {
                                printf("RX mux: %s\n", bladerf_strerror(status));
                            }

                            switch (mux_setting) {
                            case BLADERF_RX_MUX_BASEBAND:
                                mux_str = "BASEBAND - Baseband samples";
                                break;
                            case BLADERF_RX_MUX_12BIT_COUNTER:
                                mux_str = "12BIT_COUNTER - 12-bit Up-Count I, Down-Count Q";
                                break;
                            case BLADERF_RX_MUX_32BIT_COUNTER:
                                mux_str = "32BIT_COUNTER - 32-bit Up-Counter";
                                break;
                            case BLADERF_RX_MUX_DIGITAL_LOOPBACK:
                                mux_str =
                                    "DIGITAL_LOOPBACK: Digital Loopback of TX->RX in the FPGA";
                                break;
                            default:
                                mux_str = "Unknown";
                            }

                            printf("RX mux: %s\n", mux_str);

                            printf("\n");

                            status = bladerf_get_rfic_rx_fir(dev, &rxfir);
                            if (status < 0)
                                printf("RX FIR Filter: %s\n",bladerf_strerror(status));
                            else {
                                printf("RX FIR Filter: %s%s\n", _rxfir_to_str(rxfir),
                                       (BLADERF_RFIC_RXFIR_DEFAULT == rxfir) ? " (default)" : "");
                            }

                            status = bladerf_get_rfic_tx_fir(dev, &txfir);
                            if (status < 0) {
                                printf("TX FIR Filter: %s\n",bladerf_strerror(status));
                            }
                            else {
                                printf("TX FIR Filter: %s%s\n", _txfir_to_str(txfir),
                                       (BLADERF_RFIC_TXFIR_DEFAULT == txfir) ? " (default)" : "");
                            }

                            printf("\n");

                            status = bladerf_get_rf_switch_config(dev, &config);
                            if (status < 0) {
                                printf("RF routing: %s\n",bladerf_strerror(status));
                            }
                            else{
                            printf("RF routing:\n");
                            printf("RX1: RFIC 0x%x (%-7s) <= SW 0x%x (%-7s)\n",
                                   config.rx1_rfic_port, _rfic_rx_portstr(config.rx1_rfic_port),
                                   config.rx1_spdt_port, _rfswitch_portstr(config.rx1_spdt_port));
                            printf("RX2: RFIC 0x%x (%-7s) <= SW 0x%x (%-7s)\n",
                                   config.rx2_rfic_port, _rfic_rx_portstr(config.rx2_rfic_port),
                                   config.rx2_spdt_port, _rfswitch_portstr(config.rx2_spdt_port));
                            printf("TX1: RFIC 0x%x (%-7s) => SW 0x%x (%-7s)\n",
                                   config.tx1_rfic_port, _rfic_tx_portstr(config.tx1_rfic_port),
                                   config.tx1_spdt_port, _rfswitch_portstr(config.tx1_spdt_port));
                            printf("TX2: RFIC 0x%x (%-7s) => SW 0x%x (%-7s)\n",
                                   config.tx2_rfic_port, _rfic_tx_portstr(config.tx2_rfic_port),
                                   config.tx2_spdt_port, _rfswitch_portstr(config.tx2_spdt_port));
                            }

                            printf("\n");

                            printf("Frequency channel:\n");
                            status = print_device_radio(dev);
                            if (status < 0) {
                                printf("No get radio parametrs: %s\n",bladerf_strerror(status));
                            }

                            printf("\n");


                            printf("\n");

                        }
                        break;
                        default:
                            printf("\n Error enter num menu\n\n");
                            break;
                        }

                    }



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

static char const *_rfic_rx_portstr(uint32_t port)
{
    const char *pstrs[] = { "A_BAL",  "B_BAL",   "C_BAL", "A_N", "A_P",
                           "B_N",    "B_P",     "C_N",   "C_P", "TXMON1",
                           "TXMON2", "TXMON12", NULL };

    if (port > ARRAY_SIZE(pstrs)) {
        return "N/A";
    }

    return pstrs[port];
}

static char const *_rfic_tx_portstr(uint32_t port)
{
    const char *pstrs[] = { "TXA", "TXB", NULL };

    if (port > ARRAY_SIZE(pstrs)) {
        return "N/A";
    }

    return pstrs[port];
}

static char const *_rfswitch_portstr(uint32_t port)
{
    const char *pstrs[] = { "OPEN", "RF2(A)", "RF3(B)", NULL };

    if (port > ARRAY_SIZE(pstrs)) {
        return "N/A";
    }

    return pstrs[port];
}

const char *channel2str(bladerf_channel ch)
{
    switch (ch) {
    case BLADERF_CHANNEL_RX(0):
        return "RX1";
    case BLADERF_CHANNEL_TX(0):
        return "TX1";
    case BLADERF_CHANNEL_RX(1):
        return "RX2";
    case BLADERF_CHANNEL_TX(1):
        return "TX2";
    default:
        return "Unknown";
    }
}

/* filter */
char const *_rxfir_to_str(bladerf_rfic_rxfir rxfir)
{
    switch (rxfir) {
    case BLADERF_RFIC_RXFIR_BYPASS:
        return "bypass";
    case BLADERF_RFIC_RXFIR_CUSTOM:
        return "custom";
    case BLADERF_RFIC_RXFIR_DEC1:
        return "normal";
    case BLADERF_RFIC_RXFIR_DEC2:
        return "2x decimation";
    case BLADERF_RFIC_RXFIR_DEC4:
        return "4x decimation";
    }

    return "unknown";
}

bladerf_rfic_rxfir _str_to_rxfir(char const *str, bool *ok)
{
    if (ok != NULL) {
        *ok = true;
    }

    if (!strcasecmp(str, "default")) {
        return BLADERF_RFIC_RXFIR_DEFAULT;
    } else if (!strcasecmp(str, "bypass")) {
        return BLADERF_RFIC_RXFIR_BYPASS;
    } else if (!strcasecmp(str, "custom")) {
        return BLADERF_RFIC_RXFIR_CUSTOM;
    } else if (!strcasecmp(str, "normal")) {
        return BLADERF_RFIC_RXFIR_DEC1;
    } else if (!strcasecmp(str, "dec2")) {
        return BLADERF_RFIC_RXFIR_DEC2;
    } else if (!strcasecmp(str, "dec4")) {
        return BLADERF_RFIC_RXFIR_DEC4;
    } else {
        if (ok != NULL) {
            *ok = false;
        }

        return BLADERF_RFIC_RXFIR_DEFAULT;
    }
}

char const *_txfir_to_str(bladerf_rfic_txfir txfir)
{
    switch (txfir) {
    case BLADERF_RFIC_TXFIR_BYPASS:
        return "bypass";
    case BLADERF_RFIC_TXFIR_CUSTOM:
        return "custom";
    case BLADERF_RFIC_TXFIR_INT1:
        return "normal";
    case BLADERF_RFIC_TXFIR_INT2:
        return "2x interpolation";
    case BLADERF_RFIC_TXFIR_INT4:
        return "4x interpolation";
    }

    return "unknown";
}

bladerf_rfic_txfir _str_to_txfir(char const *str, bool *ok)
{
    if (ok != NULL) {
        *ok = true;
    }

    if (!strcasecmp(str, "default")) {
        return BLADERF_RFIC_TXFIR_DEFAULT;
    } else if (!strcasecmp(str, "bypass")) {
        return BLADERF_RFIC_TXFIR_BYPASS;
    } else if (!strcasecmp(str, "custom")) {
        return BLADERF_RFIC_TXFIR_CUSTOM;
    } else if (!strcasecmp(str, "normal")) {
        return BLADERF_RFIC_TXFIR_INT1;
    } else if (!strcasecmp(str, "int2")) {
        return BLADERF_RFIC_TXFIR_INT2;
    } else if (!strcasecmp(str, "int4")) {
        return BLADERF_RFIC_TXFIR_INT4;
    } else {
        if (ok != NULL) {
            *ok = false;
        }

        return BLADERF_RFIC_TXFIR_DEFAULT;
    }
}
