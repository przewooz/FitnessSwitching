#include "stdafx.h"

#include  "tools.h"




CString  Tools::sGetDirectory(CString  sFilePath)
{
	char  c_buf;
	for  (int  ii = sFilePath.GetLength() - 1; ii > 0; ii--)
	{
		c_buf = sFilePath.GetAt(ii);

		if  (c_buf  ==  '\\')  return(sFilePath.Mid(0, ii + 1));
	}//for  (int  ii = sFilePath.GetLength() - 1; ii > 0; ii++)

	return("");
}//CString  Tools::sGetDirectory(CString  sFilePath)



CString  Tools::sGetExtension(CString  sFilePath)
{
	CString  s_result;

	char c_buf;

	s_result = "";
	for  (int  ii = sFilePath.GetLength() - 1; (ii > 0)&&(ii > sFilePath.GetLength() - 4); ii--)
	{
		c_buf = sFilePath.GetAt(ii);

		if  (c_buf  ==  '.')  return(s_result);
		s_result.Insert(0, c_buf);
	}//for  (int  ii = sFilePath.GetLength() - 1; ii > 0; ii++)

	return(s_result);
}//CString  Tools::sGetExtension(CString  sFilePath)



CString  Tools::sReadLine(FILE  *pfSource,  CString  *psComment /*= NULL*/)
{
	char  c_buf, c_comment_buf;
	CString  s_res,  s_comment;

	bool  b_comment  =  false;


	//init
	c_buf  =  'a';
	c_comment_buf  =  'a';
	s_comment  =  "";
	s_res  =  "";
	while( (!feof(pfSource))&&(c_buf != '\n') )
	{
		fscanf(pfSource, "%c",&c_buf);

		if  (c_buf != '\n')
		{

			if  (b_comment  ==  false)
			{
			
				if  (c_buf  ==  '\\') //everything that after comment sign '//' is ignored
				{
					if  (c_comment_buf  ==  '\\')
						b_comment  =  true;
					else
						c_comment_buf  =  '\\';
					
				}//if  (c_buf  ==  '\\')
				else
				{
					if  (c_comment_buf  ==  '\\')  s_res  =  s_res  +  c_comment_buf; //if something was bufferred we have to give it back
					s_res  =  s_res  +  c_buf;
					c_comment_buf  =  'a';//we have to reset the comment buffer
				}//else  if  (c_buf  ==  '\\')

			}//if  (b_comment  ==  false)
			else
			{
				s_comment  =  s_comment  +  c_buf;			
			}//else  if  (b_comment  ==  false)
		}//if  (c_buf != '\n')

	}//while( (!eof(pfSource))&&(c_buf != '\n') )

	if  (feof(pfSource))
	{
		if  (b_comment  ==  false)
			if  (s_res.GetLength() > 0)
				s_res.Delete(s_res.GetLength() - 1);
		else
			if  (s_comment.GetLength() > 0)
				s_comment.Delete(s_comment.GetLength() - 1);

	
	}//if  (eof(pfSource))

	if  (psComment  !=  NULL)  *psComment  =  s_comment;
	return(s_res);
}//CString  Tools::sReadLine(FILE  *pfSource)



int  Tools::iReadLine(FILE  *pfSource,   CString  *psComment)
{
	CString  s_result;
	int  i_result;

	s_result  =  Tools::sReadLine(pfSource,  psComment);
	i_result  =  atoi(s_result);

	return(i_result);
};//int  Tools::iReadLine(FILE  *pfSource,   CString  *psComment  =  NULL)



double  Tools::dReadLine(FILE  *pfSource,   CString  *psComment)
{
	CString  s_result;
	double  d_result;

	s_result  =  Tools::sReadLine(pfSource,  psComment);
	d_result  =  atof(s_result);

	return(d_result);
};//double  Tools::dReadLine(FILE  *pfSource,   CString  *psComment)



