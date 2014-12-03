/* Time-stamp: <2001-05-31 00:26:41 pvsavola> */
#if !defined(CppString_H_INCLUDED)
#define CppString_H_INCLUDED
#include <string.h>
#include <fstream>
#include <stdlib.h>

using std::ifstream;

class CppString
{
private:
	char *our_string;
	int allocdBytes;
    CppString&  AllocUs(int hMuch=255, char *aString=0, bool ClearMem=true);
public:
	CppString();
	CppString(const char *str);
	CppString(CppString &str);
	CppString(int length);
	~CppString();
	char* 	c_str(void) { return(our_string); };
   	const char* c_str(void) const { return(our_string); };
	void 		Clean(void);
	size_t 		Length(void) const { if(our_string) return(strlen(our_string));else return 0; };
	bool 		IsSpace(char ch) const;
    bool        IsSpace() const;
	bool		IsEmpty() const { return !Length(); };
	bool		IsHex(char ch) const;
	bool		IsHex() const;
	int			FindFirst(char ch) const;
	bool		Contains(char ch) const;
	int			AsInteger(void) const { return atoi(our_string?our_string:"0"); };
	int			HexAsInt(char ch) const;
	CppString&	HexToBinary(char tChar='1', char fChar='0', char *buffer=0);
    CppString&  BinToHex();
	CppString   FirstWord(void);
    CppString&  FirstWord(CppString& str);
	CppString&	CutFrom(int from);
	CppString&	CutFirst(int from);
	CppString&	Replace(char what, char with, bool only_first=false);
	CppString& 	RmCPPComment(void);
	CppString& 	RmUnixComment(void);
	CppString&	RmAllSpaces();
	CppString&	RmFirstWord();
	void		RmLeadingSpaces(void);
	void		RmControlChars(void);
	CppString&	ToLower();
	CppString&	ToUpper();
	CppString&	AddChar(char ch);
	CppString&	operator<<(char ch) { return AddChar(ch); };
	CppString& 	operator<<(char *astr) { return (*this)+=astr; };
	CppString& 	operator<<(const CppString& astr) { return (*this)+=astr; };
	CppString& 	AddNumber(long double val);
	CppString& 	operator=(const char *astr);
	CppString& 	operator=(const CppString &astr);
	CppString& 	operator=(int c) { format("%d",c); return *this; }; // refurnish!!!
//	CppString	operator+(CppString &astr);
	CppString	operator+(const char *astr);
	CppString&	operator+=(const char ch);
	CppString& 	operator+=(const char *astr);
///	CppString&	operator+=(CppString &a_str);
	CppString&  getline(ifstream& file);
	CppString&  getword(ifstream& file, bool rmUXComm=false);
	CppString&	format(const char *fstr, ...);
	CppString&	InsertAt(int where, char what, char fillChar=' ');
	CppString&	Cut(int from, int to, CppString *sStorage=0);
	CppString&	Paste(char *n_str, int at=-1, int at2=-1, CppString* sStorage=0);
	CppString&	Insert(char *n_str, int at, char fChar=' ');
	int			Search(char *f_str, int s_from=0);
	bool		operator==(const char *a_str) const { return (strcmp(our_string, a_str)==0); };
    bool		operator==(const CppString &a_str) const { return (strcmp(our_string, a_str)==0); };
	bool		operator<(const char *a_str) const { return (strcmp(our_string,a_str)<0); };
	bool		operator>(const char *a_str) const { return (strcmp(our_string,a_str)>0); };
//	bool		operator<(const CppString &astr) { return (*this)<astr.c_str(); };
//	bool		operator>(const CppString &astr) { return (*this)<astr.c_str(); };
//	bool		operator==(const CppString &astr) { return (*this)==astr.c_str(); };
	bool		operator!=(const char *a_str) const { return (strcmp(our_string, a_str)!=0); };
//	bool		operator!=(const CppString &astr) { return (*this)!=astr.c_str(); };
	char& 	operator[](int i);
	operator char*() { return our_string; };
	operator const char*() const { return our_string;};
    operator const char*() { return our_string; };
        
	char		LastChar(void)
	{
		if(our_string) return our_string[Length()-1];
		else return 0;
	}
	friend std::ostream& operator<<(std::ostream &cou, CppString &str);
};

// friend ostream& operator<<(ostream &cou, CppString &str);

#endif // CppString_H_INCLUDED
