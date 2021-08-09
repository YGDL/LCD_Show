#include "gbk.h"
#include "lcd.h"
#include "text.h"
#include "cp936.h"
#include "stdint.h"
#include "stdlib.h"
#include "main.h"
#include "stm32h7xx_hal.h"

//字库最大频率为70MHz
//较好的频率为65MHz


//12*12、24*24、32*32点阵可显示中英文
//24*48、32*64点阵只显示英文






//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//Num_Read:要读取的字节数(最大65535)
void FontLib_Read(uint8_t * pBuffer, uint32_t ReadAddr, uint16_t Num_Read)   
{
	uint8_t TX = 0U;
	uint8_t RX = 0U;

	WS_CS_ENABLE;							//使能器件  
	TX = FontRead_CMD;						//发送读取命令
	HAL_SPI_Transmit(&hspi2, &TX, 1, 1);
	for(int8_t i = 2; i > -1; i--)
	{
		TX = ReadAddr >> (i * 8) & 0xFF;
		HAL_SPI_TransmitReceive(&hspi2, &TX, &RX, 1, 1);//发送24bit地址
	}
	
	TX = 0xFF;
	for(uint16_t i=0; i<Num_Read; i++)
	{
		HAL_SPI_TransmitReceive(&hspi2, &TX, &RX, 1, 1);	//循环读数
		pBuffer[i] = RX;
	}
 	WS_CS_DISABLE;							//失能器件
}

//读取字库信息，解锁字库可用（必须读）
//读字库型号ID--使用字库前，必须先读取一次字库，
//确认字库已经解锁，否则会出现缺失字符的现象
void GBK_ReadID(void)
{
	uint8_t Temp[24];
	uint8_t i;

	LCD_Clear(WHITE);		//清除屏幕
	
	for(i=0; i<24; i++)
		Temp[i]=0;			//初始化缓冲区

	FontLib_Read(Temp, 0, 17);  
	DrawFont_GBK16B(16, 16, RED, Temp);
	
	for(i=0; i<24; i++)
		Temp[i]=0;			//初始化缓冲区
	FontLib_Read(Temp,20,8);   	
	DrawFont_GBK16B(16, 32, RED, Temp);
	
	for(i=0;i<24;i++)
		Temp[i]=0;			//初始化缓冲区
	FontLib_Read(Temp, 30, 8);   	
	DrawFont_GBK16B(16, 48, RED, Temp);
	
	HAL_Delay(500);	//延时显示	--显示部分可以不显示，读取是必须的
} 

//*code：字符指针开始-字符串的开始地址,GBK码-从字库中查找出字模
//*Pdot：数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
// Font: 字体型号
// Num : 字体大小
void GBK_GetASC_Point(uint8_t *code, uint8_t *Pdot, uint8_t Font, uint16_t Num)
{
	uint8_t QW;    //ASC码序号
	uint32_t  ADDRESS;
		
	QW=*code;//区位码--获取序号
		
	switch(Font)
	{
		case 12:
			ADDRESS=((unsigned long)QW*Num)+ASCII6x12_ST;	  //得到字库中的字节偏移量  	
			break;
		case 16:
			ADDRESS=((unsigned long)QW*Num)+ASCII8x16_ST;	  //得到字库中的字节偏移量  	
			break;
		case 24:
			ADDRESS=((unsigned long)QW*Num)+ASCII12x24_ST;	//得到字库中的字节偏移量  
			break;
		case 32:
			ADDRESS=((unsigned long)QW*Num)+ASCII16x32_ST;	//得到字库中的字节偏移量  
			break;
		case 48:
			ADDRESS=((unsigned long)QW*Num)+ASCII24x48_ST;	//得到字库中的字节偏移量  
			break;
		case 64:
			ADDRESS=((unsigned long)QW*Num)+ASCII32x64_ST;	//得到字库中的字节偏移量  
			break;
		default:
			return;
	}
	FontLib_Read(Pdot, ADDRESS, Num);		//读取点阵字符
}  

