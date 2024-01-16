#ifndef TCS34725_H
#define TCS34725_H

#include "pico/stdlib.h"

typedef struct
{
    i2c_inst_t *i2c_instance;
    uint8_t addr;
} TCS34725;

typedef struct
{
    uint16_t clear;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} SensorData;

#define TCS_BUS_ADDR 0x29

#define TCS_COMMAND 0x80

#define TCS_TYPE_AUTOINCR 0x20

#define TCS_REG_ENABLE 0x00
#define TCS_REG_ENABLE_PON 0x01
#define TCS_REG_ENABLE_AEN 0x02
#define TCS_REG_ENABLE_WEN 0x08
#define TCS_REG_ENABLE_AIEN 0x10

#define TCS_REG_ATIME 0x01
#define TCS_ATIME_CYCLES_1 0xFF
#define TCS_ATIME_CYCLES_10 0xF6
#define TCS_ATIME_CYCLES_42 0xD5
#define TCS_ATIME_CYCLES_64 0xC0
#define TCS_ATIME_CYCLES_256 0x00

#define TCS_REG_WTIME 0x03

#define TCS_REG_AILTL 0x04

#define TCS_REG_AILTH 0x05

#define TCS_REG_AIHTL 0x06

#define TCS_REG_AIHTH 0x07

#define TCS_REG_PERS 0x0C

#define TCS_REG_CONFIG 0x0D
#define TCS_CONFIG_WLONG 0x02

#define TCS_REG_CONTROLL 0x0F
#define TCS_RGBC_GAIN_X1 0x00
#define TCS_RGBC_GAIN_X4 0x01
#define TCS_RGBC_GAIN_X16 0x02
#define TCS_RGBC_GAIN_X60 0x03

#define TCS_REG_ID 0x12
#define TCS_REG_STATUS 0x13

#define TCS_REG_CDATAL 0x14
#define TCS_REG_CDATAH 0x15

#define TCS_REG_RDATAL 0x16
#define TCS_REG_RDATAH 0x17

#define TCS_REG_GDATAL 0x18
#define TCS_REG_GDATAH 0x19

#define TCS_REG_BDATAL 0x1A
#define TCS_REG_BDATAH 0x1B

#define TCS_34725_R_COEF 0.136
#define TCS_34725_G_COEF 1.000
#define TCS_34725_B_COEF -0.444
#define TCS_34725_CT_COEF 3810
#define TCS_34725_CT_OFFSET 1391
#define TCS_34725_DF 310

TCS34725 tcs34725_new(i2c_inst_t *, int);

int tcs34725_enable(TCS34725 *);

int tcs34725_set_gain(TCS34725 *, uint8_t);
int tcs34725_set_atime(TCS34725 *, uint8_t);
int tcs34725_set_wtime(TCS34725 *, uint8_t);

int tcs34725_wait_enable(TCS34725 *);
int tcs34725_wait_disable(TCS34725 *);

int tcs34725_interrupt_enable(TCS34725 *);
int tcs34725_interrupt_disable(TCS34725 *);

int tcs34725_wlong_enable(TCS34725 *);
int tcs34725_wlong_disable(TCS34725 *);

int tcs34725_clear_interrupt(TCS34725 *);

int tcs34725_read_clear(TCS34725 *, uint16_t *);
int tcs34725_read_red(TCS34725 *, uint16_t *);
int tcs34725_read_green(TCS34725 *, uint16_t *);
int tcs34725_read_blue(TCS34725 *, uint16_t *);

int tcs34725_read_all(TCS34725 *, SensorData *);

uint8_t tcs34725_calculate_time(int);
int tcs_calculate_ir_component(SensorData *);
int tcs_calculate_ct(SensorData *);
SensorData tcs_remove_ir_component(SensorData *);
#endif