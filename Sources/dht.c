/* *** DHT Communication API *** */
/***************************************************************************
 *                                                                         *
 * This API contains the functions to configure and control a DHT22        *
 * temperature and humidity sensor over a 1-Wire bus.                      *
 *                                                                         *
 *   --------------------                                                  *
 * ||*** DHT22 LAYOUT ***||                                                *
 *   --------------------                                                  *
 *                                                                         *
 *         ------------------------------                                  *
 *       //             ____             \\                                *
 *      //            //    \\            \\                               *
 *     //            ||      ||            \\                              *
 *    //              \\____//              \\                             *
 *   //                                      \\                            *
 *   ||=======================================||                           *
 *   ||   |    |       |    |       |    |    ||                           *
 *   ||   |    |       |    |       |    |    ||                           *
 *   ||   |____|       |____|       |____|    ||                           *
 *   ||    ____         ____         ____     ||                           *
 *   ||-- |    |       |    |       |    |  --||                           *
 *   || | |    |       |    |       |    |  | ||                           *
 *   ||-- |____|       |____|       |____|  --||                           *
 *   ||    ____         ____         ____     ||                           *
 *   ||-- |    |       |    |       |    |  --||                           *
 *   || | |    |       |    |       |    |  | ||                           *
 *   ||-- |____|       |____|       |____|  --||                           *
 *   ||    ____         ____         ____     ||                           *
 *   || | |    |       |    |       |    |  | ||                           *
 *   ||-- |    |       |    |       |    |  --||                           *
 *   ||-- |____|       |____|       |____|  --||                           *
 *   ||    ____         ____         ____     ||                           *
 *   ||-- |    |       |    |       |    |  --||                           *
 *   || | |    |       |    |       |    |  | ||                           *
 *   ||-- |____|       |____|       |____|  --||                           *
 *   ||    ____         ____         ____     ||                           *
 *   ||-- |    |       |    |       |    |  --||                           *
 *   || | |    |       |    |       |    |  | ||                           *
 *   ||-- |____|       |____|       |____|  --||                           *
 *   ||    ____         ____         ____     ||                           *
 *   ||-- |    |       |    |       |    |  --||                           *
 *   || | |    |       |    |       |    |  | ||                           *
 *   ||-- |____|       |____|       |____|  --||                           *
 *   ||                                       ||                           *
 *   ||=======================================||                           *
 *      ||         ||           ||         ||                              *
 *      ||         ||           ||         ||                              *
 *      ||         ||           ||         ||                              *
 *      ||         ||           ||         ||                              *
 *      ||         ||           ||         ||                              *
 *      ||         ||           ||         ||                              *
 *     1Pin       2Pin         3Pin       4Pin                             *
 *                                                                         *
 *   -------------------                                                   *
 * ||*** PINS LEGEND ***||                                                 *
 *   -------------------                                                   *
 *                                                                         *
 * The following list displays all the DHT22 pins with the project         *
 * component which is using each one:                                      *
 *                                                                         *
 *      1Pin    =   VDD                                                    *
 *      2Pin    =   DATA                                                   *
 *      3Pin    =   Unconnected                                            *
 *      4Pin    =   GND                                                    *
 *                                                                         *
 *   -------------------------------------                                 *
 * ||*** Electrical Connection Diagram ***||                               *
 *   -------------------------------------                                 *
 *                                                                         *
 *                                                                         *
 *  _____________________        VDD               VDD                     *
 * |                     |       ___               ___       ___________   *
 * |                     |        |                 |       |           |  *
 * |                     |       |_| 5.1KΩ          |_1Pin__|           |  *
 * |                     |        |                         |           |  *
 * |                     |        |      DATA       2Pin    |   DHT22   |  *
 * |         MCU         | <------------------------------->|           |  *
 * |                     |                           _4Pin__|           |  *
 * |                     |                          |       |           |  *
 * |                     |                          |       |___________|  *
 * |                     |                         ---                     *
 * |                     |                         GND                     *
 * |_____________________|                                                 *
 *                                                                         *
 ***************************************************************************/
/*                                                                         *
 * DHT22 functions:                                                        *
 *      -dht_init_communication()                                          *
 *      -dht_read_data()                                                   *
 *      -dht_checksum()                                                    *
 *      -dht_close_communication()                                         *
 *                                                                         *
 ***************************************************************************/

#include <xbee_config.h>
#include <types.h>
#include <dht.h>


