#include "bdf.hh"
#include <iostream>
TPicType    black(0,0,0,0), white(255,255,255,0), trans(64,128,128,0);
BDFGlyph::BDFGlyph() : glyph_id(-1)
{
    SetOrigAt(0,0);
    CreatePicture(64,64);
//    glyph_name.format("%lx",this);
}

void BDFGlyph::CalculateBB(FontBB &bb, int &XShift, int &YShift)
{
    int i,j;
    int bbx_sat=128, bby_sat=128, bbx_eat=0, bby_eat=0;
    //	bool already_found=false;
    BDFPixel bdpix;
    for(j=0;j<h;j++)
	for(i=0;i<w;i++) {
	    bdpix = PixelType(Pixel(i,j));
	    if(bdpix==White) {
		bbx_eat=OlorinMax(bbx_eat, i);
		bby_eat=OlorinMax(bby_eat, j);
		bbx_sat=OlorinMin(bbx_sat, i);
		bby_sat=OlorinMin(bby_sat, j);
	    }
	}
    XShift=bbx_sat; YShift=bby_sat;
    bb.w=bbx_eat-XShift+1;
    bb.h=bby_eat-YShift+1;
    bb.ox=XShift-orig_atx;
    bb.oy=YShift+bb.h-orig_aty;
    bb.oy*=-1;
}

void 
BDFGlyph::CalculateSWidth(int &SWidth, int wdth)
    {
	SWidth=int(float(wdth+1)*32.23f);
    }
void BDFGlyph::CalculateDWidth(int &DWidth, int wdth)
    {
	DWidth=wdth+1;
}
void BDFGlyph::SetBDFPixel(int x, int y, BDFPixel col)
{
	switch(col) {
	case Transparent: Pixel(x,y)=trans; break;
	case White: Pixel(x,y)=white; break;
	case Black: Pixel(x,y)=black; break;
	}
}
void BDFGlyph::SetOrigAt(int nox, int noy)
{
    if(nox<0) nox=0;
    if(noy<0) noy=0;
    if(nox>w-1) nox=w-1;
    if(noy>h-1) nox=h-1;
    orig_atx = nox; orig_aty = noy;
}
BDFGlyph::BDFPixel BDFGlyph::PixelType(TPicType &pix)
{
	if(pix==black) return Black;
	if(pix==white) return White;
	return Transparent;
}
void BDFGlyph::Join(BDFGlyph *aGlyph)
{
    BDFGlyph::BDFPixel myT, aT;
    int i, j;
    int orig_cx, orig_cy;
    // handle difference in origo's
    orig_cy = aGlyph->orig_aty - orig_aty;
    orig_cx = aGlyph->orig_atx - orig_atx;
    for(j=0;j<h;j++)
        for(i=0;i<w;i++) {
            myT=PixelType(Pixel(i,j));
            aT=PixelType(aGlyph->Pixel(i+orig_cx,j+orig_cy));
            if(myT==White)
                if(aT==White)
                    Pixel(i,j)=black;
                else
                    Pixel(i,j)=black;
            else
                if(aT==White)
                    Pixel(i,j)=white;
                else
                    Pixel(i,j)=trans;
        }
}
void BDFGlyph::Break(BDFGlyph *aGlyph)
{
    BDFGlyph::BDFPixel myT;
    int i, j; 
    for(j=0;j<h;j++)
        for(i=0;i<w;i++) {
            myT=PixelType(Pixel(i,j));
            if(myT==White) {
                SetBDFPixel( i, j, Black);
                aGlyph->SetBDFPixel( i, j, White);
            } else if(myT==Black) {
                SetBDFPixel( i, j, White);
                aGlyph->SetBDFPixel( i, j, White);
            } else { // myT==Trans
//                SetBDFPixel( i, j, Transparent);
//                aGlyph->SetBDFPixel( i, j, Transparent);
                SetBDFPixel( i, j, Black);
                aGlyph->SetBDFPixel( i, j, Black);
            }
        }
    aGlyph->SetOrigAt(orig_atx, orig_aty);
}

void BDFGlyph::Invert(void)
{
    BDFGlyph::BDFPixel myT;
    int i, j;
    for(j=0;j<h;j++)
        for(i=0;i<w;i++) {
            myT=PixelType(Pixel(i,j));
            if(myT==White)
                SetBDFPixel( i, j, Black);
            else if(myT==Black)
                SetBDFPixel( i, j, White);
        }
}


#define ErrOut(msg,err) {if(err_msg)*err_msg<<std::endl<<"*** ERROR #" << (int)err<<" *** : " << msg << std::endl; return false;}