CString  Tools::sExtractFromStringBackwards(CString  sLineToSearch, int  iIndex, int  *piFinishIndex  /*= NULL*/)
{
	CString  s_result;

	char  c_buf;
	bool  b_analyzing  =  true;
	bool  b_string_started = false;

	int  ii;
	for  (ii = iIndex;  (ii  >= 0)&&(b_analyzing  ==  true);  ii--)
	{
		if  (ii <  sLineToSearch.GetLength())
		{
			b_analyzing  =  false;
			c_buf  =  sLineToSearch.GetAt(ii);

			if  ( (c_buf  != ' ')&&(c_buf  != '\t') )
			{
				s_result  +=  c_buf;
				b_analyzing  =  true;
				b_string_started = true;
			}//if  ( (c_buf  != ' ')&&(c_buf  != '\t') )


			if  ( (c_buf  == ' ')||(c_buf  == '\t') )
			{
				if  (b_string_started == false)  b_analyzing  =  true;
			}//if  ( (c_buf  == ' ')||(c_buf  == '\t') )

		}//if  (ii <  sLineToSearch.GetLength())		
	}//for  (int  ii = 0;  ii  <  sLineToSearch.GetLength();  ii++)


	//reverse result
	CString  s_result_reverted;
	for  (int  ii = s_result.GetLength() - 1; ii  >= 0; ii--)
		s_result_reverted += s_result.GetAt(ii);


	
	if  (piFinishIndex != NULL)  *piFinishIndex = ii;
	return(s_result_reverted);
}//CString  Tools::sExtractFromStringBackwards(CString  sLineToSearch, int  iIndex, int  *piFinishIndex = NULL)



CString  Tools::sExtractFromString(CString  sLineToSearch, int  iIndex, int  *piFinishIndex /*= NULL*/ )
{
	CString  s_result;

	char  c_buf;
	bool  b_analyzing  =  true;
	bool  b_string_started = false;

	int  ii;
	for  (ii = iIndex;  (ii  <  sLineToSearch.GetLength())&&(b_analyzing  ==  true);  ii++)
	{
		b_analyzing  =  false;
		c_buf  =  sLineToSearch.GetAt(ii);

		if  ( (c_buf  != ' ')&&(c_buf  != '\t') )
		{
			s_result  +=  c_buf;
			b_analyzing  =  true;
			b_string_started = true;
		}//if  ( (c_buf  != ' ')&&(c_buf  != '\t') )


		if  ( (c_buf  == ' ')||(c_buf  == '\t') )
		{
			if  (b_string_started == false)  b_analyzing  =  true;
		}//if  ( (c_buf  == ' ')||(c_buf  == '\t') )

	}//for  (int  ii = 0;  ii  <  sLineToSearch.GetLength();  ii++)


	if  (piFinishIndex != NULL)  *piFinishIndex = ii;
	return(s_result);
}//CString  Tools::sExtractFromString(CString  sLineToSearch, int  iIndex, int  *piFinishIndex /*= NULL*/ )



int  Tools::iExtractFromString(CString  sLineToSearch, int  iIndex /*= 0*/, int  *piFinishIndex )
{
	CString  s_result;
	int  i_result;

	char  c_buf;
	bool  b_analyzing  =  true;
	bool  b_number_started = false;

	int  ii;
	for  (ii = iIndex;  (ii  <  sLineToSearch.GetLength())&&(b_analyzing  ==  true);  ii++)
	{
		b_analyzing  =  false;
		c_buf  =  sLineToSearch.GetAt(ii);

		if  ( 
			((c_buf  >=  '0')&&(c_buf  <=  '9'))
			||
			(c_buf  ==  '-') 
			)
		{
			s_result  +=  c_buf;
			b_analyzing  =  true;
			b_number_started = true;
		}//if  ( (c_buf  >=  '0')&&(c_buf  <=  '9') )

		if  ( (c_buf  == ' ')||(c_buf  == '\t') )
		{
			if  (b_number_started == false)  b_analyzing  =  true;
		}//if  (c_buf  == ' ')  
		
	}//for  (int  ii = 0;  ii  <  sLineToSearch.GetLength();  ii++)

	
	if  (piFinishIndex != NULL)  *piFinishIndex = ii;
	i_result =  atoi(s_result);	
	return(i_result);
}//int  Tools::iExtractFromString(sLineToSearch, int  iIndex /*= 0*/)