//显示一个指定大小的ASC字符
//x,y    : ASCII的坐标--16位
//N_Word : ASCII--序号--8位
//size   : 字体大小-字号-8位
//D_Color：点阵字颜色--16位
//B_Color：背景颜色--16位
//mode:  0,正常显示,1,叠加显示	--8位
void GBK_ShowASCII(uint16_t x, uint16_t y, uint8_t *N_Word, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode)
{
	uint16_t csize; //得到字体一个字符对应点阵集所占的字节数
	uint8_t temp;
	uint16_t t,t1;
	uint16_t y0=y;
	uint8_t dzk[256] = {0U};
	
	csize = (size/8 + ((size%8)?1:0)) * (size/2);		//ASCII字符大小占半个汉字的宽度，得到字体一个字符对应点阵集所占的字节数		 
	
	if(size!=12 && size!=16 && size!=24 && size!=32 && size!=48 && size!=64)
		return;					//不支持的size
	
	GBK_GetASC_Point(N_Word, dzk, size, csize);			//得到相应大小的点阵数据
	for(t=0;t<csize;t++)
	{
		temp = dzk[t];			//得到点阵数据
		for(t1=0; t1<8; t1++)	//纵向输出点阵显示到屏幕
		{
			if(temp & 0x80)
				LCD_Fast_DrawPoint(x,y,D_Color);		//显示字符点阵
			else
				if(mode == 0)
					LCD_Fast_DrawPoint(x,y,B_Color);	//显示背景颜色
			temp <<= 1;
			y++;
			if((y-y0) == size)
			{
				y=y0;
				x++;
				break;
			}
		}
	}
}

//*code：字符指针开始-字符串的开始地址,GBK码-从字库中查找出字模
//*Pdot：数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
// Font: 字体型号
// Num : 点阵字符数量
void GBK_Lib_GetHz(uint8_t *code, uint8_t *Pdot, uint8_t Font, uint16_t Num)
{
	uint8_t  qh,ql;
	uint16_t i;	
	uint32_t foffset;
	uint32_t ADDRESS;
	
	//csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
		
	qh = *code;       //获得字符的区位码
	ql = *(++code);   //获得字符的区位码
		
	if(qh<0x81 || ql<0x40 || ql==0xff || qh==0xff)//非常用汉字
	{   		    
		for(i=0; i<Num; i++)
			*Pdot ++= 0x00;			//填充满格--0不显示
		return;						//结束访问
	}
	if(ql<0x7f)
		ql-=0x40;  //注意!
	else
		ql-=0x41;
	qh -= 0x81;
	foffset=((unsigned long)190*qh+ql)*Num;	//得到字库中的字节偏移量
	switch(Font)
	{
		case 12:
			ADDRESS=foffset+GBK12x12_ST;     //汉字  16x16 点阵的起始位置
			break;
		case 16:
			ADDRESS=foffset+GBK16x16_ST;     //汉字  16x16 点阵的起始位置
			break;
		case 24:
			ADDRESS=foffset+GBK24x24_ST;     //汉字  24x24 点阵的起始位置
			break;
		case 32:
			ADDRESS=foffset+GBK32x32_ST;     //汉字  32x32 点阵的起始位置
			break;
		default:
			return;
	}
	FontLib_Read(Pdot, ADDRESS, Num);//读取点阵字符
} 

//显示一个指定大小的汉字
//x,y    : ASCII的坐标     --16位
//N_Word : ASCII--序号     --8位
//size   : 字体大小-字号   --8位
//D_Color：点阵字颜色      --16位
//B_Color：背景颜色        --16位
//mode:  0,正常显示,1,叠加显示	--8位
void GBK_Show_Font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode)
{
	uint16_t csize;                                    //得到字体一个字符对应点阵集所占的字节数	 
	uint8_t  temp,t,t1;
	uint16_t y0=y;
	uint8_t  dzk[128];
	
	csize=(size/8+((size%8)?1:0))*(size);              //得到字体一个字符对应点阵集所占的字节数
	if(size!=12&&size!=16&&size!=24&&size!=32)return;	 //不支持的size
	GBK_Lib_GetHz(font,dzk,size,csize);	                   //得到相应大小的点阵数据 
	
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			                             //得到点阵数据
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
				LCD_Fast_DrawPoint(x,y,D_Color);//显示字符点阵
			else
				if(mode==0)
					LCD_Fast_DrawPoint(x,y,B_Color); //显示背景颜色
				temp<<=1;	
				y++;
				if((y-y0) == size)
				{
					y=y0;
					x++;
					break;
				}
		}
	}
}

