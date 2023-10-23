#ifndef IMAGE_H
#define IMAGE_H
#include <string>
#pragma pack(push, 2)
struct BITMAPFILEHEADER
{
    int16_t  bfType; //сигнатура формата файла, должно быть BM = 0x4d42
    int32_t   bfSize; //размер файла в байтах
    int16_t    bfReserved1; //2 байта зарезервированы и должны быть установлены на 0
    int16_t    bfReserved2;
    int32_t   bfOffBits; //смещение от начала до растровых данных

} ;
#pragma pack(pop)
#pragma pack(push, 2)
struct BITMAPINFOHEADER
{
    int32_t  biSize; //размер данной структуры в байтах, указывающий также на версию структуры
    int32_t  biWidth; //ширина растра в пикселях. указывается целым числом со знаком. ноль и отрицательные не документированы
    int32_t  biHeight; //целое число со знаком, содержащее 2 параметра: высота растра в пикселях (абсолютное значение числа) и порядок следования строк в двумерных массивах (знак числа). нулевое значение не документировано.
    int16_t  biPlanes; //2 байта, указывающие количество цветовых плоскостей для целевого устройста, всегда равное 1
    int16_t  biBitCount; //количество бит на пиксель, равно 0 (пиксели хранятся в формате JPEG или PNG), 1, 4, 8, 16, 24, 32. У нас 24
    int32_t  biCompression; //указывает на способ хранения пикселей. BI_RGB | BI_RLE8 | BI_RLE4 |BI_BITFIELDS | BI_JPEG | BI_PNG. реально используется только BI_RGB. Указывает тип сжатия изображения, BI_RGB означает отсутствие сжатия (это поле равно 0)
    int32_t  info_40[5];

};
#pragma pack(pop)
#pragma pack(push, 2)
struct BITMAPINFOHEADER_MORE_THAN_40 {
    int32_t  info_108[17];
    int32_t  info_124[4];
};
#pragma pack(pop)
#endif 
