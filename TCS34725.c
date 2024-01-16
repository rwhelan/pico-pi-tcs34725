#include <stdio.h>

#include "hardware/i2c.h"

#include "TCS34725.h"

int __read_8(TCS34725 *dev, uint8_t reg, uint8_t *buff)
{
    int ret;
    uint8_t reg_cmd = TCS_COMMAND | reg;

    ret = i2c_write_blocking(dev->i2c_instance, dev->addr, &reg_cmd, 1, false);
    if (ret < 0)
        return ret;

    ret = i2c_read_blocking(dev->i2c_instance, dev->addr, buff, 1, true);
    if (ret < 0)
        return ret;

    return 0;
}

int __read_16(TCS34725 *dev, uint8_t reg, uint8_t *buff)
{
    int ret;
    uint8_t reg_cmd = TCS_COMMAND | reg;

    ret = i2c_write_blocking(dev->i2c_instance, dev->addr, &reg_cmd, 1, false);
    if (ret < 0)
        return ret;

    ret = i2c_read_blocking(dev->i2c_instance, dev->addr, buff, 2, true);
    if (ret < 0)
        return ret;

    return 0;
}

int __write_8(TCS34725 *dev, uint8_t cmd_reg, uint8_t value)
{
    uint8_t reg_cmd[] = {TCS_COMMAND | cmd_reg, value};

    return i2c_write_blocking(dev->i2c_instance, dev->addr, reg_cmd, 2, true);
}

int tcs34725_enable(TCS34725 *dev)
{
    int ret = PICO_ERROR_GENERIC;
    ret = __write_8(dev,
                    TCS_COMMAND | TCS_REG_ENABLE,
                    TCS_REG_ENABLE_PON);
    if (ret < 0)
        return ret;

    sleep_ms(3);

    return __write_8(dev,
                     TCS_COMMAND | TCS_REG_ENABLE,
                     TCS_REG_ENABLE_PON | TCS_REG_ENABLE_AEN);
}

TCS34725 tcs34725_new(i2c_inst_t *instance, int auto_enable)
{
    TCS34725 s;
    s.addr = TCS_BUS_ADDR;
    s.i2c_instance = instance;

    if (auto_enable)
        tcs34725_enable(&s);

    return s;
}

uint8_t tcs34725_calculate_time(int ms)
{
    if (ms >= 614)
        return 0;

    ms *= 10;
    return 256 - (ms / 24);
}

int tcs34725_wait_enable(TCS34725 *dev)
{
    int ret;
    uint8_t current_enable;

    ret = __read_8(dev, TCS_COMMAND | TCS_REG_ENABLE, &current_enable);
    if (ret < 0)
        return ret;

    return __write_8(dev,
                     TCS_COMMAND | TCS_REG_ENABLE,
                     current_enable | TCS_REG_ENABLE_WEN);
}

int tcs34725_wait_disable(TCS34725 *dev)
{
    int ret;
    uint8_t current_enable;

    ret = __read_8(dev, TCS_COMMAND | TCS_REG_ENABLE, &current_enable);
    if (ret < 0)
        return ret;

    return __write_8(dev,
                     TCS_COMMAND | TCS_REG_ENABLE,
                     current_enable ^ TCS_REG_ENABLE_WEN);
}

int tcs34725_wlong_enable(TCS34725 *dev)
{
    return __write_8(dev, TCS_COMMAND | TCS_REG_CONFIG, TCS_CONFIG_WLONG);
}

int tcs34725_wlong_disable(TCS34725 *dev)
{
    return __write_8(dev, TCS_COMMAND | TCS_REG_CONFIG, 0);
}

int tcs34725_interrupt_enable(TCS34725 *dev)
{
    int ret;
    uint8_t current_enable;

    ret = __read_8(dev, TCS_COMMAND | TCS_REG_ENABLE, &current_enable);
    if (ret < 0)
        return ret;

    return __write_8(dev,
                     TCS_COMMAND | TCS_REG_ENABLE,
                     current_enable | TCS_REG_ENABLE_AIEN);
}

int tcs34725_interrupt_disable(TCS34725 *dev)
{
    int ret;
    uint8_t current_enable;

    ret = __read_8(dev, TCS_COMMAND | TCS_REG_ENABLE, &current_enable);
    if (ret < 0)
        return ret;

    return __write_8(dev,
                     TCS_COMMAND | TCS_REG_ENABLE,
                     current_enable ^ TCS_REG_ENABLE_AIEN);
}

int tcs34725_clear_interrupt(TCS34725 *dev)
{
    uint8_t SPECIAL_FUNCTION_TYPE = TCS_COMMAND | 0x66;
    return i2c_write_blocking(dev->i2c_instance, dev->addr, &SPECIAL_FUNCTION_TYPE, 1, false);
}

int tcs34725_set_atime(TCS34725 *dev, uint8_t atime)
{
    return __write_8(dev, TCS_COMMAND | TCS_REG_ATIME, atime);
}

int tcs34725_set_wtime(TCS34725 *dev, uint8_t wtime)
{
    return __write_8(dev, TCS_COMMAND | TCS_REG_WTIME, wtime);
}

int tcs34725_set_gain(TCS34725 *dev, uint8_t gain)
{
    return __write_8(dev, TCS_COMMAND | TCS_REG_CONTROLL, gain);
}

int tcs34725_read_clear(TCS34725 *dev, uint16_t *clear)
{
    return __read_16(dev, TCS_REG_CDATAL, (uint8_t *)clear);
}

int tcs34725_read_red(TCS34725 *dev, uint16_t *red)
{
    return __read_16(dev, TCS_REG_RDATAL, (uint8_t *)red);
}

int tcs34725_read_green(TCS34725 *dev, uint16_t *green)
{
    return __read_16(dev, TCS_REG_GDATAL, (uint8_t *)green);
}

int tcs34725_read_blue(TCS34725 *dev, uint16_t *blue)
{
    return __read_16(dev, TCS_REG_BDATAL, (uint8_t *)blue);
}

int tcs34725_read_all(TCS34725 *dev, SensorData *data)
{
    int ret;
    uint8_t reg_cmd = TCS_COMMAND | TCS_REG_CDATAL;

    ret = i2c_write_blocking(dev->i2c_instance, dev->addr, &reg_cmd, 1, false);
    if (ret < 0)
        return ret;

    ret = i2c_read_blocking(dev->i2c_instance, dev->addr, (void *)data, 8, true);
    if (ret < 0)
        return ret;

    return 0;
}

int tcs_calculate_ir_component(SensorData *sample)
{
    return (sample->red + sample->green + sample->blue - sample->clear) / 2;
}

SensorData tcs_remove_ir_component(SensorData *sample)
{
    int ir = tcs_calculate_ir_component(sample);
    return (SensorData){
        .red = sample->red - ir,
        .green = sample->green - ir,
        .blue = sample->blue - ir,
        .clear = sample->clear - ir,
    };
}

int tcs_calculate_ct(SensorData *sample)
{
    SensorData s = tcs_remove_ir_component(sample);
    return TCS_34725_CT_COEF * (s.blue / s.red) + TCS_34725_CT_OFFSET;
}