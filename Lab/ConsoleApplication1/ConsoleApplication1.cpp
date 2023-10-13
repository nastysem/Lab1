#include <iostream>
#include <fstream>
#include "Image.cpp"

int main() {
	setlocale(LC_CTYPE, "rus");
	Image bmp;
	bmp.writeBMP("copy.bmp", bmp.readBMP("picture.bmp"));
	bmp.writeBMP("copy2.bmp", bmp.rotate(90, bmp.readBMP("picture.bmp")));
	bmp.writeBMP("copy3.bmp", bmp.rotate(270, bmp.readBMP("picture.bmp")));
	bmp.writeBMP("copy4.bmp", bmp.blur_collapsed(5, 1.92, bmp.rotate(270, bmp.readBMP("picture.bmp"))));
	return(0);
}