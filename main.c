/*
 * Example of how to open a device with the specified serial number
 *
 * This file is part of the bladeRF project:
 *   http://www.github.com/nuand/bladeRF
 *
 * Copyright (C) 2014 Nuand LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <inttypes.h>
#include <libbladeRF.h>



int main(int argc, char *argv[])
{
    int status = 0;
    struct bladerf *dev;
    struct bladerf_devinfo *devinfo;
sdfsdafds
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
            printf("Open device: %s\n",
                    bladerf_strerror(status));
            return status;
        }
        else{
            printf("Open device: %s, %s\n",
                   devinfo->serial,
                   bladerf_strerror(status));
        }

        bladerf_free_device_list(devinfo);
        bladerf_close(dev);
        printf("\nDevice close!!!\n");
    }


    return status;
}
