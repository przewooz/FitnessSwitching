#pragma once


//tools
#include  "atlstr.h"  //CString
#include  "..\error\error.h"
#include  <iostream>
#include  <vector>
#include  <functional>
#include  <algorithm>


//..and other tools...
#include  "..\util\tools.h"


using namespace std;




namespace DeceptiveTools
{
	class  CReportGrid;//predefinition
	class  CDecFunc;//predefinition


	#define  DEC_PARAM_DEC_BIT_LEN	"bit length"
	#define  DEC_PARAM_DEC_MAX_VAL	"max val"
	#define  DEC_PARAM_DEC_MIN_VAL	"min val"
	#define  DEC_PARAM_DEC_LOWER_THAN_MAX_SCALE	"scaling for lower than max values"
	

	
	class  CConcatDecFunc
	{
	public:
		CConcatDecFunc();
		~CConcatDecFunc();

		CError  eGetFuncValue(int  *piArgument,  int  iArgLen,  double  *pdFuncVal);
		CError  eAddDecFunc(CDecFunc  *pcDecFunc);

		void  operator=(CConcatDecFunc  &cOther);

		CError  eCreateReportSeparateFunc(FILE  *pfReport);
		CError  eCreateReport(FILE  *pfReport);
		static  double  dGetFuncEval()  {return(d_func_evaluations);};
		static  void  vZeroEval()  {d_func_evaluations = 0;};

		double  dGetMaxFuncVal();
	
	private:
		CError  e_create_report(vector  <CReportGrid *>  *pvReportGrids, int  *piArgument, int  iBitLength,  int  iCurrentBit);

		CDecFunc  *pc_clone_func(CDecFunc  *pcDecFunc);

		vector  <CDecFunc  *>  v_dec_funcs;	

		static  double  d_func_evaluations;
	};//class  CConcatDecFunc

	
	


	#define  DEC_FUNC_UNKNOWN	0
	class  CDecFunc
	{
	public:
		CDecFunc();
		CDecFunc(int  iBitLength,  double  dMaxVal,  double  dMinVal,  double  dLowerThanMaxScale, CError  *pcErr);
		virtual  ~CDecFunc();

		virtual  CError  eConfigure(int  iBitLength,  double  dMaxVal,  double  dMinVal,  double  dLowerThanMaxScale);

		virtual  CError  eGetFuncValue(int  *piArgument,  int  iArgLen,  double  *pdFuncVal);
		virtual  double  dGetFuncValue(int  iUnitation)  =  0;

		virtual  CError  eCreateReport(FILE  *pfReport);

		double  dGetMaxFuncVal()  {return(d_max_val);};
		int  iGetBitLength()  {return(i_bit_length);};

		virtual  int  iGetFuncType()  {return(DEC_FUNC_UNKNOWN);};
	protected:
		int  i_bit_length;
		double  d_max_val;
		double  d_min_val;
		double  d_min_max_dist_grid;//(d_max_val  -  d_min_val)/(i_bit_length/2)
		double  d_lower_than_max_scaling;//d_min_max_dist_grid =  d_min_max_dist_grid/d_lower_than_max_scaling

		virtual  CError  e_create_report(vector  <CReportGrid *>  *pvReportGrids, int  *piArgument, int  iCurrentBit);
	};//class  CDecFunc


	#define  DEC_FUNC_1_MODAL	1
	class  CDecFunc1Modal  :  public  CDecFunc
	{
	public:
		CDecFunc1Modal();
		CDecFunc1Modal(int  iBitLength,  double  dMaxVal,  double  dMinVal,  double  dLowerThanMaxScale, CError  *pcErr);
		~CDecFunc1Modal();

		CError  eLoadSettings(FILE  *pfSettings);
		CError  eConfigure(int  iBitLength,  double  dMaxVal,  double  dMinVal, double  dLowerThanMaxScale);

		double  dGetFuncValue(int  iUnitation);

		int  iGetFuncType()  {return(DEC_FUNC_1_MODAL);};

	private:
		
	};//class  CDecFunc1Modal


	#define  DEC_FUNC_2_MODAL	2
	class  CDecFunc2Modal  :  public  CDecFunc
	{
	public:
		CDecFunc2Modal();
		CDecFunc2Modal(int  iBitLength,  double  dMaxVal,  double  dMinVal,  double  dLowerThanMaxScale, CError  *pcErr);
		~CDecFunc2Modal();

		CError  eConfigure(int  iBitLength,  double  dMaxVal,  double  dLowerThanMaxScale, double  dMinVal);

		double  dGetFuncValue(int  iUnitation);

		int  iGetFuncType()  {return(DEC_FUNC_2_MODAL);};
	private:
	};//class  CDecFunc2Modal


	class  CReportGrid
	{
	public:
		CReportGrid();
		CReportGrid(int  iBitLength,  int  *piArgument,  double  dVal);
		~CReportGrid();

		void  vSetData(int  iBitLength,  int  *piArgument,  double  dVal);
		void  vPrint(FILE  *pfDest);

	//private:
		int  i_bit_len;
		int  *pi_argument;
		double  d_value;
	};//class  CReportGrid


	struct SReportGridSort
	{
		bool operator()(const CReportGrid* const& p1, const CReportGrid* const& p2)
		{
			if(!p1)
			return true;
			if(!p2)
			return false;
			return p1->d_value < p2->d_value;
		}
	};



};//namespace DeceptiveTools



/*namespace  std
{
	struct less<DeceptiveTools::CReportGrid*>
	{
		bool operator()(DeceptiveTools::CReportGrid const* p1, DeceptiveTools::CReportGrid const* p2)
		{
			if(!p1)
			return true;
			if(!p2)
			return false;
			return p1->d_value < p2->d_value;
		}
	};

	struct greater<DeceptiveTools::CReportGrid*>
	{
		bool operator()(DeceptiveTools::CReportGrid const* p1, DeceptiveTools::CReportGrid const* p2)
		{
			if(!p1)
			return false;
			if(!p2)
			return true;
			return p1->d_value > p2->d_value;
		}
	};
};//namespace  std*/





