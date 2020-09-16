#include "stdafx.h"

#include "DecFunc.h"



using  namespace  DeceptiveTools;
using namespace std;



//---------------------------class  CConcatDecFunc--------------------------------------


CConcatDecFunc::CConcatDecFunc()
{

}//CConcatDecFunc::CConcatDecFunc()


CConcatDecFunc::~CConcatDecFunc()
{
	for  (int  ii = 0;  ii < (int) v_dec_funcs.size();  ii++)
		delete  v_dec_funcs.at(ii);
}//CConcatDecFunc::~CConcatDecFunc()



void  CConcatDecFunc::operator=(CConcatDecFunc  &cOther)
{
	CError  c_err;

	for  (int  ii = 0;  ii < (int) v_dec_funcs.size();  ii++)
		delete  v_dec_funcs.at(ii);

	for  (int  ii = 0;  ii < (int) cOther.v_dec_funcs.size();  ii++)
	{
		c_err  =  eAddDecFunc(cOther.v_dec_funcs.at(ii));
		if  (c_err)  return;
	}//for  (int  ii = 0;  ii < (int) cOther.v_dec_funcs.size();  ii++)

}//void  CConcatDecFunc::operator=(CConcatDecFunc  &cOther)




CError  CConcatDecFunc::eAddDecFunc(CDecFunc  *pcDecFunc)  
{
	CError  c_err;
	CDecFunc  *pc_new_func;

	pc_new_func  =  pc_clone_func(pcDecFunc);
	if  (pc_new_func  ==  NULL)
	{
		c_err.vPutError("Could not add deceptive function - probably unknown type");
		return(c_err);
	}//if  (pc_new_func  ==  NULL)

	v_dec_funcs.push_back(pc_new_func);
	return(c_err);
};//void  CConcatDecFunc::vAddDecFunc(CDecFunc  *pcDecFunc)  


CDecFunc*  CConcatDecFunc::pc_clone_func(CDecFunc  *pcDecFunc)
{
	CDecFunc  *pc_func  =  NULL;

	if  (pcDecFunc->iGetFuncType()  ==  DEC_FUNC_1_MODAL)
	{
		pc_func  =  new  CDecFunc1Modal();
	}//if  (iFuncType  ==  DEC_FUNC_2_MODAL)


	if  (pcDecFunc->iGetFuncType()  ==  DEC_FUNC_2_MODAL)
	{
		pc_func  =  new  CDecFunc2Modal();
	}//if  (iFuncType  ==  DEC_FUNC_2_MODAL)

	if  (pc_func  ==  NULL)  return(pc_func);

	*pc_func  =  *pcDecFunc;

	return(pc_func);
}//CDecFunc*  CConcatDecFunc::pc_clone_func(CDecFunc  *pcDecFunc)



CError  CConcatDecFunc::eCreateReportSeparateFunc(FILE  *pfReport)
{
	CError  c_err;

	fprintf(pfReport, "FITNESS FUNCTION VALUES\n\n");

	for  (int  ii=0; ii < (int) v_dec_funcs.size();  ii++)
	{
		fprintf(pfReport, "Subfunction (%d/%d)\n", ii, v_dec_funcs.size());
		v_dec_funcs.at(ii)->eCreateReport(pfReport);
	}//for  (int  ii=0; ii < (int) v_dec_funcs.size();  ii++)

	return(c_err);
}//CError  CConcatDecFunc::eCreateReportSeparateFunc(FILE  *pfReport)



CError  CConcatDecFunc::eCreateReport(FILE  *pfReport)
{
	CError  c_err;
	int  *pi_argument;
	int  i_bit_length  =  0;

	for  (int  ii=0; ii < (int) v_dec_funcs.size();  ii++)
		i_bit_length  +=  v_dec_funcs.at(ii)->iGetBitLength();


	pi_argument  =  new  int[i_bit_length];
	vector  <CReportGrid *>  v_result_grids;

	for  (int  ii=0; ii < i_bit_length;  ii++)  pi_argument[ii]  =  0;


	c_err  =  e_create_report(&v_result_grids,  pi_argument,  i_bit_length,  0);


	sort(v_result_grids.begin(), v_result_grids.end(), SReportGridSort());
	for  (int  ii = 0;  ii < (int) v_result_grids.size();  ii++)
	{
		v_result_grids.at(ii)->vPrint(pfReport);
		fprintf(pfReport,  "\n");	
	}//for  (int  ii = 0;  ii < v_result_grids.size();  ii++)

    for  (int  ii = 0; ii < (int) v_result_grids.size();  ii++)
		delete  v_result_grids.at(ii);

	if  (c_err)  return(c_err);

	delete  pi_argument;
	return(c_err);
};//void  CConcatDecFunc::vCreateReport(FILE  *pfReport)




