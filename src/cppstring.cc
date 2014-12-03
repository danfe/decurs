
#include <ctype.h>	// tolower()
#include <stdio.h>
#include "cppstring.hh"
#include "idefs.h"
#include <iostream>
// #include <except.h>
#define DEF_BUFFER_SIZE     512
#define CPPSTRING_ERROR(obj,msg,retval) {obj<<"*** ERROR *** : "<<msg<<std::endl;return retval;}
std::ostream& operator<<(std::ostream &cou, CppString &str)
{
	return (cou << str.our_string);
}
CppString::CppString(const char *str) : our_string(0)
{
	Clean();
//    cout << ".c" << str << ';';
	(*this)=str;
}
CppString::CppString() : our_string(0)
{
    AllocUs(8);
}
CppString::~CppString()
{
	Clean();
}
CppString::CppString(CppString &str) : our_string(0)
{
	Clean();
	(*this)=str.c_str();
}
CppString::CppString(int length) : our_string(0)
{
    AllocUs(length);
}

void CppString::Clean(void)
{
    if(our_string) {
//        cout << "<deleting " << our_string << ">";
        try {
	        delete[] our_string;
        }
        catch(...) {
//        cout << "delete failed." << endl;
        }
    	our_string=0;
    }
	allocdBytes=0;
}
CppString& CppString::AllocUs(int hMuch, char *aString, bool ClearMem)
{
    Clean();
    if(aString) {
        our_string=aString;
    } else {
        try {
            our_string=new char[hMuch];
            if(ClearMem)
                memset(our_string,0,hMuch);
        }
        catch(...) {
            CPPSTRING_ERROR(std::cout, "Unknown memory allocation error in CppString()",
                *this);
        }
    }
    allocdBytes=hMuch;
    return *this;
}
CppString& CppString::operator=(const CppString &aStr)
{
    return (*this)=aStr.c_str();
}
CppString& CppString::operator=(const char *nstr)
{
	if(!nstr) {
        AllocUs(8);
/*        Clean();
		our_string=new char[0x8];
		allocdBytes=0x8;
		our_string[0]=0; our_string[1]=0;
*/		return *this;
	}
	int nstrLength=strlen(nstr);
//    if(nstrLength>70)
//        cout << ".!;";
    if(our_string && allocdBytes>nstrLength) {
//        cout << "\nSaferCopy\n";
//        cout << '.' << our_string << ';';
//        cout << ".o" << our_string << ';';
        strcpy(our_string, nstr);
        return *this;
    }
	char *tmp_str=new char[nstrLength+0x8];
	if(!tmp_str) {
        std::cout << "Shit happened" << std::endl;
		return *this; // should NOT happen. But as always shit HAPPENS
    }
	strcpy(tmp_str,nstr);
	// nstr is no longer used after this, so if it's our own string (substring
	// of our_string, then it's not damaged during execution.
	Clean();
	allocdBytes=nstrLength+0x8;
	our_string=tmp_str;
	return *this;
}
CppString& CppString::getline(ifstream& file)
{
	if(!file) return *this;
	char *buffer=new char[DEF_BUFFER_SIZE];
	if(!buffer) return *this;	// uups
	int i=0;
	char ch='A';
	while(ch!=10 && ch!=EOF) {
		file.get(ch);
		if(ch!=13) buffer[i++]=ch;
		// file.getline(buffer,0x1ff);
	}
	buffer[i-1]=0;
	(*this)=buffer;
	delete[]buffer;
	return *this;
}
CppString& CppString::getword(ifstream& file, bool rmUXComm)
{
	if(!file) return *this;
	char *buffer=new char[DEF_BUFFER_SIZE];
	if(!buffer) return *this;	// MAlloc failed
	char ch=0; int i=0;
	while(1) {
		file.get(ch);
		if(file.eof())
			break;
		if(ch=='#')
			if(rmUXComm)
				while(1) {
					file.get(ch);
					if(file.eof())
						break;
					if(ch=='\n')
						break;
				}
		if(IsSpace(ch))
			if(i)
				break;
			else; // ch IS space(CR,LF,TAB) but we don't have WORD in buffer.
		else
			buffer[i++]=ch;
	}
	buffer[i]=0;
	(*this)=buffer;
	delete[]buffer;
	return *this;
}
CppString CppString::operator+(const char *astr)
{
	int l1=0, l2=0;
	if(our_string) l1=strlen(our_string);
	if(astr) l2=strlen(astr);
	char *t_str=new char[l1+l2+2];
	if(l1) strcpy(t_str,our_string);
	if(l2) strcpy(t_str+l1,astr);
	CppString t(t_str);
	delete[]t_str;
	return t;
}
CppString& CppString::AddNumber(long double val)
{
	char *buffer=new char[0x2f];
	sprintf(buffer,"%.19LG",val);
	(*this)+=buffer;
	delete[] buffer;
	return *this;
}
CppString& CppString::operator+=(const char* nstr)
{
	return (*this)=(*this)+nstr;
}
char& CppString::operator[](int i)
{
	if(i<0)
        // if(our_string) // this will kill us.
        return our_string[0];
	if(i < (int) Length()+1)
		return our_string[i];
	else
		return our_string[Length()];
}

