#include <iostream>
#include <fstream>
#include "Header.h"
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
using namespace std;
class Image {
private:
    BITMAPFILEHEADER myHeader;
    BITMAPINFOHEADER myInfo;
    int stride;
    int padding;
    //метод для создания padding
    void generateStride() {
        int bytesPerPixel = myInfo.biBitCount / 8;
        int alignment = 4;
        stride = (myInfo.biWidth * bytesPerPixel) + (alignment - 1);
        stride /= alignment;
        stride *= alignment;
        padding = stride - myInfo.biWidth * bytesPerPixel;
    }
    //метод для создания буфера
    unsigned char* createBuffer() {
        unsigned char* buffer = new unsigned char[stride * myInfo.biHeight];
        return buffer;
    }
    //метод для переписывания инфы из одного буфера в другой БЕЗ нулей
    void deleteNull(unsigned char* buffer, unsigned char* firstBuffer, int width, int height, int padding, int stride) {
        int r = 0;
        int q = 0;
        for (int k = 0; k < height; k++) {
            for (r; r < 3 * width * (k + 1); r++) {
                firstBuffer[r] = buffer[q];
                q++;
            }
            for (int l = 0; l < padding; l++) {
                if (q < height * stride)
                    q++;
            }
        }
    }
    //метод для переписывания инфы из одного буфера в другой ДОБАВЛЯЯ нули
    void addNull(unsigned char* buffer, unsigned char* secondBuffer, int width, int height, int padding, int stride) {
        int m = 0;
        int n = 0;
        for (int k = 0; k < myInfo.biHeight; k++) {
            for (m; m < 3 * myInfo.biWidth * (k + 1); m++) {
                buffer[n] = secondBuffer[m];
                n++;
            }
            for (int l = 0; l < padding; l++) {
                buffer[n] = 0;
                if (n < myInfo.biHeight * stride)
                    n++;
            }
        }
    }
    //метод для генерации коэффициентов гауссова ядра
    double* generate_coeff(int radius, double sigma) {
        int sq = 2 * radius + 1;
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
        //нормируем
        for (int i = 0; i < sq * sq; i++) {
            coeff[i] /= sum;
        }
        return coeff;
    }
public:
    //чтение файла
    unsigned char* readBMP(string filename) {
        std::ifstream myPicture(filename, std::ifstream::binary);
        if (!myPicture.is_open()) {
            std::cout << "Ошибка открытия файла. Попробуйте повторить попытку или введите другое имя." << std::endl;
            myPicture.close();
            exit(1);
        }
        //читаем заголовок
        myPicture.read(reinterpret_cast<char*>(&myHeader.bfType), sizeof(myHeader.bfType));
        myPicture.read(reinterpret_cast<char*>(&myHeader.bfSize), sizeof(myHeader.bfSize));
        myPicture.read(reinterpret_cast<char*>(&myHeader.bfReserved1), sizeof(myHeader.bfReserved1));
        myPicture.read(reinterpret_cast<char*>(&myHeader.bfReserved2), sizeof(myHeader.bfReserved2));
        myPicture.read(reinterpret_cast<char*>(&myHeader.bfOffBits), sizeof(myHeader.bfOffBits));
        //проверяем условия, связанные с заголовком
        if (myHeader.bfType != 0x4d42) {
            std::cout << "Данный файл не является BMP файлом." << std::endl;
            myPicture.close();
            exit(1);
        }
        //читаем инфу
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
        //проверяем условия
        if ((myInfo.biSize != 40 && myInfo.biSize != 108 && myInfo.biSize != 124) ||
            myHeader.bfReserved1 != 0 ||
            myHeader.bfReserved2 != 0 ||
            myInfo.biPlanes != 1 ||
            myInfo.biCompression != 0 || //рассматриваем только несжатые изображения
            myInfo.biBitCount != 24) //рассматриваем только полноцветные изображения
        {
            std::cout << "Неподдерживаемый BMP формат." << std::endl;
            myPicture.close();
            exit(1);
        }
        generateStride();
        //создаем буфер
        unsigned char* buffer = createBuffer();
        //читаем в буфер
        myPicture.read(reinterpret_cast<char*> (buffer), stride * myInfo.biHeight);
        myPicture.close();
        return buffer;
    }
    //записываем в bmp
    void writeBMP(string filename, unsigned char* buffer) {
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
        //записываем данные из буфера
        myNewPicture.write(reinterpret_cast<char*> (buffer), stride * myInfo.biHeight);
        delete[]buffer;
        myNewPicture.close();
    }
    //метод для поворота изображения
    unsigned char* rotate(int angle, unsigned char* buffer) {
        //меняем высоту и ширину местами
        int tmp1 = myInfo.biWidth;
        myInfo.biWidth = myInfo.biHeight;
        myInfo.biHeight = tmp1;
        int tmp2 = stride;
        generateStride();
        //создаем padding для высоты
        int padding2 = tmp2 - myInfo.biHeight * 3;
        unsigned char* firstBuffer = new unsigned char[3 * myInfo.biWidth * myInfo.biHeight];
        unsigned char* secondBuffer = new unsigned char[3 * myInfo.biWidth * myInfo.biHeight];
        //переписываем инфу о пикселях из буфера, удаляя нули
        deleteNull(buffer, firstBuffer, myInfo.biHeight, myInfo.biWidth, padding2, tmp2);
        delete[]buffer;
        //поворачиваем либо на 90, либо на 270 градусов 
        switch (angle) {
        case(90):
            for (int i = 0; i < myInfo.biHeight; i++) {
                for (int j = 0; j < myInfo.biWidth; j++) {
                    secondBuffer[3 * (i * myInfo.biWidth + j)] = firstBuffer[3 * (j * myInfo.biHeight + i)];
                    secondBuffer[3 * (i * myInfo.biWidth + j) + 1] = firstBuffer[3 * (j * myInfo.biHeight + i) + 1];
                    secondBuffer[3 * (i * myInfo.biWidth + j) + 2] = firstBuffer[3 * (j * myInfo.biHeight + i) + 2];
                }
            }
            break;
        case(270):
            for (int i = 0; i < myInfo.biHeight; i++) {
                for (int j = 0; j < myInfo.biWidth; j++) {
                    secondBuffer[3 * (i * myInfo.biWidth + j)] = firstBuffer[3 * ((myInfo.biWidth - j - 1) * myInfo.biHeight + i)];
                    secondBuffer[3 * (i * myInfo.biWidth + j) + 1] = firstBuffer[3 * ((myInfo.biWidth - j - 1) * myInfo.biHeight + i) + 1];
                    secondBuffer[3 * (i * myInfo.biWidth + j) + 2] = firstBuffer[3 * ((myInfo.biWidth - j - 1) * myInfo.biHeight + i) + 2];
                }
            }
            break;
        }
        delete[]firstBuffer;
        unsigned char* buffer2 = createBuffer();
        //переписываем полученную инфу, добавляя нули
        addNull(buffer2, secondBuffer, myInfo.biWidth, myInfo.biHeight, padding, stride);
        delete[]secondBuffer;
        myHeader.bfSize = myInfo.biSize + 14 + stride * myInfo.biHeight;
        return buffer2;
    }
    //метод для накладывания фильтра гаусса
    unsigned char* blur_collapsed(int radius, double sigma, unsigned char* buffer) {
        unsigned char* firstBuffer = new unsigned char[3 * myInfo.biWidth * myInfo.biHeight];
        unsigned char* secondBuffer = new unsigned char[3 * myInfo.biWidth * myInfo.biHeight];
        deleteNull(buffer, firstBuffer, myInfo.biWidth, myInfo.biHeight, padding, stride);
        delete[]buffer;
        double* coeff = generate_coeff(radius, sigma);
        int sq = 2 * radius + 1;
        //накладываем фильтр на пиксели 
        int i, j, m, n;
        double a1 = 0; double a2 = 0; double a3 = 0;
        for (i = 0; i < myInfo.biHeight; i++) {
            for (j = 0; j < myInfo.biWidth; j++) {
                a1 = 0;
                a2 = 0;
                a3 = 0;
                for (m = -radius; m < radius + 1; m++) {
                    for (n = -radius; n < radius + 1; n++) {
                        if (m + i >= 0 && m + i < myInfo.biHeight && n + j >= 0 && n + j < myInfo.biWidth) {
                            a1 += coeff[sq * (m + radius) + n + radius] * firstBuffer[3 * ((i + m) * (myInfo.biWidth) + (j + n))];
                            a2 += coeff[sq * (m + radius) + n + radius] * firstBuffer[3 * ((i + m) * (myInfo.biWidth) + (j + n)) + 1];
                            a3 += coeff[sq * (m + radius) + n + radius] * firstBuffer[3 * ((i + m) * (myInfo.biWidth) + (j + n)) + 2];
                        }
                    }
                }
                secondBuffer[3 * (i * (myInfo.biWidth) + j)] = (unsigned char)a1;
                secondBuffer[3 * (i * (myInfo.biWidth) + j) + 1] = (unsigned char)a2;
                secondBuffer[3 * (i * (myInfo.biWidth) + j) + 2] = (unsigned char)a3;
            }
        }
        delete[]coeff;
        delete[]firstBuffer;
        unsigned char* buffer2 = createBuffer();
        addNull(buffer2, secondBuffer, myInfo.biWidth, myInfo.biHeight, padding, stride);
        delete[]secondBuffer;
        return buffer2;
    }
};