//在指定位置开始显示一个字符串	    
//支持自动换行
//(x,y):起始坐标
//width,height:可以显示字符的区域
//str  :字符串
//size :字体大小
//D_Color：点阵字颜色      --16位
//B_Color：背景颜色        --16位
//mode:0,非叠加方式;1,叠加方式 
void GBK_Show_Str(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *str, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode)
{					
	uint16_t x0 = x;
	uint16_t y0 = y;							  	  
	uint8_t  bHz = 0;				//字符或者中文  	    			
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
				bHz = 1;				//中文 
			else					//字符
			{
				if(x > (x0 + width - size/2))	//换行
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
					GBK_ShowASCII(x, y, str1, size, D_Color, B_Color, mode);	//有效部分写入 
				str1++; 
				x += size/2; 			//字符,为全字的一半 
			}
		}
		else						//中文 
		{
			bHz=0;//有汉字库
			if(x > (x0+width-size))//换行
			{
				y += size;
				x = x0;		  
			}
			if(y > (y0+height-size))
				break;  //越界返回
			GBK_Show_Font(x, y, str1, size, D_Color, B_Color, mode);	//显示这个汉字,空心显示
			str1 += 2; 
			x += size;//下一个汉字偏移
		}
	}
	free(str2);
}

//在指定宽度的中间显示字符串
//如果字符长度超过了len,则用GBK_Show__Str显示
//len:指定要显示的宽度
//D_Color：点阵字颜色      --16位
//B_Color：背景颜色        --16位
void GBK_Show_StrMid(uint16_t x, uint16_t y, uint8_t size, uint8_t len, uint16_t D_Color, uint16_t B_Color, uint8_t *str)
{
	uint16_t strlenth=0;

	strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)
		GBK_Show_Str(x, y, LCD_Parameter.Width, LCD_Parameter.Height, str, size, D_Color, B_Color, 1);
	else
	{
		strlenth=(len-strlenth)/2;
		GBK_Show_Str(strlenth+x, y, LCD_Parameter.Width, LCD_Parameter.Height, str, size, D_Color, B_Color, 1);
	}
}

//函数功能：在指定位置开始显示一个12x12点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
//(x,y):起始坐标
//color,字符颜色
//str  :字符串
//非叠加方式;非点阵字符的部分，填充背景颜色
void DrawFont_GBK12B(uint16_t x, uint16_t y, uint16_t color, uint8_t *str)
{
	uint16_t width;
	uint16_t height;
	
	width = LCD_Parameter.Width - x;
	height = LCD_Parameter.Height - y;
	
	GBK_Show_Str(x, y, width,height, str, 12, color, BACK_COLOR, 0);
}

//在指定位置开始显示一个16x16点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)
//(x,y):起始坐标
//color,字符颜色
//str  :字符串
//非叠加方式;非点阵字符的部分，填充背景颜色
void DrawFont_GBK16B(uint16_t x, uint16_t y, uint16_t color, uint8_t *str)
{
	uint16_t width;
	uint16_t height;
	
	width = LCD_Parameter.Width - x;
	height = LCD_Parameter.Height - y;
	
	GBK_Show_Str(x, y, width, height, str, 16, color, BACK_COLOR, 0);
}

