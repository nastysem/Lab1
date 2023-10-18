#include <iostream>
#include <fstream>
#include "Header.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cmath>    
using namespace std;
class Image {
private:
    BITMAPFILEHEADER myHeader;
    BITMAPINFOHEADER myInfo;
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
    //method for buffer creation
    unsigned char** createBuffer() {
        unsigned char** buffer = new unsigned char* [myInfo.biHeight];
        for (int i = 0; i < myInfo.biHeight; i++) {
            buffer[i] = new unsigned char[3 * myInfo.biWidth];
        }
        return buffer;
    }
    
    //method for generating Gaussian kernel coefficients
    double* generate_coeff(int radius, double sigma) {
        const int sq = 2 * radius + 1;
        double* coeff = new double[sq*sq];
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
    //method for reading the file
    unsigned char** readBMP(string filename) {
        std::ifstream myPicture(filename, std::ifstream::binary);
        if (!myPicture.is_open()) {
            std::cout << "File opening error. Try again or enter a different file name." << std::endl;
            myPicture.close();
            exit(1);
        }
        //read the header
        myPicture.read(reinterpret_cast<char*>(&myHeader.bfType), sizeof(myHeader.bfType));
        myPicture.read(reinterpret_cast<char*>(&myHeader.bfSize), sizeof(myHeader.bfSize));
        myPicture.read(reinterpret_cast<char*>(&myHeader.bfReserved1), sizeof(myHeader.bfReserved1));
        myPicture.read(reinterpret_cast<char*>(&myHeader.bfReserved2), sizeof(myHeader.bfReserved2));
        myPicture.read(reinterpret_cast<char*>(&myHeader.bfOffBits), sizeof(myHeader.bfOffBits));
        //check the conditions associated with the header
        if (myHeader.bfType != 0x4d42) {
            std::cout << "This file is not a BMP file." << std::endl;
            myPicture.close();
            exit(1);
        }
        //read the information
        myPicture.read(reinterpret_cast<char*>(&myInfo.biSize), sizeof(myInfo.biSize));
        myPicture.read(reinterpret_cast<char*>(&myInfo.biWidth), sizeof(myInfo.biWidth));
        myPicture.read(reinterpret_cast<char*>(&myInfo.biHeight), sizeof(myInfo.biHeight));
        myPicture.read(reinterpret_cast<char*>(&myInfo.biPlanes), sizeof(myInfo.biPlanes));
        myPicture.read(reinterpret_cast<char*>(&myInfo.biBitCount), sizeof(myInfo.biBitCount));
        myPicture.read(reinterpret_cast<char*>(&myInfo.biCompression), sizeof(myInfo.biCompression));
        myPicture.read(reinterpret_cast<char*>(&myInfo.biSizeImage), sizeof(myInfo.biSizeImage));
        myPicture.read(reinterpret_cast<char*>(&myInfo.biXPelsPerMeter), sizeof(myInfo.biXPelsPerMeter));
        myPicture.read(reinterpret_cast<char*>(&myInfo.biYPelsPerMeter), sizeof(myInfo.biYPelsPerMeter));
        myPicture.read(reinterpret_cast<char*>(&myInfo.biClrUsed), sizeof(myInfo.biClrUsed));
        myPicture.read(reinterpret_cast<char*>(&myInfo.biClrImportant), sizeof(myInfo.biClrImportant));
        if (myInfo.biSize == 108 || myInfo.biSize == 124)
        {
            myPicture.read(reinterpret_cast<char*>(&myInfo.biRedMask), sizeof(myInfo.biRedMask));
            myPicture.read(reinterpret_cast<char*>(&myInfo.biGreenMask), sizeof(myInfo.biGreenMask));
            myPicture.read(reinterpret_cast<char*>(&myInfo.biBlueMask), sizeof(myInfo.biBlueMask));
            myPicture.read(reinterpret_cast<char*>(&myInfo.biAlphaMask), sizeof(myInfo.biAlphaMask));
            myPicture.read(reinterpret_cast<char*>(&myInfo.biCSType), sizeof(myInfo.biCSType));
            myPicture.read(reinterpret_cast<char*>(&myInfo.biEndpoints), sizeof(myInfo.biEndpoints));
            myPicture.read(reinterpret_cast<char*>(&myInfo.biGammaRed), sizeof(myInfo.biGammaRed));
            myPicture.read(reinterpret_cast<char*>(&myInfo.biGammaGreen), sizeof(myInfo.biGammaGreen));
            myPicture.read(reinterpret_cast<char*>(&myInfo.biGammaBlue), sizeof(myInfo.biGammaBlue));
            if (myInfo.biSize == 124) {
                myPicture.read(reinterpret_cast<char*>(&myInfo.biIntent), sizeof(myInfo.biIntent));
                myPicture.read(reinterpret_cast<char*>(&myInfo.biProfileData), sizeof(myInfo.biProfileData));
                myPicture.read(reinterpret_cast<char*>(&myInfo.biProfileSize), sizeof(myInfo.biProfileSize));
                myPicture.read(reinterpret_cast<char*>(&myInfo.biReserved), sizeof(myInfo.biReserved));
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
            exit(1);
        }
        generateStride();
        //read to the buffer
        unsigned char** buffer = createBuffer();
        for (int i = 0; i < myInfo.biHeight; i++) {
            myPicture.read(reinterpret_cast<char*>(buffer[i]), 3 * myInfo.biWidth);
            myPicture.seekg(padding, std::ios_base::cur);
        }
        myPicture.close();
        return buffer;
    }
    //method for writing to the bmp
    void writeBMP(string filename, unsigned char** buffer) {
        std::ofstream myNewPicture(filename, std::ofstream::binary);
        myNewPicture.write(reinterpret_cast<char*>(&myHeader.bfType), sizeof(myHeader.bfType));
        myNewPicture.write(reinterpret_cast<char*>(&myHeader.bfSize), sizeof(myHeader.bfSize));
        myNewPicture.write(reinterpret_cast<char*>(&myHeader.bfReserved1), sizeof(myHeader.bfReserved1));
        myNewPicture.write(reinterpret_cast<char*>(&myHeader.bfReserved2), sizeof(myHeader.bfReserved2));
        myNewPicture.write(reinterpret_cast<char*>(&myHeader.bfOffBits), sizeof(myHeader.bfOffBits));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo.biSize), sizeof(myInfo.biSize));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo.biWidth), sizeof(myInfo.biWidth));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo.biHeight), sizeof(myInfo.biHeight));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo.biPlanes), sizeof(myInfo.biPlanes));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo.biBitCount), sizeof(myInfo.biBitCount));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo.biCompression), sizeof(myInfo.biCompression));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo.biSizeImage), sizeof(myInfo.biSizeImage));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo.biXPelsPerMeter), sizeof(myInfo.biXPelsPerMeter));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo.biYPelsPerMeter), sizeof(myInfo.biYPelsPerMeter));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo.biClrUsed), sizeof(myInfo.biClrUsed));
        myNewPicture.write(reinterpret_cast<char*>(&myInfo.biClrImportant), sizeof(myInfo.biClrImportant));
        if (myInfo.biSize == 108 || myInfo.biSize == 124)
        {
            myNewPicture.write(reinterpret_cast<char*>(&myInfo.biRedMask), sizeof(myInfo.biRedMask));
            myNewPicture.write(reinterpret_cast<char*>(&myInfo.biGreenMask), sizeof(myInfo.biGreenMask));
            myNewPicture.write(reinterpret_cast<char*>(&myInfo.biBlueMask), sizeof(myInfo.biBlueMask));
            myNewPicture.write(reinterpret_cast<char*>(&myInfo.biAlphaMask), sizeof(myInfo.biAlphaMask));
            myNewPicture.write(reinterpret_cast<char*>(&myInfo.biCSType), sizeof(myInfo.biCSType));
            myNewPicture.write(reinterpret_cast<char*>(&myInfo.biEndpoints), sizeof(myInfo.biEndpoints));
            myNewPicture.write(reinterpret_cast<char*>(&myInfo.biGammaRed), sizeof(myInfo.biGammaRed));
            myNewPicture.write(reinterpret_cast<char*>(&myInfo.biGammaGreen), sizeof(myInfo.biGammaGreen));
            myNewPicture.write(reinterpret_cast<char*>(&myInfo.biGammaBlue), sizeof(myInfo.biGammaBlue));
            if (myInfo.biSize == 124) {
                myNewPicture.write(reinterpret_cast<char*>(&myInfo.biIntent), sizeof(myInfo.biIntent));
                myNewPicture.write(reinterpret_cast<char*>(&myInfo.biProfileData), sizeof(myInfo.biProfileData));
                myNewPicture.write(reinterpret_cast<char*>(&myInfo.biProfileSize), sizeof(myInfo.biProfileSize));
                myNewPicture.write(reinterpret_cast<char*>(&myInfo.biReserved), sizeof(myInfo.biReserved));
            }
        }
        generateStride();
        //write data from the buffer
        unsigned char* buffer2 = new unsigned char[stride];
        for (int i = 0; i < myInfo.biHeight; i++) {
            for (int j = 0; j < 3 * myInfo.biWidth; j++) {
                buffer2[j] = buffer[i][j];
            }
            myNewPicture.write(reinterpret_cast<char*>(buffer2), stride);
        }
        myNewPicture.close();
        delete[]buffer2;
        for (int i = 0; i < myInfo.biHeight; i++) {
            delete[]buffer[i];
        }
        delete[]buffer;
    }
    //method for image rotation
    unsigned char** rotate(int angle, unsigned char** buffer) {
        //change the height and width in places
        int tmp1 = myInfo.biWidth;
        myInfo.biWidth = myInfo.biHeight;
        myInfo.biHeight = tmp1;
        int tmp2 = stride;
        generateStride();
        //create padding for height
        int padding2 = tmp2 - myInfo.biHeight * 3;
        unsigned char** buffer2 = createBuffer();
        //turn either 90 or 270 degrees
        switch (angle) {
        case(90):
            for (int i = 0; i < myInfo.biHeight; i++) {
                for (int j = 0; j < myInfo.biWidth; j++) {
                    buffer2[i][3 * j] = buffer[j][3 * i];
                    buffer2[i][3 * j + 1] = buffer[j][3 * i + 1];
                    buffer2[i][3 * j + 2] = buffer[j][3 * i + 2];
                }
            }
            break;
        case(270):
            for (int i = 0; i < myInfo.biHeight; i++) {
                for (int j = 0; j < myInfo.biWidth; j++) {
                    buffer2[i][3 * j] = buffer[myInfo.biWidth - j - 1][3 * i];
                    buffer2[i][3 * j + 1] = buffer[myInfo.biWidth - j - 1][3 * i + 1];
                    buffer2[i][3 * j + 2] = buffer[myInfo.biWidth - j - 1][3 * i + 2];
                }
            }
            break;
        }
        for (int i = 0; i < myInfo.biWidth; i++) {
            delete[]buffer[i];
        }
        delete[]buffer;
        return buffer2;
    }
    // method for applying a Gauss filter
    unsigned char** blur_collapsed(int radius, double sigma, unsigned char** buffer) {
        double* coeff = generate_coeff(radius, sigma);
        int sq = 2 * radius + 1;
        //apply a filter on pixels
        int i, j, m, n;
        double a1 = 0; double a2 = 0; double a3 = 0;
        unsigned char** buffer2 = createBuffer();
        for (i = 0; i < myInfo.biHeight; i++) {
            for (j = 0; j < myInfo.biWidth; j++) {
                a1 = 0;
                a2 = 0;
                a3 = 0;
                for (m = -radius; m < radius + 1; m++) {
                    for (n = -radius; n < radius + 1; n++) {
                        //condition for correct image recording
                        if (m + i >= 0 && m + i < myInfo.biHeight && n + j >= 0 && n + j < myInfo.biWidth) {
                            a1 += coeff[sq * (m + radius) + n + radius] * buffer[i + m][3*(j + n)];
                            a2 += coeff[sq * (m + radius) + n + radius] * buffer[i + m][3*(j + n) + 1];
                            a3 += coeff[sq * (m + radius) + n + radius] * buffer[i + m][3*(j + n) + 2];
                        }
                    }
                }
                buffer2[i][3 * j] = (unsigned char)a1;
                buffer2[i][3 * j + 1] = (unsigned char)a2;
                buffer2[i][3 * j + 2] = (unsigned char)a3;
            }
        }
        delete[]coeff;
        for (int i = 0; i < myInfo.biHeight; i++) {
            delete[]buffer[i];
        }
        delete[]buffer;
        return buffer2;
    }
};

    