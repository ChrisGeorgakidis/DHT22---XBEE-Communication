/* *** DHT Communication API *** */
/* This file contains the headers of the dht.c API */

#include <types.h>
#include <hardware.h>

ssize_t dht_init_communication(void);
ssize_t dht_read_data(uint8_t *rxbuf);
ssize_t dht_checksum(uint8_t *data);