void StripComments(std::ifstream& file, CppString &str);
bool BDFFont::LoadHeader(std::ifstream &file, CppString &str, 
			 std::ostream *err_msg)
{
    //    CppString tempname;
    StripComments(file,str);
    if(str.Search("STARTFONT")==-1)
	ErrOut("not a font file",1);
    StripComments(file,str);
    if(str.Search("FONT")==-1)
	ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2);
    //    *err_msg << str << endl;
    *FontName=str.RmFirstWord();
    //    properties->Current()=str;
    StripComments(file,str);
    if(str.Search("SIZE")==-1)
	ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2);
    PointSize=str.RmFirstWord().FirstWord().AsInteger();
    XDevRes=str.RmFirstWord().FirstWord().AsInteger();
    YDevRes=str.RmFirstWord().FirstWord().AsInteger();
    StripComments(file,str);
    
    if(str.Search("FONTBOUNDINGBOX")==-1)
	ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2);
    FBB.w=str.RmFirstWord().FirstWord().AsInteger();
    FBB.h=str.RmFirstWord().FirstWord().AsInteger();
    FBB.ox=str.RmFirstWord().FirstWord().AsInteger();
    FBB.oy=str.RmFirstWord().FirstWord().AsInteger();
    StripComments(file,str);
    //while(str.IsSpace())
    //    StripComments(file,str);
    if(str.Search("STARTPROPERTIES")!=-1) { // optional
        int props;
	props=str.RmFirstWord().FirstWord().AsInteger();
	for(int i=0;i<props;i++) {
	    StripComments(file,str);
	    if(str.Search("FONT_ASCENT")!=-1)
		f_ascent=str.RmFirstWord().FirstWord().AsInteger();
	    else if(str.Search("FONT_DESCENT")!=-1)
		f_descent=str.RmFirstWord().FirstWord().AsInteger();
	    else if(str.Search("DEFAULT_CHAR")!=-1)
		def_char=str.RmFirstWord().FirstWord().AsInteger();
	    else {
                if(!i) properties->Current()=str;
                else properties->Add(new CppString(str));
            }
	}
	StripComments(file,str);
	if(str.Search("ENDPROPERTIES")==-1) {
	    ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2)
		}
        else StripComments(file,str);
    }
    return true;
}
// #include <conio.h>
bool BDFGlyph::LoadGlyph(std::ifstream &file, CppString &str, 
			 std::ostream *err_msg)
{
//cout << ".t" << hex << (ulong)this<<dec<<';';
    StripComments(file,str);
    if(str.Search("STARTCHAR")==-1)
        ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2);
    str.RmFirstWord();
    glyph_name=str;
//cout << "Got glyph name : " << str << endl;
    StripComments(file,str);
    if(str.Search("ENCODING")==-1)
        ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2);
    str.RmFirstWord();
//cout << "Got encoding : " << str << endl;
    if(str.FirstWord()=="-1") str.RmFirstWord();
    glyph_id=str.AsInteger();
//cout << "Glyph id: " << glyph_id << endl;
    StripComments(file,str);
    if(str.Search("SWIDTH")==-1)
        ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2);
    StripComments(file,str);
    if(str.Search("DWIDTH")==-1)
        ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2);
    StripComments(file,str);
    if(str.Search("BBX")==-1)
        ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2);
    int lBBw, lBBh, lBBox, lBBoy;
    lBBw=str.RmFirstWord().FirstWord().AsInteger();
    lBBh=str.RmFirstWord().FirstWord().AsInteger();
    lBBox=str.RmFirstWord().FirstWord().AsInteger();
    lBBoy=str.RmFirstWord().FirstWord().AsInteger();
    StripComments(file,str);
    if(str.Search("ATTRIBUTES")!=-1) { // optional
        str.RmFirstWord();
        attrs=str;
        StripComments(file,str);
    }
    if(str.Search("BITMAP")==-1)
        ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2);
    int i, j;
    // add half-width & half-height..
    int addx = 32 - (lBBw >> 1);// - (lBBw & 1);
    int addy = 32 - (lBBh >> 1);// - (lBBh & 1);
    char *buffer=new char[1024];
    for(i=0;i<lBBh;i++) {
        StripComments(file,str);
        str.HexToBinary('1','0',buffer);
        // str.CutFrom(lBBw);
        for(j=0;j<lBBw;j++) {
            switch(str[j]) {
            case '1':
                SetBDFPixel(addx+j,addy+i, White); break;
//                SetBDFPixel(addx+j+lBBox,addy+i-lBBoy-lBBh, White); break;
            case '0': // falling through.
            default:
                SetBDFPixel(addx+j,addy+i, Black); break;
//                SetBDFPixel(addx+j+lBBox,addy+i-lBBoy-lBBh, Black); break;
            }
        }
        // cout << str << endl;
    }
    orig_atx=addx-lBBox; orig_aty=addy+lBBoy+lBBh;
    //orig_atx=addx-lBBox; orig_aty=addy+lBBoy+lBBh;
//    orig_atx=32; orig_aty=32;
    delete[] buffer;
    StripComments(file,str);
    if(str.Search("ENDCHAR")==-1) {
        ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2)
    }
//    else StripComments(file,str);
    //    cout << "One char finished" << endl;
