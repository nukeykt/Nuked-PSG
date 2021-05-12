// Copyright (C) 2021 Nuke.YKT
// License: GPLv2+
#ifndef _YMPSG_H_
#define _YMPSG_H_
#include <stdint.h>

typedef struct {
    // IO
    uint8_t data;
    uint8_t latch;
    uint8_t write_flag;
    uint8_t write_flag_l;

    uint8_t prescaler_1;
    uint8_t prescaler_2;
    uint8_t prescaler_2_l;
    uint8_t reset_latch;
    uint8_t ic;
    uint8_t ic_latch1;
    uint8_t ic_latch2;
    uint8_t data_mask;
    uint8_t reg_reset;
    uint8_t volume[4];
    uint16_t freq[3];
    uint8_t noise_data;
    uint8_t noise_of;
    uint8_t noise_trig;
    uint8_t noise_trig_l;
    uint8_t rot;

    uint8_t chan_sel;

    uint16_t counter[4];
    uint8_t counter_of;
    uint8_t sign;
    uint8_t sign_l;
    uint8_t noise_sign_l;
    uint16_t noise;
    uint8_t test;
    uint8_t volume_out[4];
} ympsg_t;

#endif
