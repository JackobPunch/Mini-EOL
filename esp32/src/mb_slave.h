#ifndef MB_SLAVE_H
#define MB_SLAVE_H

#include "esp_err.h"
#include "esp_netif.h"

esp_err_t slave_init(esp_netif_t *sta_netif);
void slave_run(void);
esp_err_t slave_destroy(void);

#endif