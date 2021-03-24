/*

	Z88DK base graphics libraries examples
	3D Globe, inspired to a program by José Baume
	
	Set up the view with
	B = 0.0 .. 360.0   <- globe rotation
	A = 25.0   <- Globe inclinationh

	to build:  zcc +<target> <stdio options> -lm -create-app globe.c
	
	Examples:
	
	  zcc +c128 -create-app -lgfx128hr -lm -Dhires globe.c
	  
	  zcc +zx -lndos -create-app -lm globe.c
	  
	  zcc +ts2068 -lgfx2068hr -lm -Dhires -create-app globe.c
	  
	  zcc  +enterprise  -lm -create-app -lgfxephr -Dhires -lm globe.c
	   
	  zcc +cpm -subtype=tiki100 -create-app -Dhires -lm globe.c

	  zcc +zx81 -clib=wrx -subtype=wrx -create-app -lm -DVIEW=1 globe.c
	  (32K) zcc +zx81 -clib=wrxansi -subtype=wrx  -lndos -create-app -lm globe.c
	  zcc +zx81 -clib=mt  -create-app -lm globe.c
	  (after loading to activate the G007 graphics mode type CLS 2 and SLOW 4, then RUN)
	  zcc +zx81 -clib=g007  -create-app -lm -DVIEW=1  globe.c
	  zcc +zx81 -clib=mt -create-app -lm -VIEW=1 globe.c
	
	GSX graphics on CP/M (please use GENGRAF on the resulting COM file):
      zcc +cpm -oglobe -lgfxgsx -create-app -llib3d -lm -Dhires -pragma-redirect=fputc_cons=putc4x6 globe.c

	$Id: globe.c $

*/

// Target specific options (TIKI-100, ZX81..)

//#pragma output nogfxglobals
//#pragma output hrgpage = 36096


#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <games.h>

float R;
float I;
float X, Y;
float Q, E, O, U, W, H, K, L;
float J, M, Z;

int view;

int corners, eptr, count;

//#define TRANSPARENT 1


float A, B;

// GRID (remove this definition to hide the grid)
#define GRID 45.0

#define STEP 10.5

