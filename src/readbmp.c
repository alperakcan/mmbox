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
 * This file contains functions for reading bitmap structures, files.
 * And a function called _showbmpfile. Usage is in readbmp.h file.
 * Have Fun..
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vga.h>
#include <vgagl.h>

#include "readbmp.h"

/*
 * Read a BITMAPFILEHEADER structure.
 */
int readBitmapFileHeader(FILE *fp, BITMAPFILEHEADER *bfh)
{
    int rc;
  
    rc = readUINT16little(fp, &(bfh->type));
    if (rc != 0)
	return rc;
    
    rc = readUINT32little(fp, &(bfh->size));
    if (rc != 0)
	return rc;
    
    rc = readINT16little(fp, &(bfh->xHotspot));
    if (rc != 0)
	return rc;
    
    rc = readINT16little(fp, &(bfh->yHotspot));
    if (rc != 0)
	return rc;
    
    rc = readUINT32little(fp, &(bfh->offsetToBits));
    return rc;
}

/*
 * Read the BITMAPHEADER structure.
 */
int readBitmapHeader(FILE *fp, BITMAPHEADER *bh)
{
    int    rc, oldFormat, bytesRead;
    UINT16 tempVal;
    
    memset(bh, 0, sizeof(BITMAPHEADER));
    
    rc = readUINT32little(fp, &(bh->size));
    if (rc != 0)
	return rc;
    bytesRead = 4;
    
    if (bh->size <= 12)
	oldFormat = 1;
    else
	oldFormat = 0;
    
    if (oldFormat)
    {
	rc = readINT16little(fp, &tempVal);
	if (rc != 0)
	    return rc;
	bh->width = tempVal;
	bytesRead += 2;
    }
    else
    {
	rc = readINT32little(fp, &(bh->width));
	if (rc != 0)
	    return rc;
	bytesRead += 4;
    }
    if (bytesRead >= bh->size)
	return 0;
    
    if (oldFormat)
    {
	rc = readINT16little(fp, &tempVal);
	if (rc != 0)
	    return rc;
	bh->height = tempVal;
	bytesRead += 2;
    }
    else
    {
	rc = readINT32little(fp, &(bh->height));
	if (rc != 0)
	    return rc;
	bytesRead += 4;
    }
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT16little(fp, &(bh->numBitPlanes));
    if (rc != 0)
	return rc;
    bytesRead += 2;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT16little(fp, &(bh->numBitsPerPlane));
    if (rc != 0)
	return rc;
    bytesRead += 2;
    if (bytesRead >= bh->size)
	return 0;
  
    rc = readUINT32little(fp, &(bh->compressionScheme));
    if (rc != 0)
	return rc;
    bytesRead += 4;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT32little(fp, &(bh->sizeOfImageData));
    if (rc != 0)
	return rc;
    bytesRead += 4;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT32little(fp, &(bh->xResolution));
    if (rc != 0)
	return rc;
    bytesRead += 4;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT32little(fp, &(bh->yResolution));
    if (rc != 0)
	return rc;
    bytesRead += 4;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT32little(fp, &(bh->numColorsUsed));
    if (rc != 0)
	return rc;
    bytesRead += 4;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT32little(fp, &(bh->numImportantColors));
    if (rc != 0)
	return rc;
    bytesRead += 4;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT16little(fp, &(bh->resolutionUnits));
    if (rc != 0)
	return rc;
    bytesRead += 2;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT16little(fp, &(bh->padding));
    if (rc != 0)
	return rc;
    bytesRead += 2;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT16little(fp, &(bh->origin));
    if (rc != 0)
	return rc;
    bytesRead += 2;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT16little(fp, &(bh->halftoning));
    if (rc != 0)
	return rc;
    bytesRead += 2;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT32little(fp, &(bh->halftoningParam1));
    if (rc != 0)
	return rc;
    bytesRead += 4;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT32little(fp, &(bh->halftoningParam2));
    if (rc != 0)
	return rc;
    bytesRead += 4;
    if (bytesRead >= bh->size)
	return 0;
  
    rc = readUINT32little(fp, &(bh->colorEncoding));
    if (rc != 0)
	return rc;
    bytesRead += 4;
    if (bytesRead >= bh->size)
	return 0;
    
    rc = readUINT32little(fp, &(bh->identifier));
    if (rc != 0)
	return rc;
    bytesRead += 4;
    if (bytesRead >= bh->size)
	return 0;
    
    return fseek(fp, (bh->size - bytesRead), SEEK_CUR);
}

/*
 * readRgb reads in a single RGB structure from the disk.
 */