CppString& CppString::RmCPPComment(void)
{
	int i=0;
	while(true)
		switch(our_string[i++]) { // i++,so I don't have to increase it later.
		case 0x00:
			return *this;
		case '/':
			if(our_string[i]=='/') { // remember i++
				our_string[i-1]=0;	// -"-
				(*this)=our_string;
				return *this;
			}	// fall through.
		default:
			break;
		}
}

CppString& CppString::RmUnixComment(void)
{
	char *tOur=our_string;
	while(*tOur)
		if(*tOur=='#')
			*tOur=0;
		else tOur++;
	(*this)=our_string;
	return *this;
}

CppString& CppString::RmFirstWord()
{
	char *tOur=our_string;
	while(!IsSpace(*tOur)&&(*tOur)) // cutting off first space_chars;
		tOur++;
/*    if(!*tOur) {
        Clean();
        cout << "<RmFW failed>";
        return *this;
    }
*/	tOur++;
	// (*this)=tOur;
    char *t2Our=our_string;
    do {
        *t2Our=*tOur;
        t2Our++;
    } while(*tOur++);
	return *this;
}
CppString CppString::FirstWord(void)
{
	CppString s(our_string);
	s.RmLeadingSpaces();
	int i=s.FindFirst(' ');
	if(i>=0)
		s.CutFrom(i);
	return s;
}
CppString& CppString::FirstWord(CppString& str)
{
    char *tOur=our_string;
    while(*tOur&&IsSpace(*tOur))
        tOur++;
    if(!*tOur) return str;
    int i=0;
    while(*tOur&&!IsSpace(*tOur)) {
        str[i]=*tOur;
        tOur++; i++;
    }
    str[i]=0;
    return str;
}
void CppString::RmLeadingSpaces(void)
{
	int i=0;
	if(our_string[0]==0x00) return;
	while(true)
		if(IsSpace(our_string[i++]));
		else {
			if(i>1) (*this)=our_string+i-1;
			return;
		}
}
CppString& CppString::RmAllSpaces()
{
	char *tOur_from=our_string, *tOur_to=our_string;
	while(*tOur_from) {
		if(!IsSpace(*tOur_from)) {
			*tOur_to=*tOur_from;
			tOur_to++;
		}
		tOur_from++;
	}
	*tOur_to=0;
	return *this;
}
void CppString::RmControlChars(void)
{
	int i=0, j=0; char ch=our_string[0];
	char *buffer=new char[Length()+1];
	while(ch) {
		if(ch>31) {
			buffer[j++]=our_string[i++];
		}
		else i++;
		ch=our_string[i];
	}
	buffer[j]=0;
	(*this)=buffer;
	delete[]buffer;
}
bool CppString::IsSpace(char ch) const
{
	switch(ch) {
	case '\a': case '\b': case '\f': case '\n': case '\r': case '\t':
	case '\v': case ' ': case 0:
		return true;
	default:
		return false;
	}
}
bool CppString::IsSpace() const
{
    uint i=0;
    for(;i < Length(); i++)
        if(!IsSpace(our_string[i]))
           return false;
    return true;
}
int CppString::FindFirst(char ch) const
{
	int i=0;
	while(true)	{
		if(our_string[i]==ch) return(i);
		else if(our_string[i]==0x0) return -1;
		i++;
	}
}
bool CppString::IsHex(char ch) const
{
	if(	(ch >= '0' && ch <= '9') ||
		(ch >= 'a' && ch <= 'f') ||
		(ch >= 'A' && ch <= 'F') ) return true;
	return false;
}
bool CppString::IsHex() const
{
	char *tOur=our_string;
	while(*tOur)
		if(IsHex(*tOur)) tOur++;
		else return false;
	return true;
}

