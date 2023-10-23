#include <iostream>
#include <fstream>
#include "Header.h"
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <string>
using namespace std;
class Image {
private:
    BITMAPFILEHEADER myHeader;
    BITMAPINFOHEADER myInfo;
    BITMAPINFOHEADER_MORE_THAN_40 myAddInfo;
    int stride;
    int padding;
    //method for padding creation
    void generateStride() {
        int bytesPerPixel = myInfo.biBitCount / 8;
        int alignment = 4;
        stride = (myInfo.biWidth * bytesPerPixel) + (alignment - 1);
        stride /= alignment;
        stride *= alignment;
        padding = stride - myInfo.biWidth * bytesPerPixel;
    }
    //иницилизирование буфера
    unsigned char* buffer;
    //method for generating Gaussian kernel coefficients
    double* generate_coeff(int radius, double sigma) {
        const int sq = 2 * radius + 1;
        double* coeff = new double[sq * sq];
        for (int i = 0; i < sq * sq; i++) {
            coeff[i] = 0;
        }
        double sum = 0;
        for (int i = -radius; i < radius + 1; i++) {
            for (int j = -radius; j < radius + 1; j++) {
                coeff[sq * (i + radius) + j + radius] = exp(-(i * i + j * j) / (2 * sigma * sigma));
                sum += coeff[sq * (i + radius) + j + radius];

            }
        }
        //normalize
        for (int i = 0; i < sq * sq; i++) {
            coeff[i] /= sum;
        }
        return coeff;
    }
public:
    //деструктор
    ~Image() {
        delete[]buffer;
    }
    //method for reading the file
    bool readBMP(string filename) {
        std::ifstream myPicture(filename, std::ifstream::binary);
        if (!myPicture.is_open()) {
            std::cout << "File opening error. Try again or enter a different file name." << std::endl;
            myPicture.close();
            return false;
        }
        //read the header
        myPicture.read(reinterpret_cast<char*>(&myHeader), sizeof(BITMAPFILEHEADER));
        //check the conditions associated with the header
        if (myHeader.bfType != 0x4d42) {
            std::cout << "This file is not a BMP file." << std::endl;
            myPicture.close();
            return false;
        }
        //read the information
        myPicture.read(reinterpret_cast<char*>(&myInfo), sizeof(BITMAPINFOHEADER));
        if (myInfo.biSize == 108 || myInfo.biSize == 124) {
            myPicture.read(reinterpret_cast<char*>(&myAddInfo.info_108), sizeof(myAddInfo.info_108));
            if (myInfo.biSize == 124) {
                myPicture.read(reinterpret_cast<char*>(&myAddInfo.info_124), sizeof(myAddInfo.info_124));
            }
        }
        //check the conditions
        if ((myInfo.biSize != 40 && myInfo.biSize != 108 && myInfo.biSize != 124) ||
            myHeader.bfReserved1 != 0 ||
            myHeader.bfReserved2 != 0 ||
            myInfo.biPlanes != 1 ||
            myInfo.biCompression != 0 || //consider only uncompressed images
            myInfo.biBitCount != 24) //consider only full-color images
        {
            std::cout << "Unsupported BMP format." << std::endl;
            myPicture.close();
            return false;
        }
        generateStride();
        buffer = new unsigned char [3 * myInfo.biWidth * myInfo.biHeight];
        //read to the buffer
        int j = 0;
        for (int i = 0; i < myInfo.biHeight; i++) {
            for (j; j < 3 * myInfo.biWidth * (i + 1); j++) {
                buffer[j] = myPicture.get();
            }
            myPicture.seekg(padding, std::ios_base::cur);
        }
        myPicture.close();
        return true;
    }
    //method for writing to the bmp
    void writeBMP(string filename) {
        std::ofstream myNewPicture(filename, std::ofstream::binary);
        myNewPicture.write(reinterpret_cast<char*>(&myHeader), sizeof(BITMAPFILEHEADER));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo), sizeof(BITMAPINFOHEADER));
        if (myInfo.biSize == 108 || myInfo.biSize == 124) {
            myNewPicture.write(reinterpret_cast<char*>(&myAddInfo.info_108), sizeof(myAddInfo.info_108));
            if (myInfo.biSize == 124) {
                myNewPicture.write(reinterpret_cast<char*>(&myAddInfo.info_124), sizeof(myAddInfo.info_124));
            }
        }
        generateStride();
        //write data from the buffer
        unsigned char* buffer2 = new unsigned char[stride];
        int k = 0; int j = 0;
        for (int i = 0; i < myInfo.biHeight; i++) {
            for (j; j < 3 * myInfo.biWidth * (i + 1); j++) {
                buffer2[k] = buffer[j];
                k++;
            }
            myNewPicture.write(reinterpret_cast<char*>(buffer2), stride);
            k = 0;
        }
        myNewPicture.close();
        delete[]buffer2;
    }
    //method for image rotation
    Image* rotate(int angle) {
        Image* bmp = new Image();
        bmp->myHeader = this->myHeader;
        bmp->myInfo = this->myInfo;
        bmp->myAddInfo = this->myAddInfo;
        //change the height and width in places
        bmp->myInfo.biWidth = this->myInfo.biHeight;
        bmp->myInfo.biHeight = this->myInfo.biWidth;
        bmp->buffer = new unsigned char[3 * bmp->myInfo.biWidth * bmp->myInfo.biHeight];
        //turn either 90 or 270 degrees
        switch (angle) {
        case(90):
            for (int i = 0; i < bmp->myInfo.biHeight; i++) {
                for (int j = 0; j < bmp->myInfo.biWidth; j++) {
                    bmp->buffer[3 * (i * bmp->myInfo.biWidth + j)] = this->buffer[3 * (j * this->myInfo.biWidth + i)];
                    bmp->buffer[3 * (i * bmp->myInfo.biWidth + j) + 1] = this->buffer[3 * (j * this->myInfo.biWidth + i) + 1];
                    bmp->buffer[3 * (i * bmp->myInfo.biWidth + j) + 2] = this->buffer[3 * (j * this->myInfo.biWidth + i) + 2];
                }
            }
            break;
        case(270):
            for (int i = 0; i < bmp->myInfo.biHeight; i++) {
                for (int j = 0; j < bmp->myInfo.biWidth; j++) {
                    bmp->buffer[3 * (i * bmp->myInfo.biWidth + j)] = this->buffer[3 * ((this->myInfo.biHeight - j - 1) * this->myInfo.biWidth + i)];
                    bmp->buffer[3 * (i * bmp->myInfo.biWidth + j) + 1] = this->buffer[3 * ((this->myInfo.biHeight - j - 1) * this->myInfo.biWidth + i) + 1];
                    bmp->buffer[3 * (i * bmp->myInfo.biWidth + j) + 2] = this->buffer[3 * ((this->myInfo.biHeight - j - 1) * this->myInfo.biWidth + i) + 2];
                }
            }
            break;
        }
        return bmp;
    }
    // method for applying a Gauss filter
    Image* blur_collapsed(int radius, double sigma) {
        Image* bmp = new Image();
        bmp->myHeader = this->myHeader;
        bmp->myInfo = this->myInfo;
        bmp->myAddInfo = this->myAddInfo;
        bmp->buffer = new unsigned char[3 * bmp->myInfo.biWidth * bmp->myInfo.biHeight];
        double* coeff = generate_coeff(radius, sigma);
        int sq = 2 * radius + 1;
        //apply a filter on pixels
        int i, j, m, n;
        double a1 = 0; double a2 = 0; double a3 = 0;
        //зато гаусс несложный, а так придется переделывать всю логику наложения фильтра гаусса
        for (i = 0; i < bmp->myInfo.biHeight; i++) {
            for (j = 0; j < bmp->myInfo.biWidth; j++) {
                a1 = 0;
                a2 = 0;
                a3 = 0;
                for (m = -radius; m < radius + 1; m++) {
                    for (n = -radius; n < radius + 1; n++) {
                        //condition for correct image recording
                        if (m + i >= 0 && m + i < bmp->myInfo.biHeight && n + j >= 0 && n + j < bmp->myInfo.biWidth) {
                            a1 += coeff[sq * (m + radius) + n + radius] * this->buffer[3 * ((i + m) * (this->myInfo.biWidth) + (j + n))];
                            a2 += coeff[sq * (m + radius) + n + radius] * this->buffer[3 * ((i + m) * (this->myInfo.biWidth) + (j + n)) + 1];
                            a3 += coeff[sq * (m + radius) + n + radius] * this->buffer[3 * ((i + m) * (this->myInfo.biWidth) + (j + n)) + 2];
                        }
                    }
                }
                bmp->buffer[3 * (i * bmp->myInfo.biWidth + j)] = (unsigned char)a1;
                bmp->buffer[3 * (i * bmp->myInfo.biWidth + j) + 1] = (unsigned char)a2;
                bmp->buffer[3 * (i * bmp->myInfo.biWidth + j) + 2] = (unsigned char)a3;
            }
        }
        delete[]coeff;
        return bmp;
    }
};

    