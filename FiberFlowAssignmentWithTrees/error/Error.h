#pragma once

//tools
#include  "atlstr.h"  //CString
#include <windows.h>



#define  ERROR_LOG_FILE		"error_%.4d.log"
#define  ERROR_LOG_FILE_MAX_SIZE	500000   //500kB
class CError
{
public:
	CError();
	CError(CError &old);
	~CError();

	operator int()  {return(iErrorStatus);};


	//iErrorStatus: 0="OK",1="ERROR",2="Warning..."
	void vPutMessage(int iErrorStatus,CString csMessage,CString csCaption="");
	void vPutError(CString csMessage,CString csCaption="ERROR..."){vPutMessage(1,csMessage,csCaption);};
	void vPutWarning(CString csMessage,CString csCaption="Warning..."){vPutMessage(2,csMessage,csCaption);};
	
	void vPutSystemErrorMessage(CString csMessageToAdd,CString csCaption="ERROR");
	
	void vShowWindow();

	CString csMessage;
	CString csCaption;

	//0="OK",1="ERROR",2="Warning..."
	int iErrorStatus;

private:
	void vGenerateLog();
	CString  s_get_log_file_name(int  iIndex);
	bool  b_file_exists(CString  sFile);
	bool bshowed;

	int i_file_index;//error file index

};


//#endif // !defined(__CEROOR_CLASS_)