int world[] = {

/* South Pole */
28,
-8064, -7168, -8192, -7680, -8448, -8320, -9344, -9600, -9344, -10880, -9344, -12800, -9600, -12800, -9344, -16000,
-9600, -17536, -9984, -21120, -9933, 20992, -9216, 21760, -8704, 19840, -8448, 17280, -8448, 14720, -8448, 11520,
-8896, 9600, -8704, 8960, -8448, 7040, -8832, 5120, -8960, 2560, -8960, 0, -9088, -1280, -9472, -2560, -9984, -4480,
-9600, -7680, -8576, -7808, -8231, -7552, -8064, -7040,

/* America */
83,
8064,-9856,6656,-7168,6400,-8320,5888,-8192,5593,-9012,5312,-9050,5196,-9472,4736,-9728,4505,-9690,
3968,-10445,3456,-10240,3200,-10304,3584,-10586,3712,-10560,3840,-10752,3878,-11392,3712,-11520,
3801,-12032,3456,-12480,2816,-12506,2432,-12288,2355,-12032,2432,-11648,2688,-11520,2764,-11136,
2048,-11392,1996,-10624,1344,-10688,1152,-10432,1241,-10112,1024,-9856,1408,-9600,1536,-9088,1356,-8064,
512,-6656,0,-6400,-768,-4352,-1536,-4992,-2816,-5248,-3200,-6144,-3584,-6144,-5248,-8064,-6528,-8832,
-7040,-8320,-7040,-8960,-6400,-9728,-4736,-9472,-2304,-8960,-768,-10368,0,-10368,844,-9920,1152,-10112,
896,-10368,1216,-10880,1664,-11264,1792,-11712,2073,-12160,2009,-12365,2508,-13568,2816,-13530,3712,-14388,
4006,-14464,4044,-14720,3840,-14669,2944,-14016,3200,-14375,3840,-14836,4352,-15168,4416,-15450,4992,-15872,
5504,-15936,6208,-15936,7552,-17664,7808,-18944,6912,-21120,7552,-20224,7936,-21248,8704,-21376,9088,-20096,
8704,-14080,8960,-10496,7680,-12160,6912,-10240,8064,-9856,

/* Greenland */
11,
7680, -5632, 8320, -5120, 8960, -2816, 10496, -1920, 10700, -3840, 10048, -9344, 
9728, -8704, 9676, -7552, 8960, -6528, 8448, -6848 ,7808,  -6144, 7680, -5632,

/* Iceland */
6,
8512,-2880,8371,-3136,8524,-2048,8320,-1728,8064,-2432,8192,-2816,8512,-2880,

/* GB and Ireland */
18,
7488,-640,7449,-231,7168,-423,7168,-256,6784,64,6784,204,6681,217,6566,102,6566,192,6515,128,6400,-743,6579,-474,
6617,-640,6822,-576,6822,-384,7040,-448,7001,-640,7360,-832,7488,-640,
4,
7078,-832,6950,-1280,6579,-1280,6681,-807,7078,-832,

/* Sardinia and Corse */
9,
5504,1203,5427,1088,5312,1126,5235,1254,5004,1241,4979,1075,5222,1075,5286,1177,5388,1228,5504,1203,

/* EurAsian and African block */
217,
5248,3712,5376,4480,5248,4864,5440,5414,5888,4736,6144,4992,5952,4480,5888,4736,5670,4352,5824,4096,5913,4288,6016,
3968,5440,3456,5248,3712,5222,2944,4864,3072,4672,2918,5184,2496,5376,2496,5849,1753,5824,1574,5683,1574,5580,1740,
5440,1804,5120,2368,5184,2176,5081,2112,4992,2201,4864,1996,4864,1600,4684,1920,4979,2060,5120,2009,5286,1664,5504,
1344,5670,1139,5529,793,5568,512,5465,384,5350,422,5056,-52,4953,38,4684,-269,4672,-615,4608,-692,4748,-858,4736,
-1127,4940,-1127,4940,-1204,5273,-1101,5516,-1191,5593,-986,5542,-192,5900,-154,6054,-320,6144,-602,6220,-602,6246,
-397,6233,-218,6374,-256,6374,-167,6323,-141,6310,-13,6361,25,6425,192,6515,204,6579,460,6822,601,6912,1062,7296,
1036,7372,1369,7219,1523,6976,1280,6912,1817,7040,2560,7552,2816,7680,3840,7756,3584,7680,2816,8064,2688,8396,3328,
8448,2816,7808,2176,7680,2432,7168,2048,7091,1664,7552,1318,7424,972,7488,768,8000,704,8192,1280,8998,2432,9113,
3456,8678,5312,8512,4992,8601,4224,8256,4480,8192,5120,8729,5632,8832,8576,9216,8960,9856,14336,9472,14080,9216,
16640,8960,22400,8576,24320,8448,22656,8064,23040,7680,21760,7680,20864,7040,20736,6528,20096,7296,19968,7936,20864,
7936,20096,7552,19584,7552,18304,7040,17280,6912,18048,6144,17920,4992,16384,4480,16576,4352,16128,4992,16064,5248,
15488,4928,15104,3840,15616,2944,14976,2688,14080,2816,13824,2432,13504,1856,13952,1472,13952,1024,13440,1664,12864,
1152,12672,640,13248,128,13312,512,12928,1152,12544,2176,12416,2944,11776,1920,10240,1280,10240,1024,9856,1536,9536,
2688,9216,3200,8576,3200,7168,3840,6400,3776,6272,3072,6784,3200,7168,3072,7168,2944,7680,2176,7168,1600,5632,3584,
4480,3584,4224,1920,5120,1344,5760,1536,6579,512,6105,-640,4992,-2048,5248,-2560,4480,-3200,4480,-3328,4224,-3712,
4096,-4352,3328,-4480,2560,-2304,1536,-1408,1792,-128,1152,384,1280,588,1075,550,755,832,550,614,-256,588,-986,998,
-1652,1228,-1716,1587,-2138,1907,-2253,2201,-2061,2726,-2202,3584,-1652,3878,-1216,3968,-1255,4096,-1255,4262,-1063,
4339,-884,4582,-768,4595,-692,4505,-602,4480,-256,4659,128,4774,1305,4697,1331,4736,1408,4620,1344,4505,1420,4352,
1280,4198,1600,4216,1689,4147,1958,4032,1996,3840,2432,3968,2560,4096,2521,4224,2816,3968,3712,4044,3968,3993,4288,
4736,4608,4736,3584,5120,3328,5248,3712,

/* Madagascar */
5,
-1664,6272,-2176,5632,-3200,5632,-3200,6016,-1920,6464,-1664,6272,

/* Sri Lanka */
6,
1241, 10240, 896, 10496, 832, 10470, 806, 10304, 819, 10240, 1024, 10201, 1241, 10240,

/* Japan */
21,
5824, 18150, 5542, 18649, 5376, 18304, 5452, 18124, 5196, 17920, 4889, 17868, 4736, 17523, 4556, 17369, 4556, 17024,
4352, 16755, 4211, 16896, 4019, 16806, 3993, 16665, 4262, 16601, 4352, 16755, 4416, 17280, 4288, 17369, 4608, 17996,
5094, 18176, 5440, 17881, 5568, 18099, 5824, 18150,

/* --Indonesia-- */
/* Borneo */
5,
243,13990,896,14963,640,15270,-512,14848,-372,14118,243,13990,
/* Papua New Guinea*/
10,
0,16640,-320,18048,-832,18944,-871,18790,-1370,19328,-986,18470,-1191,18304,-1024,17715,-692,17676,-512,17036,0,16640,
/* Sumatra + Java */
10,
768,12160,217,12646,-410,13004,-756,13529,-845,14617,-1101,14656,-909,13516,-372,13555,51,13260,640,12480,768,12160,

/* Australia */
31,
-1344, 18227, -2240, 18048, -1920, 17344, -1536, 17536, -1408, 16896, -1920, 16512, -1792, 16256,
-2560, 15360, -2816, 14592, -3328, 14464, -4096, 14848, -4416, 14720, -4506, 15104, -4032, 16640,
-4160, 17088, -4480, 17344, -4224, 17638, -4506, 17600, -4864, 18483, -4992, 18355, -4839, 18560,
-5018, 18688, -4800, 19200, -4352, 19328, -4186, 19545, -3712, 19660, -3277, 19584, -2688, 18995,
-2407, 18726, -1856, 18521, -1882, 18432, -1344, 18227,

/* New Zealand */
13,
-4416,22105,-4698,22515,-4800,22528,-4864,22694,-4788,22848,-5325,22464,-5197,22080,
-5479,21888,-5888,21273,-5978,21683,-5146,22438,-5031,22272,-4826,22374,-4416,22105,

0

};

