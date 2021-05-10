// Copyright (C) 2021 Nuke.YKT
// License: GPLv2+
#include <stdio.h>
#include <string.h>
#include "ympsg.h"

static void YMPSG_WriteInternal(ympsg_t *chip)
{
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
    memset(chip, 0, sizeof(ympsg_t));
}

void YMPSG_Reset(ympsg_t *chip)
{

}

void YMPSG_Clock(ympsg_t *chip)
{

}

void YMPSG_ClockFast(ympsg_t *chip)
{
    // TODO
}