double  Tools::dExtractFromString(CString  sLineToSearch, int  iIndex, int  *piFinishIndex )
{
	CString  s_result;
	double  d_result;

	char  c_buf;
	bool  b_analyzing  =  true;
	bool  b_number_started = false;

	int  ii;
	for  (ii = iIndex;  (ii  <  sLineToSearch.GetLength())&&(b_analyzing  ==  true);  ii++)
	{
		b_analyzing  =  false;
		c_buf  =  sLineToSearch.GetAt(ii);

		if  ( ((c_buf  >=  '0')&&(c_buf  <=  '9'))||(c_buf  ==  '.') )
		{
			s_result  +=  c_buf;
			b_analyzing  =  true;
			b_number_started = true;
		}//if  ( (c_buf  >=  '0')&&(c_buf  <=  '9') )

		if  ( (c_buf  == ' ')||(c_buf  == '\t') )
		{
			if  (b_number_started == false)  b_analyzing  =  true;
		}//if  (c_buf  == ' ')  
		
	}//for  (int  ii = 0;  ii  <  sLineToSearch.GetLength();  ii++)

	if  (piFinishIndex != NULL)  *piFinishIndex = ii;
	d_result =  atof(s_result);	

	return(d_result);
}//double  Tools::dExtractFromString(CString  sLineToSearch, int  iIndex /*= 0*/)



int Tools::iAbs(int  iVal)
{
	if  (iVal  <  0)  return(iVal * (-1));

	return(iVal);
}//int Tools::iAbs(int  iVal)



