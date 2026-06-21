#include "mb_slave.h"
#include "modbus_params.h"
#include "mbcontroller.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h"

#define MB_READ_MASK  (MB_EVENT_INPUT_REG_RD \
                        | MB_EVENT_HOLDING_REG_RD \
                        | MB_EVENT_DISCRETE_RD \
                        | MB_EVENT_COILS_RD)
#define MB_WRITE_MASK (MB_EVENT_HOLDING_REG_WR \
                        | MB_EVENT_COILS_WR)
#define MB_READ_WRITE_MASK (MB_READ_MASK | MB_WRITE_MASK)

holding_reg_params_t holding_reg_params = {0};

static void *slave_handle = NULL;

static const char *TAG = "mb_slave";

esp_err_t slave_init(esp_netif_t *sta_netif){

    mb_communication_info_t comm_info = {
        .tcp_opts.port = 502,
        .tcp_opts.mode = MB_TCP,
        .tcp_opts.addr_type = MB_IPV4,
        .tcp_opts.ip_addr_table = NULL,
        .tcp_opts.ip_netif_ptr = (void *)sta_netif,
        .tcp_opts.uid = 1
    };

    esp_err_t err = mbc_slave_create_tcp(&comm_info, &slave_handle);
    if (err != ESP_OK || slave_handle == NULL) {
        ESP_LOGE(TAG, "mbc_slave_create_tcp failed: 0x%x", err);
        return ESP_ERR_INVALID_STATE;
    }

    mb_register_area_descriptor_t reg_area = {0};
    reg_area.type = MB_PARAM_HOLDING;
    reg_area.start_offset = 0;  
    reg_area.address = (void *)&holding_reg_params;
    reg_area.size = sizeof(holding_reg_params);
    reg_area.access = MB_ACCESS_RW;
    err = mbc_slave_set_descriptor(slave_handle, reg_area);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mbc_slave_set_descriptor failed: 0x%x", err);
        return ESP_ERR_INVALID_STATE;
    }

    err = mbc_slave_start(slave_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mbc_slave_start failed: 0x%x", err);
        return ESP_ERR_INVALID_STATE;
    }
    vTaskDelay(5);
    return ESP_OK;
}

void slave_run(void){

    mb_param_info_t reg_info;

    ESP_LOGI(TAG, "Modbus slave started");

    while (1) {
        mbc_slave_check_event(slave_handle, MB_EVENT_HOLDING_REG_WR);
        ESP_ERROR_CHECK_WITHOUT_ABORT(mbc_slave_get_param_info(slave_handle, &reg_info, 10));

        if (reg_info.type & MB_EVENT_HOLDING_REG_WR) {
            if (reg_info.address == (uint8_t *)&holding_reg_params.relay_ctrl) {
                // TODO: apply relay_ctrl to GPIO
                ESP_LOGI(TAG, "RELAY_CTRL = %u", holding_reg_params.relay_ctrl);
            } else if (reg_info.address == (uint8_t *)&holding_reg_params.pwm_freq_hz) {
                // TODO: apply pwm_freq_hz to PWM peripheral
                ESP_LOGI(TAG, "PWM_FREQ_HZ = %u", holding_reg_params.pwm_freq_hz);
            } else if (reg_info.address == (uint8_t *)&holding_reg_params.pwm_duty_pct) {
                // TODO: apply pwm_duty_pct to PWM peripheral
                ESP_LOGI(TAG, "PWM_DUTY_PCT = %u", holding_reg_params.pwm_duty_pct);
            }
        }
    }
}

esp_err_t slave_destroy(void)
{
    esp_err_t err = mbc_slave_delete(slave_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mbc_slave_delete failed: 0x%x", err);
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}


