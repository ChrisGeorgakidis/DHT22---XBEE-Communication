/* *** DHT Communication API *** */
/* This file contains the headers of the dht.c API */

#include <types.h>;

ssize_t dht_init_communication();
ssize_t dht_read_data();
ssize_t dht_read_temperature();
ssize_t dht_read_humidity();
ssize_t dht_checksum();
ssize_t dht_close_communication();