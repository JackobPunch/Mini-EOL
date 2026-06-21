#ifndef WIFI_H
#define WIFI_H

#include "esp_netif.h"

void wifi_init_sta(void);
esp_netif_t *wifi_get_netif(void);

#endif 