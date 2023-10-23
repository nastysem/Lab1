#include "Image.cpp"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
int main() {
	Image* bmp = new Image();
	bmp->readBMP("picture.bmp");
	bmp->writeBMP("copy.bmp");
	Image* bmp2 = bmp->rotate(90);
	bmp2->writeBMP("copy2.bmp");
	delete bmp2;
	Image* bmp3 = bmp->rotate(270);
	bmp3->writeBMP("copy3.bmp");
	Image* bmp4 = bmp3->blur_collapsed(5, 1.92);
	bmp4->writeBMP("copy4.bmp");
	delete bmp4;
	delete bmp3;
	delete bmp;
	_CrtDumpMemoryLeaks();
	return(0);
}