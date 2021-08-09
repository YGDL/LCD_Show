#ifndef __FONT_H
#define __FONT_H 

#include "stdint.h"

extern const uint8_t asc2_1206[95][12]; //12*12 ASCII�ַ�������
extern const uint8_t asc2_1608[95][16]; //16*16 ASCII�ַ�������
extern const uint8_t asc2_2412[95][36]; //24*24 ASICII�ַ�������

//�����ֿ�洢����
struct typFNT_GB16
{
	uint8_t Index[2];
	uint8_t Msk[32];
};

#define USE_ONCHIP_FLASH_FONT 1    //�����Ƿ���Ƭ���ֿ�

//����5��
#define hz16_num   59   //�˴������趨�ֿ⺺���ַ��Ĵ洢���ֵĸ����������ӻ��߼����ַ�����Ҫ����Ҳ��Ӧ�޸�

extern const struct typFNT_GB16 hz16[];

//�����ֿ�洢����
struct typFNT_GB24
{
	uint8_t Index[2];	
	uint8_t Msk[72];
};

#define hz24_num   9
extern const struct typFNT_GB24 hz24[];


#endif