CppString& CppString::CutFrom(int from)
{
	if(from>=0)	{
		our_string[from]=0;
		(*this)=our_string;
		return *this;
	}
	(*this).Clean();
	return *this;
}
CppString& CppString::CutFirst(int from)
{
	(*this)=our_string+from;
	return *this;
}
CppString& CppString::Replace(char what, char with, bool only_first)
{
	char *tOur=our_string;
	while(*tOur) {
		if(*tOur==what) {
			*tOur=with;
			if(only_first)
			return *this;
		}
		tOur++;
	}
	return *this;
}
CppString& CppString::AddChar(char ch)
{
	int l=Length()+1;
	if(l>=allocdBytes)
		(*this)=our_string; // expanding space
	our_string[l-1]=ch;
	our_string[l]=0;
	return *this;
}
CppString& CppString::ToLower()
{
	char *tOur=our_string;
	while(*tOur)
	{
		*tOur=(char)tolower(*tOur);
		tOur++;
	}
	return *this;
}
CppString& CppString::ToUpper()
{
	char *tOur=our_string;
	while(*tOur)
	{
		*tOur=(char)toupper(*tOur);
		tOur++;
	}
	return *this;
}
bool CppString::Contains(char ch) const
{
	char *tOur=our_string;
	while(*tOur)
	{
		if(*tOur==ch) return true;
		tOur++;
	}
	return false;
}
CppString& CppString::format(const char *fstr, ...)
{
	char *buffer=new char[0x1000]; // 4096
	char *args=(char*)&fstr+sizeof(fstr);
	vsprintf(buffer,fstr,args);
	(*this)=buffer;
	delete[]buffer;
	return *this;
}
// #include <iostream.h>