int readRgb(FILE *fp, RGB *rgb, int numBytes)
{
    int rc;
    
    if (numBytes == 0)
	return 0;
    rc = readUINT8little(fp, &(rgb->blue));
    if (rc != 0)
	return rc;
    
    if (numBytes == 1)
	return 0;
    rc = readUINT8little(fp, &(rgb->green));
    if (rc != 0)
	return rc;
    
    if (numBytes == 2)
	return 0;
    rc = readUINT8little(fp, &(rgb->red));
    if (rc != 0)
	return rc;
    
    if (numBytes == 3)
	return 0;
    
    return fseek(fp, (numBytes - 3), SEEK_CUR);
}

/*
 * A color table is a block of RGB structures, all the same size.
 */
int readColorTable(FILE *fp, RGB *rgb, int numEntries, int numBytesPerEntry)
{
    int i, rc;
    
    for (i=0; i<numEntries; i++)
    {
	rc = readRgb(fp, &(rgb[i]), numBytesPerEntry);
	if (rc != 0)
	    return rc;
    }
    return 0;
}


/*
 * ReadBitsUncompressed. Reads pixel values into an array of RGB values.
 */
int readBitsUncompressed(FILE *fp, RGB *image, int width, int height,
			 int depth, RGB *colorTable)
{
    UINT8 temp;
    int   rc, padBytes, i;
    long  row, column, pixel, value;
    
    switch (depth) {
    case 1:
	pixel = 0;
	if (((width % 32) == 0) || ((width % 32) > 24))
	    padBytes = 0;
	else if ((width % 32) <= 8)
	    padBytes = 3;
	else if ((width % 32) <= 16)
	    padBytes = 2;
	else
	    padBytes = 1;

	for (row = height; row > 0; row--)
	{
	    for (column = width; column > 0; column -= 8)
	    {
		rc = readINT8little(fp, &temp);
		if (rc != 0)
		    return rc;
		for (i=0; i < ((column < 8) ? column : 8); i++)
		{
		    value = ((temp & (1 << (7-i))) == 0) ? 0 : 1;
		    image[pixel].red   = colorTable[value].red;
		    image[pixel].green = colorTable[value].green;
		    image[pixel].blue  = colorTable[value].blue;
		    pixel++;
		}
	    }
	    if (padBytes != 0)
	    {
		rc = fseek(fp, padBytes, SEEK_CUR);
		if (rc != 0)
		    return rc;
	    }
	}
	break;

    case 4:
	pixel = 0;
	if (((width % 8) == 0) || ((width % 8) > 6))
	    padBytes = 0;
	else if ((width % 8) <= 2)
	    padBytes = 3;
	else if ((width % 8) <= 4)
	    padBytes = 2;
	else
	    padBytes = 1;
	
	for (row = height; row > 0; row--)
	{
	    for (column = width; column > 0; column -= 2)
	    {
		rc = readUINT8little(fp, &temp);
		if (rc != 0)
		    return rc;
		value = temp >> 4;
		image[pixel].red   = colorTable[value].red;
		image[pixel].green = colorTable[value].green;
		image[pixel].blue  = colorTable[value].blue;
		pixel++;

		if (column == 1)
		{
		    value = temp & 0x0f;
		    image[pixel].red   = colorTable[value].red;
		    image[pixel].green = colorTable[value].green;
		    image[pixel].blue  = colorTable[value].blue;
		    pixel++;
		}
	    }
	    if (padBytes != 0)
	    {
		rc = fseek(fp, padBytes, SEEK_CUR);
		if (rc != 0)
		    return rc;
	    }
	}
	break;

    case 8:
	pixel = 0;
	padBytes = ((width % 4) == 0) ? 0 : (4 - (width % 4));

	for (row=height; row > 0; row--)
	{
	    for (column=width; column > 0; column--)
	    {
		rc = readUINT8little(fp, &temp);
		if (rc != 0)
		    return rc;
		image[pixel].red   = colorTable[temp].red;
		image[pixel].green = colorTable[temp].green;
		image[pixel].blue  = colorTable[temp].blue;
		pixel++;
	    }
	    if (padBytes != 0)
	    {
		rc = fseek(fp, padBytes, SEEK_CUR);
		if (rc != 0)
		    return rc;
	    }
	}
	break;

    case 16:
	pixel = 0;
	padBytes = ((width % 2) == 0) ? 0 : 2;
	for (row=height; row > 0; row--)
	{
	    for (column=width; column > 0; column--)
	    {
		rc = readUINT8little(fp, &temp);
		if (rc != 0)
		    return rc;
		value = ((long)temp) << 8;
		rc = readUINT8little(fp, &temp);
		if (rc != 0)
		    return rc;
		value |= temp;

		image[pixel].red   = colorTable[value].red;
		image[pixel].green = colorTable[value].green;
		image[pixel].blue  = colorTable[value].blue;
		pixel++;
	    }
	    if (padBytes != 0)
	    {
		rc = fseek(fp, padBytes, SEEK_CUR);
		if (rc != 0)
		    return rc;
	    }
	}
	break;

    case 24:
	pixel = 0;
	padBytes = width % 4;

	for (row=height; row > 0; row--)
	{
	    for (column=width; column > 0; column--)
	    {
		rc = readRgb(fp, image+pixel, 3);
		pixel++;
	    }
	    if (padBytes != 0)
	    {
		rc = fseek(fp, padBytes, SEEK_CUR);
		if (rc != 0)
		    return rc;
	    }
	}          
	break;
    }
    
    return 0;
}

