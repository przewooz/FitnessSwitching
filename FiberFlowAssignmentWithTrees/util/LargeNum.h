#pragma once

//tools
#include  "atlstr.h"  //CString
#include  "atlpath.h"

//system tools
#include  "..\\error\\error.h"


//vector
#include  <vector>
#include  <iostream>
#include  <functional>
#include  <algorithm>
#include <windows.h>
#include  <math.h>

#include  "tools.h"

using namespace std;

#define  s_DIGITS		"0123456789"


namespace LargeNum
{
	class  CNumber
	{
	public:
		CNumber();
		~CNumber();

		void  vSetValue(int  iValue);
		void  vAdd(CNumber  *pcOther);
		CNumber cMul(CNumber  *pcOther);
		CNumber cMulByDigit(int iDigit);
		void  vShiftBy(int iShift);

		int  iGetDigitAt(int iPos);

		CString sToString();
		CString sToString10Pow();
		CString sToStringFirstDgitsDotted(int iDigitNumber);

	private:
		vector<int>  v_number;

		int  i_get_base();
	
	};//class  CNumber


};//namespace LargeNum