CppString& CppString::InsertAt(int where, char what, char fChar)
{
	if(where<0) where=0; // return *this;
//	cout << " '" << what << "' (" << where << "} ";
	int i, l=Length()+1;
	if(where>=allocdBytes || l>=allocdBytes) {
//		cout << "Not enough mem\n";
		allocdBytes=OlorinMax(l+0x08, where+0x08);
		char *tmp_str = new char[allocdBytes];
		memcpy(tmp_str, our_string, l);
		delete[] our_string;
		our_string=tmp_str;
	}
	if(where >= l) {
//		cout << "Where too big\n";
		for(i=l-1; i<where; i++)
			our_string[i]=fChar;
		our_string[where+1]=0;
		our_string[where]=what;
	}
	else {
//		cout << "other case\n";
		our_string[l]=0;  //    f
		char tChar;
		for(i=where;i<l;i++) {
			tChar=our_string[i];
			our_string[i]=what;
			what=tChar;
		}
	}
	return *this;
}
CppString& CppString::Cut(int from, int to, CppString *sStorage)
{
	int l=Length();
	if(from<0) from=0;
	if(from>l) from=l;
	if(to<from) to=from;
	if(to>l) to=l;
	if(sStorage) {
		int nl=to-from;
		char *n_str=new char[nl+1];
		int i;
		for(i=0;i<nl;i++)
			n_str[i]=our_string[from+i];
		n_str[i]=0;
		*sStorage=n_str;
		delete[]n_str;
	}
	char *t1=our_string+from, *t2=our_string+to;
	do {
		*t1=*t2;
		t1++; t2++;
	} while(*t2);
	*t1=*t2;
	return *this;
}
// Logik: n_str is string to paste into 'this'.
// at==-1  -- Whole string to be replaced.
// at!=-1  -- string to be inserted at 'at'
//  & at2!=-1 -- string between at->at2 is to be replaced.
CppString& CppString::Paste(char *n_str, int at, int at2, CppString* sStore)
{
	if(at==-1)
		if(at2==-1) {
			(*this)=n_str;
			return *this;
		}
	if(at<0) at=0;
	if(at2!=-1) {
		if(at2<at) at2=at;
		Cut(at, at2, sStore);
	}
	Insert(n_str, at);
	return *this;
}
CppString& CppString::Insert(char *n_str, int at, char fChar)
{
	if(!n_str) return *this;
	if(at<0) at=0;
	int nsl = strlen(n_str);
	int nl;// = at + nsl;
	int sl = Length();
//	if(sl>at) nl += strlen(our_string+at);
	nl=nsl+OlorinMax(at, sl);
	char *t_str=new char[nl+1];
	if(sl>at)
		memcpy(t_str, our_string, at);
	else {
		memcpy(t_str, our_string, sl);
		memset(t_str+sl, fChar, at-sl);
	}
	memcpy(t_str+at, n_str, nsl);
	if(sl>at)
		memcpy(t_str+at+nsl, our_string+at, sl-at);
	t_str[nl]=0;
	(*this)=t_str;
	delete[] t_str;
	return *this;
}
int CppString::Search(char *f_str, int s_from)
{
	if((uint) s_from > Length()) return -1;
	if(!f_str) return -1;
	char *ptr1, *ptr2;
	char *t_str=our_string+s_from;
	// int i;
	while(*t_str) {
		if(*t_str==*f_str) {
			ptr1=t_str+1;
			ptr2=f_str+1;
			while(1) {
				if(!*ptr2) return t_str-our_string-s_from; // '0' in f_str, so found.
				if(!*ptr1) return -1; // '0' in our_str, search incomplete.
				if(*ptr1!=*ptr2) break;
				ptr1++; ptr2++;
			}
		}
		t_str++;
	}
	return -1;
}
CppString& CppString::HexToBinary(char tChar, char fChar, char *buffer)
{
    bool our_buffer=false;
    if(!buffer) {
        our_buffer=true;
	    buffer=new char[4096];
    }
	char *tOur=our_string;
	int num, j, i=0;
	while(1) {
		num=HexAsInt(*tOur); tOur++;
		if(num==-1) break;
		for(j=0;j<4;j++) {
			buffer[i++]=(num&(8>>j)) ? tChar: fChar;
		}
	}
	buffer[i]=0;
	*this=buffer;
	if(our_buffer) delete[]buffer;
	return *this;
}
int CppString::HexAsInt(char ch) const
{
	switch(ch) {
	case '0': return 0;	case '1': return 1;	case '2': return 2;
	case '3': return 3;	case '4': return 4;	case '5': return 5;
	case '6': return 6;	case '7': return 7;	case '8': return 8;
	case '9': return 9;
	case 'A': case 'a': return 10; case 'B': case 'b': return 11;
	case 'C': case 'c': return 12; case 'D': case 'd': return 13;
	case 'E': case 'e': return 14; case 'F': case 'f': return 15;
	default: return -1;
	}

}
char IntAsHex(char num, bool big=true)
{
    if(num<0) return '0';
    if(num>0xf) return '0';
    if(num<10) return '0' + num;
    if(big) return ('A' + num - 10);
    else return ('a' + num - 10);
}
CppString& CppString::BinToHex()
{
// cout << "\nBinToHex(" << *this << ")\n";
    int i=0, bits=0; char *tOur=our_string;
    char byte=0;
    while(*tOur) {
        byte |= (*tOur=='1' ? 1 : 0);
        bits++;
        if(bits==4) {
            our_string[i++]=IntAsHex(byte);
            byte=0; bits=0;
        }
        else
            byte<<=1;
        tOur++;
    }
    if(bits!=0) {
//      cout << "not all used. bits("<<bits<<"), byte("<<byte<<")\n";
        byte<<=3-bits;
        our_string[i++]=IntAsHex(byte);
    }
    our_string[i]=0;
    return *this;
}