/*
 * reflectYRGB takes an array of RGB vales and the dimensions they represent
 * and flips it vertically.  This will convert a bottom-left origin array to a
 * top-left origin array.
 */
void reflectYRGB(RGB *image, int width, int height)
{
    int row, col;
    RGB temp;
    
    for (row = 0; row < (height / 2); row++)
    {
	for (col = 0; col < width; col++)
	{
	    memcpy(&temp, image+(row * width + col), sizeof(RGB));
	    memcpy(image+(row * width + col),
		   image+((height - row - 1) * width + col), sizeof(RGB));
	    memcpy(image+((height - row - 1) * width + col), &temp,
		   sizeof(RGB));
	}
    }
}

/*
 * reflectYchar takes an array of char values and the dimensions they
 * represent and flips it vertically.  This will convert a bottom-left origin
 * array to a top-left origin array.
 */
void reflectYchar(char *image, int width, int height)
{
    int row, col;
    char temp;
    
    for (row = 0; row < (height / 2); row++)
    {
	for (col = 0; col < width; col++)
	{
	    temp = image[row * width + col];
	    image[row * width + col]=image[(height - row - 1) * width + col];
	    image[(height - row - 1) * width + col] = temp;
	}
    }
}

/*
 * return values:
 *	   0 - success
 *	1000 - incorrect file type for the routine called
 *	1001 - image data out of range or damaged file
 *	1002 - good data, but the routine called can't handle it (yet)
 *	1003 - out of memory allocating color table
 *	1004 - out of memory allocating image
 *	1005 - out of memory allocating image arrays
 *	1006 - Illegal image type in a multi-image array
 *
 *     other - I/O error of some kind
 */
int readSingleImageBMP(FILE *fp, RGB **argb, UINT32 *width, UINT32 *height)
{
    BITMAPFILEHEADER  bfh;
    BITMAPHEADER      bh;
    RGB              *colorTable, *image;
    int               rc, depth, inverted;
    long              numColors, numPixels, endPos;
    
    rc = readBitmapFileHeader(fp, &bfh);
    if (rc != 0)
	return rc;
    if (bfh.type != TYPE_BMP)
	return 1000;
    
    rc = readBitmapHeader(fp, &bh);
    if (rc != 0)
	return rc;
    depth = bh.numBitPlanes * bh.numBitsPerPlane;
    if ((depth > 32) ||
	(bh.compressionScheme > COMPRESSION_LAST) ||
	(bh.origin > ORIGIN_LAST) ||
	(bh.colorEncoding > COLOR_ENCODING_LAST) ||
	(bh.width < 1) ||
	(bh.height == 0))
	return 1001;
    
    if (bh.height < 0)
    {
	inverted = 0;
	bh.height = -bh.height;
    }
    else
	inverted = 1;
    
    if ((bh.numBitPlanes > 1) ||
	((bh.numBitsPerPlane != 1) &&
	 (bh.numBitsPerPlane != 4) &&
	 (bh.numBitsPerPlane != 8) &&
	 (bh.numBitsPerPlane != 16) &&
	 (bh.numBitsPerPlane != 24)) ||
	(bh.compressionScheme != COMPRESSION_NONE))
	return 1002;
    
    if (depth < 24)
    {
	numColors = 1 << depth;
	colorTable = (RGB *)calloc(numColors, sizeof(RGB));
	if (colorTable == NULL)
	    return 1003;
	if (bh.size <= 12)
	    rc = readColorTable(fp, colorTable, numColors, 3);
	else
	    rc = readColorTable(fp, colorTable, numColors, 4);
	if (rc != 0)
	{
	    free(colorTable);
	    return rc;
	}
    }
    
    endPos = ftell(fp);

    numPixels = bh.width * bh.height;
    image = (RGB *)calloc(numPixels, sizeof(RGB));
    if (image == NULL)
    {
	free (colorTable);
	return 1004;
    }
    
    rc = fseek(fp, bfh.offsetToBits, SEEK_SET);
    if (rc != 0)
    {
	free (colorTable);
	free (image);
	return rc;
    }
    
    switch (bh.compressionScheme) {
    case COMPRESSION_NONE:
	rc = readBitsUncompressed(fp, image, bh.width, bh.height, depth,
				  colorTable);
	break;
    }
    
    if (rc != 0)
    {
	free(image);
	return rc;
    }
    
    if (inverted)
	reflectYRGB(image, bh.width, bh.height);

    *argb = image;
    *width = bh.width;
    *height = bh.height;
    fseek(fp, endPos, SEEK_SET);

    if (colorTable != NULL)
	free(colorTable);
    
    return 0;
}

