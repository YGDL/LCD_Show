#include "lcd.h"
#include "font.h"
#include "main.h"
#include "stm32h7xx_hal.h"

uint16_t POINT_COLOR = 0x0000;	//画笔颜色
uint16_t BACK_COLOR = 0xFFFF;	//背景色
uint16_t D_Color = BLUE;		//点阵颜色
uint16_t B_Color = GREEN;		//背景颜色

LCD_Parameter_TypeDef LCD_Parameter;

//90MHz时钟无问题√
//95MHz时钟有问题×

//写LCD寄存器
void LCD_WR_REG(uint16_t reg)
{
	uint8_t TX = 0U;
	TX = reg & 0xFF;
	
	LCD_CS_ENABLE;
	LCD_DC_ORDER;
	HAL_SPI_Transmit(&hspi2, &TX, 1, 1);
	LCD_CS_DISABLE;
}

//读LCD数据
uint16_t LCD_RD_DATA(void)
{
	uint8_t RX1 = 0U;
	uint8_t RX2 = 0U;
	
	LCD_CS_ENABLE;
	HAL_SPI_Receive(&hspi2, &RX2, 1, 1);
	HAL_SPI_Receive(&hspi2, &RX1, 1, 1);
	LCD_CS_DISABLE;
	
	return RX2 << 8 | RX1;
}

//写LCD数据
void LCD_WR_DATA8(uint8_t data)
{
	LCD_CS_ENABLE;
	LCD_DC_DATA;
	HAL_SPI_Transmit(&hspi2, &data, 1, 1);
	LCD_CS_DISABLE;
}

//写LCD数据
void LCD_WR_DATA16(uint16_t data)
{
	uint8_t TX1 = 0U;
	uint8_t TX2 = 0U;
	TX1 = data & 0xFF;
	TX2 = data >> 8 & 0xFF;
	
	LCD_CS_ENABLE;
	LCD_DC_DATA;
	HAL_SPI_Transmit(&hspi2, &TX2, 1, 1);
	HAL_SPI_Transmit(&hspi2, &TX1, 1, 1);
	LCD_CS_DISABLE;
}

//写LCD寄存器数据
void LCD_WR_REG_DATA(uint16_t reg, uint16_t reg_data)
{
	LCD_WR_REG(reg);
	LCD_WR_DATA8(reg_data);
}

//读LCD寄存器数据
uint16_t LCD_RD_REG_DATA(uint16_t reg)
{
	LCD_WR_REG(reg);
	HAL_Delay(1);
	return LCD_RD_DATA();
}

//设置LCD的自动扫描方向
//默认设置为L2R_U2D,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向
void LCD_Scan_Dir(uint8_t dir)
{
	uint16_t regval=0;

	//扫描方向定义——扫描方式有不同规格，可能定义不左右和上下的参照方向不同，总结方式，只有一下八种
	switch(dir)
	{
		case L2R_U2D://从左到右,从上到下
			regval|=(0<<7)|(0<<6)|(0<<5);
			break;
		case L2R_D2U://从左到右,从下到上
			regval|=(1<<7)|(0<<6)|(0<<5);
			break;
		case R2L_U2D://从右到左,从上到下
			regval|=(0<<7)|(1<<6)|(0<<5);
			break;
		case R2L_D2U://从右到左,从下到上
			regval|=(1<<7)|(1<<6)|(0<<5);
			break;	 
		case U2D_L2R://从上到下,从左到右
			regval|=(0<<7)|(0<<6)|(1<<5);
			break;
		case U2D_R2L://从上到下,从右到左
			regval|=(0<<7)|(1<<6)|(1<<5);
			break;
		case D2U_L2R://从下到上,从左到右
			regval|=(1<<7)|(0<<6)|(1<<5);
			break;
		case D2U_R2L://从下到上,从右到左
			regval|=(1<<7)|(1<<6)|(1<<5);
			break;	 
	}
	LCD_WR_REG_DATA(0x36,regval|0x08);//改变扫描方向命令  ---此处需要查看数据手册，确定RGB颜色交换位的配置
}