#ifndef VIEW


#ifndef NOLOGO
char plane[] = { 83, 38, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x20 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x08 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x06 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x1F , 0x00 , 0x00 , 0x00 , 0x03 , 0x89 , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x18 , 0xF8 , 0x00 , 0x00 , 0x00 , 0xF0 , 0x70 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x06 , 0x07 , 0xC0 , 0x00 , 0x06 , 0x20 , 0x10 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x01 , 0x80 , 0x3D , 0x78 , 0x01 , 0xC0 , 0x10 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x60 , 0x03 , 0xE7 , 0x80 , 0x80 , 0x10 , 0x00 , 0x00 , 0x08 , 0x80 
, 0x00 , 0x1C , 0x00 , 0x14 , 0xFD , 0x00 , 0x10 , 0x00 , 0x06 , 0xE6 , 0x00 
, 0x00 , 0x07 , 0x00 , 0x01 , 0x83 , 0xC0 , 0x30 , 0x00 , 0xBD , 0xA8 , 0x00 
, 0x00 , 0x00 , 0xE0 , 0x00 , 0xC0 , 0x70 , 0x30 , 0x1B , 0x52 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x38 , 0x18 , 0x3C , 0x2C , 0x30 , 0xEB , 0x20 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x0F , 0x38 , 0x07 , 0xE3 , 0x37 , 0xA6 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x06 , 0xF0 , 0x04 , 0x00 , 0xD5 , 0x50 , 0x00 , 0x0E , 0x00 
, 0x00 , 0x00 , 0x03 , 0x60 , 0x01 , 0x00 , 0x30 , 0x00 , 0x00 , 0x3A , 0x00 
, 0x00 , 0x00 , 0x01 , 0x80 , 0x00 , 0x20 , 0x0C , 0x20 , 0x00 , 0x62 , 0x00 
, 0x00 , 0x00 , 0x00 , 0xF0 , 0x00 , 0x04 , 0x03 , 0x00 , 0x00 , 0xC6 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x3F , 0x0C , 0x00 , 0x80 , 0xFF , 0xFF , 0xFF , 0x80 
, 0x00 , 0x00 , 0x01 , 0x5B , 0xEB , 0x00 , 0x20 , 0x40 , 0x00 , 0x00 , 0xC0 
, 0x00 , 0x00 , 0x16 , 0x55 , 0x78 , 0xE0 , 0x02 , 0x10 , 0x01 , 0xC0 , 0x60 
, 0x00 , 0x03 , 0x3B , 0x90 , 0xB8 , 0x1C , 0x00 , 0x04 , 0x01 , 0x39 , 0xE0 
, 0x00 , 0x18 , 0xD4 , 0x8A , 0x1C , 0x03 , 0x80 , 0x01 , 0x01 , 0x8F , 0x00 
, 0x00 , 0xCD , 0xC6 , 0x50 , 0x0C , 0x00 , 0x60 , 0x00 , 0x40 , 0x86 , 0x00 
, 0x04 , 0x36 , 0x90 , 0xA0 , 0x76 , 0x00 , 0x7E , 0x00 , 0x11 , 0xC6 , 0x00 
, 0x1A , 0x58 , 0x8A , 0x40 , 0x0E , 0x00 , 0x67 , 0xE0 , 0x0E , 0x46 , 0x00 
, 0x11 , 0x0C , 0x21 , 0x07 , 0x07 , 0x00 , 0x60 , 0x3E , 0x04 , 0x42 , 0x00 
, 0x80 , 0x10 , 0x82 , 0x01 , 0xF3 , 0x00 , 0x60 , 0x03 , 0xCC , 0x62 , 0x00 
, 0x01 , 0x84 , 0x08 , 0x00 , 0x7F , 0x80 , 0x60 , 0x00 , 0x78 , 0x3A , 0x00 
, 0x06 , 0x20 , 0x00 , 0x00 , 0x0F , 0x80 , 0x60 , 0x20 , 0x00 , 0x0E , 0x00 
, 0x00 , 0x00 , 0x20 , 0x01 , 0x03 , 0xC0 , 0x20 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x40 , 0x90 , 0x00 , 0x00 , 0x00 , 0xC0 , 0x20 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x0C , 0xE0 , 0x20 , 0x08 , 0x00 , 0x00 , 0x00 
, 0x01 , 0x00 , 0x00 , 0x02 , 0x03 , 0x60 , 0x30 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x02 , 0x00 , 0x00 , 0x00 , 0x70 , 0x38 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x1C , 0x20 , 0x02 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x08 , 0x00 , 0x00 , 0x00 , 0x03 , 0x20 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x04 , 0x00 , 0x00 , 0xE0 , 0x00 , 0x80 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x40 , 0x00 , 0x00 
 };
