#ifndef _LCD_H
#define _LCD_H

#include "stdint.h"

//支持横竖屏快速定义切换
#define	LCD_DIR_Mode	0	//4种工作模式，0和1是竖屏模式，2和3是横屏模式
#define	USE_HORIZONTAL	0	//方向设置： 		0,竖屏模式   1,横屏模式.

//LCD的画笔颜色和背景色	   
extern	uint16_t	POINT_COLOR;	//默认红色    
extern	uint16_t	BACK_COLOR;		//背景颜色.默认为白色
extern	uint16_t	D_Color;		//点阵颜色
extern	uint16_t	B_Color;		//背景颜色

//扫描方向定义--扫描方式有不同规格，可能定义不左右和上下的参照方向不同，总结方式，只有一下八种
#define	L2R_U2D	0	//从左到右,从上到下
#define L2R_D2U	1	//从左到右,从下到上
#define R2L_U2D	2	//从右到左,从上到下
#define R2L_D2U	3	//从右到左,从下到上
#define U2D_L2R	4	//从上到下,从左到右
#define U2D_R2L	5	//从上到下,从右到左
#define D2U_L2R	6	//从下到上,从左到右
#define	D2U_R2L	7	//从下到上,从右到左	 

//画笔颜色
#define	WHITE		0xFFFF
#define	BLACK		0x0000	  
#define	BLUE		0x001F  
#define	BRED		0xF81F
#define	GRED		0xFFE0
#define	GBLUE		0x07FF
#define	RED			0xF800
#define	MAGENTA		0xF81F
#define	GREEN		0x07E0
#define	CYAN		0x7FFF
#define YELLOW		0xFFE0
#define BROWN		0xBC40	//棕色
#define BRRED		0xFC07	//棕红色
#define GRAY		0x8430	//灰色

//GUI颜色
#define	DARKBLUE	0x01CF	//深蓝色
#define	LIGHTBLUE	0x7D7C	//浅蓝色  
#define	GRAYBLUE	0x5458	//灰蓝色

//以上三色为PANEL的颜色 
#define	LIGHTGREEN	0x841F	//浅绿色
//#define LIGHTGRAY	0XEF5B	//浅灰色(PANNEL)
#define	LGRAY		0xC618	//浅灰色(PANNEL),窗体背景色

#define	GRAY0		0xEF7D	//灰色0 
#define	GRAY1		0x8410	//灰色1   
#define	GRAY2		0x4208	//灰色2  

#define LGRAYBLUE	0xA651	//浅灰蓝色(中间层颜色)
#define LBBLUE		0x2B12	//浅棕蓝色(选择条目的反色)


typedef struct  
{										    
	uint16_t	Width;			//LCD 宽度
	uint16_t	Height;			//LCD 高度
	uint16_t	ID;				//LCD ID
	uint8_t		Dir;			//横屏还是竖屏控制：竖屏和横屏。	
	uint16_t	Wram_Cmd;		//开始写gram指令
	uint16_t	Setx_Cmd;		//设置x坐标指令
	uint16_t	Sety_Cmd;		//设置y坐标指令 
}LCD_Parameter_TypeDef;

extern LCD_Parameter_TypeDef LCD_Parameter;
extern uint16_t POINT_COLOR;	//画笔颜色
extern uint16_t BACK_COLOR;		//背景色

extern void		LCD_WR_REG_DATA(uint16_t reg, uint16_t reg_data);
extern uint16_t	LCD_RD_REG_DATA(uint16_t reg);
extern void		LCD_WR_REG(uint16_t reg);
extern uint16_t	LCD_RD_DATA(void);
extern void		LCD_WR_DATA8(uint8_t data);
extern void		LCD_Display_Dir(uint8_t dir);
extern void		LCD_Scan_Dir(uint8_t dir);
extern void		LCD_Clear(uint16_t color);
extern void		LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);
extern void		LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
extern void		LCD_WriteRAM_Prepare(void);
extern void		LCD_SoftRest(void);
extern void		LCD_Init(void);
extern void		LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
extern void		Color_Test(void);
extern void		Draw_Test(void);
extern void		LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color);
extern void		LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
extern void		LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color);
extern void		LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t Color);

#endif
