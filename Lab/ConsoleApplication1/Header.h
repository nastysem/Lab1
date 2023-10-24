#ifndef IMAGE_H
#define IMAGE_H
#include <string>
#pragma pack(push, 2)
/* Названия страшненькие */
struct BITMAPFILEHEADER
{
    int16_t  bfType; //��������� ������� �����, ������ ���� BM = 0x4d42
    int32_t   bfSize; //������ ����� � ������
    int16_t    bfReserved1; //2 ����� ��������������� � ������ ���� ����������� �� 0
    int16_t    bfReserved2;
    int32_t   bfOffBits; //�������� �� ������ �� ��������� ������

} ;
#pragma pack(pop)
#pragma pack(push, 2)
struct BITMAPINFOHEADER
{
    int32_t  biSize; //������ ������ ��������� � ������, ����������� ����� �� ������ ���������
    int32_t  biWidth; //������ ������ � ��������. ����������� ����� ������ �� ������. ���� � ������������� �� ���������������
    int32_t  biHeight; //����� ����� �� ������, ���������� 2 ���������: ������ ������ � �������� (���������� �������� �����) � ������� ���������� ����� � ��������� �������� (���� �����). ������� �������� �� ���������������.
    int16_t  biPlanes; //2 �����, ����������� ���������� �������� ���������� ��� �������� ���������, ������ ������ 1
    int16_t  biBitCount; //���������� ��� �� �������, ����� 0 (������� �������� � ������� JPEG ��� PNG), 1, 4, 8, 16, 24, 32. � ��� 24
    int32_t  biCompression; //��������� �� ������ �������� ��������. BI_RGB | BI_RLE8 | BI_RLE4 |BI_BITFIELDS | BI_JPEG | BI_PNG. ������� ������������ ������ BI_RGB. ��������� ��� ������ �����������, BI_RGB �������� ���������� ������ (��� ���� ����� 0)
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