/*
 * Read functions.
 */
int readINT8little(FILE *f, INT8 *i)
{
    int rc;
    
    rc = fgetc(f);
    if (rc == EOF)
	return rc;
    
    *i = (rc & 0xff);
    return 0;
}

int readUINT8little(FILE *f, UINT8 *i)
{
    int  rc;
    
    rc = fgetc(f);
    if (rc == EOF)
	return rc;
    
    *i = (rc & 0xff);
    return 0;
}

int readINT16little(FILE *f, INT16 *i)
{
    int rc;
    INT16 temp = 0;
    
    temp = (fgetc(f) & 0xff);
    
    rc = fgetc(f);
    if (rc == EOF)
	return rc;
    
    temp |= ((rc & 0xff) << 8);
    *i = temp;
    return 0;
}

int readUINT16little(FILE *f, UINT16 *i)
{
    int rc;
    UINT16 temp = 0;
    
    temp = (fgetc(f) & 0xff);
    
    rc = fgetc(f);
    if (rc == EOF)
	return rc;
    
    temp |= ((rc & 0xff) << 8);
    *i = temp;
    return 0;
}

int readINT32little(FILE *f, INT32 *i)
{
    int rc;
    INT32 temp = 0;
    
    temp = ((long)fgetc(f) & 0xff);
    temp |= (((long)fgetc(f) & 0xff) << 8);
    temp |= (((long)fgetc(f) & 0xff) << 16);
    
    rc = fgetc(f);
    if (rc == EOF)
	return rc;
    
    temp |= (((long)rc & 0xff) << 24);
    *i = temp;
    return 0;
}

int readUINT32little(FILE *f, UINT32 *i)
{
    int rc;
    UINT32 temp = 0;
    
    temp = ((long)fgetc(f) & 0xff);
    temp |= (((long)fgetc(f) & 0xff) << 8);
    temp |= (((long)fgetc(f) & 0xff) << 16);
    
    rc = fgetc(f);
    if (rc == EOF)
	return rc;
    
    temp |= (((long)rc & 0xff) << 24);
    *i = temp;
    return 0;
}

/* look in readbmp.h for function details */
int _showbmpfile(char *file, int x, int y, int xo, int yo, int w, int h)
{
    FILE *fp;
    RGB **argbs;
    UINT32 *heights, *widths, row, col;
    UINT16 fileType;
    long filePos;
    int i, widdth, heigght;
    int rc;

    fp = fopen(file, "rb");
    if ( fp == NULL )
    {
#ifdef DEBUG
	perror ("Error opening source file");
#endif
	return 2;
    }
    
    filePos = ftell(fp);
    fseek(fp, filePos, SEEK_SET);

    /* calloc rutins */
    argbs = (RGB **)calloc(1, sizeof(RGB *));
    heights = (UINT32 *)calloc(1, sizeof(UINT32));
    widths = (UINT32 *)calloc(1, sizeof(UINT32));
    if (!(argbs && heights && widths))
    {
	free(argbs); free(heights); free(widths);
	rc = 1005;
    }
    
    rc = 1000;
    rc = readSingleImageBMP(fp, argbs, widths, heights);
    
    /* Error Test */
    switch(rc) {
    case 0:
#ifdef DEBUG
	printf("Got good data from file, writing results.\n");
#endif
	break;
    default:
#ifdef DEBUG
	printf("An error occured while reading file. rc = %d\n", rc);
#endif
	break;
    }
    
    fclose(fp);
    
    /* This is it */
    i=0;
    (h!=(int)NULL) ? (heigght=h+yo) : (heigght=heights[i]);
    (w!=(int)NULL) ? (widdth=w+xo) : (widdth=widths[i]);
    
    for (row=yo; row<heigght; row++)
    {
    for (col=xo; col<widdth; col++)
    {
        gl_setpixelrgb(col+x-xo, y, argbs[i][row * widths[i] + col].red,
    			     argbs[i][row * widths[i] + col].green,
    			     argbs[i][row * widths[i] + col].blue);
    }
    y++;
    }
    
    /* Clean up */
    if (argbs[i] != NULL)
        free(argbs[i]);
    free(argbs);
    free(widths);
    free(heights);
    return 0;
}
