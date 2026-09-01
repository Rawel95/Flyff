#pragma once

#ifdef __ZCK_MINIDUMP

#include <windows.h>
#include <dbghelp.h>
#include <tchar.h>
#include <stdio.h>
#include <stddef.h>

namespace ZckMiniDump
{
	typedef BOOL (WINAPI* PFN_ZCK_MINIDUMPWRITEDUMP)(
		HANDLE,
		DWORD,
		HANDLE,
		MINIDUMP_TYPE,
		PMINIDUMP_EXCEPTION_INFORMATION,
		PMINIDUMP_USER_STREAM_INFORMATION,
		PMINIDUMP_CALLBACK_INFORMATION
		);

	inline volatile LONG* GetDumpState()
	{
		static volatile LONG s_lDumpInProgress = 0;
		return &s_lDumpInProgress;
	}

	inline HMODULE LoadDbgHelp()
	{
		TCHAR szDbgHelpPath[1024] = { 0, };
		const size_t cchDbgHelpPath = sizeof(szDbgHelpPath) / sizeof(szDbgHelpPath[0]);

		const DWORD dwLength = ::GetModuleFileName(
			NULL,
			szDbgHelpPath,
			static_cast<DWORD>(cchDbgHelpPath)
			);

		if (dwLength > 0 && dwLength < cchDbgHelpPath)
		{
			TCHAR* pSlash = _tcsrchr(szDbgHelpPath, _T('\\'));
			if (pSlash != NULL)
			{
				*(pSlash + 1) = _T('\0');

				if (_tcscat_s(szDbgHelpPath, cchDbgHelpPath, _T("DbgHelp.dll")) == 0)
				{
					HMODULE hDbgHelp = ::LoadLibrary(szDbgHelpPath);
					if (hDbgHelp != NULL)
						return hDbgHelp;
				}
			}
		}

		return ::LoadLibrary(_T("DbgHelp.dll"));
	}

	inline bool BuildDumpPath(TCHAR* pszDumpPath, size_t cchDumpPath)
	{
		if (pszDumpPath == NULL || cchDumpPath == 0)
			return false;

		pszDumpPath[0] = _T('\0');

		const DWORD dwLength = ::GetModuleFileName(
			NULL,
			pszDumpPath,
			static_cast<DWORD>(cchDumpPath)
			);

		if (dwLength == 0 || dwLength >= cchDumpPath)
		{
			pszDumpPath[0] = _T('\0');
			return false;
		}

		TCHAR* pSlash = _tcsrchr(pszDumpPath, _T('\\'));
		TCHAR* pDot = _tcsrchr(pszDumpPath, _T('.'));

		if (pDot != NULL && (pSlash == NULL || pDot > pSlash))
			*pDot = _T('\0');

		SYSTEMTIME st = { 0, };
		::GetLocalTime(&st);

		const size_t cchCurrent = _tcslen(pszDumpPath);
		if (cchCurrent >= cchDumpPath)
			return false;

		const int nResult = _sntprintf_s(
			pszDumpPath + cchCurrent,
			cchDumpPath - cchCurrent,
			_TRUNCATE,
			_T("_%04u-%02u-%02u_%02u-%02u-%02u-%03u.dmp"),
			static_cast<unsigned int>(st.wYear),
			static_cast<unsigned int>(st.wMonth),
			static_cast<unsigned int>(st.wDay),
			static_cast<unsigned int>(st.wHour),
			static_cast<unsigned int>(st.wMinute),
			static_cast<unsigned int>(st.wSecond),
			static_cast<unsigned int>(st.wMilliseconds)
			);

		return nResult >= 0;
	}

	inline bool Write(EXCEPTION_POINTERS* pException, DWORD* pdwError = NULL)
	{
		if (pdwError != NULL)
			*pdwError = ERROR_SUCCESS;

		if (pException == NULL)
		{
			if (pdwError != NULL)
				*pdwError = ERROR_INVALID_PARAMETER;
			return false;
		}

		volatile LONG* pDumpState = GetDumpState();
		if (::InterlockedCompareExchange(pDumpState, 1, 0) != 0)
		{
			if (pdwError != NULL)
				*pdwError = ERROR_BUSY;
			return false;
		}

		TCHAR szDumpPath[1024] = { 0, };
		const size_t cchDumpPath = sizeof(szDumpPath) / sizeof(szDumpPath[0]);

		if (BuildDumpPath(szDumpPath, cchDumpPath) == false)
		{
			if (pdwError != NULL)
				*pdwError = ERROR_INSUFFICIENT_BUFFER;
			::InterlockedExchange(pDumpState, 0);
			return false;
		}

		HMODULE hDbgHelp = LoadDbgHelp();
		if (hDbgHelp == NULL)
		{
			if (pdwError != NULL)
				*pdwError = ::GetLastError();
			::InterlockedExchange(pDumpState, 0);
			return false;
		}

		PFN_ZCK_MINIDUMPWRITEDUMP pMiniDumpWriteDump =
			reinterpret_cast<PFN_ZCK_MINIDUMPWRITEDUMP>(
				::GetProcAddress(hDbgHelp, "MiniDumpWriteDump")
				);

		if (pMiniDumpWriteDump == NULL)
		{
			if (pdwError != NULL)
				*pdwError = ERROR_PROC_NOT_FOUND;
			::FreeLibrary(hDbgHelp);
			::InterlockedExchange(pDumpState, 0);
			return false;
		}

		HANDLE hFile = ::CreateFile(
			szDumpPath,
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			if (pdwError != NULL)
				*pdwError = ::GetLastError();
			::FreeLibrary(hDbgHelp);
			::InterlockedExchange(pDumpState, 0);
			return false;
		}

		MINIDUMP_EXCEPTION_INFORMATION kExceptionInfo = { 0, };
		kExceptionInfo.ThreadId = ::GetCurrentThreadId();
		kExceptionInfo.ExceptionPointers = pException;
		kExceptionInfo.ClientPointers = FALSE;

		const BOOL bResult = pMiniDumpWriteDump(
			::GetCurrentProcess(),
			::GetCurrentProcessId(),
			hFile,
			MiniDumpWithFullMemory,
			&kExceptionInfo,
			NULL,
			NULL
			);

		DWORD dwError = ERROR_SUCCESS;
		if (bResult == FALSE)
			dwError = ::GetLastError();

		::CloseHandle(hFile);
		::FreeLibrary(hDbgHelp);
		::InterlockedExchange(pDumpState, 0);

		if (pdwError != NULL)
			*pdwError = dwError;

		return bResult == TRUE;
	}
}

#endif // __ZCK_MINIDUMP