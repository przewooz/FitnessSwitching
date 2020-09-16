#pragma once

//tools
#include  "atlstr.h"  //CString
#include  "atlpath.h"

//system tools
#include  "..\error\Error.h"

#define  WATCH_FITNESS_LEVELS	false



namespace Tools
{
	CString  sGetDirectory(CString  sFilePath);
	CString  sGetExtension(CString  sFilePath);

	CError  eAnalyzeRunFile(FILE  *pfSource,  FILE*  pfDest);
	CString  sReadLine(FILE  *pfSource,   CString  *psComment  =  NULL);
	int  iReadLine(FILE  *pfSource,   CString  *psComment  =  NULL);
	double  dReadLine(FILE  *pfSource,   CString  *psComment  =  NULL);
	int  iExtractFromString(CString  sLineToSearch, int  iIndex /*= 0*/, int  *piFinishIndex = NULL );
	double  dExtractFromString(CString  sLineToSearch, int  iIndex, int  *piFinishIndex = NULL );
	CString  sExtractFromString(CString  sLineToSearch, int  iIndex, int  *piFinishIndex = NULL );

	CString  sExtractFromStringBackwards(CString  sLineToSearch, int  iIndex, int  *piFinishIndex = NULL );


	int iAbs(int  iVal);

	void  vRepInFile(CString  sFile, CString  sText, bool  bMakeNew = false);
	void  vShow(CString  sText);
	void  vShow(double  dVal);
};//namespace Tools