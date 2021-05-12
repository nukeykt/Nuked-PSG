// Copyright (C) 2021 Nuke.YKT
// License: GPLv2+
#include <stdio.h>
#include <string.h>
#include "ympsg.h"

static void YMPSG_WriteLatch(ympsg_t *chip)
{
    uint8_t data = chip->data;
    if (chip->data_mask)
    {
        data = 0;
    }
    if (data & 128)
    {
        chip->latch = (data >> 4) & 7;
    }
}

static void YMPSG_UpdateRegisters(ympsg_t* chip)
{
    uint8_t data = chip->data;
    if (chip->data_mask)
    {
        data = 0;
    }
    if (chip->reg_reset || (chip->write_flag_l && chip->latch == 1))
    {
        chip->volume[0] = data & 15;
        if (chip->data_mask)
        {
            chip->volume[0] = 15;
        }
    }
    if (chip->reg_reset || (chip->write_flag_l && chip->latch == 3))
    {
        chip->volume[1] = data & 15;
        if (chip->data_mask)
        {
            chip->volume[1] = 15;
        }
    }
    if (chip->reg_reset || (chip->write_flag_l && chip->latch == 4))
    {
        if ((data & 128) || chip->reg_reset)
        {
            chip->freq[2] &= 1008;
            chip->freq[2] |= data & 15;
        }
        if (!(data & 128))
        {
            chip->freq[2] &= 15;
            chip->freq[2] |= (data << 4) & 1008;
        }
    }
    if (chip->reg_reset || (chip->write_flag_l && chip->latch == 2))
    {
        if ((data & 128) || chip->reg_reset)
        {
            chip->freq[1] &= 1008;
            chip->freq[1] |= data & 15;
        }
        if (!(data & 128))
        {
            chip->freq[1] &= 15;
            chip->freq[1] |= (data << 4) & 1008;
        }
    }
    if (chip->reg_reset || (chip->write_flag_l && chip->latch == 5))
    {
        chip->volume[2] = data & 15;
        if (chip->data_mask)
        {
            chip->volume[2] = 15;
        }
    }
    if (chip->reg_reset || (chip->write_flag_l && chip->latch == 0))
    {
        if ((data & 128) || chip->reg_reset)
        {
            chip->freq[0] &= 1008;
            chip->freq[0] |= data & 15;
        }
        if (!(data & 128))
        {
            chip->freq[0] &= 15;
            chip->freq[0] |= (data << 4) & 1008;
        }
    }
    if (chip->reg_reset || (chip->write_flag_l && chip->latch == 7))
    {
        chip->volume[3] = data & 15;
        if (chip->data_mask)
        {
            chip->volume[3] = 15;
        }
    }
    if (chip->reg_reset || (chip->write_flag_l && chip->latch == 6))
    {
        chip->noise_data = data & 7;
        chip->noise_trig = 1;
    }
}

static void YMPSG_ClockInternal1(ympsg_t *chip)
{
    uint16_t freq = 0;
    uint8_t chan_sel = chip->chan_sel;
    if (chip->ic_latch2 & 2)
    {
        chan_sel = 0;
    }
    if (chip->chan_sel & 1)
    {
        freq |= chip->freq[0];
    }
    if (chip->chan_sel & 2)
    {
        freq |= chip->freq[1];
    }
    if (chip->chan_sel & 4)
    {
        freq |= chip->freq[2];
    }
    if (chip->chan_sel & 1)
    {
        chip->sign ^= chip->counter_of & 15;
    }
    if (chip->ic_latch2 & 2)
    {
        chip->sign = 0;
    }
    chip->counter_of <<= 1;
    if (chip->counter[chip->rot] >= freq)
    {
        chip->counter_of |= 1;
        chip->counter[chip->rot] = 0;
    }
    if (chip->ic_latch2 & 2)
    {
        chip->counter[chip->rot] = 0;
    }
    chip->counter[chip->rot]++;
    chip->counter[chip->rot] &= 1023;
    if ((chip->ic_latch2 & 1) || (chip->chan_sel & 7) != 0)
    {
        chip->chan_sel <<= 1;
    }
    else
    {
        chip->chan_sel <<= 1;
        chip->chan_sel |= 1;
    }
    chip->ic_latch2 <<= 1;
    chip->ic_latch2 |= chip->ic & 1;
    chip->noise_trig_l = chip->noise_trig;
}

static void YMPSG_ClockInternal2(ympsg_t *chip)
{
    chip->data_mask = (chip->ic_latch2 >> 1) & 1;
    chip->reg_reset = (chip->ic_latch2 >> 0) & 1;
    if (chip->noise_trig_l)
    {
        chip->noise_trig = 0;
    }
    YMPSG_UpdateRegisters(chip);

    chip->rot = (chip->rot + 1) & 3;
}

void YMPSG_Write(ympsg_t *chip, uint8_t data)
{
    chip->data = data;
    chip->write_flag = 1;
}

void YMPSG_WriteImmediate(ympsg_t *chip, uint8_t data)
{
    // TODO
    //chip->data = data;
    //YMPSG_WriteInternal(chip);
}

void YMPSG_Init(ympsg_t *chip)
{
    uint32_t i;
    memset(chip, 0, sizeof(ympsg_t));
    YMPSG_SetIC(chip, 1);
    for (i = 0; i < 16; i++)
    {
        YMPSG_Clock(chip);
    }
    YMPSG_SetIC(chip, 0);
}

void YMPSG_SetIC(ympsg_t *chip, uint32_t ic)
{
    chip->ic = (uint8_t)ic;
}

void YMPSG_Clock(ympsg_t *chip)
{
    uint8_t prescaler2_latch;
    prescaler2_latch = chip->prescaler_2;
    chip->prescaler_2 += chip->prescaler_1;
    chip->prescaler_2 &= 1;
    chip->prescaler_1 ^= 1;
    if ((chip->ic_latch1 & 3) == 2)
    {
        chip->prescaler_1 = 0;
        chip->prescaler_2 = 0;
    }
    chip->ic_latch1 <<= 1;
    chip->ic_latch1 |= chip->ic & 1;
    YMPSG_UpdateRegisters(chip);
    chip->write_flag_l = 0;
    if (chip->write_flag)
    {
        YMPSG_WriteLatch(chip);
        chip->write_flag = 0;
        chip->write_flag_l = 1;
    }
    if (chip->prescaler_1)
    {
        if (!prescaler2_latch)
        {
            YMPSG_ClockInternal1(chip);
        }
        else
        {
            YMPSG_ClockInternal2(chip);
        }
    }
}

float YMPSG_GetOutput(ympsg_t *chip)
{
    return 0.f;
}

void YMPSG_ClockFast(ympsg_t *chip)
{
    // TODO
}

