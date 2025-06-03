#include "main.h" 

// for PICOVGA01 examples. Format 1 byte, single array
// format: 8-bit indexed pixel graphics, palette
//  image width  :2 pixels
//  image height :8 lines

// uint8_t *
// bits per pixel 8


  const u8 Fire1Img[16] __attribute__ ((aligned(4))) = {
0xfA, 0xfA,
0xfA, 0xfA,
0xfA, 0xfA,
0xf7, 0xf7,
0xf7, 0xf7,
0xf0, 0xf0,
0xf0, 0xf0,
0xf0, 0xf0,
};
