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
#include <stdlib.h>
#include <math.h>

#ifndef PI
 #ifdef M_PI
  #define PI M_PI
 #else
  #define PI            3.14159265358979323846  /* pi */
 #endif
#endif

#include "fft.h"

struct _struct_fft_state {
    float real[FFT_BUFFER_SIZE];
    float imag[FFT_BUFFER_SIZE];
};

static void fft_prepare (const sound_sample *input, float * re, float * im);
static void fft_calculate (float * re, float * im);
static void fft_output (const float *re, const float *im, float *output);
static int reverseBits (unsigned int initial);

static unsigned int bitReverse[FFT_BUFFER_SIZE];

static float sintable[FFT_BUFFER_SIZE / 2];
static float costable[FFT_BUFFER_SIZE / 2];

fft_state *fft_init (void)
{
    fft_state *state;
    unsigned int i;

    state = (fft_state *) malloc (sizeof(fft_state));
    if (!state)
	return NULL;

    for (i = 0; i < FFT_BUFFER_SIZE; i++) {
	bitReverse[i] = reverseBits(i);
    }
    for (i = 0; i < FFT_BUFFER_SIZE / 2; i++) {
	float j = 2 * PI * i / FFT_BUFFER_SIZE;
	costable[i] = cos(j);
	sintable[i] = sin(j);
    }

    return state;
}

void fft_perform (const sound_sample *input, float *output, fft_state *state)
{
    fft_prepare(input, state->real, state->imag);
    fft_calculate(state->real, state->imag);
    fft_output(state->real, state->imag, output);
}


void fft_close (fft_state *state)
{
    if (state)
	free(state);
}

static void fft_prepare (const sound_sample *input, float * re, float * im)
{
    unsigned int i;
    float *realptr = re;
    float *imagptr = im;
    
    for (i = 0; i < FFT_BUFFER_SIZE; i++) {
	*realptr++ = input[bitReverse[i]];
	*imagptr++ = 0;
    }
}

static void fft_output (const float * re, const float * im, float *output)
{
    float *outputptr = output;
    const float *realptr   = re;
    const float *imagptr   = im;
    float *endptr    = output + FFT_BUFFER_SIZE / 2;
 
    while (outputptr <= endptr) {
	*outputptr = (*realptr * *realptr) + (*imagptr * *imagptr);
	outputptr++; realptr++; imagptr++;
    }

    *output /= 4;
    *endptr /= 4;
}

static void fft_calculate (float * re, float * im)
{
    unsigned int i, j, k;
    unsigned int exchanges;
    float fact_real, fact_imag;
    float tmp_real, tmp_imag;
    unsigned int factfact;
    
    exchanges = 1;
    factfact = FFT_BUFFER_SIZE / 2;

    for (i = FFT_BUFFER_SIZE_LOG; i != 0; i--) {
	for (j = 0; j != exchanges; j++) {
	    fact_real = costable[j * factfact];
	    fact_imag = sintable[j * factfact];
	    
	    for (k = j; k < FFT_BUFFER_SIZE; k += exchanges << 1) {
		int k1 = k + exchanges;
		tmp_real = fact_real * re[k1] - fact_imag * im[k1];
		tmp_imag = fact_real * im[k1] + fact_imag * re[k1];
		re[k1] = re[k] - tmp_real;
		im[k1] = im[k] - tmp_imag;
		re[k]  += tmp_real;
		im[k]  += tmp_imag;
	    }
	}
	exchanges <<= 1;
	factfact >>= 1;
    }
}

static int reverseBits (unsigned int initial)
{
    unsigned int reversed = 0, loop;
    for (loop = 0; loop < FFT_BUFFER_SIZE_LOG; loop++) {
	reversed <<= 1;
	reversed += (initial & 1);
	initial >>= 1;
    }
    return reversed;
}
