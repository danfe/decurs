#if !defined(OLORIN_BDF_HPP)
#define OLORIN_BDF_HPP

#include "picture.hh"
#include "cppstring.hh"
#include "nlist.hh" // stack
#include <fstream>

typedef TNListIterator<CppString> TStringList;

struct FontBB
{
  int w, h, ox, oy;
};

class BDFGlyph : public TPicture
{
public:
    int orig_atx, orig_aty;
    BDFGlyph();
    enum BDFPixel { Transparent, Black, White };
    BDFPixel PixelType(TPicType &pic);
    int glyph_id;
    bool operator==(BDFGlyph &agl) {
	if(agl.glyph_id==-1)
	    return agl.glyph_name == glyph_name;
	else
	    return glyph_id==agl.glyph_id;
    }
    CppString glyph_name, attrs;
    void CalculateBB(FontBB &bb,int &XShift,int &YShift);
    void CalculateSWidth(int &SWidth,int wdth);
    void CalculateDWidth(int &DWidth,int wdth);
    void SetBDFPixel(int x, int y, BDFPixel col);
    void SetOrigAt(int nox, int noy);
    bool LoadGlyph(std::ifstream &file, CppString &str, std::ostream *err_msg=0);
    bool SaveGlyph(std::ofstream &file);
    void Join(BDFGlyph *aGlyph);
    void Break(BDFGlyph *aGlyph);
    void Invert(void);
};
extern TPicType white;
extern TPicType black;
extern TPicType trans;
typedef TNListIterator<BDFGlyph> TGlyphList;
class BDFFont
{
    bool LoadHeader(std::ifstream &file, CppString &str, std::ostream *err_msg=0);
    CppString *FontName;
  bool masked;
public:
  int PointSize, XDevRes, YDevRes;
  FontBB FBB;
  TStringList *properties;
  TGlyphList *glyphs;
  int f_ascent, f_descent, def_char;
  int chars;
  BDFFont();
  ~BDFFont();
  bool LoadFont(const char *fname, std::ostream *err_msd=0);
  bool SaveFont(const char *fname);
  bool GetMasked() { return masked; }
  void MaskFont();
  void UnMaskFont();
};

#endif // OLORIN_BDF_HPP
