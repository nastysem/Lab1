#include <iostream>
#include <fstream>
#include "Image.h"
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

    //method for padding creation
    void Image::generateStride() {
        padding = 0;
        stride = 3 * myInfo.biWidth;
        if (stride % 4 != 0) {
            padding = 4 - stride % 4;
            stride += padding;
        }
    }
    //method for bmp creation with "empty" buffer
    Image* Image::createBMP() {
        Image* bmp = new Image();
        bmp->myHeader = this->myHeader;
        bmp->myInfo = this->myInfo;
        bmp->buffer = new unsigned char[3 * bmp->myInfo.biWidth * bmp->myInfo.biHeight];
        return bmp;
    }
    Image* Image::createRotatedBMP() {
        Image* bmp = createBMP();
        bmp->myInfo.biWidth = this->myInfo.biHeight;
        bmp->myInfo.biHeight = this->myInfo.biWidth;
        return bmp;
    }
    //method for generating Gaussian kernel coefficients
    double* Image::generate_coeff(int radius, double sigma) {
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
    //makes sum for every pizel 
    void Image::apply_filter(Image* bmp, double* coeff, int radius, int i, int j) {
        int sq = 2 * radius + 1;
        for (int m = -radius; m < radius + 1; m++) {
            for (int n = -radius; n < radius + 1; n++) {
                //condition for correct image recording
                if (m + i >= 0 && m + i < bmp->myInfo.biHeight && n + j >= 0 && n + j < bmp->myInfo.biWidth) {
                    bmp->sum.a1 += coeff[sq * (m + radius) + n + radius] * this->buffer[3 * ((i + m) * (this->myInfo.biWidth) + (j + n))];
                    bmp->sum.a2 += coeff[sq * (m + radius) + n + radius] * this->buffer[3 * ((i + m) * (this->myInfo.biWidth) + (j + n)) + 1];
                    bmp->sum.a3 += coeff[sq * (m + radius) + n + radius] * this->buffer[3 * ((i + m) * (this->myInfo.biWidth) + (j + n)) + 2];
                }
            }
        }
    }
    //destructor
    Image::~Image() {
        delete[]buffer;
    }
    //method for reading the file
    bool Image::readBMP(std::string filename) {
        std::ifstream myPicture(filename, std::ifstream::binary);
        if (!myPicture.is_open()) {
            std::cerr << "File opening error. Try again or enter a different file name." << std::endl;
            myPicture.close();
            return false;
        }
        //read the header
        myPicture.read(reinterpret_cast<char*>(&myHeader), sizeof(BITMAPFILEHEADER));
        //check the conditions associated with the header
        if (myHeader.bfType != 0x4d42) {
            std::cerr << "This file is not a BMP file." << std::endl;
            myPicture.close();
            return false;
        }
        //read the information
        myPicture.read(reinterpret_cast<char*>(&myInfo), sizeof(BITMAPINFOHEADER));
        //check the conditions
        if ((myInfo.biSize != 40 && myInfo.biSize != 108 && myInfo.biSize != 124) ||
            myHeader.bfReserved1 != 0 ||
            myHeader.bfReserved2 != 0 ||
            myInfo.biPlanes != 1 ||
            myInfo.biCompression != 0 || //consider only uncompressed images
            myInfo.biBitCount != 24) //consider only full-color images
        {
            std::cerr << "Unsupported BMP format." << std::endl;
            myPicture.close();
            return false;
        }
        generateStride();
        buffer = new unsigned char [3 * myInfo.biWidth * myInfo.biHeight];
        //read to the buffer
        int j = 0;
        for (int i = 0; i < myInfo.biHeight; i++) {
            for (; j < 3 * myInfo.biWidth * (i + 1); j++) {
                buffer[j] = myPicture.get();
            }
            myPicture.seekg(padding, std::ios_base::cur);
        }
        myPicture.close();
        return true;
    }
    //method for writing to the bmp
    void Image::writeBMP(std::string filename) {
        std::ofstream myNewPicture(filename, std::ofstream::binary);
        myNewPicture.write(reinterpret_cast<char*>(&myHeader), sizeof(BITMAPFILEHEADER));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo), sizeof(BITMAPINFOHEADER));
        generateStride();
        //write data from the buffer
        unsigned char* buffer2 = new unsigned char[stride];
        int k = 0; int j = 0;
        for (int i = 0; i < myInfo.biHeight; i++) {
            for (;j < 3 * myInfo.biWidth * (i + 1); j++) {
                buffer2[k] = buffer[j];
                k++;
            }
            myNewPicture.write(reinterpret_cast<char*>(buffer2), stride);
            k = 0;
        }
        myNewPicture.close();
        delete[]buffer2;
    }
    //methods for image rotation
    Image* Image::rotate_90() {
        Image* bmp = createRotatedBMP();
        //turn 90 degrees
        for (int i = 0; i < bmp->myInfo.biHeight; i++) {
            for (int j = 0; j < bmp->myInfo.biWidth; j++) {
                bmp->buffer[3 * (i * bmp->myInfo.biWidth + j)] = this->buffer[3 * (j * this->myInfo.biWidth + i)];
                bmp->buffer[3 * (i * bmp->myInfo.biWidth + j) + 1] = this->buffer[3 * (j * this->myInfo.biWidth + i) + 1];
                bmp->buffer[3 * (i * bmp->myInfo.biWidth + j) + 2] = this->buffer[3 * (j * this->myInfo.biWidth + i) + 2];
            }
        }
        return bmp;
    }
    Image* Image::rotate_270() {
        Image* bmp = createRotatedBMP();
        //turn 270 degrees
        for (int i = 0; i < bmp->myInfo.biHeight; i++) {
            for (int j = 0; j < bmp->myInfo.biWidth; j++) {
                bmp->buffer[3 * (i * bmp->myInfo.biWidth + j)] = this->buffer[3 * ((this->myInfo.biHeight - j - 1) * this->myInfo.biWidth + i)];
                bmp->buffer[3 * (i * bmp->myInfo.biWidth + j) + 1] = this->buffer[3 * ((this->myInfo.biHeight - j - 1) * this->myInfo.biWidth + i) + 1];
                bmp->buffer[3 * (i * bmp->myInfo.biWidth + j) + 2] = this->buffer[3 * ((this->myInfo.biHeight - j - 1) * this->myInfo.biWidth + i) + 2];
            }
        }
        return bmp;
    }
    // method for applying a Gauss filter
    Image* Image::blur_collapsed(int radius, double sigma) {
        Image* bmp = createBMP();
        double* coeff = generate_coeff(radius, sigma);
        int sq = 2 * radius + 1;
        //apply a filter on pixels
        for (int i = 0; i < bmp->myInfo.biHeight; i++) {
            for (int j = 0; j < bmp->myInfo.biWidth; j++) {
                bmp->sum.a1 = 0;
                bmp->sum.a2 = 0;
                bmp->sum.a3 = 0;
                apply_filter(bmp, coeff, radius, i, j);
                bmp->buffer[3 * (i * bmp->myInfo.biWidth + j)] = (unsigned char)bmp->sum.a1;
                bmp->buffer[3 * (i * bmp->myInfo.biWidth + j) + 1] = (unsigned char)bmp->sum.a2;
                bmp->buffer[3 * (i * bmp->myInfo.biWidth + j) + 2] = (unsigned char)bmp->sum.a3;
            }
        }
        delete[]coeff;
        return bmp;
    }


    