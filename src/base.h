#if !defined(BASE_H_INCLUDED)
#define BASE_H_INCLUDED
#include "cppstring.hh"
#define	TBC_SETFATAL(x)	((x)|=(1<<31))
#define	TBC_GETFATAL(x)	((x)&(1<<31))
#define	TBC_OK			(0)
#define	TBC_FATAL		(1<<31)
class TBaseClass
{
private:
	CppString error, error_explain;
	long errorN;
protected:
	void GenerateError(char *nerr=0, char *nerr_expl=0, long nerrN=TBC_OK)
	{
		error=nerr;
		error_explain=nerr_expl;
		errorN=nerrN;
	}
	void ClearErrors() { GenerateError("OK","OK",TBC_OK); };
public:
	TBaseClass() : errorN(TBC_OK) { ClearErrors(); };
	bool OK() { return(errorN==TBC_OK); };
	bool Fail() { return(errorN!=TBC_OK); };
	bool Fatal() { return(TBC_GETFATAL(errorN)); };
	const char * const Error() const { return error; };
	const char * ErrorExplain() const { return error_explain; };
};
#endif // BASE_H_INCLUDED
