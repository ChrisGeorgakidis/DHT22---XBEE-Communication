/***** XBEE APPLICATION PROJECT *****
 * 
 * Auto-generated header with information about the 
 * relation between the XBee module pins and the 
 * project components.
 * 
 ************ XBEE LAYOUT ***********
 * 
 * This layout represents the XBee 865/868LP DigiMesh (S8) module 
 * selected for the project with its pin distribution:
 *            ________________________
 *           |                        |- XPIN37
 *           |                ____    |- XPIN36
 *           |              //    \\  |- XPIN35
 *   XPIN1  -|             ||      || |- XPIN34
 *   XPIN2  -|              \\____//  |- XPIN33
 *   XPIN3  -|                        |- XPIN32
 *   XPIN4  -| =====================  |- XPIN31
 *   XPIN5  -| #   # ####  #### ####  |- XPIN30
 *   XPIN6  -|  # #  #   # #    #     |- XPIN29
 *   XPIN7  -|   #   ####  ###  ###   |- XPIN28
 *   XPIN8  -|  # #  #   # #    #     |- XPIN27
 *   XPIN9  -| #   # ####  #### ####  |- XPIN26
 *   XPIN10 -| =====================  |- XPIN25
 *   XPIN11 -|                        |- XPIN24
 *   XPIN12 -|                        |- XPIN23
 *   XPIN13 -|                        |- XPIN22
 *           |________________________|
 *             |  |  |  |  |  |  |  |
 *             |  |  |  |  |  |  |  XPIN21
 *             |  |  |  |  |  |  XPIN20
 *             |  |  |  |  |  XPIN19
 *             |  |  |  |  XPIN18
 *             |  |  |  XPIN17
 *             |  |  XPIN16
 *             |  XPIN15
 *             XPIN14
 * 
 ************ PINS LEGEND ***********
 * 
 * The following list displays all the XBee Module pins 
 * with the project component which is using each one:
 * 
 *   XPIN1 = GND
 *   XPIN2 = VCC
 *   XPIN3 = uart0 [TX Pin]
 *   XPIN4 = uart0 [RX Pin]
 *   XPIN5 = <<UNUSED>>
 *   XPIN6 = special0 [Reset Pin]
 *   XPIN7 = special0 [RSSI PWM Pin]
 *   XPIN8 = <<UNUSED>>
 *   XPIN9 = special0 [BKGD Pin]
 *   XPIN10 = <<UNUSED>>
 *   XPIN11 = GND
 *   XPIN12 = <<UNUSED>>
 *   XPIN13 = GND
 *   XPIN14 = <<UNUSED>>
 *   XPIN15 = <<UNUSED>>
 *   XPIN16 = <<UNUSED>>
 *   XPIN17 = <<UNUSED>>
 *   XPIN18 = <<UNUSED>>
 *   XPIN19 = <<UNUSED>>
 *   XPIN20 = <<UNUSED>>
 *   XPIN21 = Do not Connect
 *   XPIN22 = GND
 *   XPIN23 = Do not Connect
 *   XPIN24 = <<UNUSED>>
 *   XPIN25 = <<UNUSED>>
 *   XPIN26 = <<UNUSED>>
 *   XPIN27 = VCC REF
 *   XPIN28 = special0 [Association Pin]
 *   XPIN29 = <<UNUSED>>
 *   XPIN30 = <<UNUSED>>
 *   XPIN31 = <<UNUSED>>
 *   XPIN32 = <<UNUSED>>
 *   XPIN33 = special0 [Commissioning Pin]
 *   XPIN34 = Do not Connect
 *   XPIN35 = GND
 *   XPIN36 = <<UNUSED>>
 *   XPIN37 = Do not Connect
 *
 ************************************/

#include <xbee_config.h>
#include <types.h>

ssize_t dht22_search(uint8_t *rom_search)
{
	#ifndef SINGLE_DEVICE_ON_ONEWIRE_BUS
		ssize_t ret;
		uint8_t rom[3 * 8], i = 0;
		bool_t found = FALSE;

		// Search for maximum 3 devices on the 1-Wire bus. //
		ret = one_wire_search(rom, 3);
		if (ret < 0)
		{
			// An error occured while looking for devices //
			return ret;
		}
		else if (ret == 0)
		{
			// No devices found //
			return -ENODEV;
		}
		else
		{
			// Devices found //
		}
	#else
		ssize_t ret;
		uint8_t rom[8];

		ret = one_wire_read_rom(rom);
		if (ret < 0)
		{
			return ret;
		}
		
		memcpy(rom_search, rom, 8);

		return 0;

	#endif
}


void main(void)
{
	ssize_t ret; uint8_t rom_search[8];
	uint8_t *rom;
	uint8_t data[5];

	#ifndef SINGLE_DEVICE_ON_ONEWIRE_BUS
		rom = rom_search;
	#else
		rom = NULL;
	#endif
	
	sys_hw_init();
	sys_xbee_init();
	sys_app_banner();

	printf("--------------------------------------------------------\n");
	printf("------------------DHT22 Test application----------------\n");
	printf("--------------------------------------------------------\n");
	printf("-------System will automatically look for a DHT22-------\n");
	printf("--------------------------------------------------------\n");

	ret = dht22_search(rom_search);
	if (ret < 0)
	{
		if (ret == -ENODEV)
			printf("\nNo device found. You can perform a new search by pressing 'S' key\n");
		else
			printf("\nAn error has occurred. Error code: %d\n", ret);
	}
	else
	{
		printf("\nDHT22 found. ROM: %02X%02X%02X%02X%02X%02X%02X%02X\n",
			   rom_search[7], rom_search[6], rom_search[5], rom_search[4],
			   rom_search[3], rom_search[2], rom_search[1], rom_search[0]);

		for (;;)
		{

			// Starts the communication between the xbee and the DHT22 sensor. //
			ret = dht_init_communication();
			if (ret < 0)
			{
				printf("\nCommunication Initialisation:\t[ERROR]\n");
			}
			else
			{
				printf("\nCommunication Initialisation:\t[OK]\n");

				// Host reads data sent by DHT22 sensor. //
				ret = dht_read_data(data);
				if (ret < 0)
				{
					printf("Data Transmission:\t[ERROR]\n");
				}
				else
				{
					printf("Data Transmission:\t[OK]\n");
				}

				// Now has the whole data: Humidity + Temperature + Parity //
				// Now host must check if he received the data correctly //
				ret = dht_checksum(data);
				if (ret)
				{
					printf("Check Data:\t[OK]\n");
				}
				else
				{
					printf("Check Data:\t[ERROR]\n");
					printf("\t INFO	: Parity Error\n");
				}
			}

			sys_watchdog_reset();
			sys_xbee_tick();
		}
	}

	
}
