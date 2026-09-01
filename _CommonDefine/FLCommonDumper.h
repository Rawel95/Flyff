
#ifndef	__FLCOMMONDUMPER_H__
#define	__FLCOMMONDUMPER_H__


#include <dump/FLDumper.h>
#include <dump/FLGenericDumper.h>
#ifdef __ZCK_MINIDUMP
#include "ZckMiniDump.h"
#endif // __ZCK_MINIDUMP

class	FLCommonDumper : public FLDumper
{
public:

	static FLCommonDumper*	GetInstance()
	{
		static FLCommonDumper kCommonDumper( NULL );
		return &kCommonDumper;
	}

public:

	explicit FLCommonDumper( const TCHAR* szPrefix )
	{
		m_kGenericDumper.SetPrefix( szPrefix );
	}

private:

bool	DoDump( T_DUMP_PARAM* pParam )
{
	FLLOG_FLUSH();

#ifdef __ZCK_MINIDUMP
	if( pParam != NULL && pParam->pException != NULL )
	{
		DWORD dwZckMiniDumpError = ERROR_SUCCESS;
		if( ZckMiniDump::Write( pParam->pException, &dwZckMiniDumpError ) == false )
		{
			FLERROR_LOG(
				PROGRAM_NAME,
				_T( "[ ZCK_MINIDUMP - FAILED : ERROR(%u) ]" ),
				dwZckMiniDumpError
				);
		}
	}
#endif // __ZCK_MINIDUMP

	return m_kGenericDumper.Dump( pParam );
}

private:

	FLGenericDumper	m_kGenericDumper;

};






#endif