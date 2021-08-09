#include "text.h"
#include "string.h"
#include "lcd.h"
#include "font.h"
#include "cp936.h"
#include "stdlib.h"

//内存拷贝函数memcpy();
//字符串复制函数strcpy();
//字符串长度测量函数strlen();

void Copy_Mem (uint8_t *P1, const uint8_t *P2, uint32_t Count)
{
	uint32_t i ;
	for(i=0; i<Count; i++)
		*P1++ = *P2++;
}

//根据区位码和字号，获取对应的点阵
void Copy_HZK16(uint8_t * Dot, uint8_t num, uint8_t Q, uint8_t W)
{
	uint16_t k;
	
	for (k=0; k< hz16_num; k++) //限制在字库里查找对应字符的次数
	{
		if ((hz16[k].Index[0] == Q) && (hz16[k].Index[1] == W))
		{
			Copy_Mem(Dot, &hz16[k].Msk[0], num);
		}
	}
}

//根据区位码和字号，获取对应的点阵
void Copy_HZK24(uint8_t * Dot, uint8_t num, uint8_t Q, uint8_t W)
{
	uint16_t k;
	
	for (k=0; k < hz24_num; k++) //限制在字库里查找对应字符的次数
	{
		if ((hz24[k].Index[0] == Q) && (hz24[k].Index[1] == W))
		{
			Copy_Mem(Dot, &hz24[k].Msk[0], num);
		}
	}
}

//根据区位码和字号，获取对应的点阵 
//code 字符指针开始 从字库中查找出字模
//mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
//size:字体点阵数据的个数
//Font:字体类型
void Get_HzMat(uint8_t * code, uint8_t * mat, uint8_t size, uint8_t Font)
{		    
	unsigned char qh,ql;
	unsigned char i;

	qh=*code;

	ql=*(++code);

	if(qh<0x81 || ql<0x40 || ql==0xff || qh==0xff)//非常用汉字
	{
	    for(i=0;i<size;i++)
			*mat++=0x00;//填充满格
	    return; //结束访问
	}
	switch(Font)
	{
		case 12:
			break;
		case 16:
			Copy_HZK16(mat, size, qh, ql);
			break;
		case 24:
			Copy_HZK24(mat, size, qh, ql);
			break;
	}
}

//字符串绘制
void Show_Char(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint16_t color, uint8_t mode)
{  							  
	uint8_t temp,t1,t;
	uint16_t y0=y;
	uint8_t csize = (size/8 + ((size%8)?1:0)) * (size/2);		//得到字体一个字符对应点阵集所占的字节数	
	//设置窗口		   
	num = num - ' ';						//得到偏移后的值
	for(t=0; t < csize; t++)				//csize为循环次数
	{   
		if(size==12)						//根据字体尺寸选择合适的数组，并根据偏移值调用
			temp=asc2_1206[num][t];			//调用1206字体
		else
			if(size==16)
				temp=asc2_1608[num][t];		//调用1608字体
			else
				if(size==24)
					temp=asc2_2412[num][t];	//调用2412字体
				else
					return;					//没有的字库
		for(t1=0; t1 < 8; t1++)
		{			    
			if(temp & 0x80)
				LCD_Fast_DrawPoint(x, y, color);
			else
				if(mode==0)
					LCD_Fast_DrawPoint(x, y, BACK_COLOR);
			temp <<= 1;
			y++;
			if(y >= LCD_Parameter.Height)
				return;							//超区域了
			if((y-y0) == size)
			{
				y=y0;
				x++;
				if(x >= LCD_Parameter.Width)
					return;						//超区域了
				break;
			}
		}
	}
}

//显示一个指定大小的汉字
//x,y :汉字的坐标
//font:汉字区位码
//size:字体大小
//color:字符颜色
//mode:0,正常显示,1,叠加显示
void Show_Font(uint16_t x, uint16_t y, uint8_t * font, uint8_t size, uint16_t color, uint8_t mode)
{
	uint8_t temp,t,t1;
	uint16_t x0=x;
	uint8_t dzk[72];
	uint8_t Num;
	
	Num = (size/8 + ((size%8) ? 1 : 0)) * (size);//得到字体一个字符对应点阵集所占的字节数	 
	
	if(size!=12 && size!=16 && size!=24)
		return;	//不支持的size
	Get_HzMat(font, dzk, Num, size);	//得到相应大小的点阵数据 

	for(t = 0; t < Num; t++)
	{
		temp=dzk[t];			//得到点阵数据
		for(t1 = 0; t1 < 8; t1++)
		{
			if(temp & 0x80)
				LCD_Fast_DrawPoint(x, y, color);
			else
				if(mode==0)
					LCD_Fast_DrawPoint(x, y, BACK_COLOR);
			temp<<=1;
			x++;
			if((x-x0) == size)
			{
				x=x0;
				y++;
//				break;
			}
		}
	}
}

//在指定位置开始显示一个字符串，支持自动换行，支持中英文混搭
//(x,y):起始坐标
//width,height:显示区域
//str  :字符串
//size :字体大小
//color:字符颜色
//mode :0,非叠加方式;1,叠加方式
void Show_Str(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t * str, uint8_t size, uint16_t color, uint8_t mode)
{
	uint16_t x0= x;
	uint16_t y0= y;
	uint8_t bHz=0;					//字符或者中文
	uint8_t  *str1 = NULL;
	uint8_t  *str2 = NULL;
	
	#if	CODING
	str1 = str2 = UTF8_TO_GB2312(str);
	#endif

	while(*str1 != 0)					//数据未结束
    {
        if(!bHz)
        {
	        if(*str1 > 0x80)
				bHz=1;				//中文
	        else					//字符
	        {
				if(x > (x0+width-size/2))	//换行
				{
					y += size;
					x = x0;
				}
		        if(y > (y0+height-size))
					break;			//越界返回
		        if(*str1 == 13)		//换行符号
		        {
		            y += size;
					x = x0;
		            str1++;
		        }
		        else
					Show_Char(x, y, *str1, size, color, mode);//有效部分写入
				str1++;
		        x += size/2;			//字符,为全字的一半
	        }
        }
		else						//中文
        {
            bHz=0;					//有汉字库
            if(x > (x0+width-size))	//换行
			{
				y += size;
				x = x0;
			}
	        if(y > (y0+height-size))
				break;				//越界返回
	        Show_Font(x, y, str1, size, color, mode);	//显示这个汉字,空心显示
	        str1 += 2;
	        x += size;//下一个汉字偏移
        }
    }
	free(str2);
}

//在指定位置开始显示一个16x16点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)
//(x,y):起始坐标
//color:字符颜色
//str  :字符串
void Draw_Font16B(uint16_t x, uint16_t y, uint16_t color, uint8_t * str)
{
	uint16_t width;
	uint16_t height;
	
	width = LCD_Parameter.Width - x;
	height = LCD_Parameter.Height - y;
	
	Show_Str(x, y, width, height, str, 16, color, 0);
}

//在指定位置开始显示一个24x24点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)
//(x,y):起始坐标
//color:字符颜色
//str  :字符串
void Draw_Font24B(uint16_t x, uint16_t y, uint16_t color, uint8_t * str)
{
	uint16_t width;
	uint16_t height;
	
	width = LCD_Parameter.Width - x;
	height = LCD_Parameter.Height - y;
	
	Show_Str(x, y, width, height, str, 24, color, 0);
}