#endif
#endif



float rsin(float x) {
	return sin(pi()*x/180.0);
}

float rcos(float x) {
	return cos(pi()*x/180.0);
}

void plotline()
{
	Q=rsin(O);   E=rcos(O);
	U=rcos(L);     W=rsin(L);
#ifdef hires
	X=R*(U*Q*K+W*H*J+U*E*M*J)*2;
#else
	X=R*(U*Q*K+W*H*J+U*E*M*J);
#endif
	Y=R*(W*M-U*E*H);
	#ifndef TRANSPARENT
	Z=R*(W*H*K-U*Q*J+U*E*M*K);
	if (Z < 0.0) {
		xorplot ((float)getmaxx()/2.0+X,(float)getmaxy()/2.0-Y);
		xorplot ((float)getmaxx()/2.0+X,(float)getmaxy()/2.0-Y);
	} else {
		drawto ((float)getmaxx()/2.0+X,(float)getmaxy()/2.0-Y);
	}
	#else
	drawto ((float)getmaxx()/2.0+X,(float)getmaxy()/2.0-Y);
	#endif
}

void setorg()
{
	Q=rsin(O);   E=rcos(O);
	U=rcos(L);     W=rsin(L);
#ifdef hires
	X=R*(U*Q*K+W*H*J+U*E*M*J)*2;
#else
	X=R*(U*Q*K+W*H*J+U*E*M*J);
#endif	
	Y=R*(W*M-U*E*H);
	
	unplot ((float)getmaxx()/2.0+X,(float)getmaxy()/2.0-Y);
}


