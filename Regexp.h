#include "kln90b.h"
#ifndef __REGEXP_H__
#define __REGEXP_H__

typedef std::string cString;
class regexp;

class Regexp
{
public:
	enum { NSUBEXP = 10 };

	Regexp();
	Regexp( LPCTSTR exp, BOOL iCase = 0 );
	Regexp( const Regexp &r );
	~Regexp();
	const Regexp & operator=( const Regexp & r );

	bool Match( const TCHAR * s );
	int SubStrings() const;
	
	const cString operator[]( unsigned int i ) const;
	int SubStart( unsigned int i ) const;
	int SubLength( unsigned int i ) const;

	cString GetReplaceString( LPCTSTR source ) const;

	cString GetErrorString() const;
	bool CompiledOK() const;

#if defined( _RE_DEBUG )
	void Dump();
#endif
private:
	const TCHAR * string;	/* used to return substring offsets only */
	mutable cString m_szError;
	regexp * rc;

	void ClearErrorString() const;
	int safeIndex( unsigned int i ) const;

};

#endif