ssize_t dht_init_communication()
{
    bool_t ret;
    uint8_t delay;
    // *** Host sends start signal *** //

    // Host pull low //
    gpio_config(XPIN_1_WIRE_BUS, GPIO_CFG_OUTPUT);
    gpio_set(XPIN_1_WIRE_BUS, 0);

    // Delay 1ms //
    sys_udelay(250);
    sys_udelay(250);
    sys_udelay(250);
    sys_udelay(250);

    // Host pulls up //
    gpio_config(XPIN_1_WIRE_BUS, GPIO_CFG_OUTPUT);
    gpio_set(XPIN_1_WIRE_BUS, 1);

    // Delay 30us //
    sys_udelay(30);

    //sys_watchdog_reset();

    // *** Host waits for DHT22 response *** //
    gpio_config(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
    // Check if Sensor pulled low //
    if (gpio_get(XPIN_1_WIRE_BUS))
        return -ENODEV;
    else
    {
        // Wait minimum delay //
        delay = 75;
        sys_udelay(delay);

        while(1)
        {
            // Now check if Sensor pulled up //
            gpio_config(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
            if (!gpio_get(XPIN_1_WIRE_BUS))
            {
                delay += 1;
                if (delay > 85)
                {
                    printf("TIMEOUT 1\n");
                    return -ETIMEDOUT;
                }
                else
                {
                    sys_udelay(1);
                }
            }
            else
            {
                // Sensor pulled up //
                break; 
            }
        }

        //sys_watchdog_reset();

        // Wait minimum delay //
        delay = 75;
        sys_udelay(delay);

        while (1)
        {
            // Now check if Sensor pulled down to start data transmission //
            ret = gpio_get(XPIN_1_WIRE_BUS); 
            if (ret)
            {
                delay += 1;
                if (delay > 85)
                {
                    printf("TIMEOUT 2\n");
                    return -ETIMEDOUT;
                }
                else
                {
                    sys_udelay(1);
                }
            }
            else
            {
                // Sensor pulled down to start data transmission //
                break;
            }
        }
        //printf("ret = %d\n", ret);
    }
    return 0;
}

ssize_t dht_read_data(uint8_t *rxbuf)
{
    ssize_t i;
    uint8_t j;
    // maximum time interval for the whole data transmission happens //
    // when all the data are '1'. So the maximum time for 1-bit of   //
    // data '1' is: 55us + 75us = 130us. So the total 40-bit data    //
    // need 5200us = 5.2ms. This function takes data samples every   //
    // 20us. So by the end of the whole process we will have 260     //
    // samples.                                                      //
    bool_t samples[260];
    bool_t data[40];
    uint8_t mask;
    uint8_t *rxbyte;

    // Host collect all the samples of the communication //
    i = 0;
    while (i < 260)
    {
        samples[i] = gpio_get(XPIN_1_WIRE_BUS);
        i++;
        sys_udelay(20);
    }
    
    // for (i = 0; i < 260; i++)
    // {
    //     printf("%d", samples[i]); 
    // }
    // printf("\n");
    sys_watchdog_reset();

    // Host decodes the samples to take the sensor's data //
    for (j = 0; j < 40; j++)
    {
    	data[i] = 0;
    }
    
    j = 0;
    i = 0;

    while(i < 260)
    {
        sys_watchdog_reset();
        printf("sample[%d] = %d | j = %d\n", i, samples[i], j);
        if (samples[i] == 0)
        {
            i++;
        }
        else // sample[i] == 1 //
        {
            if (samples[i - 1] == 1)
            {
                // This sample is part of the previous one //
                i++;
                continue;
            }
            if (samples[i + 1] == 0 || (samples[i + 1] == 1 && samples[i + 2] == 0))
            {
                // data = 0 //
                data[j] = 0;
                //printf("%d", data[j]);
                i++;
            }
            else if (samples[i + 1] == 1 && samples[i + 2] == 1 && (samples[i + 3] == 0 || (samples[i + 3] == 1 && samples[i + 4] == 0)))
            {
                // data = 1 //
                data[j] = 1;
                //printf("%d", data[j]);
                i++;
            }
            else{
                i++;
                continue;
            }
            j++;
        }
    }
    printf("\n");

    for (j = 0; j < 40; j++)
    {
        printf("data[%d] = %d\n", j, data[j]);
    }

    for (i = 0; i < 5; i++)
    {
        rxbyte = rxbuf + i;
        *rxbyte = 0;
        mask = 128;
        for (j = 0; j < 8; j++)
        {
            if (data[i * 8 + j])
            {
                *rxbyte |= mask;
            }
            mask >>= 1;
        }
    }
     
     for (i = 0; i < 5; i++)
     {
    	 printf("%d\n", rxbuf[i]);
     }

    return 0;
}

ssize_t dht_checksum(uint8_t *data)
{
    uint8_t humidity_high = *data;
    uint8_t humidity_low = *(data + 1);
    uint8_t temperature_high = *(data + 2);
    uint8_t temperature_low = *(data + 3);
    uint8_t parity = *(data + 4);
    uint8_t sum;
    uint16_t humidity, humidity_high_shifted;
    int16_t temperature;
    uint16_t temperature_high_shifted;

    // Translate the Humidity and Temperature //
    humidity_high_shifted = humidity_high;
    humidity = ((humidity_high_shifted << 8) | humidity_low) / 10;
    printf("Humidity: %d RH\n", humidity);

    temperature_high_shifted = temperature_high;
    temperature = ((temperature_high_shifted << 8) | temperature_low) / 10;
    printf("Temperature: %d oC\n", temperature);

    sum = humidity_high  + humidity_low + temperature_high + temperature_low;

    printf("sum = %u,  parity = %u\n", sum, parity);

    // Returns "1" if sum == parity, "0" otherwise //
    return (sum == parity);
}