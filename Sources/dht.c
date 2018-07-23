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


ssize_t dht_init_communication(uint8_t rom)
{

    // *** Host sends start signal *** //

        // Host pull low //
        gpio_set(XPIN_1_WIRE_BUS, 0);
        gpio_config(XPIN_1_WIRE_BUS, GPIO_CFG_OUTPUT);

        // Delay 1ms //
        sys_udelay(250);
        sys_udelay(250);
        sys_udalay(250);
        sys_udelay(250);

        // Host pulls up //
        gpio_set(XPIN_1_WIRE_BUS, 1);
        gpio_config(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);

        // Delay 30us //
        sys_udelay(30);

    // *** Host waits for DHT22 response *** //

        // Check if Sensor pulled low //
        if (gpio_get(XPIN_1_WIRE_BUS))
            return -ENODEV;
        else
        {
            // Delay 80us //
            sys_udelay(80);

            // Now check if Sensor pulled up //
            if (!gpio_get(XPIN_1_WIRE_BUS))
                // Bus has been too long at low level, probably timed out. //
                return -ETIMEDOUT;
            else
                // Handshake made correctly //
                return 0;
        }
}

ssize_t dht_read_data(uint8_t *rxbuf)
{
    uint8_t i;
    uint8_t j, mask;
    uint8_t *rxbyte;
    ssize_t ret;
    ssize_t data = 0;

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
        mask = 0x01;

        gpio_config_dir(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
        for (j = 0; j < 8; j++)
        {
            // *** Starts transmittion of 1 bit data. Delay 50us ***//
            sys_udelay(50);

            // *** Sensor outputs 1 bit data *** //
            ret = gpio_get(XPIN_1_WIRE_BUS);
            if (ret)
            {
                // Depending of the timing it can be "0" or "1". //
                sys_udelay(26);
                ret = gpio_get(XPIN_1_WIRE_BUS);
                if (ret)
                {
                    // Data = "1" //
                    data = 1;
                    sys_udelay(44);
                }
                else
                {
                    data = 0;
                    // Data = "0" //
                }

                // Store data to the appropriate bit //
                if (data == 1)
                {
                    // Store "1" into current bit //
                    *rxbyte |= mask;
                }

                // Point to the next bit //
                mask <<= 1;
            }
        }
    }

    // Host pulls up. Wait for new transmission. //
    gpio_set(XPIN_1_WIRE_BUS, 1);
    gpio_config(XPIN_1_WIRE_BUS, GPIO_CFG_INPUT);
}

ssize_t dht_checksum()
{
    

    return ret;
}


  