//设置LCD的显示窗口
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
	width=sx+width-1;
	height=sy+height-1;

	LCD_WR_REG(LCD_Parameter.Setx_Cmd); 
	LCD_WR_DATA16(sx);      //设置 X方向起点
	LCD_WR_DATA16(width);   //设置 X方向终点

	LCD_WR_REG(LCD_Parameter.Sety_Cmd);
	LCD_WR_DATA16(sy);      //设置 Y方向起点
	LCD_WR_DATA16(height);  //设置 Y方向终点
}

//设置光标位置
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	LCD_WR_REG(LCD_Parameter.Setx_Cmd); 
	LCD_WR_DATA16(Xpos);

	LCD_WR_REG(LCD_Parameter.Sety_Cmd);
	LCD_WR_DATA16(Ypos);
}

//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD_WR_REG(LCD_Parameter.Wram_Cmd);
}

//软复位
void LCD_SoftRest(void)
{					   
	LCD_WR_REG(0x01);	//发送软复位命令
	HAL_Delay(50);
}

//设置LCD显示方向
//  dir:   0,竖屏  正
//         1,竖屏  反
//         2,横屏  左
//         3,横屏  右
void LCD_Display_Dir(uint8_t dir)
{
	uint8_t SCAN_DIR;
	
	if(dir == 0)					//竖屏	正
	{
		LCD_Parameter.Dir = 0;		//竖屏
		LCD_Parameter.Width = 240;
		LCD_Parameter.Height = 320;
		LCD_Parameter.Wram_Cmd = 0X2C;
	 	LCD_Parameter.Setx_Cmd = 0X2A;
		LCD_Parameter.Sety_Cmd = 0X2B;
		
		SCAN_DIR = L2R_U2D;			//选择扫描方向		

	}
	else if(dir == 1)				//横屏
	{	  				
		LCD_Parameter.Dir = 0;		//竖屏
		LCD_Parameter.Width = 240;
		LCD_Parameter.Height = 320;
		LCD_Parameter.Wram_Cmd = 0X2C;
	 	LCD_Parameter.Setx_Cmd = 0X2A;
		LCD_Parameter.Sety_Cmd = 0X2B;
		
		SCAN_DIR = R2L_D2U;			//选择扫描方向
	}
	else if(dir == 2)				//横屏
	{	  				
		LCD_Parameter.Dir = 1;		//横屏
		LCD_Parameter.Width = 320;
		LCD_Parameter.Height = 240;
		LCD_Parameter.Wram_Cmd = 0X2C;
	 	LCD_Parameter.Setx_Cmd = 0X2A;
		LCD_Parameter.Sety_Cmd = 0X2B;  
		
		SCAN_DIR = U2D_R2L;			//选择扫描方向
	} 
	else if(dir == 3)				//横屏
	{	  				
		LCD_Parameter.Dir = 1;		//横屏
		LCD_Parameter.Width = 320;
		LCD_Parameter.Height = 240;
		LCD_Parameter.Wram_Cmd = 0X2C;
	 	LCD_Parameter.Setx_Cmd = 0X2A;
		LCD_Parameter.Sety_Cmd = 0X2B; 
		
		SCAN_DIR = D2U_L2R;			//选择扫描方向
	} 	
	else							//默认为竖屏——正
	{
		LCD_Parameter.Dir = 0;		//竖屏
		LCD_Parameter.Width = 240;
		LCD_Parameter.Height = 320;
		LCD_Parameter.Wram_Cmd = 0X2C;
		LCD_Parameter.Setx_Cmd = 0X2A;
		LCD_Parameter.Sety_Cmd = 0X2B;

		SCAN_DIR = L2R_U2D;			//选择扫描方向
	}
	
	//以下设置，为窗口参数设置，设置了全屏的显示范围
	LCD_WR_REG(LCD_Parameter.Setx_Cmd);
	LCD_WR_DATA16(0);
	LCD_WR_DATA16(LCD_Parameter.Width-1);
	LCD_WR_REG(LCD_Parameter.Sety_Cmd);
	LCD_WR_DATA16(0);
	LCD_WR_DATA16(LCD_Parameter.Height-1);

	//设置屏幕显示——扫描方向
	LCD_Scan_Dir(SCAN_DIR);
}