CError  Tools::eAnalyzeRunFile(FILE  *pfSource,  FILE*  pfDest)
{
	CError  c_err;

	CString  s_read_line, s_comment,  s_text_to_search;

	
	int  i_found,  i_buf;
	CString  s_buf;

	bool  b_line_analyzed;
	int  i_epoch_number;
	int  i_generation;
	int  i_ct_number;
	double  d_time,  d_best_fitness,  d_avr_fitness, d_eval;

	i_epoch_number  =  0;
	i_generation  =  0;
	i_ct_number  =  0;
	d_time  =  0;
	d_best_fitness  =  0;
	d_avr_fitness  =  0;
	d_eval  =  0;

	fprintf
	(
	pfDest,
	"Epochs \t Generations: \t CT number: \t Best fitness: \t CT average fitness: \t Evaluations: \t Time\n\n", 
	i_epoch_number, i_generation, i_ct_number, d_best_fitness, d_avr_fitness, d_eval
	);

	i_epoch_number  =  0;
	while  (!feof(pfSource))
	{
		b_line_analyzed  =  false;
		s_read_line  =  sReadLine(pfSource,  &s_comment);
		
		//first check if it ct nummber refresh
		s_text_to_search  =  "ct number refresh; number after refresh:";
		i_found  =  s_read_line.Find(s_text_to_search);
		if  ( (i_found  >=  0)&&(b_line_analyzed  ==  false) )
		{
			b_line_analyzed  =  true;

			fprintf
				(
				pfDest,
				"%d\t%d\t%d\t%.8lf\t%.8lf\t%.8lf\t%.8lf\n", 
				i_epoch_number, i_generation, i_ct_number, d_best_fitness, d_avr_fitness, d_eval, d_time
				);
			i_epoch_number++;		
		}//if  ( (i_found  >=  0)&&(b_line_analyzed  ==  false) )

		//generation - extracting generation, time, fitness, ct number...
		s_text_to_search  =  "Generation:";
		i_found  =  s_read_line.Find(s_text_to_search);
		if  ( (i_found  >=  0)&&(b_line_analyzed  ==  false) )
		{
			b_line_analyzed  =  true;


			i_buf  =  s_read_line.Find(s_text_to_search);
			if  (i_buf  <  0)
			{
				c_err.vPutError("couldnt find generation");
				return(c_err);			
			}//if  (i_buf  <  0)
			i_generation  =  iExtractFromString(s_read_line, i_buf + s_text_to_search.GetLength());


			s_text_to_search  =  "ct number:";
			i_buf  =  s_read_line.Find(s_text_to_search);
			if  (i_buf  <  0)
			{
				c_err.vPutError("couldnt find ct number");
				return(c_err);			
			}//if  (i_buf  <  0)
			i_ct_number  =  iExtractFromString(s_read_line, i_buf + s_text_to_search.GetLength());


			s_text_to_search  =  "best fitness:";
			i_buf  =  s_read_line.Find(s_text_to_search);
			if  (i_buf  <  0)
			{
				c_err.vPutError("couldnt find best fitness:");
				return(c_err);			
			}//if  (i_buf  <  0)
			d_best_fitness  =  dExtractFromString(s_read_line, i_buf + s_text_to_search.GetLength());


			s_text_to_search  =  "average:";
			i_buf  =  s_read_line.Find(s_text_to_search);
			if  (i_buf  <  0)
			{
				c_err.vPutError("couldnt find average:");
				return(c_err);			
			}//if  (i_buf  <  0)
			d_avr_fitness  =  dExtractFromString(s_read_line, i_buf + s_text_to_search.GetLength());


			s_text_to_search  =  "fitness evaluations:";
			i_buf  =  s_read_line.Find(s_text_to_search);
			if  (i_buf  <  0)
			{
				c_err.vPutError("couldnt find fitness evaluations:");
				return(c_err);			
			}//if  (i_buf  <  0)
			d_eval  =  dExtractFromString(s_read_line, i_buf + s_text_to_search.GetLength());



			s_text_to_search  =  "time passed:";
			i_buf  =  s_read_line.Find(s_text_to_search);
			if  (i_buf  <  0)
			{
				c_err.vPutError("couldnt find time passed:");
				return(c_err);			
			}//if  (i_buf  <  0)
			d_time  =  dExtractFromString(s_read_line, i_buf + s_text_to_search.GetLength());
			
		}//if  ( (i_found  >=  0)&&(b_line_analyzed  ==  false) )

	}//while  (!feof(pfSource))
	

	fprintf
	(
	pfDest,
	"%d\t%d\t%d\t%.8lf\t%.8lf\t%.8lf\t%.8lf\n", 
	i_epoch_number, i_generation, i_ct_number, d_best_fitness, d_avr_fitness, d_eval, d_time
	);

	return(c_err);
};//CError  Tools::eAnalyzeRunFile(FILE  *pfSource,  FILE*  pfDest)



void  Tools::vRepInFile(CString  sFile, CString  sText, bool  bMakeNew)
{
	FILE  *pf_save;


	if  (bMakeNew  ==  true)
		pf_save = fopen(sFile, "w+");
	else
		pf_save = fopen(sFile, "a+");

    if  (pf_save  !=  NULL)
    {
        fprintf(pf_save, "%s\n", sText);
        fclose(pf_save);
    }//if  (pf_save  !=  NULL)


	fclose(pf_save);
}//void  CSystem::v_log(CString  sLog)


void  Tools::vShow(CString  sText)
{
	::MessageBox(NULL, sText, sText, MB_OK);
};//void  Tools::vShow(CString)

void  Tools::vShow(double  dVal)
{
	CString  s_buf;
	s_buf.Format("%lf", dVal);
	::MessageBox(NULL, s_buf, s_buf, MB_OK);
};//void  Tools::vShow(CString)

