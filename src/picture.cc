/*
	picture.cpp: picture functions.
	Copyright (C) 1998 Olorin aka Pasi Savolainen

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	You can contact author via olorins@hotmail.com
	Bug reports are welcomed.
*/

#include "picture.hh"
#include <math.h>
#if defined (__WIN32__)
#define OLORIN_WIN32_MEMALLOC
#define OLORIN_WIN32_MSG
#include <windows.h>
#endif
inline double distance3d(unsigned char a, unsigned char b, unsigned char c)
{
	double t1=a*a, t2=b*b;
	t1+=t2;	t2=c*c;	t1+=t2;
	return sqrt(t1);
}

void TPicture::GrayScale()
{
	int i, t=w*h;
	double dt, sqr3=sqrt(3);
	unsigned char ch;
	for(i=0; i < t; i++) {
		dt = distance3d(data[i].r, data[i].g, data[i].b)/sqr3;
		MyLims(dt, 0, 255);
		ch = (unsigned char)dt;
		data[i].r=ch;
		data[i].g=ch;
		data[i].b=ch;
	}

}
bool TPicture::CreatePicture(int nw, int nh)
{
	CleanPicture();
#if defined(OLORIN_WIN32_MEMALLOC)
	data = (RGB32*)GlobalAlloc(GMEM_FIXED,sizeof(RGB32) * (nh * nw));
#else
	data = new RGB32[nh * nw];
#endif
	if(!data)
		return false;
	w=nw; h=nh;
	return true;
}

void TPicture::CleanPicture()
{
	if(data) {
		#if defined(OLORIN_WIN32_MEMALLOC)
		GlobalFree(data);
		#else
		delete[] data;
		#endif
	}
	w=0; h=0; data=0;
	return;
}

void TPicture::BuildBasePattern()
{
	if(!CreatePicture(128,128)) return;
	RGB32 red(255,0,0), green(0,255,0), blue(0,0,255), white(255,255,255),
		black(0,0,0), yellow(255,255,0);
	int x, y;
	for(y = 0; y < 64; y++)
		for( x=0; x < 64; x++) {
			Pixel(x, y) = red;
			Pixel(x + 64, y) = green;
			Pixel(x, y + 64) = yellow;
			Pixel(x + 64, y + 64) = blue;
		}
	for(y = 0; y < 16; y++)
		for(x = 0; x < 16; x++) {
			Pixel(63 - x, 63 - y) = black;
			Pixel(64 + x, 64 + y) = black;
			Pixel(63 - x, 64 + y) = white;
			Pixel(64 + x, 63 - y) = white;
		}
}
TPicType* TPicture::FPixel(float atx, float aty)
{
	int fx = (int) floor(atx), fy = (int) floor(aty);
	int cx = (int) ceil(atx), cy = (int) ceil(aty);
	double x, y;
	RGB32 p1, p2, p3, p4, *r;
	r=new RGB32;
	// p1
	x=fx; y=fy;
	p1=Pixel(int(x),int(y));
	// p2
	x=cx; y=fy;
	p2=Pixel(int(x),int(y));
	// p3
	x=fx; y=cy;
	p3=Pixel(int(x),int(y));
	// p4
	x=cx; y=cy;
	p4=Pixel(int(x),int(y));
	double dx=atx-fx;
	double dy=aty-fy;
	p1*=dx*dy;
	p2*=(1-dx)*dy;
	p3*=dx*(1-dy);
	p4*=(1-dx)*(1-dy);
	*r=p1; *r+=p2; *r+=p3; *r+=p4;
	return r;
}