//清屏函数
void LCD_Clear(uint16_t color)
{
	uint32_t index = 0;
	uint32_t totalpoint;
	
	LCD_Set_Window(0, 0, LCD_Parameter.Width, LCD_Parameter.Height);//设置全屏窗口
	
	totalpoint = LCD_Parameter.Width * LCD_Parameter.Height;//得到总点数
 
	LCD_SetCursor(0x00, 0x00);			//设置光标位置
	
	LCD_WriteRAM_Prepare();				//开始写入GRAM
	
	for(index = 0; index < totalpoint; index++)
	{
		LCD_WR_DATA16(color);	
	}
}

//LCD初始化函数
void LCD_Init(void)
{ 	
	LCD_SoftRest();		//软复位
	HAL_Delay(50);

	LCD_WR_REG(0xCF);  
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xD9); 
	LCD_WR_DATA8(0X30);

	LCD_WR_REG(0xED);  
	LCD_WR_DATA8(0x64); 
	LCD_WR_DATA8(0x03); 
	LCD_WR_DATA8(0X12); 
	LCD_WR_DATA8(0X81);

	LCD_WR_REG(0xE8);  
	LCD_WR_DATA8(0x85); 
	LCD_WR_DATA8(0x10); 
	LCD_WR_DATA8(0x78);

	LCD_WR_REG(0xCB);  
	LCD_WR_DATA8(0x39); 
	LCD_WR_DATA8(0x2C); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x34); 
	LCD_WR_DATA8(0x02);

	LCD_WR_REG(0xF7);  
	LCD_WR_DATA8(0x20);

	LCD_WR_REG(0xEA);
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0xC0);     //Power control 
	LCD_WR_DATA8(0x21);   //VRH[5:0] 

	LCD_WR_REG(0xC1);     //Power control 
	LCD_WR_DATA8(0x12);   //SAP[2:0];BT[3:0]

	LCD_WR_REG(0xC5);      //VCM control 
	LCD_WR_DATA8(0x32); 	 //3F
	LCD_WR_DATA8(0x3C); 	 //3C

	LCD_WR_REG(0xC7);     //VCM control2 
	LCD_WR_DATA8(0xC1);

	LCD_WR_REG(0x36);     // Memory Access Control 
	LCD_WR_DATA8(0x08);

	LCD_WR_REG(0x3A);   
	LCD_WR_DATA8(0x55);

	LCD_WR_REG(0xB1);     //֡帧速率控制
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x18); 

	LCD_WR_REG(0xB6);    // Display Function Control 
	LCD_WR_DATA8(0x0A); 
	LCD_WR_DATA8(0xA2);

	LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
	LCD_WR_DATA8(0x00); 

	LCD_WR_REG(0x26);    //Gamma curve selected 
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xE0);    //Set Gamma 
	LCD_WR_DATA8(0x0F); 
	LCD_WR_DATA8(0x20); 
	LCD_WR_DATA8(0x1E); 
	LCD_WR_DATA8(0x09); 
	LCD_WR_DATA8(0x12); 
	LCD_WR_DATA8(0x0B); 
	LCD_WR_DATA8(0x50); 
	LCD_WR_DATA8(0xBA); 
	LCD_WR_DATA8(0x44); 
	LCD_WR_DATA8(0x09); 
	LCD_WR_DATA8(0x14); 
	LCD_WR_DATA8(0x05); 
	LCD_WR_DATA8(0x23); 
	LCD_WR_DATA8(0x21); 
	LCD_WR_DATA8(0x00); 


	LCD_WR_REG(0xE1);    //Set Gamma 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x19); 
	LCD_WR_DATA8(0x19); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x12); 
	LCD_WR_DATA8(0x07); 
	LCD_WR_DATA8(0x2D); 
	LCD_WR_DATA8(0x28); 
	LCD_WR_DATA8(0x3F); 
	LCD_WR_DATA8(0x02); 
	LCD_WR_DATA8(0x0A); 
	LCD_WR_DATA8(0x08); 
	LCD_WR_DATA8(0x25); 
	LCD_WR_DATA8(0x2D); 
	LCD_WR_DATA8(0x0F);


	LCD_WR_REG(0x11); //Exit Sleep

	HAL_Delay(120);

	LCD_WR_REG(0x29); //display on	

	LCD_Display_Dir(LCD_DIR_Mode);	//选择屏幕显示方式

	LCD_BLK_ENABLE;					//开启背光

	LCD_Clear(GREEN);
}

