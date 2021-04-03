#include <Windows.h>
#include <tchar.h>
#include <locale.h>

#include "Sample.hpp"

int _tmain(int argc, TCHAR** argv)
{
	_tsetlocale(LC_ALL, _T("Japanese"));

	Sample sample;
	sample.TEST(3, _T("サンプル"), _T("テスト"), _T(""));
}