CError  CConcatDecFunc::e_create_report(vector  <CReportGrid *>  *pvReportGrids, int  *piArgument, int  iBitLength,  int  iCurrentBit)
{
	CError  c_err;

	if  (iCurrentBit  <  iBitLength)
	{
		piArgument[iCurrentBit]  =  0;
		c_err  =  e_create_report(pvReportGrids,  piArgument,  iBitLength,  iCurrentBit+1);
		if  (c_err)  return(c_err);


		piArgument[iCurrentBit]  =  1;
		c_err  =  e_create_report(pvReportGrids,  piArgument,  iBitLength,  iCurrentBit+1);
		if  (c_err)  return(c_err);
	
	}//if  (iCurrentBit  <  i_bit_length)
	else
	{
		double  d_func_val;
		CReportGrid  *pc_rep_grid;

		c_err  =  eGetFuncValue(piArgument,  iBitLength,  &d_func_val);
		if  (c_err)  return(c_err);
		pc_rep_grid  =  new  CReportGrid(iBitLength, piArgument, d_func_val);
		pvReportGrids->push_back(pc_rep_grid);
	}//else  if  (iCurrentBit  <  i_bit_length)


	return(c_err);
}//void  CConcatDecFunc::v_create_report(vector  <CReportGrid *>  *pvReportGrids, int  *piArgument, int  ICurrentBit)




double  CConcatDecFunc::dGetMaxFuncVal()
{
	double  d_result;

	d_result  =  0;
	for  (int  ii = 0;  ii < (int) v_dec_funcs.size();  ii++)
	{
		d_result  +=  v_dec_funcs.at(ii)->dGetMaxFuncVal();
	}//for  (int  ii = 0;  ii < (int) v_dec_funcs.size();  ii++)

	return(d_result);
}//double  CConcatDecFunc::dGetMaxFuncVal()



CError  CConcatDecFunc::eGetFuncValue(int  *piArgument,  int  iArgLen,  double  *pdFuncVal)
{
	CError  c_err;

	double  d_sum  =  0;
	int  i_unitation;
	int  iCurrentBit  =  0;

	for  (int  ii = 0;  ii < (int) v_dec_funcs.size();  ii++)
	{
		i_unitation  =  0;
		for  (int  ik = 0;  ik < (int) v_dec_funcs.at(ii)->iGetBitLength();  ik++)
		{
			if  (iCurrentBit  >=  iArgLen)
			{
				c_err.vPutError("Argument legnth below function argument legnth");
				return(c_err);
			}//if  (iCurrentBit  >=  iArgLen)

			i_unitation  +=  piArgument[iCurrentBit];
			iCurrentBit++;		
		}//for  (int  ii = 0;  ii < (int) v_dec_funcs.at(ii)->iGetBitLength();  ii++)

		d_sum  +=  v_dec_funcs.at(ii)->dGetFuncValue(i_unitation);	
	}//for  (int  ii = 0;  ii < v_dec_funcs.size();  ii++)


	if  (iCurrentBit  <  iArgLen)
	{
		c_err.vPutError("Argument legnth exceeds function argument legnth");
		return(c_err);
	}//if  (iCurrentBit  >=  iArgLen)

	*pdFuncVal  =  d_sum;
	d_func_evaluations++;
	/*CString  s_buf;
	s_buf.Format("%.0lf", d_func_evaluations);
	::MessageBox(NULL, s_buf, "", MB_OK);*/

	return(c_err);
}//CError  CConcatDecFunc::eGetFuncValue(int  *piArgument,  int  iArgLen,  double  *pdFuncVal)



//---------------------------class  CDecFunc--------------------------------------


CDecFunc::CDecFunc()
{
	i_bit_length  =  0;
	d_max_val  =  0;
	d_min_val  =  0;
	d_min_max_dist_grid  =  0;
}//CDecFunc::CDecFunc()