//函数功能：快速画点
//输入参数：
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{	   
	LCD_WR_REG(LCD_Parameter.Setx_Cmd); 
	LCD_WR_DATA16(x);

	LCD_WR_REG(LCD_Parameter.Sety_Cmd); 
	LCD_WR_DATA16(y);

	LCD_WR_REG(LCD_Parameter.Wram_Cmd); 
	LCD_WR_DATA16(color); //写16位颜色
}

//函数功能：画线
//输入参数：
//x1,y1:起点坐标
//x2,y2:终点坐标
//Color;线条颜色
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color)
{
	uint16_t t; 
	int32_t xerr=0,yerr=0,delta_x,delta_y,distance; 
	int32_t incx,incy,uRow,uCol; 

	delta_x = x2 - x1;		//计算坐标增量
	delta_y = y2 - y1; 
	uRow = x1; 
	uCol = y1; 

	if(delta_x > 0)
		incx = 1;			//设置单步方向
	else
		if(delta_x == 0)
			incx = 0;		//垂直线
		else
		{
			incx = -1;
			delta_x = -delta_x;
		}
	if(delta_y > 0)
		incy = 1; 
	else
		if(delta_y == 0)
			incy = 0;		//水平线
		else
		{ 
			incy = -1;
			delta_y = -delta_y;
		}
	if(delta_x > delta_y)
		distance = delta_x;	//选取基本增量坐标轴
	else
		distance = delta_y; 
	
	for(t=0;t<=distance+1;t++ )				//划线输出
	{  
		LCD_Fast_DrawPoint(uRow,uCol,Color);//画点，使用输入参数
		xerr += delta_x ; 
		yerr += delta_y ; 
		if(xerr > distance) 
		{ 
			xerr -= distance; 
			uRow += incx; 
		} 
		if(yerr > distance) 
		{ 
			yerr -= distance; 
			uCol += incy; 
		} 
	}  
}

//函数功能：画矩形
//输入参数：
//(x1,y1),(x2,y2):矩形的对角坐标
//Color;线条颜色
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color)
{
	LCD_DrawLine(x1, y1, x2, y1, Color);
	LCD_DrawLine(x1, y1, x1, y2, Color);
	LCD_DrawLine(x1, y2, x2, y2, Color);
	LCD_DrawLine(x2, y1, x2, y2, Color);
}

//函数功能：在指定位置画一个指定大小的圆
//输入参数：
//(x,y):中心点
//r    :半径
//Color;线条颜色
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t Color)
{
	int32_t a,b;
	int32_t di;
	a = 0;
	b = r;	  
	di = 3 - (r << 1);             //判断下个点位置的标志
	while(a <= b)
	{
		LCD_Fast_DrawPoint(x0+a, y0-b, Color);             //5
		LCD_Fast_DrawPoint(x0+b, y0-a, Color);             //0           
		LCD_Fast_DrawPoint(x0+b, y0+a, Color);             //4               
		LCD_Fast_DrawPoint(x0+a, y0+b, Color);             //6 
		LCD_Fast_DrawPoint(x0-a, y0+b, Color);             //1       
		LCD_Fast_DrawPoint(x0-b, y0+a, Color);             //
		LCD_Fast_DrawPoint(x0-a, y0-b, Color);             //2             
		LCD_Fast_DrawPoint(x0-b, y0-a, Color);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)
			di += 4 * a + 6;	  
		else
		{
			di += 10 + 4 * (a - b);   
			b--;
		} 						    
	}
}

