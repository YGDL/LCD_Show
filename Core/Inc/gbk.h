#ifndef _GBK_H
#define _GBK_H	 

#include "lcd.h"  //加入关联头文件
#include "string.h"	
#include "stdint.h"

#define   FontRead_CMD  0x03//接口读取字库--命令字

//汉字字库存储偏移位置
#define	ASCII6x12_ST   0x00080000
#define	ASCII8x16_ST   0x00080800
#define	ASCII12x24_ST  0x00081200
#define	ASCII16x32_ST  0x00082600
#define	ASCII24x48_ST  0x00084800
#define	ASCII32x64_ST  0x00089200
#define	GBK12x12_ST    0x00091400
#define	GBK16x16_ST    0x0011DD00
#define	GBK24x24_ST    0x001DA000
#define	GBK32x32_ST    0x00380000 

void GBK_Lib_Init(void);//初始化引脚接口
void FontLib_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t Num_Read);//字库读写接口函数
void GBK_ReadID(void);//读字库ID
void GBK_GetASC_Point(uint8_t *code, uint8_t *Pdot, uint8_t Font, uint16_t Num);   // 获取点阵ASCII点阵字符 		
void GBK_ShowASCII(uint16_t x, uint16_t y, uint8_t *N_Word, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode); // 在指定位置显示ASCII码 字符--点阵字符
void GBK_Lib_GetHz(uint8_t *code, uint8_t *Pdot, uint8_t Font, uint16_t Num);			   //得到汉字的点阵码
void GBK_Show_Font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode);			 //在指定位置显示一个汉字
void GBK_Show_Str(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *str, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode);	//在指定位置显示一个字符串 
void GBK_Show_StrMid(uint16_t x,uint16_t y, uint8_t size, uint8_t len, uint16_t D_Color, uint16_t B_Color,uint8_t*str); //在指定宽度的中间显示字符串
void DrawFont_GBK12B(uint16_t x,uint16_t y, uint16_t color, uint8_t *str);//在指定位置开始显示一个12x12点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
void DrawFont_GBK16B(uint16_t x,uint16_t y, uint16_t color, uint8_t *str);//在指定位置开始显示一个16x16点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
void DrawFont_GBK24B(uint16_t x,uint16_t y, uint16_t color, uint8_t *str);//在指定位置开始显示一个24x24点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
void DrawFont_GBK32B(uint16_t x,uint16_t y, uint16_t color, uint8_t *str);//在指定位置开始显示一个32x32点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
void DrawFontASC_GBK48B(uint16_t x, uint16_t y, uint16_t color, uint8_t *str);//在指定位置开始显示一个24x48点阵的--ACSII码 
void DrawFontASC_GBK64B(uint16_t x, uint16_t y, uint16_t color, uint8_t *str);//在指定位置开始显示一个32x64点阵的--ACSII码 
void GBK_LibFont_Test(void);//GBK字库测试函数

#endif