//    getch();
    return true;
}
bool BDFFont::LoadFont(const char *fname, std::ostream *err_msg)
{
    std::cout << "Loading font: " << fname << std::endl;
    std::ifstream file(fname,std::ios::in);
    if(!file)
    {
        std::cout << "something vbaad!\n";
        ErrOut("Unable to open file "<<fname<<'.',5);
    }
    CppString str(512);
    if(!LoadHeader(file, str, err_msg))
        ErrOut("Font file header loader failed",4);
    if(str.Search("CHARS")==-1)
	ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2);
    chars=str.RmFirstWord().AsInteger();
    //    glyphs->MakeNElements(chars).First();
    //    StripComments(file,str);
    for(int i=0;i<chars;i++) {
        if(i) glyphs->Add(new BDFGlyph);
        if(!glyphs->Current().LoadGlyph(file,str,err_msg))
            ErrOut("Font glyph loader failed at char #"<<i,4);
        // glyphs->Next();
    }
    glyphs->First();
    StripComments(file,str); // tuning
    if(str.Search("ENDFONT")==-1)
	ErrOut("Unexpected word "<<str.FirstWord()<<" in file", 2);
    file.close();
    //    *FontName=properties->First().Current();
    //    FontName->RmFirstWord();
    return true;
}
void BDFFont::MaskFont()
{
  masked = true;
  int i, j=chars>>1;
  glyphs->First();
  BDFGlyph *pic, *mask;
  for(i=0;i<j;i++) {
    pic=&glyphs->Current();
    mask=&glyphs->Next().Current();
    pic->Join(mask);
    glyphs->Next();
  }
  glyphs->First();
}
void BDFFont::UnMaskFont()
{
  masked = false;
  int i, j = chars >> 1;
  glyphs->First();
  BDFGlyph *pic, *mask;
  for( i = 0; i < j; i++) {
    pic = &glyphs->Current();
    mask = &glyphs->Next().Current();
    pic->Break(mask);
    glyphs->Next();
  }
  glyphs->First();
}
bool BDFGlyph::SaveGlyph(std::ofstream &file)
{
    const uchar eol=0x0a;
    FontBB bb; int Xoff, Yoff, t;
    CppString str(128);
    file << "STARTCHAR " << glyph_name; file.put(eol);
    file << "ENCODING " << glyph_id; file.put(eol);
    CalculateBB(bb, Xoff, Yoff);
    CalculateSWidth(t, bb.w);
    file << "SWIDTH " << t << " 0"; file.put(eol);
    CalculateDWidth(t, bb.w);
    file << "DWIDTH " << t << " 0"; file.put(eol);
    file << "BBX " << bb.w << ' ' << bb.h << ' ' << bb.ox << ' ' << bb.oy;
        file.put(eol);
//    file << "ATTRIBUTES " << attrs; file.put(eol);
    file << "BITMAP"; file.put(eol);
    int x, y;
    for( y = 0; y < bb.h; y++) {
        for( x = 0; x < bb.w; x++) {
            str[x] = Pixel(Xoff+x,Yoff+y)==white ? '1': '0' ;
//            file << '0';
        }
        str.BinToHex();
        // got to check length: it must be even
        if(str.Length() & 1) {
            str+="0";
        }
        file << str; file.put(eol);
    }
    file << "ENDCHAR"; file.put(eol);
    return true;
}
bool BDFFont::SaveFont(const char *fname)
{
    std::ofstream file(fname, std::ios::binary);
    const uchar eol=0x0a;
    if(!file) return false;
    file << "STARTFONT 2.1"; file.put(eol);
    file << "COMMENT Possibly created or at least modified by DeCurs"; file.put(eol);
    file << "COMMENT (c) 2001 Pasi Savolainen <pvsavola@cc.hut.fi>"; file.put(eol);
    file << "FONT " << FontName->c_str(); file.put(eol);
    if(PointSize < 0) PointSize = 31; // fixes some fonts produced by pcf2bdf.
    file << "SIZE " << PointSize << ' ' << XDevRes << ' ' << YDevRes; file.put(eol);
    file << "FONTBOUNDINGBOX " << 64 << ' ' << 64 << ' ' << 32 << ' ' << 32; file.put(eol);
    file << "STARTPROPERTIES " << 3; file.put(eol);
    file << "FONT_ASCENT " << f_ascent; file.put(eol);
    file << "FONT_DESCENT " << f_descent; file.put(eol);
    file << "DEFAULT_CHAR " << def_char; file.put(eol);
    file << "ENDPROPERTIES"; file.put(eol);
    file << "CHARS " << chars; file.put(eol);
    glyphs->First();
    for(int i=0; i<chars; i++) {
        glyphs->Current().SaveGlyph(file);
        glyphs->Next();
    }
    file << "ENDFONT"; file.put(eol); // GNU consider as line only one with LF at EOL
    file.close();
    return true;
}
BDFFont::BDFFont()
{
  masked = false;
  FontName=new CppString("Undefined");
  PointSize=-32000; XDevRes=-32000; YDevRes=-32000;
  properties=new TStringList;
  glyphs=new TGlyphList;
  properties->First().Current()="Undefined proppy";
}
BDFFont::~BDFFont()
{
  delete glyphs;
  delete FontName;
  delete properties;
}

void StripComments(ifstream& file, CppString &str)
{
  str.getline(file);
  while(str.Search("COMMENT")!=-1 || str.IsSpace()) {
    //		cout << str << endl;
    str.getline(file);
  }
}
