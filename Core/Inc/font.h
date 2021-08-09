#ifndef __FONT_H
#define __FONT_H 

#include "stdint.h"

extern const uint8_t asc2_1206[95][12]; //12*12 ASCII字符集点阵
extern const uint8_t asc2_1608[95][16]; //16*16 ASCII字符集点阵
extern const uint8_t asc2_2412[95][36]; //24*24 ASICII字符集点阵

//定义字库存储划分
struct typFNT_GB16
{
	uint8_t Index[2];
	uint8_t Msk[32];
};

#define USE_ONCHIP_FLASH_FONT 1    //定义是否开启片上字库

//宋体5号
#define hz16_num   59   //此处用于设定字库汉字字符的存储汉字的个数，如果添加或者减少字符，需要这里也对应修改

extern const struct typFNT_GB16 hz16[];

//定义字库存储划分
struct typFNT_GB24
{
	uint8_t Index[2];	
	uint8_t Msk[72];
};

#define hz24_num   9
extern const struct typFNT_GB24 hz24[];


#endif
