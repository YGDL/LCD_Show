#ifndef _TEXT_H
#define _TEXT_H

#include "stdint.h"

#define GB2312	0U
#define UTF_8	1U
#define CODING	UTF_8

extern void Get_HzMat(uint8_t *code, uint8_t *mat, uint8_t size, uint8_t Font);								//得到汉字的点阵码
extern void	Show_Char(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint16_t color, uint8_t mode);
extern void Show_Font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint16_t color, uint8_t mode);	//在指定位置显示一个汉字
extern void Show_Str(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t*str, uint8_t size, uint16_t color, uint8_t mode);	//在指定位置显示一个字符串 
extern void Draw_Font16B(uint16_t x, uint16_t y, uint16_t color, uint8_t*str);								//在指定位置--显示 16x16 大小的点阵字符串
extern void Draw_Font24B(uint16_t x, uint16_t y, uint16_t color, uint8_t*str);								//在指定位置--显示 24x24 大小的点阵字符串

#endif
