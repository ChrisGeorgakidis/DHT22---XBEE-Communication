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

        sys_watchdog_reset();

    // // *** Host waits for DHT22 response *** //
    //     gpio_config(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
    //     // Check if Sensor pulled low //
    //     if (gpio_get(XPIN_1_WIRE_BUS))
    //         return -ENODEV;
    //     else
    //     {
    //         // Wait minimum delay //
    //         delay = 75;
    //         sys_udelay(delay);

    //         while(1)
    //         {
    //             // Now check if Sensor pulled up //
    //             gpio_config(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
    //             if (!gpio_get(XPIN_1_WIRE_BUS))
    //             {
    //                 delay += 5;
    //                 if (delay > 85)
    //                 {
    //                     printf("TIMEOUT 1\n");
    //                     return -ETIMEDOUT;
    //                 }
    //                 else
    //                 {
    //                     sys_udelay(5);
    //                 }
    //             }
    //             else
    //             {
    //                 // Sensor pulled up //
    //                 break; 
    //             }
    //         }

    //         sys_watchdog_reset();

    //         // Wait minimum delay //
    //         delay = 75;
    //         sys_udelay(delay);

    //         while (1)
    //         {
    //             // Now check if Sensor pulled down to start data transmission //
    //             gpio_config(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
    //             if (gpio_get(XPIN_1_WIRE_BUS))
    //             {
    //                 delay += 5;
    //                 if (delay > 85)
    //                 {
    //                     printf("TIMEOUT 2\n");
    //                     return -ETIMEDOUT;
    //                 }
    //                 else
    //                 {
    //                     sys_udelay(5);
    //                 }
    //             }
    //             else
    //             {
    //                 // Sensor pulled down to start data transmission //
    //                 break;
    //             }
    //         }
    //     }
    return 0;
}

ssize_t dht_read_data(uint8_t *rxbuf)
{
    uint8_t i, j;
    uint8_t samples[100000];
    uint8_t data[40];
    uint8_t mask = 128;

    rxbuf[0] = 0;
    rxbuf[1] = 0;
    rxbuf[2] = 0;
    rxbuf[3] = 0;
    rxbuf[4] = 0;
 
    // Host collect all the samples of the communication //
    printf("sampling...\n");
    i = 0;
    while (i < 100000)
    {
        gpio_config_dir(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
        samples[i] = gpio_get(XPIN_1_WIRE_BUS);
        printf("%d", samples[i]);
        i++;
        sys_watchdog_reset();
        sys_udelay(20);
    }

    // Host decodes the samples to take the sensor's data //
    j = 0;
    while(i < 100000)
    {
        sys_watchdog_reset();
        if (samples[i] == 0)
        {
            i++;
        }
        else // sample[i] == 1 //
        {
            if (samples[i + 1] == 0 || (samples[i + 1] == 1 && samples[i + 2] == 0))
            {
                // data = 0 //
                data[j] = 0;
                i = (samples[i + 1] == 0) ? (i + 1) : (i + 2);
            }
            else if (samples[i + 1] == 1 && samples[i + 2] == 1 && (samples[i + 3] == 0 || (samples[i + 3] == 1 && samples[i + 4] == 0)))
            {
                // data = 1 //
                data[j] = 1;
                i = (samples[i + 3] == 0) ? (i + 3) : (i + 4);
            }
            printf("%d", data[j]);
            j++;
        }
    }
    printf("\n");

    // Decode the data //
    for (j = 0; j < 40; j++)
    {
        i = j / 8;
        if (data[j] == 1)
        {
            rxbuf[i] |= mask;
        }
        mask >>= 1;
    }


    return 0;
}

ssize_t dht_checksum(uint8_t *data)
{
    uint8_t humidity_high = data[0];
    uint8_t humidity_low = data[1];
    uint8_t temperature_high = data[2];
    uint8_t temperature_low = data[3];
    uint8_t parity = data[4];

    uint8_t sum = humidity_high  + humidity_low + temperature_high + temperature_low;

    printf("sum = %u,  parity = %u\n", sum, parity);

    // Returns "1" if sum == parity, "0" otherwise //
    return (sum == parity);
}


  