//
// ServiceInfo.h
//

#pragma once

#include "pch.h"

#define INITIAL_FILE _T("ServiceConfig.ini")

class DECLSPEC ServiceInfo
{
public:
	ServiceInfo();
	ServiceInfo(LPCTSTR lpctszServiceName);

	BOOL Initialize(LPCTSTR lpctszSectionName);

	LPCTSTR GetWorkDirectory()		{ return _tszWorkDirectory; };
	LPCTSTR GetName()				{ return _tszServiceName; };
	LPCTSTR GetDisplayName()		{ return _tszServiceDisplayName; };
	LPCTSTR GetDescription()		{ return _tszServiceDescription; };
	LPCTSTR GetExecutePath()		{ return _tszExecutePath; };
	LPCTSTR GetDependentService()	{ return _tszDependentService; };
	DWORD GetStartType()
	{
		return _bAutoStart ? SERVICE_AUTO_START : SERVICE_DEMAND_START;
	};

private:

	TCHAR _tszWorkDirectory[MAX_PATH];
	TCHAR _tszServiceName[MAX_PATH];
	TCHAR _tszServiceDisplayName[MAX_PATH];
	TCHAR _tszServiceDescription[MAX_PATH];
	TCHAR _tszExecutePath[MAX_PATH];
	TCHAR _tszDependentService[MAX_PATH];
	BOOL _bAutoStart;
};