#include "stdafx.h"

#include  "LargeNum.h"


using namespace LargeNum;

CNumber::CNumber()
{

};//CNumber::CNumber()


CNumber::~CNumber()
{

};//CNumber::~CNumber()


void  CNumber::vSetValue(int  iValue)
{
	int  i_base;
	i_base = i_get_base();

	v_number.clear();
	
	int  i_digit;
	while (iValue  >  0)
	{
		i_digit = iValue / i_base;
		i_digit = iValue - i_digit * i_base;
		iValue = iValue / i_base;

		v_number.push_back(i_digit);
	}//while (iValue  >  0)

}//void  CNumber::vSetValue(int  iValue)


void  CNumber::vAdd(CNumber  *pcOther)
{
	int i_carry, i_cur_sum;
	int  i_base;
	i_base = i_get_base();

	int  i_longer_number;
	if  (v_number.size()  >  pcOther->v_number.size())
		i_longer_number = v_number.size();
	else
		i_longer_number = pcOther->v_number.size();


	vector<int>  v_result;
	i_carry = 0;
	for  (int  ii = 0; ii < i_longer_number; ii++)
	{
		i_cur_sum = iGetDigitAt(ii) + pcOther->iGetDigitAt(ii) + i_carry;

		i_carry = 0;
		while  (i_cur_sum  >=  i_base)
		{
			i_carry++;
			i_cur_sum  -=  i_base;	
		}//while  (i_cur_sum  >=  i_base)

		v_result.push_back(i_cur_sum);
	}//for  (int  ii = 0; ii < pcOther->v_number.size(); ii++)
	if  (i_carry  >  0)  v_result.push_back(i_carry);

	v_number = v_result;
}//void  CNumber::vAdd(CNumber  *pcOther)


void  CNumber::vShiftBy(int iShift)
{
	for  (int  ii = 0; ii < iShift; ii++)
		v_number.insert(v_number.begin(), 0);
}//void  CNumber::vShiftBy(int iShift)


CNumber  CNumber::cMul(CNumber  *pcOther)
{
	CNumber  c_result, c_mid_res;

	c_result.vSetValue(0);
	for  (int  ii = 0; ii < pcOther->v_number.size(); ii++)
	{
		c_mid_res = cMulByDigit(pcOther->v_number.at(ii));
		c_mid_res.vShiftBy(ii);
		c_result.vAdd(&c_mid_res);	
	}//for  (int  ii = 0; ii < pcOther->v_number.size(); ii++)
	
	return(c_result);
}//void  CNumber::vMul(CNumber  *pcOther)


CNumber CNumber::cMulByDigit(int iDigit)
{
	int i_carry, i_cur_sum;
	int  i_base;
	i_base = i_get_base();


	vector<int>  v_result;
	i_carry = 0;
	for  (int  ii = 0; ii < v_number.size(); ii++)
	{
		i_cur_sum = iGetDigitAt(ii) * iDigit + i_carry;

		i_carry = 0;
		while  (i_cur_sum  >=  i_base)
		{
			i_carry++;
			i_cur_sum  -=  i_base;	
		}//while  (i_cur_sum  >=  i_base)

		v_result.push_back(i_cur_sum);
	}//for  (int  ii = 0; ii < pcOther->v_number.size(); ii++)
	if  (i_carry  >  0)  v_result.push_back(i_carry);

	CNumber  c_result;
	c_result.v_number = v_result;

	return(c_result);	
}//CNumber  CNumber::vMulByDigit(int iDigit)



int  CNumber::iGetDigitAt(int iPos)
{
	if  (iPos >= v_number.size())  return(0);
	return(v_number.at(iPos));
}//int  CNumber::iGetDigitAt(int iPos)



CString CNumber::sToString10Pow()
{
	CString  s_result;

	s_result.Format("%d", v_number.size() -1);

	return(s_result);
}//CString CNumber::sToString10Pow()


CString CNumber::sToStringFirstDgitsDotted(int iDigitNumber)
{
	CString  s_buf;


	//v_number.clear();
	//v_number.push_back(9);
	//v_number.push_back(8);

	CString  s_result, s_digit;
	int  i_digits_added;

	bool  b_coma = false;
	i_digits_added = 0;
	for  (int ii = v_number.size() - 1; (ii >= 0)&&(i_digits_added  <=  iDigitNumber); ii--)
	{
		i_digits_added++;
		//s_buf.Format("%d > %d", ii, v_number.size() - 1 - iDigitNumber);
		//Tools::vShow(s_buf);

		s_digit.Format("%d", v_number.at(ii));
		s_result += s_digit;

		if  (b_coma == false)
		{
			s_result += ".";
			b_coma = true;
		}//if  (b_coma == false)
	}//for  (int ii = 0; ii < v_number.size(); ii++)


	
	//CString  s_buf;
	//s_buf.Format("%d",v_number.size() -1 - iDigitNumber);
	//s_result = s_buf;
	

	return(s_result);
}//CString CNumber::sToString10Pow()



CString CNumber::sToString()
{
	CString  s_result, s_digit;

	for  (int ii = v_number.size() - 1; ii >= 0; ii--)
	{
		s_digit.Format("%d", v_number.at(ii));
		s_result += s_digit;
	}//for  (int ii = 0; ii < v_number.size(); ii++)

	return(s_result);
};//CString CNumber::sToString()


int  CNumber::i_get_base()
{
	CString  s_digits;

	s_digits = s_DIGITS;

	return(s_digits.GetLength());
}//int  CNumber::i_get_base()