#include <iostream>
#include <cstdio>
#include <vector>
#include <queue>
//#include "raspicam.h"
#include <string.h>
 
#include "types.h"
 
#pragma pack(1) 
typedef struct tagBITMAPFILEHEADER {
  WORD    bfType;
  DWORD   bfSize;
  WORD    bfReserved1;
  WORD    bfReserved2;
  DWORD   bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;
 
typedef struct tagBITMAPINFOHEADER{
  DWORD  biSize;
  LONG   biWidth;
  LONG   biHeight;
  WORD   biPlanes;
  WORD   biBitCount;
  DWORD  biCompression;
  DWORD  biSizeImage;
  LONG   biXPelsPerMeter;
  LONG   biYPelsPerMeter;
  DWORD  biClrUsed;
  DWORD  biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
 
typedef struct tagRGBQUAD {
  BYTE    rgbBlue;
  BYTE    rgbGreen;
  BYTE    rgbRed;
  BYTE    rgbReserved;
} RGBQUAD;

typedef struct tagRGB {
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
} RGB;
#pragma pack()

typedef enum 
{
	BI_RGB = 0,
	BI_RLE8 = 1,
	BI_RLE4 = 2,
	BI_BITFIELDS = 3,
	BI_JPEG = 4,
	BI_PNG = 5
} Compression;

typedef struct RI {
	int row;
	int idx;
} *pRI;



void SaveBitmapToFile( BYTE* pBitmapBits, 
					  LONG lWidth,   
					  LONG lHeight,  
					  WORD wBitsPerPixel,   
					  const unsigned long& padding_size, 
					  char *lpszFileName );
BYTE *LoadBMP(int *width, int *heigt, unsigned long *size, char *filename);
RI *FilterLines(RI *pri, int &elements);
//bool GetFrameStats(BYTE* pmatrix, int width, int height, int firstrow, FrameStats &fs);
int StreetRow(BYTE* pmatrix, int width, int height);
int firstroadrowFrows(int *rows, int count);
int findfirstroadrow(BYTE *pmatrix, int width, int height);
//void processthread();
//void actionthread();
