/*
    Copyright (C) 2002  Alper Akcan

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    distch@hotmail.com
    icq : 651561
    tel : 0532 414 05 12
*/
/*
 * An ugly function to show bmp files on svgalib with gl_setpixel
 *
 * (x,y) 
 *     o-------------------  w
 *     |
 *     |      (xo,yo)
 *     |            o------  w
 *   h |         h  |
 *     |            |
 *
 * Usage is like gl_putboxpart.
 *
 * If you like to show image, at position (x,y), as it is;
 *	_showbmpfile(char *file, x, y, NULL, NULL, NULL, NULL);
 *
 * Or to show some part of the image to the ends of its dimentions
 * at position (x,y);
 *	_showbmpfile(char *file, x, y, xo, yo, NULL, NULL);
 *
 * Last, to show some part of the image with w, h at position (x,y);
 * 	_showbmpfile(char *file, x, y, xo, yo, w, h);
 */
int _showbmpfile(char *file, int x, int y, int xo, int yo, int w, int h);

/*
 * Below taken from endian.h and readbmp.h of Dr. Dobbs BMP example source.
 * This file and .c are slightly modified versions of them.
 */
#ifndef __BMPTYPES_H_INCLUDED__
#define __BMPTYPES_H_INCLUDED__

typedef char           INT8;
typedef short          INT16;
typedef long           INT32;
typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned long  UINT32;

#define TYPE_BMP        (0x4d42)   /* 'BM' */
   
#define COMPRESSION_NONE       (0)
#define COMPRESSION_RLE_8      (1)
#define COMPRESSION_RLE_4      (2)
#define COMPRESSION_HUFFMAN1D  (3)
#define COMPRESSION_BITFIELDS  (3)
#define COMPRESSION_RLE_24     (4)
#define COMPRESSION_LAST       (4)
   
#define UNITS_PELS_PER_METER (0)
#define UNITS_LAST           (0)

#define ORIGIN_LOWER_LEFT  (0)
#define ORIGIN_LAST        (0)

#define HALFTONING_NONE             (0)
#define HALFTONING_ERROR_DIFFUSION  (1)
#define HALFTONING_PANDA            (2)
#define HALFTONING_SUPER_CIRCLE     (3)
#define HALFTONING_LAST             (3)
   
#define COLOR_ENCODING_RGB   (0)
#define COLOR_ENCODING_LAST  (0)

typedef struct BITMAPFILEHEADER
{
    UINT16    type;
    UINT32    size;
    INT16     xHotspot;
    INT16     yHotspot;
    UINT32    offsetToBits;
} BITMAPFILEHEADER;

typedef struct BITMAPHEADER
{
    UINT32 size;
    INT32  width;
    INT32  height;
    UINT16 numBitPlanes;
    UINT16 numBitsPerPlane;
    UINT32 compressionScheme;
    UINT32 sizeOfImageData;
    UINT32 xResolution;
    UINT32 yResolution;
    UINT32 numColorsUsed;
    UINT32 numImportantColors;
    UINT16 resolutionUnits;
    UINT16 padding;
    UINT16 origin;
    UINT16 halftoning;
    UINT32 halftoningParam1;
    UINT32 halftoningParam2;
    UINT32 colorEncoding;
    UINT32 identifier;
} BITMAPHEADER;

typedef struct RGB
{
    UINT8 blue;
    UINT8 green;
    UINT8 red;
} RGB;

#endif

#ifndef __READBMP_H_INCLUDED__
#define __READBMP_H_INCLUDED__

int readBitmapFileHeader(FILE *fp, BITMAPFILEHEADER *bfh);
int readBitmapHeader(FILE *fp, BITMAPHEADER *bh);
int readRgb(FILE *fp, RGB *rgb, int numBytes);
int readColorTable(FILE *fp, RGB *rgb, int numEntries, int numBytesPerEntry);

int readBitsUncompressed(FILE *fp, RGB *image, int width, int height,
			 int depth, RGB* colorTable);
void reflectYRGB(RGB *image, int width, int height);
void reflectYchar(char *image, int width, int height);

int readSingleImageBMP(FILE *fp, RGB **argb, UINT32 *width, UINT32 *height);

#endif

#ifndef __ENDIAN_H_INCLUDED__
#define __ENDIAN_H_INCLUDED__
 
int readINT8little(FILE *f, INT8 *i);
int readINT16little(FILE *f, INT16 *i);
int readINT32little(FILE *f, INT32 *i);
int readUINT8little(FILE *f, UINT8 *i);
int readUINT16little(FILE *f, UINT16 *i);
int readUINT32little(FILE *f, UINT32 *i);

#endif
