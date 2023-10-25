#ifndef IMAGE_H
#define IMAGE_H
#include <iostream>
#include <fstream>
#include "Struct.h"
class Image {
private:
    BITMAPFILEHEADER myHeader;
    BITMAPINFOHEADER myInfo;
    GaussFilterSum sum;
    int stride;
    int padding;
    unsigned char* buffer;
    void generateStride();
    Image* createBMP();
    Image* createRotatedBMP();
    double* generate_coeff(int, double);
    void apply_filter(Image*, double*, int, int, int);
public:
    ~Image();
    bool readBMP(std::string);
    void writeBMP(std::string);
    Image* rotate_90();
    Image* rotate_270();
    Image* blur_collapsed(int, double);
};
#endif 