CDecFunc::CDecFunc(int  iBitLength,  double  dMaxVal,  double  dMinVal,  double  dLowerThanMaxScale,  CError  *pcErr)
{
	i_bit_length  =  0;
	d_max_val  =  0;
	d_min_val  =  0;
	d_min_max_dist_grid  =  0;
	d_lower_than_max_scaling  =  dLowerThanMaxScale;
	*pcErr  =  eConfigure(iBitLength,  dMaxVal,  dMinVal, dLowerThanMaxScale);
}//CDecFunc::CDecFunc(int  iBitLength,  double  dMaxVal,  double  dMinVal)



CDecFunc::~CDecFunc()
{

}//CDecFunc::~CDecFunc()


CError  CDecFunc::eConfigure(int  iBitLength,  double  dMaxVal,  double  dMinVal,  double  dLowerThanMaxScale)
{
	CError  c_err;

	if  (iBitLength  <  0)
	{
		c_err.vPutError("Bit length below 0");
		return(c_err);
	}//if  (iBitLength  <  0)

	if  (d_max_val  <  d_min_val)
	{
		c_err.vPutError("Min function value greater than max function value");
		return(c_err);
	}//if  (iBitLength  <  0)

	i_bit_length  =  iBitLength;
	d_max_val  =  dMaxVal;
	d_min_val  =  dMinVal;
	d_lower_than_max_scaling  =  dLowerThanMaxScale;
	//d_min_max_dist_grid  =  (d_max_val  -  d_min_val)/(i_bit_length/2);

	return(c_err);
}//CError  CDecFunc::eConfigure(int  iBitLength,  double  dMaxVal,  double  dMinVal)


CError  CDecFunc::eGetFuncValue(int  *piArgument,  int  iArgLen,  double  *pdFuncVal)
{
	CError  c_err;

	if  (iArgLen  !=  i_bit_length)
	{
		c_err.vPutError("Argument length not equals the function argument length");
		return(c_err);	
	}//if  (iArgLen  !=  i_bit_length)

	int  i_unitation  =  0;
    for  (int  ii = 0;  ii < i_bit_length;  ii++)
	{
		if  (piArgument[ii]  >  0)  i_unitation++;	
	}//for  (int  ii = 0;  ii < i_bit_length;  ii++)

	*pdFuncVal  =  dGetFuncValue(i_unitation);
	if  (c_err)  return(c_err);

	return(c_err);
};//CError  CDecFunc::eGetFuncValue(int  *piArgument,  int  iArgLen,  double  *pdFuncVal)




CError  CDecFunc::eCreateReport(FILE  *pfReport)
{
	CError  c_err;
	int  *pi_argument;

	pi_argument  =  new  int[i_bit_length];
	vector  <CReportGrid *>  v_result_grids;

	for  (int  ii=0; ii < i_bit_length;  ii++)  pi_argument[ii]  =  0;


	c_err  =  e_create_report(&v_result_grids,  pi_argument,  0);


	sort(v_result_grids.begin(), v_result_grids.end(), SReportGridSort());
	for  (int  ii = 0;  ii < (int) v_result_grids.size();  ii++)
	{
		v_result_grids.at(ii)->vPrint(pfReport);
		fprintf(pfReport,  "\n");	
	}//for  (int  ii = 0;  ii < v_result_grids.size();  ii++)

    for  (int  ii = 0; ii < (int) v_result_grids.size();  ii++)
		delete  v_result_grids.at(ii);

	if  (c_err)  return(c_err);

	delete  pi_argument;
	return(c_err);
};//void  CDecFunc::vCreateReport(FILE  *pfReport)



CError  CDecFunc::e_create_report(vector  <CReportGrid *>  *pvReportGrids, int  *piArgument, int  iCurrentBit)
{
	CError  c_err;

	if  (iCurrentBit  <  i_bit_length)
	{
		piArgument[iCurrentBit]  =  0;
		c_err  =  e_create_report(pvReportGrids,  piArgument,  iCurrentBit+1);
		if  (c_err)  return(c_err);


		piArgument[iCurrentBit]  =  1;
		c_err  =  e_create_report(pvReportGrids,  piArgument,  iCurrentBit+1);
		if  (c_err)  return(c_err);
	
	}//if  (iCurrentBit  <  i_bit_length)
	else
	{
		double  d_func_val;
		CReportGrid  *pc_rep_grid;

		c_err  =  eGetFuncValue(piArgument,  i_bit_length,  &d_func_val);
		if  (c_err)  return(c_err);
		pc_rep_grid  =  new  CReportGrid(i_bit_length, piArgument, d_func_val);
		pvReportGrids->push_back(pc_rep_grid);
	}//else  if  (iCurrentBit  <  i_bit_length)


	return(c_err);
}//void  CDecFunc::v_create_report(vector  <CReportGrid *>  *pvReportGrids, int  *piArgument, int  ICurrentBit)