//函数功能：区域填充函数--填充单个颜色
//输入参数：
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{          
	uint32_t i;
	uint32_t xlen = 0;

	//设置窗口
	LCD_WR_REG(LCD_Parameter.Setx_Cmd); 
	LCD_WR_DATA16(sx);		//设置X方向起点
	LCD_WR_DATA16(ex-1);	//设置X方向终点

	LCD_WR_REG(LCD_Parameter.Sety_Cmd);
	LCD_WR_DATA16(sy);		//设置Y方向起点
	LCD_WR_DATA16(ey-1);	//设置Y方向终点

	LCD_WR_REG(LCD_Parameter.Wram_Cmd);	  

	xlen = (ex-sx)*(ey-sy);	//计算出总共需要写入的点数

	LCD_WriteRAM_Prepare();	//开始写入GRAM

	for(i=0;i<=xlen;i++)
	{
		LCD_WR_DATA16(color);	//显示颜色 	    
	}

	//恢复全屏窗口
	//以下设置，为窗口参数设置，设置了全屏的显示范围
	LCD_WR_REG(LCD_Parameter.Setx_Cmd); 
	LCD_WR_DATA16(0);             			//设置X方向起点
	LCD_WR_DATA16(LCD_Parameter.Width-1);	//设置X方向终点

	LCD_WR_REG(LCD_Parameter.Sety_Cmd);
	LCD_WR_DATA16(0);              			//设置Y方向起点
	LCD_WR_DATA16(LCD_Parameter.Height-1);	//设置Y方向终点		
		 
}

//绘制图形函数
void Draw_Test(void)
{
	LCD_Clear(WHITE); //清屏

	LCD_DrawLine(20, 64, 220, 128, RED);//划线函数
	LCD_DrawLine(20, 128, 220, 64, RED);//划线函数

	LCD_DrawRectangle(20, 64, 220, 128, BLUE);//绘制方形形状

	LCD_Draw_Circle(120, 96, 81, BRED);//绘制圆形
	LCD_Draw_Circle(120, 96, 80, BRED);//绘制圆形
	LCD_Draw_Circle(120, 96, 79, BRED);//绘制圆形

	HAL_Delay(2000);
}

//颜色填充测试
void Color_Test(void)
{
	LCD_Clear(GRAY0); //清屏

//	Draw_Font16B(24, 16, BLUE, "1:颜色填充测试");

	LCD_Fill(5, 5, LCD_Parameter.Width-5, LCD_Parameter.Height-5, RED);			//设置一个窗口，写入指定区域颜色
	LCD_Fill(20, 20, LCD_Parameter.Width-20, LCD_Parameter.Height-20, YELLOW);
	LCD_Fill(50, 50, LCD_Parameter.Width-50, LCD_Parameter.Height-50, BLUE);
	LCD_Fill(80, 100, LCD_Parameter.Width-80, LCD_Parameter.Height-80, MAGENTA);

	HAL_Delay(2000);
	LCD_Clear(WHITE);

	Draw_Test();	//绘图函数测试

	HAL_Delay(300);	//延时
	LCD_Clear(BLACK);
	HAL_Delay(300);
	LCD_Clear(RED);
	HAL_Delay(300);
	LCD_Clear(YELLOW);
	HAL_Delay(300);
	LCD_Clear(GREEN);
	HAL_Delay(300);
	LCD_Clear(BLUE);
	HAL_Delay(300);
}
