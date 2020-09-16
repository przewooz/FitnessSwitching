#include "stdafx.h"

#include "error.h"


using namespace System;
using namespace System::IO;
using namespace System::Reflection;


//this have to be like that, beacuse cwndErrorDialog is static
//CWnd* CError::cwndErrorDialog=NULL;

CError::CError()
{
	bshowed=false;
	csMessage="";
	csCaption="";
	iErrorStatus=0;
	i_file_index  =  0;
//	cwndErrorDialog=NULL;

}

/*CError::CError(CWnd* cwndErrorDialog)
{
	bshowed=false;
	csMessage="";
	csCaption="";
	iErrorStatus=0;
//	this->cwndErrorDialog=cwndErrorDialog;

}*/

CError::CError(CError &old)
{
	bshowed=old.bshowed;
	csMessage=old.csMessage;
	csCaption=old.csCaption;
	i_file_index  =  old.i_file_index;
	iErrorStatus=old.iErrorStatus;
	//cwndErrorDialog=old.cwndErrorDialog;
}//CError::CError(CError &old)

CError::~CError()
{

}

void CError::vPutMessage(int iErrorStatus,CString csMessage,CString csCaption/*=""*/)
{
	this->csMessage=csMessage;
	if (csCaption!="")
	{
		this->csCaption=csCaption;
	}else
	{
		if (iErrorStatus==1)
		{
			this->csCaption="ERROR...";
		}else
		if (iErrorStatus==2)
		{
			this->csCaption="Warning...";
		}

	}
	bshowed=false;
	this->iErrorStatus=iErrorStatus;
	vGenerateLog();
}


bool  CError::b_file_exists(CString  sFile)
{
	FILE  *pf_test;
	pf_test = fopen(sFile, "r");
	if  (pf_test  ==  NULL)  return(false);

	fclose(pf_test);
	return(true);
}//bool  CError::b_file_exists(CString  sFile)


void CError::vGenerateLog()
{

	FILE  *pf_error_log;

	CString s_application_dir;
	CString s_error_log_file_path;

	s_application_dir  =  System::Windows::Forms::Application::StartupPath;


	bool  b_index_found;
	b_index_found  =  false;
	long l_file_size;
	
	for  (int  ii = i_file_index;  b_index_found  ==  false; ii++)
	{
		s_error_log_file_path  =  s_application_dir + "\\" + s_get_log_file_name(ii);
		if  (b_file_exists(s_error_log_file_path)  ==  true)
		{
			pf_error_log  =  fopen(s_error_log_file_path,"a");
			//l_file_size = GetFileSize (pf_error_log, NULL);
			if  (pf_error_log  ==  NULL)  return;
			fseek(pf_error_log, 0, SEEK_END);
			l_file_size  =  ftell(pf_error_log);

			if  (l_file_size  <  ERROR_LOG_FILE_MAX_SIZE)
			{
				rewind(pf_error_log);
				b_index_found  =  true;
			}//if  (l_file_size  <  ERROR_LOG_FILE_MAX_SIZE)
			else
			{
				fclose(pf_error_log);
			}//else  if  (l_file_size  <  ERROR_LOG_FILE_MAX_SIZE)
			
		}//if  (System::IO::File::Exists(s_error_log_file_path)  ==  true)
		else
		{
			pf_error_log  =  fopen(s_error_log_file_path,"a");
			if  (pf_error_log  ==  NULL)  return;
			b_index_found  =  true;
		}//else  if  (System::IO::File::Exists(s_error_log_file_path)  ==  true)
	
	}//for  (int  ii = i_file_index;  b_index_found  ==  false; ii++)

			
	

	DateTime  dt_now;
	dt_now  =  DateTime::Now;

	CString  s_buf;

	s_buf  =  dt_now.ToString("yyyy-MM-dd HH:mm:ss");

	fprintf(pf_error_log,  "Error local time: %s\n",  (LPCSTR)  s_buf);
	
		
	fprintf(pf_error_log,  "Error Message:\n%s\n",  csMessage);
	fprintf(pf_error_log,  "______________\n\n");


	fclose(pf_error_log);
}//void CErrorSimple::vGenerateLog()


CString  CError::s_get_log_file_name(int  iIndex)
{
	CString  s_result;

	s_result.Format(ERROR_LOG_FILE, iIndex);
	
	return(s_result);	
}//CString  CErrorSimple::s_get_log_file_name(int  iIndex)


/*void CError::vShowWindow(UINT nID,CWnd* cwndParent/*=NULL*//*)
{
	if (!bshowed)
		INT_PTR handle=DialogBox(NULL,MAKEINTRESOURCE(nID),cwndParent->m_hWnd,NULL);
	bshowed=true;
}*/

void CError::vShowWindow()
{
	if (!iErrorStatus) return;
	if (!bshowed)
	{
//		if (cwndErrorDialog==NULL)
			MessageBox(NULL,csMessage,csCaption,MB_OK);
/*		else 
		{
			cwndErrorDialog->ShowWindow(SW_SHOW);
			cwndErrorDialog->RunModalLoop();
		}*/
	}
	bshowed=true;
}

void CError::vPutSystemErrorMessage(CString csMessageToAdd,CString csCaption/*="ERROR"*/)
{
	LPVOID lpMsgBuf;
	FormatMessage
	( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	csMessageToAdd +=  (LPCTSTR) lpMsgBuf;

	vPutError(csMessageToAdd, csCaption);
}