//---------------------------class  CDecFunc1Modal--------------------------------------

CDecFunc1Modal::CDecFunc1Modal()  :  CDecFunc()
{

}//CDecFunc1Modal::CDecFunc1Modal()


CDecFunc1Modal::CDecFunc1Modal(int  iBitLength,  double  dMaxVal,  double  dMinVal,  double  dLowerThanMaxScale, CError  *pcErr)  :  CDecFunc(iBitLength,  dMaxVal,  dMinVal,  dLowerThanMaxScale, pcErr)
{
	*pcErr  =  eConfigure(iBitLength,  dMaxVal,  dMinVal, dLowerThanMaxScale);
}//CDecFunc1Modal::CDecFunc1Modal(int  iBitLength,  double  dMaxVal,  double  dMinVal,  CError  *pcErr)


CDecFunc1Modal::~CDecFunc1Modal()
{

}//CDecFunc1Modal::~CDecFunc1Modal()



CError  CDecFunc1Modal::eConfigure(int  iBitLength,  double  dMaxVal,  double  dMinVal,  double  dLowerThanMaxScale)
{
	CError  c_err;

	c_err  =  CDecFunc::eConfigure(iBitLength,  dMaxVal,  dMinVal, dLowerThanMaxScale);

	d_min_max_dist_grid  =  (d_max_val  -  d_min_val)/(i_bit_length);
	d_min_max_dist_grid  =  d_min_max_dist_grid/dLowerThanMaxScale;

	return(c_err);
}//CError  CDecFunc2Modal::eConfigure(int  iBitLength,  double  dMaxVal,  double  dMinVal)




CError  CDecFunc1Modal::eLoadSettings(FILE  *pfSettings)
{
	CError  c_err;
	CString  s_setting_name,  s_buf;


	
	i_bit_length  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  DEC_PARAM_DEC_BIT_LEN)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", DEC_PARAM_DEC_BIT_LEN, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	
	d_max_val  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  DEC_PARAM_DEC_MAX_VAL)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", DEC_PARAM_DEC_MAX_VAL, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_min_val  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  DEC_PARAM_DEC_MIN_VAL)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", DEC_PARAM_DEC_MIN_VAL, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_lower_than_max_scaling  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  DEC_PARAM_DEC_LOWER_THAN_MAX_SCALE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", DEC_PARAM_DEC_LOWER_THAN_MAX_SCALE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)



	c_err  =  eConfigure(i_bit_length,  d_max_val,  d_min_val, d_lower_than_max_scaling);
		

	return(c_err);
}//CError  CDecFunc1Modal::eLoadSettings(FILE  *pfSettings)




double  CDecFunc1Modal::dGetFuncValue(int  iUnitation)
{
	int  i_mirror_uni;


	i_mirror_uni  =  i_bit_length  -  iUnitation;

	if  (i_mirror_uni  ==  0)  return(d_max_val);

	double  d_buf;
	d_buf  =  i_mirror_uni  -  1;

	double  d_func_val;
	d_func_val  =  d_min_val  +  d_buf  *  d_min_max_dist_grid;
	
	return(d_func_val);
}//double  CDecFunc1Modal::dGetFuncValue(int  iUnitation)






//---------------------------class  CDecFunc2Modal--------------------------------------

CDecFunc2Modal::CDecFunc2Modal()  :  CDecFunc()
{

}//CDecFunc2Modal::CDecFunc2Modal()


CDecFunc2Modal::CDecFunc2Modal(int  iBitLength,  double  dMaxVal,  double  dMinVal,  double  dLowerThanMaxScale,  CError  *pcErr)  :  CDecFunc(iBitLength,  dMaxVal,  dMinVal,  dLowerThanMaxScale,  pcErr)
{
	*pcErr  =  eConfigure(iBitLength,  dMaxVal,  dMinVal,  dLowerThanMaxScale);
}//CDecFunc2Modal::CDecFunc2Modal(int  iBitLength,  double  dMaxVal,  double  dMinVal,  CError  *pcErr)


