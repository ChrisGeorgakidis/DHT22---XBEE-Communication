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
                    delay += 5;
                    if (delay > 85)
                    {
                        printf("TIMEOUT 1\n");
                        return -ETIMEDOUT;
                    }
                    else
                    {
                        sys_udelay(5);
                    }
                }
                else
                {
                    // Sensor pulled up //
                    break; 
                }
            }

            // Wait minimum delay //
            delay = 75;
            sys_udelay(delay);

            while (1)
            {
                // Now check if Sensor pulled down to start data transmission //
                gpio_config(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
                if (gpio_get(XPIN_1_WIRE_BUS))
                {
                    delay += 5;
                    if (delay > 85)
                    {
                        printf("TIMEOUT 2\n");
                        return -ETIMEDOUT;
                    }
                    else
                    {
                        sys_udelay(5);
                    }
                }
                else
                {
                    // Sensor pulled down to start data transmission //
                    break;
                }
            }
        }
}

ssize_t dht_read_data(uint8_t *rxbuf)
{
    uint8_t i;
    uint8_t j, mask;
    uint8_t *rxbyte;
    ssize_t ret;
    ssize_t data = 0;
    uint8_t delay;

    // 2 Bytes for Humidity:            //  
    //      1st byte: Humidity High     //
    //      2nd byte: Humidity Low      // 
    // 2 Bytes for Temperature:         //
    //      1st byte: Temperature High  //
    //      2nd byte: Temperature Low   //
    // 1 Byte for Parity                //
    // Total = 5 Bytes = 40bits         //

    for (i = 0; i < 5; i++)
    {
        // Store data to the appropriate byte //
        rxbyte = rxbuf + i;
        *rxbyte = 0;

        // Needed to store each bit of data //
        mask = 128; // = 'b10000000 //

        //gpio_config_dir(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
        for (j = 0; j < 8; j++)
        {
            //sys_irqs_disable();
            data = 0;

            // *** Starts transmission of 1 bit data. Delay 50us *** //
            
            // Wait minimum delay //
            delay = 48;
            sys_udelay(delay);

            // Check if Sensor pulled up to send data //
            gpio_config_dir(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
            while (!gpio_get(XPIN_1_WIRE_BUS))
            {
                delay += 1;
                if (delay > 55)
                {
                    printf("TIMEOUT 3\n");
                    return -ETIMEDOUT;
                }
                else
                {
                    sys_udelay(1);
                }
            }

            // Sensor has pulled up to send data //

            // Wait minimum delay //
            delay = 22;
            sys_udelay(delay);

            // Check if Sensor pulled down //
            gpio_config_dir(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
            while (gpio_get(XPIN_1_WIRE_BUS))
            {
                delay += 1;
                
                if (delay > 30)
                {
                    sys_udelay(68 - delay);
                    data = 1;
                    delay = 68;
                }
                else if (delay > 75)
                {
                    printf("TIMEOUT 4\n");
                    return -ETIMEDOUT;
                }
                else
                {
                    sys_udelay(1);
                }
            }
                    
            printf("%u", data);

            if (data == 1)
            {
                *rxbyte |= mask;
                mask >>= 1;

                // gpio_config_dir(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
                // while (gpio_get(XPIN_1_WIRE_BUS))
                // {
                //     delay += 1;
                //     if (delay > 75)
                //     {
                //         printf("TIMEOUT 5\n");
                //         return -ETIMEDOUT;
                //     }
                //     else
                //     {
                //         sys_udelay(1);
                //     }
                // }
            }
            else // data == 0 //
            {
                mask >>= 1;

                // gpio_config_dir(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
                // while (gpio_get(XPIN_1_WIRE_BUS))
                // {
                //     delay += 1;
                //     if (delay > 30)
                //     {
                //         printf("TIMEOUT 6\n");
                //         return -ETIMEDOUT;
                //     }
                //     else
                //     {
                //         sys_udelay(1);
                //     }
                // }
            }
        }
    }


    //         // *** Starts transmission of 1 bit data. Delay 50us *** //
    //         gpio_config_dir(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
    //         ret = gpio_get(XPIN_1_WIRE_BUS);
    //         if (ret == 0)
    //         {
    //             sys_udelay(50);

    //             // *** Sensor outputs 1 bit data *** //
    //             gpio_config_dir(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
    //             ret = gpio_get(XPIN_1_WIRE_BUS);
    //             if (ret)
    //             {
    //                 // Depending of the timing it can be "0" or "1". //
    //                 sys_udelay(26);
    //                 gpio_config_dir(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
    //                 ret = gpio_get(XPIN_1_WIRE_BUS);
    //                 if (ret)
    //                 {
    //                     // Data = "1" //
    //                     data = 1;
    //                     printf("1");
    //                     // Store "1" into current bit //
    //                     *rxbyte |= mask;
    //                     sys_udelay(44);
                        
    //                 }
    //                 else
    //                 {
    //                     data = 0;
    //                     printf("0");
    //                     // Data = "0" //
    //                 }
                    
    //                 // Point to the next bit //
    //                 mask = mask >> 1;
    //             }
    //             else
    //             {
    //                 printf("TIMEOUT 1\n");
    //                 return -ETIMEDOUT;
    //             }

    //             //sys_irqs_enable();
    //         }
    //         else
    //         {
    //             printf("TIMEOUT 2\n");
    //             return -ETIMEDOUT;
    //         }
    //    }
    // }

    printf("\n");
    // Host pulls up. Wait for new transmission. //
    gpio_config(XPIN_1_WIRE_BUS, GPIO_CFG_OUTPUT);
    gpio_set(XPIN_1_WIRE_BUS, 1);

    return 0;
}

ssize_t dht_checksum(uint8_t *data)
{
    uint8_t humidity_high = data[0];
    uint8_t humidity_low = data[1];
    uint8_t temperature_high = data[2];
    uint8_t temperature_low = data[3];
    uint8_t parity = data[4];

    uint8_t sum = humidity_high + humidity_low + temperature_high + temperature_low;

    printf("sum = %u,  parity = %u\n", sum, parity);

    // Returns "1" if sum == parity, "0" otherwise //
    return (sum == parity);
}


  