#pragma once

#include <stdarg.h>

class Sample
{
public:
	Sample () {};
	~Sample () {};

	VOID TEST (int args, ...)
	{
		LPCTSTR lpctszArgs[3] = { _T (""), _T (""), _T ("") };
		va_list argptr;

		va_start (argptr, args);
		for (int i = 0; i < args; i++)
		{
			lpctszArgs[i] = va_arg (argptr, LPCTSTR);
		}
		va_end (argptr);

		for (int i = 0; i < args; i++)
		{
			_tprintf (_T ("%d:%s\n"), i, lpctszArgs[i]);
		}
	}
};