CDecFunc2Modal::~CDecFunc2Modal()
{

}//CDecFunc2Modal::~CDecFunc2Modal()


CError  CDecFunc2Modal::eConfigure(int  iBitLength,  double  dMaxVal,  double  dMinVal,  double  dLowerThanMaxScale)
{
	CError  c_err;

	c_err  =  CDecFunc::eConfigure(iBitLength,  dMaxVal,  dMinVal,  dLowerThanMaxScale);
	if  (c_err)  return(c_err);


	d_min_max_dist_grid  =  (d_max_val  -  d_min_val)/(i_bit_length/2);
	d_min_max_dist_grid  =  d_min_max_dist_grid/dLowerThanMaxScale;

	return(c_err);
}//CError  CDecFunc2Modal::eConfigure(int  iBitLength,  double  dMaxVal,  double  dMinVal)


double  CDecFunc2Modal::dGetFuncValue(int  iUnitation)
{
	int  i_mirror_uni;
	int  i_bit_length_div_2;

	i_bit_length_div_2  =  i_bit_length / 2;

	if  (iUnitation >  i_bit_length_div_2)
		i_mirror_uni  =  i_bit_length_div_2  -  (iUnitation  -  i_bit_length_div_2);
	else
		i_mirror_uni  =  iUnitation;

	if  (i_mirror_uni  ==  0)  return(d_max_val);

	double  d_buf;
	d_buf  =  i_mirror_uni  -  1;

	double  d_func_val;
	d_func_val  =  d_min_val  +  d_buf  *  d_min_max_dist_grid;

	return(d_func_val);

}//double  CDecFunc2Modal::dGetFuncValue(int  iUnitation)




//---------------------------class  CReportGrid--------------------------------------

/*bool operator<(const CReportGrip*  const&pc_rep_g_1, const CReportGrip* const&pc_rep_g_2)
{
	return pc_rep_g_1->d_value < pc_rep_g_2->d_value;
}//bool operator<(const CReportGrid& pc_rep_g_1, const CReportGrid& pc_rep_g_2)


bool operator==(const CReportGrid* const&pc_rep_g_1, const CReportGrid* const&pc_rep_g_2)
{
	
	if  (pc_rep_g_1->d_value  !=  pc_rep_g_2->d_value)  return(false);
	if  (pc_rep_g_1->i_bit_len  !=  pc_rep_g_2->i_bit_len)  return(false);
	for  (int  ii = 0; ii < pc_rep_g_1->i_bit_len;  ii++)
		if  (pc_rep_g_1->pi_argument[ii]  !=  pc_rep_g_2->pi_argument[ii])  return(false);

	return(true);
}//bool operator==(const CReportGrid& pc_rep_g_1, const CReportGrid& pc_rep_g_2)*/


CReportGrid::CReportGrid()
{
	i_bit_len  =  0;
	d_value  =  0;

	pi_argument  =  NULL;
}//CReportGrid::CReportGrid()


CReportGrid::CReportGrid(int  iBitLength,  int  *piArgument,  double  dVal)
{
	i_bit_len  =  0;
	d_value  =  0;

	pi_argument  =  NULL;
	vSetData(iBitLength,  piArgument,  dVal);
}//CReportGrid::CReportGrid(int  iBitLength,  int  *piArgument,  double  dVal)


CReportGrid::~CReportGrid()
{
	if  (pi_argument  !=  NULL)  delete  []  pi_argument;
}//CReportGrid::~CReportGrid()



void  CReportGrid::vSetData(int  iBitLength,  int  *piArgument,  double  dVal)
{
	i_bit_len  =  iBitLength;
	d_value  =  dVal;

	if  (pi_argument  !=  NULL)  delete  []  pi_argument;
	pi_argument  =  NULL;

	pi_argument  =  new  int[i_bit_len];

	for  (int  ii = 0;  ii < i_bit_len;  ii++)
		pi_argument[ii]  =  piArgument[ii];


};//CReportGrid::vSetData(int  iBitLength,  int  *piArgument,  double  dVal)


void  CReportGrid::vPrint(FILE  *pfDest)
{
	for  (int  ii = 0;  ii < i_bit_len;  ii++)
		fprintf(pfDest, "%d", pi_argument[ii]);

	fprintf(pfDest, "\t");

	fprintf(pfDest, "%.8lf",  d_value);
};//void  CReportGrid::vPrint(FILE  *pfDest)











