#ifndef IDEFS_H_INCLUDED
#define IDEFS_H_INCLUDED

#ifndef NULL
    #define NULL            0x00
#endif // NULL
//-=[Generic errors]=-----------------------------------------------------------
#define GEN_OK              0x00
#define GEN_ERROR           0x01

#define TOGGLE_OPTION1(a,b)	case a: (b)=!(b); break
#define TOGGLE_OPTION(a,b,c)	case a: case b: (c)=!(c); break

typedef unsigned char uchar;
typedef signed char schar;

typedef unsigned int uint;
typedef signed int sint;

typedef unsigned long ulong;
typedef signed long slong;

typedef long double ldouble;
// typedef float ldouble;
#include <stdlib.h>
#define LDRND	(((long double)(random(30000)))/30000.0f)
#define LDRND2	((LDRND-0.5f)*2.0f)

#define MyLims(x,lmin,lmax) {if((x)<(lmin))(x)=(lmin);else if((x)>(lmax))(x)=(lmax);}
#define OlorinMax(a,b) (((a)>(b))?(a):(b))
#define OlorinMin(a,b) (((a)<(b))?(a):(b))

struct FixedNumber
{
	union
	{
		uint down;
		sint up;
	} s;
	slong whole;
};
typedef unsigned char Color_Type;
struct RGB24
{
	Color_Type r,g,b; // this's how it's on monitor...
	RGB24() { r=0; g=0; b=0; }
	RGB24(ulong c) { *this=c; }
	RGB24(Color_Type nr, Color_Type ng, Color_Type nb) : r(nr), g(ng), b(nb){} 
	RGB24& operator=(RGB24& aRgb){ r=aRgb.r; g=aRgb.g; b=aRgb.b; return *this;}
	RGB24& operator=(uchar c) { r=c; g=c; b=c; return *this; }
	RGB24& operator=(ulong c) { r=uchar(c&0x000000ffL); g=uchar((c&0x0000ff00L)>>8);
		b=uchar((c&0x00ff0000L)>>16); return *this; }
	RGB24& operator*=(float k) {
		float 	t=r; t*=k; r=uchar(t);
				t=g; t*=k; g=uchar(t);
				t=b; t*=k; b=uchar(t);
		return *this; }

	RGB24& operator+=(RGB24& aR) {r+=aR.r; g+=aR.g; b+=aR.b; return *this; }
	RGB24& operator-=(RGB24& aR) {r-=aR.r; g-=aR.g; b-=aR.b; return *this; }
	operator ulong() { return ((ulong(r))|(ulong(g)<<8)|(ulong(b)<<16)); }
	RGB24& TakeXFrom(RGB24& orgb, double percentToTake)
	{
		double t1, t2, percentToLeave = (1 - percentToTake);
		// R
		t1=orgb.r; 	t1*=percentToTake;
		t2=r; 		t2*=percentToLeave;
		t1+=t2; MyLims(t1, 0, 255);
		r=Color_Type(t1);
		// G
		t1=orgb.g; 	t1*=percentToTake;
		t2=g; 		t2*=percentToLeave;
		t1+=t2; MyLims(t1, 0, 255);
		g=Color_Type(t1);
		// B
		t1=orgb.b; 	t1*=percentToTake;
		t2=b; 		t2*=percentToLeave;
		t1+=t2; MyLims(t1, 0, 255);
		b=Color_Type(t1);
		return *this;
	}
};
struct RGB32 : public RGB24
{
	Color_Type a;
	RGB32(Color_Type nr=0, Color_Type ng=0, Color_Type nb=0, Color_Type na=255) :
		RGB24(nr,ng,nb), a(na) {}
	RGB32& operator =(RGB24& aRGB) {
		RGB24::operator=(aRGB);
		a=255; return *this;
	}
};
#endif // IDEFS_H_INCLUDED
