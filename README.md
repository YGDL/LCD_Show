# LCD_Show
 目前只有ILI9341的串行驱动程序，完成了库的初步整理核移植，能够使用片上字库显示，也能够使用片外字库显示（片外字库型号暂时不知，为大越创新的SPI屏所带）。同时能够使用UTF-8编码对字符进行显示，如果使用了GB2312编码，可以在源文件中将CODING的值替换。预计将添加曲线显示、坐标显示、已经对片外uincode字库的使用。
