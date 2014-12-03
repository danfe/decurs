#if !defined(PICTURE_H_INCLUDED)
#define PICTURE_H_INCLUDED
#include "base.h"
#include "idefs.h"
typedef RGB32 TPicType;
class TPicture : public TBaseClass
{
protected:
	TPicType *data;
	TPicType defColor;
	int h, w;
public:
	TPicture() : data(0), defColor(0,0,0) { h=0; w=0; }
	~TPicture() { CleanPicture(); };
	int Height() { return h; }
	int Width() { return w; }
	void CleanPicture();
	bool CreatePicture(int nw, int nh);
	void GrayScale();
	void SetDefColor(RGB32& ndCol) { defColor=ndCol; }
	TPicType* DataPointer() { return data; }
	TPicType& Pixel(int x, int y)
	{
		if( (x>=0) && (x<w) && (y<h) && (y>=0) )
				return data[y*w+x];
		return defColor;
	}
	TPicType* FPixel(float x, float y);
	TPicture& SetPixel(int x, int y, Color_Type nr, Color_Type ng, Color_Type nb)
	{
		TPicType *tDot=& Pixel(x,y);
		tDot->r=nr; tDot->g=ng; tDot->b=nb;
		return *this;
	}
	void BuildBasePattern();
};

#endif // PICTURE_H_INCLUDED