void main()
{
restart:
	fputc_cons(12);

#ifdef VIEW
	view='0'+VIEW;
#else
	clg();
#ifndef NOLOGO
	putsprite (spr_or, 2,2, plane);
#else
#ifdef ZX81
	hrg_off();
#endif
#endif
	
	printf ("\n\n\n\n\n\n\n   1 - Europe\n");
	printf ("   2 - Pacific - Asia\n");
	printf ("   3 - Australia\n");
	printf ("   4 - Africa\n");
	printf ("   5 - Atlantic - North America\n");
	printf ("   6 - Pacific - South America\n");
	printf ("   7 - North Pole view\n");
	printf ("   8 - South Pole\n");
	printf ("   9 - Pacific - Indonesia - JP\n");
	
	view=0;
	while ((view <'1')||(view >'9')) view=getk();

#endif

#ifdef NOLOGO
#ifdef ZX81
	hrg_on();
#endif
#endif

	clg();
	
	switch (view) {
	case '1':
		A = 40.0;
		B = 0.0;
		break;
	case '2':
		A = 330.0;
		B = 230.0;
		break;
	case '3':
		A = 30.0;
		B = 230.0;
		break;
	case '4':
		A = 10.0;
		B = 340.0;
		break;
	case '5':
		A = 45.0;
		B = 45.0;
		break;
	case '6':
		A = 20.0;
		B = 90.0;
		break;
	case '7':
		A = 80.0;
		B = 0.0;
		break;
	case '8':
		A = 260.0;
		B = 0.0;
		break;
	case '9':
		A = 300.0;
		B = 230.0;
		break;
	
	}
	
	// Radius
	R=(float)getmaxy()/2.1;
	
#ifdef hires
	plot ((float)getmaxx()/2.0+R*2,(float)getmaxy()/2.0);
#else
	plot ((float)getmaxx()/2.0+R,(float)getmaxy()/2.0);
#endif

	for (I = 0.0; I < 720.0; I += STEP) {
#ifdef hires
		X = R * 2 * rcos(I);  Y = R * rsin(I);
#else
		X = R * rcos(I);  Y = R * rsin(I);
#endif
		drawto ((float)getmaxx()/2.0+X,(float)getmaxy()/2.0-Y);
	}

	J=rsin(B);	K=rcos(B);	H=rsin(A);	M=rcos(A);
	
#ifdef GRID
	// Meridians
	for (O = 0.0; O < 360.0; O += GRID) {
		L = 90.0;
		setorg();
		for (L = 90.0; L > -90.0; L -= STEP)
			plotline();
	}
	
	// Parallels
	for (L = -90.0+GRID; L < 90.0; L += GRID) {
		setorg();
		for (O = 0.0; O < 360.0; O += STEP)
			plotline();
	}

#endif


	eptr=0;
	corners=world[eptr++];
	
	while (corners) {
		L=((float) world[eptr++]/128.0);
		O=((float) world[eptr++]/128.0);
		setorg();
		for (count=0; count < corners; count++) {
			L=((float) world[eptr++]/128.0);
			O=((float) world[eptr++]/128.0);
			plotline();	
		}
		corners=world[eptr++];
	}
		
	printf ("Done.  Press ENTER");
	while ((getk() != 10)&&(getk() != 13)) {};
	goto restart;
}

