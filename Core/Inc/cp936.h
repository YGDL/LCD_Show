#ifndef CP936_H_
#define CP936_H_

#define CODE_TABLE_SIZE 6919

typedef struct
{
	unsigned short unicode;
	unsigned short gb;
}UNICODE_GB;

extern const  UNICODE_GB code_table[6919];

extern unsigned char *UTF8_TO_GB2312(const unsigned char *str);

#endif
