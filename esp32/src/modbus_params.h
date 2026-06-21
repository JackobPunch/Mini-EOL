#ifndef MODBUS_PARAMS_H
#define MODBUS_PARAMS_H

#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t relay_ctrl;
    uint16_t pwm_freq_hz;
    uint16_t pwm_duty_pct;
} holding_reg_params_t;
#pragma pack(pop)

extern holding_reg_params_t holding_reg_params;

#endif
