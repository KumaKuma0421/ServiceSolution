//
// ServiceInfo.h
//

#pragma once

#include "pch.h"

class DECLSPEC ServiceInfo
{
public:
	ServiceInfo(
		LPCTSTR lpctszWorkDirectory,
		LPCTSTR lpctszServiceName,
		LPCTSTR lpctszServiceDisplayName,
		LPCTSTR lpctszServiceDescription)
	{
		_lpctszWorkDirectory = lpctszWorkDirectory;
		_lpctszServiceName = lpctszServiceName;
		_lpctszServiceDisplayName = lpctszServiceDisplayName;
		_lpctszServiceDescription = lpctszServiceDescription;
	};

	LPCTSTR GetWorkDirectory() { return _lpctszWorkDirectory; };
	LPCTSTR GetName() { return _lpctszServiceName; };
	LPCTSTR GetDisplayName() { return _lpctszServiceDisplayName; };
	LPCTSTR GetDescription() { return _lpctszServiceDescription; };

private:
	ServiceInfo();

	LPCTSTR _lpctszWorkDirectory;
	LPCTSTR _lpctszServiceName;
	LPCTSTR _lpctszServiceDisplayName;
	LPCTSTR _lpctszServiceDescription;
};