//在指定位置开始显示一个24x24点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
//(x,y):起始坐标
//color,字符颜色
//str  :字符串
//非叠加方式;非点阵字符的部分，填充背景颜色
void DrawFont_GBK24B(uint16_t x, uint16_t y, uint16_t color, uint8_t *str)
{
	uint16_t width;
	uint16_t height;
	
	width = LCD_Parameter.Width - x;
	height = LCD_Parameter.Height - y;
	
	GBK_Show_Str(x, y, width, height, str, 24, color, BACK_COLOR, 0);
}

//在指定位置开始显示一个32x32点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
//(x,y):起始坐标
//color,字符颜色
//str  :字符串
//非叠加方式;非点阵字符的部分，填充背景颜色
void DrawFont_GBK32B(uint16_t x, uint16_t y, uint16_t color, uint8_t *str)
{
	uint16_t width;
	uint16_t height;
	
	width = LCD_Parameter.Width - x;
	height = LCD_Parameter.Height - y;
	
	GBK_Show_Str(x, y, width, height, str, 32, color, BACK_COLOR, 0);
}

//在指定位置开始显示一个24x48点阵的--ACSII码 （本字库只支持ASCII码），支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)
//(x,y):起始坐标
//color,字符颜色
//str  :字符串
//非叠加方式;非点阵字符的部分，填充背景颜色
void DrawFontASC_GBK48B(uint16_t x, uint16_t y, uint16_t color, uint8_t *str)
{
	uint16_t width;
	uint16_t height;
	
	width = LCD_Parameter.Width - x;
	height = LCD_Parameter.Height - y;
	
	GBK_Show_Str(x, y, width, height, str, 48, color, BACK_COLOR, 0);
}

//在指定位置开始显示一个32x64点阵的--ACSII码 （本字库只支持ASCII码），支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
//(x,y):起始坐标
//color,字符颜色
//str  :字符串
//非叠加方式;非点阵字符的部分，填充背景颜色
void DrawFontASC_GBK64B(uint16_t x, uint16_t y, uint16_t color, uint8_t *str)
{
	uint16_t width;
	uint16_t height;
	
	width = LCD_Parameter.Width - x;
	height = LCD_Parameter.Height - y;
	
	GBK_Show_Str(x, y, width, height, str, 64, color, BACK_COLOR, 0);
}

//GBK字体字形显示测试
void GBK_LibFont_Test(void)
{
	
	LCD_Clear(GRAY0);
	
	DrawFont_GBK16B(24, 16, BLUE, (uint8_t *)"2: GBK字库--显示测试");
	HAL_Delay(1000);
	
	LCD_Clear(GREEN);
	
	GBK_Show_Str(24, 0, 240, 32, (uint8_t *)"十种字库测试", 32, D_Color, GRAY0, 0);
	GBK_Show_Str(16, 40, 240, 16, (uint8_t *)"设计：大越电子", 16, D_Color, B_Color, 0);
	GBK_Show_Str(16, 56, 240, 16, (uint8_t *)"From: mcudev.taobao.com", 16, D_Color, B_Color, 0);
	DrawFont_GBK12B(16, 72, BLUE, (uint8_t *)"Date: 2019/05/08");
	GBK_Show_StrMid(16, 96, 12, 200, BLUE, GRAY0, (uint8_t *)"居中显示函数: 2019/05/08");		//居中显示
	GBK_Show_Str(16, 120, 240, 12, (uint8_t *)"汉字12x12-GBK字库：", 12, D_Color, B_Color, 0);	//显示内码低字节
	GBK_Show_Str(16, 136, 240, 16, (uint8_t *)"汉字16x16字库：", 16, D_Color, B_Color, 0);		//显示内码低字节
	GBK_Show_Str(16, 156, 240, 24, (uint8_t *)"24x24字库：", 24, D_Color, B_Color, 0);			//显示内码低字节
	DrawFont_GBK32B(16, 184, BLUE, (uint8_t *)"32x32字库：");									//显示内码低字节
	DrawFontASC_GBK48B(16, 220, BLUE, (uint8_t *)"48");
	DrawFontASC_GBK64B(120, 220, BLUE, (uint8_t *)"64");
	
	HAL_Delay(3000);
}

//GBK_Show_Str()函数是底层函数
