#include "stdafx.h"
#include "Form1.h"
#include <windows.h>

using namespace MessyGATester;
using namespace MessyGA;
using namespace System::IO;


double  DeceptiveTools::CConcatDecFunc::d_func_evaluations  =  0;
double  FiberNets::CFiberSolution::d_func_evaluations  =  0;
double  FiberNets::CFiberSolution::d_clever_paths_evaluations  =  0;
double  FiberNets::CFiberSolution::d_clever_paths_initializations  =  0;
double  FiberNets::CFiberSolution::d_construction_time  =  0;


void  v_generate_test_from_list();//predefinition


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;
	//::MessageBox(NULL, lpCmdLine, lpCmdLine, MB_OK);

	/*FILE  *pf_test;
	CString  s_out_test_name;
	s_out_test_name.Format("%s_out.txt", lpCmdLine);
	pf_test = fopen(s_out_test_name, "w+");
	fprintf(pf_test, "out  %s", lpCmdLine);
	fclose(pf_test);//*/

	CString  s_in_data;	
	s_in_data.Format("%s", lpCmdLine);
	Application::Run(new CSurvP2PMainForm(s_in_data));
	return 0;
};



System::Void CSurvP2PMainForm::CSurvP2PMainForm_Shown(System::Object*  sender, System::EventArgs*  e)
{
	if  (pc_system->sInParameters  !=  "")
	{
		CString  s_file_name, s_buf;
		FILE  *pf_in_file;

		s_file_name.Format("%s_entry.txt", pc_system->sInParameters);
		pf_in_file = fopen(s_file_name, "r");
		if  (pf_in_file  ==  NULL)
		{
			s_buf.Format("Reading data input from in file (%s) unscuccessfull", s_file_name);
			list_info->Items->Add((String *)s_buf);
			list_info->Items->Add(S"Closing the system");
			list_info->Refresh();
			
			System::Threading::Thread::Sleep(5000);
			Close();
		}//if  (pf_in_file  ==  NULL)

		
		CString  s_in_data;
		s_in_data = Tools::sReadLine(pf_in_file);
		fclose(pf_in_file);

		s_buf.Format("In parameters read: %s", s_in_data);
		list_info->Items->Add((String *)s_buf);
		list_info->Refresh();

		

		CError  c_err;
		c_err  =  pc_system->eRun("", s_in_data, list_info);	
		if  (c_err)
		{
			list_info->Items->Add(S"An error occured");
			list_info->Items->Add((String *)c_err.csMessage);
			
			list_info->Refresh();			
		}//if  (c_err)

		System::Threading::Thread::Sleep(5000);
		Close();

	}//if  (pc_system->sInParameters  !=  "")
}//System::Void CSurvP2PMainForm::CSurvP2PMainForm_Load(System::Object*  sender, System::EventArgs*  e)


System::Void CSurvP2PMainForm::but_tun_100_Click(System::Object*  sender, System::EventArgs*  e)
{
	CString  s_buf;
	CString  s_problem_name;
	CString  s_settings_name;
	CString  s_node_name, s_tree_name, s_cost_name, s_isp_name, s_fit_func_name, s_fit_func_sutec_name;

	int  i_gen, i_pop;
	double  d_cut, d_splice;
	double  d_mut;
	double  d_time;

	FILE  *pf_serie, *pf_settings;
	int  i_repetation_number = 1;
	pf_serie = fopen("!serie.txt", "w+");

	d_mut = 0.05;
	d_cut = 0.12;
	d_splice = 0.10;
	i_gen = 300;
	i_pop = 70000;
	d_time = 1500;

	int  i_count = 0;
	for  (int  i_test = 0; i_test < 3; i_test++)
	{
		for  (int  ii = 0; ii < 3; ii++)
		{
			for  (int  ik = 0; ik < 1; ik++)
			{
				if  (ii == 0)  d_cut = 0.05;
				if  (ii == 1)  d_cut = 0.01;
				if  (ii == 2)  d_cut = 0.005;
			

				if  (i_test == 0)
				{
					s_node_name = "b00";
					s_tree_name = "23b";
					s_cost_name = "b00";
					s_isp_name = "b00";
					s_fit_func_name = "51";
					s_fit_func_sutec_name = "SCM";
				}//if  (i_test == 0)

				if  (i_test == 1)
				{
					s_node_name = "b05";
					s_tree_name = "23b";
					s_cost_name = "b05";
					s_isp_name = "b00";
					s_fit_func_name = "61";
					s_fit_func_sutec_name = "SCM_LD";
				}//if  (i_test == 0)

				if  (i_test == 2)
				{
					s_node_name = "b09";
					s_tree_name = "23b";
					s_cost_name = "b09";
					s_isp_name = "b00";
					s_fit_func_name = "71";
					s_fit_func_sutec_name = "SCM_ND";
				}//if  (i_test == 0)


				s_problem_name = s_node_name + s_tree_name + s_cost_name + s_isp_name + s_fit_func_name;
				s_buf.Format("%d", i_count);
				s_settings_name.Format("settings_%s_%.3d.txt", s_problem_name, i_count);
				fprintf(pf_serie, "%s\\\\1\n",s_settings_name);
				pf_settings = fopen(s_settings_name, "w+");


				s_buf.Format(".\\%s\\\\%s\n", s_problem_name, SYSTEM_PARAM_FILE_NAME);
				fprintf(pf_settings, s_buf);
				s_buf.Format(".\\summarize_%d_%d.txt\\\\%s\n", ii, ik, SYSTEM_PARAM_SUMMARIZE);
				fprintf(pf_settings, s_buf);
				s_buf.Format(".\\data\\%s.node\\\\%s\n", s_node_name, SYSTEM_PARAM_NET_FILE);
				fprintf(pf_settings, s_buf);

				if  (i_test != 3)
					s_buf.Format(".\\data\\%s.tree\\\\%s\n", s_tree_name, SYSTEM_PARAM_PATHS_FILE);
				else
					s_buf.Format(".\\data\\sep%s.tree\\\\%s\n", s_tree_name, SYSTEM_PARAM_PATHS_FILE);

				fprintf(pf_settings, s_buf);
				s_buf.Format(".\\data\\%s.cost\\\\%s\n", s_cost_name, SYSTEM_PARAM_PATHS_FILE);
				fprintf(pf_settings, s_buf);
				s_buf.Format(".\\data\\%s.isp\\\\%s\n", s_isp_name, SYSTEM_PARAM_MOD_FILE);
				fprintf(pf_settings, s_buf);
				s_buf.Format("%s\\\\%s\n", s_fit_func_sutec_name, SYSTEM_PARAM_FIT_FUNC);
				fprintf(pf_settings, s_buf);
				s_buf.Format("\\\\%s\n", SYSTEM_PARAM_SOLF_FILE);
				fprintf(pf_settings, s_buf);
				//s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);


//				s_buf.Format("%.0lf\\\\max time\n", d_time, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);
//				s_buf.Format("%d\\\\virus generations\n", i_gen, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);
//				s_buf.Format("%d\\\\population size\n", i_pop, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);

				//s_buf.Format("%.8lf\\\\virus prob cut\n", d_cut, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);
				//s_buf.Format("%.8lf\\\\virus prob splice\n", d_splice, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);

//				s_buf.Format("%.8lf\\\\virus prob mutation\n", d_mut, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);
//				s_buf.Format("%.8lf\\\\virus prob rem gene\n", d_mut, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);
//				s_buf.Format("%.8lf\\\\virus prob add gene\n", d_mut, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);
		
				

				i_count++;
				fclose(pf_settings);
			}//for  (int  ik = 0; ik < 3; ik++)
		}//for  (int  ii = 0; ii < 3; ii++)
	}//for  (int  i_test = 0; i_test < 8; i_test++)

	fclose(pf_serie);

}//System::Void CSurvP2PMainForm::but_tun_100_Click(System::Object*  sender, System::EventArgs*  e)


/*
template<typename T>
class Base
{
public:
	Base<T>() {i_test = 0;}
protected:
	int  i_test;
	T field;
};//class Base<T>


template<typename T>
class Child : public Base<T>
{
public:
	Child<T>() {i_test = 0;}
protected:
	int  i_buf;
};//class Child : pubilc Base<T>*/



System::Void CSurvP2PMainForm::but_gen_tun_Click(System::Object*  sender, System::EventArgs*  e)
{
	//Base<int>  c_test;
	//Child<int>  c_test_child;

	//return;

	v_generate_test_from_list();
	return;

	CString  s_buf;
	CString  s_problem_name;
	CString  s_settings_name;
	CString  s_node_name, s_tree_name, s_cost_name, s_isp_name, s_fit_func_name, s_fit_func_sutec_name;

	int  i_gen, i_pop;
	double  d_cut, d_splice;
	double  d_mut;

	FILE  *pf_serie, *pf_settings;
	int  i_repetation_number = 5;
	pf_serie = fopen("!serie.txt", "w+");

	d_mut = 0.05;
	d_cut = 0.12;
	d_splice = 0.10;
	i_gen = 200;
	i_pop = 16000;

	int  i_count = 0;
	for  (int  i_test = 0; i_test < 4; i_test++)
	{
		for  (int  ii = 0; ii < 2; ii++)
		{
			for  (int  ik = 0; ik < 2; ik++)
			{
			

				if  (i_test == 0)
				{
					s_node_name = "20a";
					s_tree_name = "22a";
					s_cost_name = "20b";
					s_isp_name = "20b";
					s_fit_func_name = "51";
					s_fit_func_sutec_name = "SCM";
				}//if  (i_test == 0)

				if  (i_test == 1)
				{
					s_node_name = "20e";
					s_tree_name = "23a";
					s_cost_name = "20b";
					s_isp_name = "20b";
					s_fit_func_name = "61";
					s_fit_func_sutec_name = "SCM_LD";
				}//if  (i_test == 0)

				if  (i_test == 2)
				{
					s_node_name = "21h";
					s_tree_name = "24a";
					s_cost_name = "21b";
					s_isp_name = "21b";
					s_fit_func_name = "71";
					s_fit_func_sutec_name = "SCM_ND";
				}//if  (i_test == 0)


				if  (i_test == 3)
				{
					s_node_name = "21k";
					s_tree_name = "23a";
					s_cost_name = "21b";
					s_isp_name = "21b";
					s_fit_func_name = "81";
					s_fit_func_sutec_name = "SCM_ID";
				}//if  (i_test == 0)

				s_problem_name = s_node_name + s_tree_name + s_cost_name + s_isp_name + s_fit_func_name;
				s_buf.Format("%d", i_count);
				s_settings_name.Format("settings_%s_%.3d.txt", s_problem_name, i_count);
				fprintf(pf_serie, "%s\\\\1\n",s_settings_name);
				pf_settings = fopen(s_settings_name, "w+");


				s_buf.Format(".\\%s\\\\%s\n", s_problem_name, SYSTEM_PARAM_FILE_NAME);
				fprintf(pf_settings, s_buf);
				s_buf.Format(".\\summarize_%d_%d.txt\\\\%s\n", ii, ik, SYSTEM_PARAM_SUMMARIZE);
				fprintf(pf_settings, s_buf);
				s_buf.Format(".\\data\\%s.node\\\\%s\n", s_node_name, SYSTEM_PARAM_NET_FILE);
				fprintf(pf_settings, s_buf);

				if  (i_test != 3)
					s_buf.Format(".\\data\\%s.tree\\\\%s\n", s_tree_name, SYSTEM_PARAM_PATHS_FILE);
				else
					s_buf.Format(".\\data\\sep%s.tree\\\\%s\n", s_tree_name, SYSTEM_PARAM_PATHS_FILE);

				fprintf(pf_settings, s_buf);
				s_buf.Format(".\\data\\%s.cost\\\\%s\n", s_cost_name, SYSTEM_PARAM_PATHS_FILE);
				fprintf(pf_settings, s_buf);
				s_buf.Format(".\\data\\%s.isp\\\\%s\n", s_isp_name, SYSTEM_PARAM_MOD_FILE);
				fprintf(pf_settings, s_buf);
				s_buf.Format("%s\\\\%s\n", s_fit_func_sutec_name, SYSTEM_PARAM_FIT_FUNC);
				fprintf(pf_settings, s_buf);
				s_buf.Format(".\\data\\%s.solf\\\\%s\n", s_problem_name, SYSTEM_PARAM_SOLF_FILE);
				fprintf(pf_settings, s_buf);
//				s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);


//				s_buf.Format("300\\\\max time\n", SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);
//				s_buf.Format("%d\\\\virus generations\n", i_gen, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);
//				s_buf.Format("%d\\\\population size\n", i_pop, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);

//				s_buf.Format("%.8lf\\\\virus prob cut\n", d_cut, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);
//				s_buf.Format("%.8lf\\\\virus prob splice\n", d_splice, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);

//				s_buf.Format("%.8lf\\\\virus prob mutation\n", d_mut, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);
//				s_buf.Format("%.8lf\\\\virus prob rem gene\n", d_mut, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);
//				s_buf.Format("%.8lf\\\\virus prob add gene\n", d_mut, SYSTEM_PARAM_SUTEC, SYSTEM_PARAM_ALGORITHM);
				fprintf(pf_settings, s_buf);
		
				

				i_count++;
				fclose(pf_settings);
			}//for  (int  ik = 0; ik < 3; ik++)
		}//for  (int  ii = 0; ii < 3; ii++)
	}//for  (int  i_test = 0; i_test < 8; i_test++)

	fclose(pf_serie);
}//System::Void CSurvP2PMainForm::but_gen_tun_Click(System::Object*  sender, System::EventArgs*  e)


System::Void CSurvP2PMainForm::but_gen_100_Click(System::Object*  sender, System::EventArgs*  e)
{
	CString  s_buf;
	CString  s_problem_name;
	CString  s_settings_name;
	CString  s_node_name, s_tree_name, s_cost_name, s_isp_name, s_fit_func_name, s_fit_func_sutec_name;
	CString  s_folder_prefix;


	FILE  *pf_serie, *pf_settings;

	int  i_repetation_number = 1;
	int  i_virginity_rounds, i_the_same_pattern_check, i_minimal_pattern_len, i_pattern_pool_size;

	int  i_gen, i_pop;
	double  d_cut, d_splice;
	double  d_mut;
	double  d_time;

	i_pattern_pool_size = 50;
	i_the_same_pattern_check = 0;
	i_minimal_pattern_len = 3;
	i_virginity_rounds = 5;
	i_gen = 3;
	i_pop = 25000;
	//i_pop = 250;
	d_mut = 0.05;
	d_cut = 0.05;
	d_splice = 0.10;

	//d_time = 10;//1500;
	d_time = 1500;//1500;

	int  i_count = 0;
	pf_serie = fopen("!serie.txt", "w+");


	
	//separate
	for  (int  i_cost = 2; i_cost < 10; i_cost++)
	{
		i_count = 0;
		for  (int  i_node = 0; i_node < 10; i_node++)
		{
			for  (int  i_tree = 0; i_tree < 3; i_tree++)
			{
				for  (int  i_isp = 0; i_isp < 1; i_isp++)
				{
					for  (int  i_fit_func = 0; i_fit_func < 4; i_fit_func++)
					{
						
						if  (i_node  ==  0)  s_node_name = "b00";
						if  (i_node  ==  1)  s_node_name = "b01";
						if  (i_node  ==  2)  s_node_name = "b02";
						if  (i_node  ==  3)  s_node_name = "b03";
						if  (i_node  ==  4)  s_node_name = "b04";
						if  (i_node  ==  5)  s_node_name = "b05";
						if  (i_node  ==  6)  s_node_name = "b06";
						if  (i_node  ==  7)  s_node_name = "b07";
						if  (i_node  ==  8)  s_node_name = "b08";
						if  (i_node  ==  9)  s_node_name = "b09";

						if  (i_tree  ==  0)  s_tree_name = "24a";
						if  (i_tree  ==  1)  s_tree_name = "26a";
						if  (i_tree  ==  2)  s_tree_name = "28a";

						//if  (i_cost  ==  0)  s_cost_name = "b01";
						s_cost_name.Format("b%.2d", i_cost);

						if  (i_isp  ==  0)  s_isp_name = "b00";
						


						if  (i_fit_func  ==  0)  s_fit_func_name = "51";
						if  (i_fit_func  ==  0)  s_fit_func_sutec_name = "SCM";

						if  (i_fit_func  ==  1)  s_fit_func_name = "61";
						if  (i_fit_func  ==  1)  s_fit_func_sutec_name = "SCM_LD";

						if  (i_fit_func  ==  2)  s_fit_func_name = "71";
						if  (i_fit_func  ==  2)  s_fit_func_sutec_name = "SCM_ND";

						if  (i_fit_func  ==  3)  s_fit_func_name = "81";
						if  (i_fit_func  ==  3)  s_fit_func_sutec_name = "SCM_ID";

						
						
						s_folder_prefix.Format("b%d\\", i_cost);

						s_problem_name = s_node_name + s_tree_name + s_cost_name + s_isp_name + s_fit_func_name;
						s_buf.Format("%d", i_count);
						//s_settings_name.Format("settings_%s_%.3d.txt", s_problem_name, i_count);
						s_settings_name.Format("%ssettings_%s_%.3d.txt", s_folder_prefix, s_problem_name, i_count);
						fprintf(pf_serie, "%s\\\\1\n",s_settings_name);
						pf_settings = fopen(s_settings_name, "w+");


						s_buf.Format("%s.\\%s\\\\%s\n", s_folder_prefix, s_problem_name, SYSTEM_PARAM_FILE_NAME);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%s.\\summarize_separate.txt\\\\%s\n", s_folder_prefix, SYSTEM_PARAM_SUMMARIZE);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%s.\\data\\%s.node\\\\%s\n", s_folder_prefix, s_node_name, SYSTEM_PARAM_NET_FILE);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%s.\\data\\%s.tree\\\\%s\n", s_folder_prefix, s_tree_name, SYSTEM_PARAM_PATHS_FILE);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%s.\\data\\%s.cost\\\\%s\n", s_folder_prefix, s_cost_name, SYSTEM_PARAM_PATHS_FILE);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%s.\\data\\%s.isp\\\\%s\n", s_folder_prefix, s_isp_name, SYSTEM_PARAM_MOD_FILE);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%s\\\\%s\n", s_fit_func_sutec_name, SYSTEM_PARAM_FIT_FUNC);
						fprintf(pf_settings, s_buf);
						s_buf.Format("\\\\%s\n", SYSTEM_PARAM_SOLF_FILE);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_ALGORITHM_MUPPETS, SYSTEM_PARAM_ALGORITHM);
						fprintf(pf_settings, s_buf);


						s_buf.Format("%.0lf\\\\max time\n", d_time);
						fprintf(pf_settings, s_buf);

						
						s_buf.Format("%d\\\\pattern pool size\n", i_pattern_pool_size);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%d\\\\the same pattern check\n", i_the_same_pattern_check);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%d\\\\minimal pattern length\n", i_minimal_pattern_len);
						fprintf(pf_settings, s_buf);

						

						s_buf.Format("%d\\\\virus generations\n", i_gen);
						fprintf(pf_settings, s_buf);

						s_buf.Format("%d\\\\population size\n", i_pop);
						fprintf(pf_settings, s_buf);

						s_buf.Format("%.8lf\\\\virus prob cut\n", d_cut);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%.8lf\\\\virus prob splice\n", d_splice);
						fprintf(pf_settings, s_buf);

						s_buf.Format("%.8lf\\\\virus prob mutation\n", d_mut);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%.8lf\\\\virus prob rem gene\n", d_mut);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%.8lf\\\\virus prob add gene\n", d_mut);
						fprintf(pf_settings, s_buf);


						s_buf.Format("%d\\\\virginity rounds\n", i_virginity_rounds);
						fprintf(pf_settings, s_buf);


						s_buf.Format("1\\\\tree construction (0-old, 1-new)\n", i_virginity_rounds);
						fprintf(pf_settings, s_buf);
						s_buf.Format("0.5\\\\parent/level mutation ratio\n", i_virginity_rounds);
						fprintf(pf_settings, s_buf);

				
						

						i_count++;
						fclose(pf_settings);
					}//for  (int  i_fit_func = 0; i_fit_func < 1; i_fit_func++)				
				}//for  (int  i_isp = 0; i_isp < 1; i_isp++)		
			}//	for  (int  i_cost = 0; i_cost < 1; i_cost++)
		}//for  (int  i_tree = 0; i_tree < 3; i_tree++)
	}//for  (int  i_node = 0; i_node < 12; i_node++)

	fclose(pf_serie);
	return;//*/

	

	/*//common
	for  (int  i_node = 0; i_node < 10; i_node++)
	{
		for  (int  i_tree = 0; i_tree < 1; i_tree++)
		{
			for  (int  i_cost = 0; i_cost < 1; i_cost++)
			{
				for  (int  i_isp = 0; i_isp < 1; i_isp++)
				{
					for  (int  i_fit_func = 0; i_fit_func < 4; i_fit_func++)
					{
						
						if  (i_node  ==  0)  s_node_name = "b00";
						if  (i_node  ==  1)  s_node_name = "b01";
						if  (i_node  ==  2)  s_node_name = "b02";
						if  (i_node  ==  3)  s_node_name = "b03";
						if  (i_node  ==  4)  s_node_name = "b04";
						if  (i_node  ==  5)  s_node_name = "b05";
						if  (i_node  ==  6)  s_node_name = "b06";
						if  (i_node  ==  7)  s_node_name = "b07";
						if  (i_node  ==  8)  s_node_name = "b08";
						if  (i_node  ==  9)  s_node_name = "b09";

						if  (i_tree  ==  0)  s_tree_name = "23b";

						if  (i_cost  ==  0)  s_cost_name = s_node_name;

						if  (i_isp  ==  0)  s_isp_name = "b00";

						if  (i_fit_func  ==  0)  s_fit_func_name = "51";
						if  (i_fit_func  ==  0)  s_fit_func_sutec_name = "SCM";

						if  (i_fit_func  ==  1)  s_fit_func_name = "61";
						if  (i_fit_func  ==  1)  s_fit_func_sutec_name = "SCM_LD";

						if  (i_fit_func  ==  2)  s_fit_func_name = "71";
						if  (i_fit_func  ==  2)  s_fit_func_sutec_name = "SCM_ND";

						if  (i_fit_func  ==  3)  s_fit_func_name = "81";
						if  (i_fit_func  ==  3)  s_fit_func_sutec_name = "SCM_ID";

						
						

						s_problem_name = s_node_name + s_tree_name + s_cost_name + s_isp_name + s_fit_func_name;
						s_buf.Format("%d", i_count);
						s_settings_name.Format("settings_%s_%.3d.txt", s_problem_name, i_count);
						fprintf(pf_serie, "%s\\\\1\n",s_settings_name);
						pf_settings = fopen(s_settings_name, "w+");


						s_buf.Format(".\\%s\\\\%s\n", s_problem_name, SYSTEM_PARAM_FILE_NAME);
						fprintf(pf_settings, s_buf);
						s_buf.Format(".\\summarize_common.txt\\\\%s\n", SYSTEM_PARAM_SUMMARIZE);
						fprintf(pf_settings, s_buf);
						s_buf.Format(".\\data\\%s.node\\\\%s\n", s_node_name, SYSTEM_PARAM_NODE_FILE);
						fprintf(pf_settings, s_buf);
						s_buf.Format(".\\data\\%s.tree\\\\%s\n", s_tree_name, SYSTEM_PARAM_TREE_FILE);
						fprintf(pf_settings, s_buf);
						s_buf.Format(".\\data\\%s.cost\\\\%s\n", s_cost_name, SYSTEM_PARAM_COST_FILE);
						fprintf(pf_settings, s_buf);
						s_buf.Format(".\\data\\%s.isp\\\\%s\n", s_isp_name, SYSTEM_PARAM_ISP_FILE);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%s\\\\%s\n", s_fit_func_sutec_name, SYSTEM_PARAM_FIT_FUNC);
						fprintf(pf_settings, s_buf);
						s_buf.Format("\\\\%s\n", SYSTEM_PARAM_SOLF_FILE);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_ALGORITHM_MUPPETS, SYSTEM_PARAM_ALGORITHM);
						fprintf(pf_settings, s_buf);


						s_buf.Format("%.0lf\\\\max time\n", d_time);
						fprintf(pf_settings, s_buf);

						
						s_buf.Format("%d\\\\pattern pool size\n", i_pattern_pool_size);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%d\\\\the same pattern check\n", i_the_same_pattern_check);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%d\\\\minimal pattern length\n", i_minimal_pattern_len);
						fprintf(pf_settings, s_buf);

						

						s_buf.Format("%d\\\\virus generations\n", i_gen);
						fprintf(pf_settings, s_buf);

						s_buf.Format("%d\\\\population size\n", i_pop);
						fprintf(pf_settings, s_buf);

						s_buf.Format("%.8lf\\\\virus prob cut\n", d_cut);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%.8lf\\\\virus prob splice\n", d_splice);
						fprintf(pf_settings, s_buf);

						s_buf.Format("%.8lf\\\\virus prob mutation\n", d_mut);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%.8lf\\\\virus prob rem gene\n", d_mut);
						fprintf(pf_settings, s_buf);
						s_buf.Format("%.8lf\\\\virus prob add gene\n", d_mut);
						fprintf(pf_settings, s_buf);


						s_buf.Format("%d\\\\virginity rounds\n", i_virginity_rounds);
						fprintf(pf_settings, s_buf);


						s_buf.Format("1\\\\tree construction (0-old, 1-new)\n", i_virginity_rounds);
						fprintf(pf_settings, s_buf);
						s_buf.Format("0.5\\\\parent/level mutation ratio\n", i_virginity_rounds);
						fprintf(pf_settings, s_buf);


				
						

						i_count++;
						fclose(pf_settings);
					}//for  (int  i_fit_func = 0; i_fit_func < 1; i_fit_func++)				
				}//for  (int  i_isp = 0; i_isp < 1; i_isp++)		
			}//	for  (int  i_cost = 0; i_cost < 1; i_cost++)
		}//for  (int  i_tree = 0; i_tree < 3; i_tree++)
	}//for  (int  i_node = 0; i_node < 12; i_node++)//*/

	fclose(pf_serie);

}//System::Void CSurvP2PMainForm::but_gen_100_Click(System::Object*  sender, System::EventArgs*  e)




int  i_random_search_generate_sett_file_from_line(CString  sLine, double  *pdOptimalValue = NULL)
{
	if  (sLine.GetLength()  <  11)  return(0);

	CString  s_buf;
	CString  s_settings_name;

	FILE  *pf_settings;
	s_settings_name = "settings_" + sLine + ".txt";
	pf_settings = fopen(s_settings_name, "w+");
	if  (pf_settings  ==  NULL)
	{
		CError  c_err;

		s_buf.Format("unable to open (%s)", s_settings_name);
		c_err.vPutError(s_buf);
		return(0);
	}//if  (pf_settings  ==  NULL)


	int  i_time;
	
	i_time = 1200;
	


	CString  s_network;
	s_network = sLine.GetAt(0);
	s_network += sLine.GetAt(1);

	CString  s_replica;
	s_replica = sLine.GetAt(9);
	s_replica += sLine.GetAt(10);


	CString  s_dem_unicast;
	s_dem_unicast = sLine.GetAt(2);
	s_dem_unicast += sLine.GetAt(3);


	CString  s_dem_anycast;
	s_dem_anycast = sLine.GetAt(4);
	s_dem_anycast += sLine.GetAt(5);

	CString  s_pat_len;
	s_pat_len = sLine.GetAt(6);
	s_pat_len += sLine.GetAt(7);
	s_pat_len += sLine.GetAt(8);
	

	s_buf.Format(".\\%s\\\\%s\n", s_settings_name, SYSTEM_PARAM_FILE_NAME);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\summarize.txt\\\\%s\n", SYSTEM_PARAM_SUMMARIZE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.net\\\\%s\n", s_network, SYSTEM_PARAM_NET_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.rep\\\\%s\n", s_replica, SYSTEM_PARAM_REP_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.dem\\\\%s\n", s_dem_unicast, SYSTEM_PARAM_DEMAND_UNICAST_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.dea\\\\%s\n", s_dem_anycast, SYSTEM_PARAM_DEMAND_ANYCAST_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.pat\\\\%s\n", s_pat_len, SYSTEM_PARAM_PATHS_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.len\\\\%s\n", s_pat_len, SYSTEM_PARAM_MOD_FILE);
	fprintf(pf_settings, s_buf);

	if  (pdOptimalValue  !=  NULL)
		s_buf.Format("%.0lf\\\\%s\n", *pdOptimalValue, SYSTEM_PARAM_OPTIMAL);
	else
	{
		//s_buf.Format(".\\sol\\%s.max\\\\%s\n", sLine, SYSTEM_PARAM_SOLF_FILE);
		s_buf.Format("\\\\%s\n", SYSTEM_PARAM_SOLF_FILE);
	}//else  if  (pdOptimalValue  !=  NULL)
	fprintf(pf_settings, s_buf);


	s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_FIT_FUNC_HIGHEST_SLOT, SYSTEM_PARAM_FIT_FUNC);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_ALGORITHM_RANDOM_SEARCH, SYSTEM_PARAM_ALGORITHM);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%d\\\\max time\n", i_time);
	fprintf(pf_settings, s_buf);


	fclose(pf_settings);

	return(1);
}//int  i_random_search_generate_sett_file_from_line(CString  sLine)




int  i_sga_generate_sett_file_from_line(CString  sLine, double  *pdOptimalValue = NULL)
{
	if  (sLine.GetLength()  <  11)  return(0);

	CString  s_buf;
	CString  s_settings_name;

	FILE  *pf_settings;
	s_settings_name = "settings_" + sLine + ".txt";
	pf_settings = fopen(s_settings_name, "w+");
	if  (pf_settings  ==  NULL)
	{
		CError  c_err;

		s_buf.Format("unable to open (%s)", s_settings_name);
		c_err.vPutError(s_buf);
		return(0);
	}//if  (pf_settings  ==  NULL)


	int  i_time;
	int  i_sga_pop_size;
	double  d_sga_cross, d_sga_mut;
	
	i_sga_pop_size = 12000;
	d_sga_cross = 0.7;
	d_sga_mut = 0.4;

	
	
	i_time = 1200;
	


	CString  s_network;
	s_network = sLine.GetAt(0);
	s_network += sLine.GetAt(1);

	CString  s_replica;
	s_replica = sLine.GetAt(9);
	s_replica += sLine.GetAt(10);


	CString  s_dem_unicast;
	s_dem_unicast = sLine.GetAt(2);
	s_dem_unicast += sLine.GetAt(3);


	CString  s_dem_anycast;
	s_dem_anycast = sLine.GetAt(4);
	s_dem_anycast += sLine.GetAt(5);

	CString  s_pat_len;
	s_pat_len = sLine.GetAt(6);
	s_pat_len += sLine.GetAt(7);
	s_pat_len += sLine.GetAt(8);
	

	s_buf.Format(".\\%s\\\\%s\n", s_settings_name, SYSTEM_PARAM_FILE_NAME);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\summarize.txt\\\\%s\n", SYSTEM_PARAM_SUMMARIZE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.net\\\\%s\n", s_network, SYSTEM_PARAM_NET_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.rep\\\\%s\n", s_replica, SYSTEM_PARAM_REP_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.dem\\\\%s\n", s_dem_unicast, SYSTEM_PARAM_DEMAND_UNICAST_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.dea\\\\%s\n", s_dem_anycast, SYSTEM_PARAM_DEMAND_ANYCAST_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.pat\\\\%s\n", s_pat_len, SYSTEM_PARAM_PATHS_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.len\\\\%s\n", s_pat_len, SYSTEM_PARAM_MOD_FILE);
	fprintf(pf_settings, s_buf);

	if  (pdOptimalValue  !=  NULL)
		s_buf.Format("%.0lf\\\\%s\n", *pdOptimalValue, SYSTEM_PARAM_OPTIMAL);
	else
	{
		//s_buf.Format(".\\sol\\%s.max\\\\%s\n", sLine, SYSTEM_PARAM_SOLF_FILE);
		s_buf.Format("\\\\%s\n", SYSTEM_PARAM_SOLF_FILE);
	}//else  if  (pdOptimalValue  !=  NULL)
	fprintf(pf_settings, s_buf);


	s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_FIT_FUNC_HIGHEST_SLOT, SYSTEM_PARAM_FIT_FUNC);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_ALGORITHM_SIMPLE_GA, SYSTEM_PARAM_ALGORITHM);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%d\\\\max time\n", i_time);
	fprintf(pf_settings, s_buf);




	s_buf.Format("%d\\\\simple GA popsize\n", i_sga_pop_size);
	fprintf(pf_settings, s_buf);
	
	s_buf.Format("%.8lf\\\\simple GA cross\n", d_sga_cross);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%.8lf\\\\simple GA mut\n", d_sga_mut);
	fprintf(pf_settings, s_buf);

	

	fclose(pf_settings);

	return(1);
}//int  i_sga_generate_sett_file_from_line(CString  sLine)


int  i_generate_sett_file_from_line_trees(CString  sLine, double  *pdOptimalValue = NULL)
{
	if  (sLine.GetLength()  <  11)  return(0);

	CString  s_buf;
	CString  s_settings_name;

	FILE  *pf_settings;
	s_settings_name = "settings_" + sLine + ".txt";
	pf_settings = fopen(s_settings_name, "w+");
	if  (pf_settings  ==  NULL)
	{
		CError  c_err;

		s_buf.Format("unable to open (%s)", s_settings_name);
		c_err.vPutError(s_buf);
		return(0);
	}//if  (pf_settings  ==  NULL)


	int  i_gen, i_pop;
	double  d_cut, d_splice;
	double  d_mut;
	int  i_pattern_pool_size, i_the_same_pattern_check, i_minimal_pattern_len, i_virginity_rounds;
	int  i_time;
	int  i_ct_strategy;

	i_gen = 3;
	//i_pop = 15000;
	i_pop = 50;
	d_mut = 0.05;
	d_cut = 0.12;
	d_splice = 0.15;
	i_pattern_pool_size = 500;
	i_the_same_pattern_check = 0;
	i_minimal_pattern_len = 3;
	i_virginity_rounds = 100;
	i_time = 1200;
	i_ct_strategy = 0;//0-classic, 1-new, 2-optimum assumption



	CString  s_network;
	s_network = sLine.GetAt(0);
	s_network += sLine.GetAt(1);

	CString  s_replica;
	s_replica = sLine.GetAt(9);
	s_replica += sLine.GetAt(10);


	CString  s_dem_unicast;
	s_dem_unicast = sLine.GetAt(2);
	s_dem_unicast += sLine.GetAt(3);


	CString  s_dem_anycast;
	s_dem_anycast = sLine.GetAt(4);
	s_dem_anycast += sLine.GetAt(5);

	CString  s_pat_len;
	s_pat_len = sLine.GetAt(6);
	s_pat_len += sLine.GetAt(7);
	s_pat_len += sLine.GetAt(8);
	

	s_buf.Format(".\\%s\\\\%s\n", s_settings_name, SYSTEM_PARAM_FILE_NAME);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\summarize.txt\\\\%s\n", SYSTEM_PARAM_SUMMARIZE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.net\\\\%s\n", s_network, SYSTEM_PARAM_NET_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.rep\\\\%s\n", s_replica, SYSTEM_PARAM_REP_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.dem\\\\%s\n", s_dem_unicast, SYSTEM_PARAM_DEMAND_UNICAST_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.dea\\\\%s\n", s_dem_anycast, SYSTEM_PARAM_DEMAND_ANYCAST_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.pat\\\\%s\n", s_pat_len, SYSTEM_PARAM_PATHS_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.len\\\\%s\n", s_pat_len, SYSTEM_PARAM_MOD_FILE);
	fprintf(pf_settings, s_buf);

	if  (pdOptimalValue  !=  NULL)
		s_buf.Format("%.0lf\\\\%s\n", *pdOptimalValue, SYSTEM_PARAM_OPTIMAL);
	else
	{
		//s_buf.Format(".\\sol\\%s.max\\\\%s\n", sLine, SYSTEM_PARAM_SOLF_FILE);
		s_buf.Format("\\\\%s\n", SYSTEM_PARAM_SOLF_FILE);
	}//else  if  (pdOptimalValue  !=  NULL)
	fprintf(pf_settings, s_buf);

	s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_FIT_FUNC_HIGHEST_SLOT, SYSTEM_PARAM_FIT_FUNC);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_ALGORITHM_MUPPETS, SYSTEM_PARAM_ALGORITHM);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%d\\\\max time\n", i_time);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%d\\\\pattern pool size\n", i_pattern_pool_size);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%d\\\\the same pattern check\n", i_the_same_pattern_check);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%d\\\\minimal pattern length\n", i_minimal_pattern_len);
	fprintf(pf_settings, s_buf);

	

	s_buf.Format("%d\\\\virus generations\n", i_gen);
	fprintf(pf_settings, s_buf);

	s_buf.Format("%d\\\\population size\n", i_pop);
	fprintf(pf_settings, s_buf);

	s_buf.Format("%.8lf\\\\virus prob cut\n", d_cut);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%.8lf\\\\virus prob splice\n", d_splice);
	fprintf(pf_settings, s_buf);

	s_buf.Format("%.8lf\\\\virus prob mutation\n", d_mut);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%.8lf\\\\virus prob rem gene\n", d_mut);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%.8lf\\\\virus prob add gene\n", d_mut);
	fprintf(pf_settings, s_buf);


	s_buf.Format("%d\\\\virginity rounds\n", i_virginity_rounds);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%d\\\\ct add remove strategy (0-classic; 1-new)\n", i_ct_strategy);
	fprintf(pf_settings, s_buf);



	fclose(pf_settings);

	return(1);
}//int  i_generate_sett_file_from_line(CSrting  sLine)






int  i_generate_sett_file_from_line(CString  sLine, double  *pdOptimalValue = NULL)
{
	if  (sLine.GetLength()  <  11)  return(0);

	CString  s_buf;
	CString  s_settings_name;

	FILE  *pf_settings;
	s_settings_name = "settings_" + sLine + ".txt";
	pf_settings = fopen(s_settings_name, "w+");
	if  (pf_settings  ==  NULL)
	{
		CError  c_err;

		s_buf.Format("unable to open (%s)", s_settings_name);
		c_err.vPutError(s_buf);
		return(0);
	}//if  (pf_settings  ==  NULL)


	int  i_gen, i_pop;
	double  d_cut, d_splice;
	double  d_mut;
	int  i_pattern_pool_size, i_the_same_pattern_check, i_minimal_pattern_len, i_virginity_rounds;
	int  i_time;
	int  i_ct_strategy;

	i_gen = 3;
	//i_pop = 15000;
	i_pop = 50;
	d_mut = 0.05;
	d_cut = 0.12;
	d_splice = 0.15;
	i_pattern_pool_size = 500;
	i_the_same_pattern_check = 0;
	i_minimal_pattern_len = 3;
	i_virginity_rounds = 100;
	i_time = 1200;
	i_ct_strategy = 0;//0-classic, 1-new, 2-optimum assumption



	CString  s_network;
	s_network = sLine.GetAt(0);
	s_network += sLine.GetAt(1);

	CString  s_replica;
	s_replica = sLine.GetAt(9);
	s_replica += sLine.GetAt(10);


	CString  s_dem_unicast;
	s_dem_unicast = sLine.GetAt(2);
	s_dem_unicast += sLine.GetAt(3);


	CString  s_dem_anycast;
	s_dem_anycast = sLine.GetAt(4);
	s_dem_anycast += sLine.GetAt(5);

	CString  s_pat_len;
	s_pat_len = sLine.GetAt(6);
	s_pat_len += sLine.GetAt(7);
	s_pat_len += sLine.GetAt(8);
	

	s_buf.Format(".\\%s\\\\%s\n", s_settings_name, SYSTEM_PARAM_FILE_NAME);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\summarize.txt\\\\%s\n", SYSTEM_PARAM_SUMMARIZE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.net\\\\%s\n", s_network, SYSTEM_PARAM_NET_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.rep\\\\%s\n", s_replica, SYSTEM_PARAM_REP_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.dem\\\\%s\n", s_dem_unicast, SYSTEM_PARAM_DEMAND_UNICAST_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.dea\\\\%s\n", s_dem_anycast, SYSTEM_PARAM_DEMAND_ANYCAST_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.pat\\\\%s\n", s_pat_len, SYSTEM_PARAM_PATHS_FILE);
	fprintf(pf_settings, s_buf);
	s_buf.Format(".\\data\\%s.len\\\\%s\n", s_pat_len, SYSTEM_PARAM_MOD_FILE);
	fprintf(pf_settings, s_buf);

	if  (pdOptimalValue  !=  NULL)
		s_buf.Format("%.0lf\\\\%s\n", *pdOptimalValue, SYSTEM_PARAM_OPTIMAL);
	else
	{
		//s_buf.Format(".\\sol\\%s.max\\\\%s\n", sLine, SYSTEM_PARAM_SOLF_FILE);
		s_buf.Format("\\\\%s\n", SYSTEM_PARAM_SOLF_FILE);
	}//else  if  (pdOptimalValue  !=  NULL)
	fprintf(pf_settings, s_buf);

	s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_FIT_FUNC_HIGHEST_SLOT, SYSTEM_PARAM_FIT_FUNC);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_ALGORITHM_MUPPETS, SYSTEM_PARAM_ALGORITHM);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%d\\\\max time\n", i_time);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%d\\\\pattern pool size\n", i_pattern_pool_size);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%d\\\\the same pattern check\n", i_the_same_pattern_check);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%d\\\\minimal pattern length\n", i_minimal_pattern_len);
	fprintf(pf_settings, s_buf);

	

	s_buf.Format("%d\\\\virus generations\n", i_gen);
	fprintf(pf_settings, s_buf);

	s_buf.Format("%d\\\\population size\n", i_pop);
	fprintf(pf_settings, s_buf);

	s_buf.Format("%.8lf\\\\virus prob cut\n", d_cut);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%.8lf\\\\virus prob splice\n", d_splice);
	fprintf(pf_settings, s_buf);

	s_buf.Format("%.8lf\\\\virus prob mutation\n", d_mut);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%.8lf\\\\virus prob rem gene\n", d_mut);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%.8lf\\\\virus prob add gene\n", d_mut);
	fprintf(pf_settings, s_buf);


	s_buf.Format("%d\\\\virginity rounds\n", i_virginity_rounds);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%d\\\\ct add remove strategy (0-classic; 1-new)\n", i_ct_strategy);
	fprintf(pf_settings, s_buf);



	fclose(pf_settings);

	return(1);
}//int  i_generate_sett_file_from_line(CSrting  sLine)







void  v_fiber_generate_from_list_trees()
{
	CError  c_err;
	CString  s_buf;
	CString  s_list_file;

	s_list_file = ::Application::ExecutablePath;
	s_buf.Replace("FiberFlowAssignment.exe", "");
	s_buf += "problem_list.txt";


	FILE  *pf_generation_list;
	pf_generation_list = fopen(s_list_file, "r");
	
	if  (pf_generation_list  ==  NULL)
	{
		s_buf.Format("Cannot open (%s) for settings list generation", s_list_file);
		c_err.vPutError(s_buf);
		::MessageBox(NULL, "NOK", "NOK", MB_OK);
		return;
	}//if  (pf_generation_list  ==  NULL)

	FILE  *pf_serie;
	pf_serie = fopen("serie.txt", "w+");

	CString  s_line = "a";
	CString  s_optimal_value = "";
	double  d_optimal_value;
	
	int  i_sett_counter = 0;
	int  i_buf;

	while  ( (s_line  !=  "")&&(feof(pf_generation_list)  ==  false) )
	{
		s_line = Tools::sReadLine(pf_generation_list, &s_optimal_value);

		if  (s_optimal_value.GetLength() > 0)
		{
			d_optimal_value = atof(s_optimal_value);
		}//if  (s_optimal_value.GetLength() > 0)


		if  (s_optimal_value.GetLength() > 0)
		{
			i_buf = i_generate_sett_file_from_line_trees(s_line, &d_optimal_value);
			//i_buf = i_sga_generate_sett_file_from_line(s_line, &d_optimal_value);
			//i_buf = i_random_search_generate_sett_file_from_line(s_line, &d_optimal_value);
		}//if  (s_optimal_value.GetLength() > 0)
		else
		{
			//i_buf = i_generate_sett_file_from_line(s_line);
			//i_buf = i_sga_generate_sett_file_from_line(s_line);
			//i_buf = i_random_search_generate_sett_file_from_line(s_line);
		}//else  if  (s_optimal_value.GetLength() > 0)
		

		if  (i_buf  >  0)
		{
			fprintf(pf_serie, "settings_" + s_line + ".txt\\\\1\n");
		}//if  (i_buf  >  0)
		i_sett_counter += i_buf;//*/
	}//while  ( (s_line  !=  "")&&(feof(pf_generation_list)  ==  false) )

	fclose(pf_generation_list);
	fclose(pf_serie);
	
	::MessageBox(NULL, "ok", "ok", MB_OK);

}//void  v_fiber_generate_from_list()




void  v_generate_tuning_for_line(bool  bTuning, bool  bStartSolution, CString  sLine, FILE  *pfSerie, int iTreePathOffset, int iGenNum, int iPopSize, double  dCutProb, double  dSpliceProb, double  dMutationProb, int iSettingsRunNumber, int iCleverDecoding)
{
	CString  s_buf;

	CString  s_network;
	s_network = sLine.GetAt(0);
	s_network += sLine.GetAt(1);
	s_network += ".net";

	CString  s_demands;
	s_demands = sLine.GetAt(2);
	s_demands += sLine.GetAt(3);

	if  (s_demands  ==  "xx")
		s_demands = "";
	else
		s_demands += ".dem";


	CString  s_demands_anycast;
	s_demands_anycast = sLine.GetAt(4);
	s_demands_anycast += sLine.GetAt(5);
	s_demands_anycast += ".dea";

	CString  s_demands_multicast;
	s_demands_multicast = sLine.GetAt(6);
	s_demands_multicast += sLine.GetAt(7);
	
	CString  s_paths;
	s_paths = sLine.GetAt(8);
	
	CString  s_replicas;
	s_replicas = sLine.GetAt(11);
	s_replicas += sLine.GetAt(12);
	s_replicas += ".rep";


	int  pi_paths[7] = {2, 4, 6, 8, 10, 20, 30};
	int  pi_trees[7] = {10, 50, 100, 200, 300, 500, 1000};
	int  pi_comp_time[7] = {7200, 7200, 7200, 9000, 10800, 12600, 14400};
	//int  pi_comp_time[7] = {19600, 19600, 19600, 19600, 19600, 19600, 19600};
	//int  i_paths_trees_offset;
	//i_paths_trees_offset = 0;

	int  i_time_secs = pi_comp_time[iTreePathOffset];
	//i_time_secs = 5;

	int  i_virus_generations = iGenNum;
	int  i_pop_size = iPopSize;
	double  d_virus_prob_cut = dCutProb;
	double  d_virus_prob_splice = dSpliceProb;
	double  d_virus_prob_mutation = dMutationProb;

	int  i_fluent_optimization = iCleverDecoding;
	
	
	CString  s_result_filename;

	if  (bTuning  ==  true)
		s_result_filename.Format("settings_%.4d_%s.txt", (int) (dMutationProb * 100), sLine);
	else
		s_result_filename.Format("mup_settings_%s_%d.txt", sLine, iTreePathOffset);


	CString  s_sett_filename;
	s_sett_filename = "z_" + s_result_filename;
	
	CString  s_serie_filename;

	if  (bTuning  ==  true)
		s_serie_filename.Format("serie_%.4d.txt", (int) (dMutationProb * 100));
	else
		s_serie_filename.Format("serie_%d.txt", iTreePathOffset);


	FILE  *pf_serie;
	pf_serie = fopen(s_serie_filename, "a");
	s_buf.Format("%s\\\\%d\n", s_sett_filename, iSettingsRunNumber);
	fprintf(pf_serie, s_buf);
	fclose(pf_serie);


	FILE  *pf_settings;
	pf_settings = fopen(s_sett_filename, "w+");

	fprintf(pf_settings, ".\\%s\\\\file name\n", s_result_filename);
	fprintf(pf_settings, ".\\test_summarize_2_10.txt\\\\summarize\n");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\net\n", s_network);
	fprintf(pf_settings, "\\\\replic\n");
	if  (s_demands  ==  "")
		fprintf(pf_settings, "\\\\unicast\n", s_demands);
	else
		fprintf(pf_settings, ".\\..\\data\\%s\\\\unicast\n", s_demands);
	fprintf(pf_settings, "\\\\anycast\n");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\multicast\n", s_demands_multicast + ".dmc");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\paths\n", s_paths + ".pat");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unispec\n", s_paths + "1.spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unireg\n", s_paths + "1.regs");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\trees\n", s_demands_multicast + ".tree");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treespec\n", s_demands_multicast + ".spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treereg\n", s_demands_multicast + ".regs");

	fprintf(pf_settings, "%d\\\\paths to use\n", pi_paths[iTreePathOffset]);
	fprintf(pf_settings, "%d\\\\trees to use\n",  pi_trees[iTreePathOffset]);

	fprintf(pf_settings, ".\\data\\u30.len\\\\modulation\n");

	if  (bStartSolution  ==  false)
		fprintf(pf_settings, "\\\\solution\n");
	else
		fprintf(pf_settings, ".\\..\\US26_init\\t=%d\\%s.max\\\\solution\n",pi_trees[iTreePathOffset], sLine);

	fprintf(pf_settings, "HighestSlot\\\\fit func\n");
	fprintf(pf_settings, "muppets\\\\algorithm\n");

	fprintf(pf_settings, "%d\\\\max time\n", i_time_secs);
	//fprintf(pf_settings, "1800\\\\probing time\n");

	fprintf(pf_settings, "7000\\\\pattern pool size\n");
	fprintf(pf_settings, "0\\\\the same pattern check\n");
	fprintf(pf_settings, "3\\\\minimal pattern length\n");

	fprintf(pf_settings, "%d\\\\virus generations\n", i_virus_generations);
	fprintf(pf_settings, "%d\\\\population size\n", i_pop_size);
	fprintf(pf_settings, "%.8lf\\\\virus prob cut\n", d_virus_prob_cut);
	fprintf(pf_settings, "%.8lf\\\\virus prob splice\n", d_virus_prob_splice);
	fprintf(pf_settings, "%.8lf\\\\virus prob mutation\n", d_virus_prob_mutation);
	fprintf(pf_settings, "%.8lf\\\\virus prob rem gene\n", d_virus_prob_mutation);
	fprintf(pf_settings, "%.8lf\\\\virus prob add gene\n", d_virus_prob_mutation);
	
	fprintf(pf_settings, "100\\\\virginity rounds\n");
	fprintf(pf_settings, "0\\\\ct add remove strategy (0-classic; 1-new)\n");
	fprintf(pf_settings, "0\\\\Bayesian effect (0-none; 1-brutal; 2-clever)\n");
	fprintf(pf_settings, "%d\\\\Solution constructon (0-normal; 1-fluent optimization)\n", i_fluent_optimization);


	fclose(pf_settings);
}//void  v_generate_tuning_for_line(CString  sLine, FILE  *pfSerie)




void  v_sim_annealing_generate_sett_file_from_line(bool  bStartSolution, CString  sLine, FILE  *pfSerie, int iTreePathOffset, int iSettingsRunNumber, int iCleverDecoding)
{
	CString  s_buf;

	//s_buf.Format("%d", sLine.GetLength());
	//::Tools::vRepInFile("test.txt", s_buf);
	if  (sLine.GetLength()  <  9)  return;

	CString  s_network;
	s_network = sLine.GetAt(0);
	s_network += sLine.GetAt(1);
	s_network += ".net";

	CString  s_demands;
	s_demands = sLine.GetAt(2);
	s_demands += sLine.GetAt(3);

	if  (s_demands  ==  "xx")
		s_demands = "";
	else
		s_demands += ".dem";


	CString  s_demands_anycast;
	s_demands_anycast = sLine.GetAt(4);
	s_demands_anycast += sLine.GetAt(5);
	s_demands_anycast += ".dea";

	CString  s_demands_multicast;
	s_demands_multicast = sLine.GetAt(6);
	s_demands_multicast += sLine.GetAt(7);
	
	CString  s_paths;
	s_paths = sLine.GetAt(8);
	
	CString  s_replicas;
	s_replicas = sLine.GetAt(11);
	s_replicas += sLine.GetAt(12);
	s_replicas += ".rep";


	int  pi_paths[7] = {2, 4, 6, 8, 10, 20, 30};
	int  pi_trees[7] = {10, 50, 100, 200, 300, 500, 1000};
	//int  pi_comp_time[7] = {7200, 7200, 7200, 9000, 10800, 12600, 14400};
	int  pi_comp_time[7] = {19600, 19600, 19600, 19600, 19600, 19600, 19600};
	//int  i_paths_trees_offset;
	//i_paths_trees_offset = 0;

	int  i_time_secs = pi_comp_time[iTreePathOffset];


	int  i_fluent_optimization = iCleverDecoding;
	int  i_max_iteration_number = 200000;
	
	
	CString  s_result_filename;

	s_result_filename.Format("sa_settings_%s_%d.txt", sLine, iTreePathOffset);


	CString  s_sett_filename;
	s_sett_filename = "z_" + s_result_filename;
	
	CString  s_serie_filename;
	s_serie_filename.Format("serie_%d.txt", iTreePathOffset);

	
	FILE  *pf_serie;
	pf_serie = fopen(s_serie_filename, "a");
	s_buf.Format("%s\\\\%d\n", s_sett_filename, iSettingsRunNumber);
	fprintf(pf_serie, s_buf);
	fclose(pf_serie);


	FILE  *pf_settings;
	pf_settings = fopen(s_sett_filename, "w+");
	if  (pf_settings  ==  NULL)  return;

	fprintf(pf_settings, ".\\%s\\\\file name\n", s_result_filename);
	fprintf(pf_settings, ".\\test_summarize_2_10.txt\\\\summarize\n");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\net\n", s_network);
	fprintf(pf_settings, "\\\\replic\n");
	if  (s_demands  ==  "")
		fprintf(pf_settings, "\\\\unicast\n", s_demands);
	else
		fprintf(pf_settings, ".\\..\\data\\%s\\\\unicast\n", s_demands);
	fprintf(pf_settings, "\\\\anycast\n");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\multicast\n", s_demands_multicast + ".dmc");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\paths\n", s_paths + ".pat");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unispec\n", s_paths + "1.spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unireg\n", s_paths + "1.regs");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\trees\n", s_demands_multicast + ".tree");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treespec\n", s_demands_multicast + ".spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treereg\n", s_demands_multicast + ".regs");

	fprintf(pf_settings, "%d\\\\paths to use\n", pi_paths[iTreePathOffset]);
	fprintf(pf_settings, "%d\\\\trees to use\n",  pi_trees[iTreePathOffset]);

	fprintf(pf_settings, ".\\..\\data\\u30.len\\\\modulation\n");

	if  (bStartSolution  ==  false)
		fprintf(pf_settings, "\\\\solution\n");
	else
		fprintf(pf_settings, ".\\..\\US26_init\\t=%d\\%s.max\\\\solution\n",pi_trees[iTreePathOffset], sLine);
	fprintf(pf_settings, "HighestSlot\\\\fit func\n");


	s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_ALGORITHM_SIMMULATED_ANNEALING, SYSTEM_PARAM_ALGORITHM);
	fprintf(pf_settings, s_buf);
	fprintf(pf_settings, "%d\\\\max time\n", i_time_secs);
	fprintf(pf_settings, "1800\\\\probing time\n");

	fprintf(pf_settings, "%d\\\\Solution constructon (0-normal; 1-fluent optimization)\n", i_fluent_optimization);

	fprintf(pf_settings, "%d\\\\%s\n", i_max_iteration_number, VGA_PARAM_SIMULATED_ANNEALING_MAX_ITER_NUMBER);
	

	fclose(pf_settings);
}//void  v_sim_annealing_generate_sett_file_from_line(CString  sLine, FILE  *pfSerie, int iTreePathOffset, int iSettingsRunNumber, int iCleverDecoding)





void  v_random_search_generate_sett_file_from_line(CString  sLine, FILE  *pfSerie, int iTreePathOffset, int iSettingsRunNumber, int iCleverDecoding)
{
	CString  s_buf;

	CString  s_network;
	s_network = sLine.GetAt(0);
	s_network += sLine.GetAt(1);
	s_network += ".net";

	CString  s_demands;
	s_demands = sLine.GetAt(2);
	s_demands += sLine.GetAt(3);

	if  (s_demands  ==  "xx")
		s_demands = "";
	else
		s_demands += ".dem";


	CString  s_demands_anycast;
	s_demands_anycast = sLine.GetAt(4);
	s_demands_anycast += sLine.GetAt(5);
	s_demands_anycast += ".dea";

	CString  s_demands_multicast;
	s_demands_multicast = sLine.GetAt(6);
	s_demands_multicast += sLine.GetAt(7);
	
	CString  s_paths;
	s_paths = sLine.GetAt(8);
	
	CString  s_replicas;
	s_replicas = sLine.GetAt(11);
	s_replicas += sLine.GetAt(12);
	s_replicas += ".rep";


	int  pi_paths[7] = {2, 4, 6, 8, 10, 20, 30};
	int  pi_trees[7] = {10, 50, 100, 200, 300, 500, 1000};
	//int  pi_comp_time[7] = {7200, 7200, 7200, 9000, 10800, 12600, 14400};
	int  pi_comp_time[7] = {19600, 19600, 19600, 19600, 19600, 19600, 19600};
	//int  i_paths_trees_offset;
	//i_paths_trees_offset = 0;

	int  i_time_secs = pi_comp_time[iTreePathOffset];


	int  i_fluent_optimization = iCleverDecoding;
	
	CString  s_result_filename;

	s_result_filename.Format("rs_settings_%s_%d.txt", sLine, iTreePathOffset);


	CString  s_sett_filename;
	s_sett_filename = "z_" + s_result_filename;
	
	CString  s_serie_filename;
	s_serie_filename.Format("serie_%d.txt", iTreePathOffset);

	FILE  *pf_serie;
	pf_serie = fopen(s_serie_filename, "a");
	s_buf.Format("%s\\\\%d\n", s_sett_filename, iSettingsRunNumber);
	fprintf(pf_serie, s_buf);
	fclose(pf_serie);


	FILE  *pf_settings;
	pf_settings = fopen(s_sett_filename, "w+");

	fprintf(pf_settings, ".\\%s\\\\file name\n", s_result_filename);
	fprintf(pf_settings, ".\\test_summarize_2_10.txt\\\\summarize\n");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\net\n", s_network);
	fprintf(pf_settings, "\\\\replic\n");
	if  (s_demands  ==  "")
		fprintf(pf_settings, "\\\\unicast\n", s_demands);
	else
		fprintf(pf_settings, ".\\..\\data\\%s\\\\unicast\n", s_demands);
	fprintf(pf_settings, "\\\\anycast\n");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\multicast\n", s_demands_multicast + ".dmc");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\paths\n", s_paths + ".pat");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unispec\n", s_paths + "1.spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unireg\n", s_paths + "1.regs");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\trees\n", s_demands_multicast + ".tree");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treespec\n", s_demands_multicast + ".spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treereg\n", s_demands_multicast + ".regs");

	fprintf(pf_settings, "%d\\\\paths to use\n", pi_paths[iTreePathOffset]);
	fprintf(pf_settings, "%d\\\\trees to use\n",  pi_trees[iTreePathOffset]);

	fprintf(pf_settings, ".\\data\\u30.len\\\\modulation\n");
	fprintf(pf_settings, "\\\\solution\n");
	fprintf(pf_settings, "HighestSlot\\\\fit func\n");


	s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_ALGORITHM_RANDOM_SEARCH, SYSTEM_PARAM_ALGORITHM);
	fprintf(pf_settings, s_buf);
	fprintf(pf_settings, "%d\\\\max time\n", i_time_secs);
	fprintf(pf_settings, "1800\\\\probing time\n");

	fprintf(pf_settings, "%d\\\\Solution constructon (0-normal; 1-fluent optimization)\n", i_fluent_optimization);
	

	fclose(pf_settings);
}//void  v_sga_generate_sett_file_from_line(CString  sLine)







void  v_sga_generate_sett_file_from_line(bool  bTuning, bool  bStartSolution, CString  sLine, FILE  *pfSerie, int iTreePathOffset, int iSgaPopSize, double  dCrossProb, double  dMutProb, int iSettingsRunNumber, int iCleverDecoding)
{
	CString  s_buf;

	CString  s_network;
	s_network = sLine.GetAt(0);
	s_network += sLine.GetAt(1);
	s_network += ".net";

	CString  s_demands;
	s_demands = sLine.GetAt(2);
	s_demands += sLine.GetAt(3);

	if  (s_demands  ==  "xx")
		s_demands = "";
	else
		s_demands += ".dem";


	CString  s_demands_anycast;
	s_demands_anycast = sLine.GetAt(4);
	s_demands_anycast += sLine.GetAt(5);
	s_demands_anycast += ".dea";

	CString  s_demands_multicast;
	s_demands_multicast = sLine.GetAt(6);
	s_demands_multicast += sLine.GetAt(7);
	
	CString  s_paths;
	s_paths = sLine.GetAt(8);
	
	CString  s_replicas;
	s_replicas = sLine.GetAt(11);
	s_replicas += sLine.GetAt(12);
	s_replicas += ".rep";


	int  pi_paths[7] = {2, 4, 6, 8, 10, 20, 30};
	int  pi_trees[7] = {10, 50, 100, 200, 300, 500, 1000};
	//int  pi_comp_time[7] = {7200, 7200, 7200, 9000, 10800, 12600, 14400};
	int  pi_comp_time[7] = {19600, 19600, 19600, 19600, 19600, 19600, 19600};
	//int  i_paths_trees_offset;
	//i_paths_trees_offset = 0;

	int  i_time_secs = pi_comp_time[iTreePathOffset];


	int  i_sga_pop_size;
	double  d_sga_cross, d_sga_mut;
	
	i_sga_pop_size = iSgaPopSize;
	d_sga_cross = dCrossProb;
	d_sga_mut = dMutProb;
	

	int  i_fluent_optimization = iCleverDecoding;
	
	
	CString  s_result_filename;

	if  (bTuning  ==  true)
		s_result_filename.Format("settings_%.4d_%s.txt", (int) (dMutProb * 100), sLine);
	else
		s_result_filename.Format("sga_settings_%s_%d.txt", sLine, iTreePathOffset);


	CString  s_sett_filename;
	s_sett_filename = "z_" + s_result_filename;
	
	CString  s_serie_filename;
	if  (bTuning  ==  true)
		s_serie_filename.Format("serie_%.4d.txt", (int) (dMutProb * 100));
	else
		s_serie_filename.Format("serie_%d.txt", iTreePathOffset);


	FILE  *pf_serie;
	pf_serie = fopen(s_serie_filename, "a");
	s_buf.Format("%s\\\\%d\n", s_sett_filename, iSettingsRunNumber);
	fprintf(pf_serie, s_buf);
	fclose(pf_serie);


	FILE  *pf_settings;
	pf_settings = fopen(s_sett_filename, "w+");

	fprintf(pf_settings, ".\\%s\\\\file name\n", s_result_filename);
	fprintf(pf_settings, ".\\test_summarize_2_10.txt\\\\summarize\n");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\net\n", s_network);
	fprintf(pf_settings, "\\\\replic\n");
	if  (s_demands  ==  "")
		fprintf(pf_settings, "\\\\unicast\n", s_demands);
	else
		fprintf(pf_settings, ".\\..\\data\\%s\\\\unicast\n", s_demands);
	fprintf(pf_settings, "\\\\anycast\n");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\multicast\n", s_demands_multicast + ".dmc");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\paths\n", s_paths + ".pat");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unispec\n", s_paths + "1.spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unireg\n", s_paths + "1.regs");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\trees\n", s_demands_multicast + ".tree");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treespec\n", s_demands_multicast + ".spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treereg\n", s_demands_multicast + ".regs");

	fprintf(pf_settings, "%d\\\\paths to use\n", pi_paths[iTreePathOffset]);
	fprintf(pf_settings, "%d\\\\trees to use\n",  pi_trees[iTreePathOffset]);

	fprintf(pf_settings, ".\\data\\u30.len\\\\modulation\n");

	if  (bStartSolution  ==  false)
		fprintf(pf_settings, "\\\\solution\n");
	else
		fprintf(pf_settings, ".\\..\\US26_init\\t=%d\\%s.max\\\\solution\n",pi_trees[iTreePathOffset], sLine);

	fprintf(pf_settings, "HighestSlot\\\\fit func\n");


	s_buf.Format("%s\\\\%s\n", SYSTEM_PARAM_ALGORITHM_SIMPLE_GA, SYSTEM_PARAM_ALGORITHM);
	fprintf(pf_settings, s_buf);
	fprintf(pf_settings, "%d\\\\max time\n", i_time_secs);
	fprintf(pf_settings, "1800\\\\probing time\n");
	




	s_buf.Format("%d\\\\simple GA popsize\n", i_sga_pop_size);
	fprintf(pf_settings, s_buf);
	
	s_buf.Format("%.8lf\\\\simple GA cross\n", d_sga_cross);
	fprintf(pf_settings, s_buf);
	s_buf.Format("%.8lf\\\\simple GA mut\n", d_sga_mut);
	fprintf(pf_settings, s_buf);
	fprintf(pf_settings, "%d\\\\Solution constructon (0-normal; 1-fluent optimization)\n", i_fluent_optimization);
	

	fclose(pf_settings);
}//void  v_sga_generate_sett_file_from_line(CString  sLine)




void  v_generate_test_from_list()
{
	//v_generate_tuning_for_line("ff000000f10201b048", NULL, 0);
	//return;

	CError  c_err;
	CString  s_buf;
	CString  s_list_file;

	s_list_file = ::Application::ExecutablePath;
	s_list_file.Replace("FiberFlowAssignment.exe", "");
	//s_list_file = "D:\\projekty_pwr\\FiberFlowAssignment\\release\\problem_list.txt";
	s_list_file += "\\problem_list.txt";



	FILE  *pf_generation_list;
	pf_generation_list = fopen(s_list_file, "r");
	
	if  (pf_generation_list  ==  NULL)
	{
		s_buf.Format("Cannot open (%s) for settings list generation", s_list_file);
		c_err.vPutError(s_buf);
		::MessageBox(NULL, "NOK", "NOK", MB_OK);
		return;
	}//if  (pf_generation_list  ==  NULL)

	//FILE  *pf_serie;
	//pf_serie = fopen("serie.txt", "w+");


	CString  s_line = "a";
	CString  s_optimal_value = "";
	double  d_optimal_value;
	
	int  i_sett_counter = 0;
	int  i_buf;

	int  i_setting_run_number = 2;

	while  ( (s_line  !=  "")&&(feof(pf_generation_list)  ==  false) )
	{
		s_line = Tools::sReadLine(pf_generation_list, &s_optimal_value);


		//sga-clever
		//v_sga_generate_sett_file_from_line(true, s_line, NULL, i_sett_counter, 8000, 0.6, 0.4, i_setting_run_number, 1);

		


		//sga-old
		//v_sga_generate_sett_file_from_line(true, s_line, NULL, i_sett_counter, 12000, 0.7, 0.4, i_setting_run_number, 0);	
		


		//muppets-clever
		//v_generate_tuning_for_line(true, false, s_line, NULL, i_sett_counter, 16, 100, 0.12, 0.15, 0.05, i_setting_run_number, 1);//chosen configuration
		
		

		//muppets-old
		//v_generate_tuning_for_line(true, s_line, NULL, i_sett_counter, 64, 150, 0.18, 0.07, 0.1, i_setting_run_number, 0);//chosen configuration


		//v_random_search_generate_sett_file_from_line(s_line, NULL, i_sett_counter, i_setting_run_number, 1);
		//v_random_search_generate_sett_file_from_line(s_line, NULL, i_sett_counter, i_setting_run_number, 0);


		/*v_generate_tuning_for_line(false, s_line, NULL, i_sett_counter, 64, 100, 0.12, 0.1, 0.05, i_setting_run_number, 0);


		





		//v_generate_tuning_for_line(s_line, NULL, i_sett_counter, 2, 70, 0.12, 0.1, 0.02, i_setting_run_number, 1);
		//v_generate_tuning_for_line(s_line, NULL, i_sett_counter, 2, 70, 0.12, 0.1, 0.035, i_setting_run_number, 1);
		//v_generate_tuning_for_line(s_line, NULL, i_sett_counter, 2, 70, 0.12, 0.1, 0.07, i_setting_run_number, 1);
		//v_generate_tuning_for_line(s_line, NULL, i_sett_counter, 2, 70, 0.12, 0.1, 0.1, i_setting_run_number, 1);


		//v_generate_tuning_for_line(s_line, NULL, i_sett_counter, 64, 100, 0.12, 0.1, 0.02, i_setting_run_number, 0);
		//v_generate_tuning_for_line(s_line, NULL, i_sett_counter, 64, 100, 0.12, 0.1, 0.035, i_setting_run_number, 0);
		//v_generate_tuning_for_line(s_line, NULL, i_sett_counter, 64, 100, 0.12, 0.1, 0.07, i_setting_run_number, 0);
		//v_generate_tuning_for_line(s_line, NULL, i_sett_counter, 64, 100, 0.12, 0.1, 0.1, i_setting_run_number, 0);


		
		
		
		//i_setting_run_number++;//*/

		i_sett_counter++;
	}//while  ( (s_line  !=  "")&&(feof(pf_generation_list)  ==  false) )




	fclose(pf_generation_list);
	//fclose(pf_serie);
	
	::MessageBox(NULL, "ok", "ok", MB_OK);
}//void  v_generate_test_from_list()





bool  b_get_testcase
	(
	CString  sLine,  
	CString  *psNetwork, 
	CString  *psPrimaryPat, CString  *psBackPat, CString  *psPrimarySpec, CString  *psBackSpec,
	CString  *psDemUni,
	CString  *psDemAny,
	CString  *psRep,
	CString  *psCore, CString  *psPaths
	)
{
	if  (sLine.GetLength()  <  12)  return(false);

	*psNetwork = sLine.GetAt(0);
	*psNetwork += sLine.GetAt(1);


	*psPrimaryPat = *psNetwork;
	*psBackPat = *psNetwork;

	*psPrimarySpec = psNetwork->GetAt(0);
	*psBackSpec = psNetwork->GetAt(0);


	*psNetwork += ".net";

	*psPrimaryPat += "30_p.pat";
	*psBackPat += "30_b.pat";

	*psPrimarySpec +=  "_p.spec";
	*psBackSpec +=  "_b.spec";



	//other
	*psDemUni = sLine.GetAt(2);
	*psDemUni += sLine.GetAt(3);

	*psDemAny = sLine.GetAt(4);
	*psDemAny += sLine.GetAt(5);

	*psRep = sLine.GetAt(6);
	*psRep += sLine.GetAt(7);

	*psCore = sLine.GetAt(8);
	*psCore += sLine.GetAt(9);


	*psPaths = sLine.GetAt(10);
	*psPaths += sLine.GetAt(11);

	*psDemUni +=  ".dem";
	*psDemAny +=  ".dea";
	*psRep +=  ".rep";


	return(true);
}//CString  b_get_testcase(CString  sLine)






void  v_generate_eon_cores_tabu_old(CString  sDataFolder, int  iTimeSec, bool  bTuning, CString  sLine, FILE  *pfSerie, double  dProRep, double  dProPath, double  dTabuPerc, double  dNoImprovementLimit, double  dWorseningFactor, int iSettingsRunNumber, int iCleverDecoding)
{
	CString  s_buf;

	CString  s_network;
	CString  s_primary_pat, s_back_pat, s_primary_spec, s_back_spec;

	CString  s_dem_uni;
	CString  s_dem_any;
	CString  s_rep;
	CString  s_core, s_paths;


	//::Tools::vShow("'" + sLine + "'");


	if  (
		b_get_testcase
			(
			sLine, 
			&s_network,
			&s_primary_pat, &s_back_pat, &s_primary_spec, &s_back_spec,

			&s_dem_uni,	&s_dem_any,
			&s_rep, &s_core, &s_paths
			) 
			==  false
		)
		return;

	//::Tools::vShow(2.1);


	//double  dProRep, double  dProPath, double  dTabuPerc, double  dNoImprovementLimit, double  dWorseningFactor
	/*9999\\probing time
	1\\Solution constructon (0-normal; 1-fluent optimization)
	0.05\\ProRep
	0.05\\ProPath
	2.0\\TabuListLimitPercent
	0.5\\NoImprovementLimitPercent
	0.6\\WorseningFactor*/



	
	CString  s_result_filename;

	if  (bTuning  ==  true)
		s_result_filename.Format("settings_%.4d_%s.txt", (int) (dProRep * 100), sLine);
	else
		s_result_filename.Format("mup_settings_%s.txt", sLine);


	CString  s_sett_filename;
	s_sett_filename = "z_" + s_result_filename;
	
	CString  s_serie_filename;

	if  (bTuning  ==  true)
		s_serie_filename.Format("serie_%.4d.txt", (int) (dProRep * 100));
	else
		s_serie_filename = "serie.txt";


	//::Tools::vShow(2.2);
	//::Tools::vShow(s_serie_filename);

	FILE  *pf_serie;
	pf_serie = fopen(s_serie_filename, "a");
	s_buf.Format("%s\\\\%d\n", s_sett_filename, iSettingsRunNumber);
	fprintf(pf_serie, s_buf);
	fclose(pf_serie);

	//::Tools::vShow(2.3);

	FILE  *pf_settings;
	pf_settings = fopen(s_sett_filename, "w+");

	//::Tools::vShow(2.4);

	fprintf(pf_settings, ".\\%s\\\\file name\n", s_result_filename);
	fprintf(pf_settings, ".\\test_summarize_2_10.txt\\\\summarize\n");
	fprintf(pf_settings, ".\\%s\\%s\\\\net\n", sDataFolder, s_network);
	fprintf(pf_settings, ".\\%s\\%s\\\\replic\n", sDataFolder, s_rep);


	if  (s_dem_uni  ==  "")
		fprintf(pf_settings, "\\\\unicast\n");
	else
		fprintf(pf_settings, ".\\%s\\%s\\\\unicast\n", sDataFolder, s_dem_uni);

	fprintf(pf_settings, "\\\\anycast\n");
	fprintf(pf_settings, "\\\\multicast\n");
	
	if  (s_dem_any  ==  "")
		fprintf(pf_settings, "\\\\anycast1dir\n");
	else
		fprintf(pf_settings, ".\\%s\\%s\\\\anycast1dir\n", sDataFolder, s_dem_any);

	fprintf(pf_settings, "50\\\\transfer multiplier\n");

	fprintf(pf_settings, ".\\%s\\%s\\\\paths\n", sDataFolder, s_primary_pat);
	fprintf(pf_settings, ".\\%s\\%s\\\\paths_backup\n", sDataFolder, s_back_pat);

	fprintf(pf_settings, ".\\%s\\%s\\\\unispec\n", sDataFolder, s_primary_spec);
	fprintf(pf_settings, ".\\%s\\%s\\\\unispec_backup\n", sDataFolder, s_back_spec);


	fprintf(pf_settings, "\\\\unireg\n");
	fprintf(pf_settings, "\\\\trees\n");
	fprintf(pf_settings, "\\\\treespec\n");
	fprintf(pf_settings, "\\\\treereg\n");


	fprintf(pf_settings, "%s\\\\paths to use\n",s_paths);
	fprintf(pf_settings, "0\\\\trees to use\n");

	fprintf(pf_settings, "%s\\\\cores\n",s_core);


	
	fprintf(pf_settings, "\\\\modulation\n");
	fprintf(pf_settings, "\\\\solution\n");




	fprintf(pf_settings, "HighestSlot\\\\fit func\n");
	fprintf(pf_settings, "tabu\\\\algorithm\n");


	fprintf(pf_settings, "%d\\\\max time\n", iTimeSec);
	fprintf(pf_settings, "9999\\\\probing time\n");

	fprintf(pf_settings, "%d\\\\Solution constructon (0-normal; 1-fluent optimization)\n", iCleverDecoding);

	//double  dProRep, double  dProPath, double  dTabuPerc, double  dNoImprovementLimit, double  dWorseningFactor
	/*1\\Solution constructon (0-normal; 1-fluent optimization)
	0.05\\ProRep
	0.05\\ProPath
	2.0\\TabuListLimitPercent
	0.5\\NoImprovementLimitPercent
	0.6\\WorseningFactor*/

	fprintf(pf_settings, "%.8lf\\\\ProRep\n", dProRep);
	fprintf(pf_settings, "%.8lf\\\\ProPath\n", dProPath);
	fprintf(pf_settings, "%.8lf\\\\TabuListLimitPercent\n", dTabuPerc);
	fprintf(pf_settings, "%.8lf\\\\NoImprovementLimitPercent\n", dNoImprovementLimit);
	fprintf(pf_settings, "%.8lf\\\\WorseningFactor\n", dWorseningFactor);

		
	/*if  (s_demands  ==  "")
		fprintf(pf_settings, "\\\\unicast\n", s_demands);
	else
		fprintf(pf_settings, ".\\..\\data\\%s\\\\unicast\n", s_demands);
	fprintf(pf_settings, "\\\\anycast\n");
	fprintf(pf_settings, ".\\%s\\%s\\\\multicast\n", sDataFolder, s_demands_multicast + ".dmc");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\paths\n", s_paths + ".pat");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unispec\n", s_paths + "1.spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unireg\n", s_paths + "1.regs");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\trees\n", s_demands_multicast + ".tree");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treespec\n", s_demands_multicast + ".spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treereg\n", s_demands_multicast + ".regs");

	fprintf(pf_settings, "%d\\\\paths to use\n", pi_paths[iTreePathOffset]);
	fprintf(pf_settings, "%d\\\\trees to use\n",  pi_trees[iTreePathOffset]);

	fprintf(pf_settings, ".\\data\\u30.len\\\\modulation\n");

	fprintf(pf_settings, "\\\\solution\n");

	fprintf(pf_settings, "HighestSlot\\\\fit func\n");
	fprintf(pf_settings, "muppets\\\\algorithm\n");

	fprintf(pf_settings, "%d\\\\max time\n", iTimeSec);
	//fprintf(pf_settings, "1800\\\\probing time\n");

	fprintf(pf_settings, "7000\\\\pattern pool size\n");
	fprintf(pf_settings, "0\\\\the same pattern check\n");
	fprintf(pf_settings, "3\\\\minimal pattern length\n");

	fprintf(pf_settings, "%d\\\\virus generations\n", i_virus_generations);
	fprintf(pf_settings, "%d\\\\population size\n", i_pop_size);
	fprintf(pf_settings, "%.8lf\\\\virus prob cut\n", d_virus_prob_cut);
	fprintf(pf_settings, "%.8lf\\\\virus prob splice\n", d_virus_prob_splice);
	fprintf(pf_settings, "%.8lf\\\\virus prob mutation\n", d_virus_prob_mutation);
	fprintf(pf_settings, "%.8lf\\\\virus prob rem gene\n", d_virus_prob_mutation);
	fprintf(pf_settings, "%.8lf\\\\virus prob add gene\n", d_virus_prob_mutation);
	
	fprintf(pf_settings, "100\\\\virginity rounds\n");
	fprintf(pf_settings, "0\\\\ct add remove strategy (0-classic; 1-new)\n");
	fprintf(pf_settings, "0\\\\Bayesian effect (0-none; 1-brutal; 2-clever)\n");
	fprintf(pf_settings, "%d\\\\Solution constructon (0-normal; 1-fluent optimization)\n", i_fluent_optimization);*/


	fclose(pf_settings);
}//void  v_generate_tuning_for_line(CString  sLine, FILE  *pfSerie)





void  v_generate_eon_cores_tabu(CString  sDataFolder, int  iTimeSec, int iTimeSwitchSec, bool  bTuning, CString  sLine, FILE  *pfSerie, double  dProRep, double dProPath, double dTabuListLimitPercent,  double  dNoImprovementLimitPercent, double  dWorseningFactor, int iSettingsRunNumber, int iCleverDecoding)
{
	CString  s_buf;

	CString  s_network;
	CString  s_primary_pat, s_back_pat, s_primary_spec, s_back_spec;

	CString  s_dem_uni;
	CString  s_dem_any;
	CString  s_rep;
	CString  s_core, s_paths;


	//::Tools::vShow("'" + sLine + "'");


	if  (
		b_get_testcase
			(
			sLine, 
			&s_network,
			&s_primary_pat, &s_back_pat, &s_primary_spec, &s_back_spec,

			&s_dem_uni,	&s_dem_any,
			&s_rep, &s_core, &s_paths
			) 
			==  false
		)
		return;

	//::Tools::vShow(2.1);



	CString  s_result_filename;

	if  (bTuning  ==  true)
		s_result_filename.Format("settings_%.4d_%s.txt", (int) (dProRep * 100), sLine);
	else
		s_result_filename.Format("tabu_settings_%s.txt", sLine);


	CString  s_sett_filename;
	s_sett_filename = "z_" + s_result_filename;
	
	CString  s_serie_filename;

	if  (bTuning  ==  true)
		s_serie_filename.Format("serie_%.4d.txt", (int) (dProRep * 100));
	else
		s_serie_filename = "serie.txt";


	//::Tools::vShow(2.2);
	//::Tools::vShow(s_serie_filename);

	FILE  *pf_serie;
	pf_serie = fopen(s_serie_filename, "a");
	s_buf.Format("%s\\\\%d\n", s_sett_filename, iSettingsRunNumber);
	fprintf(pf_serie, s_buf);
	fclose(pf_serie);

	//::Tools::vShow(2.3);

	FILE  *pf_settings;
	pf_settings = fopen(s_sett_filename, "w+");

	//::Tools::vShow(2.4);

	fprintf(pf_settings, ".\\%s\\\\file name\n", s_result_filename);
	fprintf(pf_settings, ".\\test_summarize_2_10.txt\\\\summarize\n");
	fprintf(pf_settings, ".\\%s\\%s\\\\net\n", sDataFolder, s_network);
	fprintf(pf_settings, ".\\%s\\%s\\\\replic\n", sDataFolder, s_rep);


	if  (s_dem_uni  ==  "")
		fprintf(pf_settings, "\\\\unicast\n");
	else
		fprintf(pf_settings, ".\\%s\\%s\\\\unicast\n", sDataFolder, s_dem_uni);

	fprintf(pf_settings, "\\\\anycast\n");
	fprintf(pf_settings, "\\\\multicast\n");
	
	if  (s_dem_any  ==  "")
		fprintf(pf_settings, "\\\\anycast1dir\n");
	else
		fprintf(pf_settings, ".\\%s\\%s\\\\anycast1dir\n", sDataFolder, s_dem_any);

	fprintf(pf_settings, "50\\\\transfer multiplier\n");

	fprintf(pf_settings, ".\\%s\\%s\\\\paths\n", sDataFolder, s_primary_pat);
	fprintf(pf_settings, ".\\%s\\%s\\\\paths_backup\n", sDataFolder, s_back_pat);

	fprintf(pf_settings, ".\\%s\\%s\\\\unispec\n", sDataFolder, s_primary_spec);
	fprintf(pf_settings, ".\\%s\\%s\\\\unispec_backup\n", sDataFolder, s_back_spec);


	fprintf(pf_settings, "\\\\unireg\n");
	fprintf(pf_settings, "\\\\trees\n");
	fprintf(pf_settings, "\\\\treespec\n");
	fprintf(pf_settings, "\\\\treereg\n");


	fprintf(pf_settings, "%s\\\\paths to use\n",s_paths);
	fprintf(pf_settings, "0\\\\trees to use\n");

	fprintf(pf_settings, "%s\\\\cores\n",s_core);


	
	fprintf(pf_settings, "\\\\modulation\n");
	fprintf(pf_settings, "\\\\solution\n");




	fprintf(pf_settings, "HighestSlot\\\\fit func\n");
	fprintf(pf_settings, "tabu\\\\algorithm\n");


	fprintf(pf_settings, "%d\\\\max time\n", iTimeSec);
	fprintf(pf_settings, "%d\\\\switch time\n", iTimeSwitchSec);	
	fprintf(pf_settings, "8889999\\\\probing time\n");
	fprintf(pf_settings, "%d\\\\Solution constructon (0-normal; 1-fluent optimization)\n", iCleverDecoding);

	//dProRep, double dProPath, double dTabuListLimitPercent,  double  dNoImprovementLimitPercent, double  dWorseningFactor
	fprintf(pf_settings, "%.8lf\\\\ProRep\n", dProRep);
	fprintf(pf_settings, "%.8lf\\\\ProPath\n", dProPath);
	fprintf(pf_settings, "%.8lf\\\\TabuListLimitPercent\n", dTabuListLimitPercent);
	fprintf(pf_settings, "%.8lf\\\\NoImprovementLimitPercent\n", dNoImprovementLimitPercent);
	fprintf(pf_settings, "%.8lf\\\\WorseningFactor\n", dWorseningFactor);
	

		
	/*if  (s_demands  ==  "")
		fprintf(pf_settings, "\\\\unicast\n", s_demands);
	else
		fprintf(pf_settings, ".\\..\\data\\%s\\\\unicast\n", s_demands);
	fprintf(pf_settings, "\\\\anycast\n");
	fprintf(pf_settings, ".\\%s\\%s\\\\multicast\n", sDataFolder, s_demands_multicast + ".dmc");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\paths\n", s_paths + ".pat");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unispec\n", s_paths + "1.spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unireg\n", s_paths + "1.regs");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\trees\n", s_demands_multicast + ".tree");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treespec\n", s_demands_multicast + ".spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treereg\n", s_demands_multicast + ".regs");

	fprintf(pf_settings, "%d\\\\paths to use\n", pi_paths[iTreePathOffset]);
	fprintf(pf_settings, "%d\\\\trees to use\n",  pi_trees[iTreePathOffset]);

	fprintf(pf_settings, ".\\data\\u30.len\\\\modulation\n");

	fprintf(pf_settings, "\\\\solution\n");

	fprintf(pf_settings, "HighestSlot\\\\fit func\n");
	fprintf(pf_settings, "muppets\\\\algorithm\n");

	fprintf(pf_settings, "%d\\\\max time\n", iTimeSec);
	//fprintf(pf_settings, "1800\\\\probing time\n");

	fprintf(pf_settings, "7000\\\\pattern pool size\n");
	fprintf(pf_settings, "0\\\\the same pattern check\n");
	fprintf(pf_settings, "3\\\\minimal pattern length\n");

	fprintf(pf_settings, "%d\\\\virus generations\n", i_virus_generations);
	fprintf(pf_settings, "%d\\\\population size\n", i_pop_size);
	fprintf(pf_settings, "%.8lf\\\\virus prob cut\n", d_virus_prob_cut);
	fprintf(pf_settings, "%.8lf\\\\virus prob splice\n", d_virus_prob_splice);
	fprintf(pf_settings, "%.8lf\\\\virus prob mutation\n", d_virus_prob_mutation);
	fprintf(pf_settings, "%.8lf\\\\virus prob rem gene\n", d_virus_prob_mutation);
	fprintf(pf_settings, "%.8lf\\\\virus prob add gene\n", d_virus_prob_mutation);
	
	fprintf(pf_settings, "100\\\\virginity rounds\n");
	fprintf(pf_settings, "0\\\\ct add remove strategy (0-classic; 1-new)\n");
	fprintf(pf_settings, "0\\\\Bayesian effect (0-none; 1-brutal; 2-clever)\n");
	fprintf(pf_settings, "%d\\\\Solution constructon (0-normal; 1-fluent optimization)\n", i_fluent_optimization);*/


	fclose(pf_settings);
}//void  v_generate_tuning_for_line(CString  sLine, FILE  *pfSerie)

















void  v_generate_eon_cores_grasp
  (
  CString  sDataFolder, int  iTimeSec, int iTimeSwitchSec, bool  bTuning, CString  sLine, FILE  *pfSerie, 
  
  long lRestrictedCandidateListSize, double dRestrictedCandidateListSizeFactor, int iValueBasedCandidateListSize, long  lAcceptableDistanceFromOptimal, long lLocalSearchMaxIterations,
  double  dLocalSearchCoolingRateFactor, double dLocalSearchNrOfDemandsToSwapFactor, long lLocalSearchNrOfDemandsToSwap,
  double  dLocalSearchInitialTemperature,  double dLocalSearchInitialDistanceFromOptimal,  double dLocalSearchInitialProbabilityOfAcceptingWorseSolution,
  
  int iSettingsRunNumber, int iCleverDecoding
  )
{
	CString  s_buf;

	CString  s_network;
	CString  s_primary_pat, s_back_pat, s_primary_spec, s_back_spec;

	CString  s_dem_uni;
	CString  s_dem_any;
	CString  s_rep;
	CString  s_core, s_paths;


	//::Tools::vShow("'" + sLine + "'");


	if  (
		b_get_testcase
			(
			sLine, 
			&s_network,
			&s_primary_pat, &s_back_pat, &s_primary_spec, &s_back_spec,

			&s_dem_uni,	&s_dem_any,
			&s_rep, &s_core, &s_paths
			) 
			==  false
		)
		return;

	//::Tools::vShow(2.1);



	CString  s_result_filename;

	if  (bTuning  ==  true)
		s_result_filename.Format("settings_%.4d_%s.txt", (int) (dRestrictedCandidateListSizeFactor * 100), sLine);
	else
		s_result_filename.Format("grasp_settings_%s.txt", sLine);


	CString  s_sett_filename;
	s_sett_filename = "z_" + s_result_filename;
	
	CString  s_serie_filename;

	if  (bTuning  ==  true)
		s_serie_filename.Format("serie_%.4d.txt", (int) (dRestrictedCandidateListSizeFactor * 100));
	else
		s_serie_filename = "serie.txt";


	//::Tools::vShow(2.2);
	//::Tools::vShow(s_serie_filename);

	FILE  *pf_serie;
	pf_serie = fopen(s_serie_filename, "a");
	s_buf.Format("%s\\\\%d\n", s_sett_filename, iSettingsRunNumber);
	fprintf(pf_serie, s_buf);
	fclose(pf_serie);

	//::Tools::vShow(2.3);

	FILE  *pf_settings;
	pf_settings = fopen(s_sett_filename, "w+");

	//::Tools::vShow(2.4);

	fprintf(pf_settings, ".\\%s\\\\file name\n", s_result_filename);
	fprintf(pf_settings, ".\\test_summarize_2_10.txt\\\\summarize\n");
	fprintf(pf_settings, ".\\%s\\%s\\\\net\n", sDataFolder, s_network);
	fprintf(pf_settings, ".\\%s\\%s\\\\replic\n", sDataFolder, s_rep);


	if  (s_dem_uni  ==  "")
		fprintf(pf_settings, "\\\\unicast\n");
	else
		fprintf(pf_settings, ".\\%s\\%s\\\\unicast\n", sDataFolder, s_dem_uni);

	fprintf(pf_settings, "\\\\anycast\n");
	fprintf(pf_settings, "\\\\multicast\n");
	
	if  (s_dem_any  ==  "")
		fprintf(pf_settings, "\\\\anycast1dir\n");
	else
		fprintf(pf_settings, ".\\%s\\%s\\\\anycast1dir\n", sDataFolder, s_dem_any);

	fprintf(pf_settings, "50\\\\transfer multiplier\n");

	fprintf(pf_settings, ".\\%s\\%s\\\\paths\n", sDataFolder, s_primary_pat);
	fprintf(pf_settings, ".\\%s\\%s\\\\paths_backup\n", sDataFolder, s_back_pat);

	fprintf(pf_settings, ".\\%s\\%s\\\\unispec\n", sDataFolder, s_primary_spec);
	fprintf(pf_settings, ".\\%s\\%s\\\\unispec_backup\n", sDataFolder, s_back_spec);


	fprintf(pf_settings, "\\\\unireg\n");
	fprintf(pf_settings, "\\\\trees\n");
	fprintf(pf_settings, "\\\\treespec\n");
	fprintf(pf_settings, "\\\\treereg\n");


	fprintf(pf_settings, "%s\\\\paths to use\n",s_paths);
	fprintf(pf_settings, "0\\\\trees to use\n");

	fprintf(pf_settings, "%s\\\\cores\n",s_core);


	
	fprintf(pf_settings, "\\\\modulation\n");
	fprintf(pf_settings, "\\\\solution\n");




	fprintf(pf_settings, "HighestSlot\\\\fit func\n");
	fprintf(pf_settings, "grasp\\\\algorithm\n");


	fprintf(pf_settings, "%d\\\\max time\n", iTimeSec);
	fprintf(pf_settings, "%d\\\\switch time\n", iTimeSwitchSec);	
	fprintf(pf_settings, "8889999\\\\probing time\n");
	fprintf(pf_settings, "%d\\\\Solution constructon (0-normal; 1-fluent optimization)\n", iCleverDecoding);

	//dProRep, double dProPath, double dTabuListLimitPercent,  double  dNoImprovementLimitPercent, double  dWorseningFactor
	fprintf(pf_settings, "%d\\\\lRestrictedCandidateListSize\n", lRestrictedCandidateListSize);
	fprintf(pf_settings, "%.8lf\\\\dRestrictedCandidateListSizeFactor\n", dRestrictedCandidateListSizeFactor);
	fprintf(pf_settings, "%d\\\\iValueBasedCandidateListSize\n", iValueBasedCandidateListSize);
	fprintf(pf_settings, "%d\\\\lAcceptableDistanceFromOptimal\n", lAcceptableDistanceFromOptimal);
	fprintf(pf_settings, "%d\\\\lLocalSearchMaxIterations\n", lLocalSearchMaxIterations);
	fprintf(pf_settings, "%.8lf\\\\dLocalSearchCoolingRateFactor\n", dLocalSearchCoolingRateFactor);
	fprintf(pf_settings, "%.8lf\\\\dLocalSearchNrOfDemandsToSwapFactor\n", dLocalSearchNrOfDemandsToSwapFactor);
	fprintf(pf_settings, "%d\\\\lLocalSearchNrOfDemandsToSwap\n", lLocalSearchNrOfDemandsToSwap);
	fprintf(pf_settings, "%.8lf\\\\dLocalSearchInitialTemperature\n", dLocalSearchInitialTemperature);
	fprintf(pf_settings, "%.8lf\\\\dLocalSearchInitialDistanceFromOptimal\n", dLocalSearchInitialDistanceFromOptimal);
	fprintf(pf_settings, "%.8lf\\\\dLocalSearchInitialProbabilityOfAcceptingWorseSolution\n", dLocalSearchInitialProbabilityOfAcceptingWorseSolution);

		
	/*if  (s_demands  ==  "")
		fprintf(pf_settings, "\\\\unicast\n", s_demands);
	else
		fprintf(pf_settings, ".\\..\\data\\%s\\\\unicast\n", s_demands);
	fprintf(pf_settings, "\\\\anycast\n");
	fprintf(pf_settings, ".\\%s\\%s\\\\multicast\n", sDataFolder, s_demands_multicast + ".dmc");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\paths\n", s_paths + ".pat");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unispec\n", s_paths + "1.spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unireg\n", s_paths + "1.regs");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\trees\n", s_demands_multicast + ".tree");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treespec\n", s_demands_multicast + ".spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treereg\n", s_demands_multicast + ".regs");

	fprintf(pf_settings, "%d\\\\paths to use\n", pi_paths[iTreePathOffset]);
	fprintf(pf_settings, "%d\\\\trees to use\n",  pi_trees[iTreePathOffset]);

	fprintf(pf_settings, ".\\data\\u30.len\\\\modulation\n");

	fprintf(pf_settings, "\\\\solution\n");

	fprintf(pf_settings, "HighestSlot\\\\fit func\n");
	fprintf(pf_settings, "muppets\\\\algorithm\n");

	fprintf(pf_settings, "%d\\\\max time\n", iTimeSec);
	//fprintf(pf_settings, "1800\\\\probing time\n");

	fprintf(pf_settings, "7000\\\\pattern pool size\n");
	fprintf(pf_settings, "0\\\\the same pattern check\n");
	fprintf(pf_settings, "3\\\\minimal pattern length\n");

	fprintf(pf_settings, "%d\\\\virus generations\n", i_virus_generations);
	fprintf(pf_settings, "%d\\\\population size\n", i_pop_size);
	fprintf(pf_settings, "%.8lf\\\\virus prob cut\n", d_virus_prob_cut);
	fprintf(pf_settings, "%.8lf\\\\virus prob splice\n", d_virus_prob_splice);
	fprintf(pf_settings, "%.8lf\\\\virus prob mutation\n", d_virus_prob_mutation);
	fprintf(pf_settings, "%.8lf\\\\virus prob rem gene\n", d_virus_prob_mutation);
	fprintf(pf_settings, "%.8lf\\\\virus prob add gene\n", d_virus_prob_mutation);
	
	fprintf(pf_settings, "100\\\\virginity rounds\n");
	fprintf(pf_settings, "0\\\\ct add remove strategy (0-classic; 1-new)\n");
	fprintf(pf_settings, "0\\\\Bayesian effect (0-none; 1-brutal; 2-clever)\n");
	fprintf(pf_settings, "%d\\\\Solution constructon (0-normal; 1-fluent optimization)\n", i_fluent_optimization);*/


	fclose(pf_settings);
}//void  v_generate_eon_cores_grasp(CString  sDataFolder, int  iTimeSec, int iTimeSwitchSec, bool  bTuning, CString  sLine, FILE  *pfSerie, double  dProRep, double dProPath, double dTabuListLimitPercent,  double  dNoImprovementLimitPercent, double  dWorseningFactor, int iSettingsRunNumber, int iCleverDecoding)













void  v_generate_eon_cores(CString  sDataFolder, int  iTimeSec, int iTimeSwitchSec, bool  bTuning, CString  sLine, FILE  *pfSerie, int iGenNum, int iPopSize, double  dCutProb, double  dSpliceProb, double  dMutationProb, int iSettingsRunNumber, int iCleverDecoding)
{
	CString  s_buf;

	CString  s_network;
	CString  s_primary_pat, s_back_pat, s_primary_spec, s_back_spec;

	CString  s_dem_uni;
	CString  s_dem_any;
	CString  s_rep;
	CString  s_core, s_paths;


	//::Tools::vShow("'" + sLine + "'");


	if  (
		b_get_testcase
			(
			sLine, 
			&s_network,
			&s_primary_pat, &s_back_pat, &s_primary_spec, &s_back_spec,

			&s_dem_uni,	&s_dem_any,
			&s_rep, &s_core, &s_paths
			) 
			==  false
		)
		return;

	//::Tools::vShow(2.1);



	int  i_virus_generations = iGenNum;
	int  i_pop_size = iPopSize;
	double  d_virus_prob_cut = dCutProb;
	double  d_virus_prob_splice = dSpliceProb;
	double  d_virus_prob_mutation = dMutationProb;

	int  i_fluent_optimization = iCleverDecoding;
	
	
	CString  s_result_filename;

	if  (bTuning  ==  true)
		s_result_filename.Format("settings_%.4d_%s.txt", (int) (dMutationProb * 100), sLine);
	else
		s_result_filename.Format("mup_settings_%s.txt", sLine);


	CString  s_sett_filename;
	s_sett_filename = "z_" + s_result_filename;
	
	CString  s_serie_filename;

	if  (bTuning  ==  true)
		s_serie_filename.Format("serie_%.4d.txt", (int) (dMutationProb * 100));
	else
		s_serie_filename = "serie.txt";


	//::Tools::vShow(2.2);
	//::Tools::vShow(s_serie_filename);

	FILE  *pf_serie;
	pf_serie = fopen(s_serie_filename, "a");
	s_buf.Format("%s\\\\%d\n", s_sett_filename, iSettingsRunNumber);
	fprintf(pf_serie, s_buf);
	fclose(pf_serie);

	//::Tools::vShow(2.3);

	FILE  *pf_settings;
	pf_settings = fopen(s_sett_filename, "w+");

	//::Tools::vShow(2.4);

	fprintf(pf_settings, ".\\%s\\\\file name\n", s_result_filename);
	fprintf(pf_settings, ".\\test_summarize_2_10.txt\\\\summarize\n");
	fprintf(pf_settings, ".\\%s\\%s\\\\net\n", sDataFolder, s_network);
	fprintf(pf_settings, ".\\%s\\%s\\\\replic\n", sDataFolder, s_rep);


	if  (s_dem_uni  ==  "")
		fprintf(pf_settings, "\\\\unicast\n");
	else
		fprintf(pf_settings, ".\\%s\\%s\\\\unicast\n", sDataFolder, s_dem_uni);

	fprintf(pf_settings, "\\\\anycast\n");
	fprintf(pf_settings, "\\\\multicast\n");
	
	if  (s_dem_any  ==  "")
		fprintf(pf_settings, "\\\\anycast1dir\n");
	else
		fprintf(pf_settings, ".\\%s\\%s\\\\anycast1dir\n", sDataFolder, s_dem_any);

	fprintf(pf_settings, "50\\\\transfer multiplier\n");

	fprintf(pf_settings, ".\\%s\\%s\\\\paths\n", sDataFolder, s_primary_pat);
	fprintf(pf_settings, ".\\%s\\%s\\\\paths_backup\n", sDataFolder, s_back_pat);

	fprintf(pf_settings, ".\\%s\\%s\\\\unispec\n", sDataFolder, s_primary_spec);
	fprintf(pf_settings, ".\\%s\\%s\\\\unispec_backup\n", sDataFolder, s_back_spec);


	fprintf(pf_settings, "\\\\unireg\n");
	fprintf(pf_settings, "\\\\trees\n");
	fprintf(pf_settings, "\\\\treespec\n");
	fprintf(pf_settings, "\\\\treereg\n");


	fprintf(pf_settings, "%s\\\\paths to use\n",s_paths);
	fprintf(pf_settings, "0\\\\trees to use\n");

	fprintf(pf_settings, "%s\\\\cores\n",s_core);


	
	fprintf(pf_settings, "\\\\modulation\n");
	fprintf(pf_settings, "\\\\solution\n");




	fprintf(pf_settings, "HighestSlot\\\\fit func\n");
	fprintf(pf_settings, "muppets\\\\algorithm\n");


	fprintf(pf_settings, "%d\\\\max time\n", iTimeSec);
	fprintf(pf_settings, "%d\\\\switch time\n", iTimeSwitchSec);	
	fprintf(pf_settings, "8889999\\\\probing time\n");

	fprintf(pf_settings, "7000\\\\pattern pool size\n");
	fprintf(pf_settings, "0\\\\the same pattern check\n");
	fprintf(pf_settings, "3\\\\minimal pattern length\n");

	fprintf(pf_settings, "%d\\\\virus generations\n", i_virus_generations);
	fprintf(pf_settings, "%d\\\\population size\n", i_pop_size);
	fprintf(pf_settings, "%.8lf\\\\virus prob cut\n", d_virus_prob_cut);
	fprintf(pf_settings, "%.8lf\\\\virus prob splice\n", d_virus_prob_splice);
	fprintf(pf_settings, "%.8lf\\\\virus prob mutation\n", d_virus_prob_mutation);
	fprintf(pf_settings, "%.8lf\\\\virus prob rem gene\n", d_virus_prob_mutation);
	fprintf(pf_settings, "%.8lf\\\\virus prob add gene\n", d_virus_prob_mutation);
	
	fprintf(pf_settings, "100\\\\virginity rounds\n");
	fprintf(pf_settings, "0\\\\ct add remove strategy (0-classic; 1-new)\n");
	fprintf(pf_settings, "0\\\\Bayesian effect (0-none; 1-brutal; 2-clever)\n");
	fprintf(pf_settings, "%d\\\\Solution constructon (0-normal; 1-fluent optimization)\n", i_fluent_optimization);

		
	/*if  (s_demands  ==  "")
		fprintf(pf_settings, "\\\\unicast\n", s_demands);
	else
		fprintf(pf_settings, ".\\..\\data\\%s\\\\unicast\n", s_demands);
	fprintf(pf_settings, "\\\\anycast\n");
	fprintf(pf_settings, ".\\%s\\%s\\\\multicast\n", sDataFolder, s_demands_multicast + ".dmc");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\paths\n", s_paths + ".pat");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unispec\n", s_paths + "1.spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\unireg\n", s_paths + "1.regs");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\trees\n", s_demands_multicast + ".tree");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treespec\n", s_demands_multicast + ".spec");
	fprintf(pf_settings, ".\\..\\data\\%s\\\\treereg\n", s_demands_multicast + ".regs");

	fprintf(pf_settings, "%d\\\\paths to use\n", pi_paths[iTreePathOffset]);
	fprintf(pf_settings, "%d\\\\trees to use\n",  pi_trees[iTreePathOffset]);

	fprintf(pf_settings, ".\\data\\u30.len\\\\modulation\n");

	fprintf(pf_settings, "\\\\solution\n");

	fprintf(pf_settings, "HighestSlot\\\\fit func\n");
	fprintf(pf_settings, "muppets\\\\algorithm\n");

	fprintf(pf_settings, "%d\\\\max time\n", iTimeSec);
	//fprintf(pf_settings, "1800\\\\probing time\n");

	fprintf(pf_settings, "7000\\\\pattern pool size\n");
	fprintf(pf_settings, "0\\\\the same pattern check\n");
	fprintf(pf_settings, "3\\\\minimal pattern length\n");

	fprintf(pf_settings, "%d\\\\virus generations\n", i_virus_generations);
	fprintf(pf_settings, "%d\\\\population size\n", i_pop_size);
	fprintf(pf_settings, "%.8lf\\\\virus prob cut\n", d_virus_prob_cut);
	fprintf(pf_settings, "%.8lf\\\\virus prob splice\n", d_virus_prob_splice);
	fprintf(pf_settings, "%.8lf\\\\virus prob mutation\n", d_virus_prob_mutation);
	fprintf(pf_settings, "%.8lf\\\\virus prob rem gene\n", d_virus_prob_mutation);
	fprintf(pf_settings, "%.8lf\\\\virus prob add gene\n", d_virus_prob_mutation);
	
	fprintf(pf_settings, "100\\\\virginity rounds\n");
	fprintf(pf_settings, "0\\\\ct add remove strategy (0-classic; 1-new)\n");
	fprintf(pf_settings, "0\\\\Bayesian effect (0-none; 1-brutal; 2-clever)\n");
	fprintf(pf_settings, "%d\\\\Solution constructon (0-normal; 1-fluent optimization)\n", i_fluent_optimization);*/


	fclose(pf_settings);
}//void  v_generate_tuning_for_line(CString  sLine, FILE  *pfSerie)






void  v_eon_cores_generate_from_list()
{
	CError  c_err;
	CString  s_buf;
	CString  s_list_file;

	//::Tools::vShow(0);

	s_list_file = ::Application::ExecutablePath;
	s_list_file.Replace("FiberFlowAssignment.exe", "");
	//s_list_file = "D:\\projekty_pwr\\FiberFlowAssignment\\release\\problem_list.txt";
	s_list_file += "\\problem_list.txt";

	//::Tools::vShow(s_list_file);


	FILE  *pf_generation_list;
	pf_generation_list = fopen(s_list_file, "r");
	
	if  (pf_generation_list  ==  NULL)
	{
		s_buf.Format("Cannot open (%s) for settings list generation", s_list_file);
		c_err.vPutError(s_buf);
		::MessageBox(NULL, "NOK", "NOK", MB_OK);
		return;
	}//if  (pf_generation_list  ==  NULL)

	//FILE  *pf_serie;
	//pf_serie = fopen("serie.txt", "w+");

	//::Tools::vShow(2);

	CString  s_line = "a";
	CString  s_optimal_value = "";
	double  d_optimal_value;
	
	int  i_buf;

	int  i_setting_run_number = 1;
	//int  i_max_time_sec = 43200;
	//int  i_switch_time_sec = 21600;
	//int  i_switch_time_sec = 9921600;

	int  i_max_time_sec = 7200;
	int  i_switch_time_sec = 3600;
	CString  s_data_folder = "data_random";

	while  ( (s_line  !=  "")&&(feof(pf_generation_list)  ==  false) )
	{
		s_line = Tools::sReadLine(pf_generation_list, &s_optimal_value);

		//muppets
		//v_generate_eon_cores(s_data_folder, i_max_time_sec, false, s_line, NULL, 16, 100, 0.12, 0.15, 0.05, i_setting_run_number, 1);//chosen configuration

		//muppets_simple
		//v_generate_eon_cores(s_data_folder, i_max_time_sec, i_switch_time_sec, false, s_line, NULL, 16, 100, 0.12, 0.15, 0.05, i_setting_run_number, 2);//chosen configuration

		//tabu
		v_generate_eon_cores_tabu(s_data_folder, i_max_time_sec, i_switch_time_sec, false, s_line, NULL, 0.05, 0.05, 2.0, 0.5, 0.6, i_setting_run_number, 1);//chosen configuration

		/*v_generate_eon_cores_grasp
			(
			s_data_folder, i_max_time_sec, i_switch_time_sec, false, s_line, NULL, 
			6, 0, 0, 0, 20000, 0.999, 120, 0, 0, 1, 0.1,
			i_setting_run_number, 1);//chosen configuration*/

	}//while  ( (s_line  !=  "")&&(feof(pf_generation_list)  ==  false) )


	//::Tools::vShow(3);



	fclose(pf_generation_list);
	//fclose(pf_serie);
	
	::MessageBox(NULL, "ok", "ok", MB_OK);

}//void  v_fiber_generate_from_list()


void  v_fiber_generate_from_list()
{
	CError  c_err;
	CString  s_buf;
	CString  s_list_file;

	s_list_file = ::Application::ExecutablePath;
	s_list_file.Replace("FiberFlowAssignment.exe", "");
	//s_list_file = "D:\\projekty_pwr\\FiberFlowAssignment\\release\\problem_list.txt";asdsad
	s_list_file += "\\problem_list.txt";



	FILE  *pf_generation_list;
	pf_generation_list = fopen(s_list_file, "r");
	
	if  (pf_generation_list  ==  NULL)
	{
		s_buf.Format("Cannot open (%s) for settings list generation", s_list_file);
		c_err.vPutError(s_buf);
		::MessageBox(NULL, "NOK", "NOK", MB_OK);
		return;
	}//if  (pf_generation_list  ==  NULL)

	//FILE  *pf_serie;
	//pf_serie = fopen("serie.txt", "w+");


	CString  s_line = "a";
	CString  s_optimal_value = "";
	double  d_optimal_value;
	
	int  i_buf;

	int  i_setting_run_number = 5;

	while  ( (s_line  !=  "")&&(feof(pf_generation_list)  ==  false) )
	{
		s_line = Tools::sReadLine(pf_generation_list, &s_optimal_value);

		for  (int  i_sett_counter = 0; i_sett_counter < 7; i_sett_counter++)
		{
			//sga-clever
			//v_sga_generate_sett_file_from_line(false, false, s_line, NULL, i_sett_counter, 8000, 0.6, 0.4, i_setting_run_number, 1);
		
			//sga-old
			//v_sga_generate_sett_file_from_line(false, true, s_line, NULL, i_sett_counter, 12000, 0.7, 0.4, i_setting_run_number, 0);	
		
			//muppets-clever
			//v_generate_tuning_for_line(false, false, s_line, NULL, i_sett_counter, 16, 100, 0.12, 0.15, 0.05, i_setting_run_number, 1);//chosen configuration

			//muppets-clever simple opt
			v_generate_tuning_for_line(false, false, s_line, NULL, i_sett_counter, 16, 100, 0.12, 0.15, 0.05, i_setting_run_number, 2);//chosen configuration
		
			//muppets-old
			//v_generate_tuning_for_line(false, true, s_line, NULL, i_sett_counter, 64, 150, 0.18, 0.07, 0.1, i_setting_run_number, 0);//chosen configuration

			//v_random_search_generate_sett_file_from_line(s_line, NULL, i_sett_counter, i_setting_run_number, 1);
			//v_random_search_generate_sett_file_from_line(s_line, NULL, i_sett_counter, i_setting_run_number, 0);

			//v_sim_annealing_generate_sett_file_from_line(false, s_line, NULL, i_sett_counter, i_setting_run_number, 2);//*/
		}//for  (int  i_sett_counter = 0; i_sett_counter < 7; i_sett_counter++)
	}//while  ( (s_line  !=  "")&&(feof(pf_generation_list)  ==  false) )




	fclose(pf_generation_list);
	//fclose(pf_serie);
	
	::MessageBox(NULL, "ok", "ok", MB_OK);

}//void  v_fiber_generate_from_list()


System::Void CSurvP2PMainForm::but_generate_Click(System::Object*  sender, System::EventArgs*  e)
{
	v_eon_cores_generate_from_list();
	//v_fiber_generate_from_list();
}//System::Void CSurvP2PMainForm::but_generate_Click(System::Object*  sender, System::EventArgs*  e)




double  d_get_marker_value(CString  sLine, CString  sMarker)
{
	int  i_mark_start;

	i_mark_start = sLine.Find(sMarker);
	if  (i_mark_start > 0)
	{
		return(Tools::dExtractFromString(sLine, i_mark_start + sMarker.GetLength()));
	}//if  (i_mark_start > 0)

	return(-1);
}//double  d_get_marker_value(CString  sLine, CString  sMarker)



void  v_extract_best_fit_time_val_ffe(FILE  *pfRunFile, double  *pdBestFitTime, double  *pdBestFitVal, double  *pdBestFitFFE)
{
	CString  s_best_fit_marker = "(best fitness: ";
	CString  s_best_fit_marker_alt = "fiber best fitness: ";
	CString  s_time_marker = "[time passed:";
	CString  s_ffe_marker = "[evaluations:";

	CString  s_line;
	int  i_mark_start;

	double  d_fit, d_ffe, d_time;


	*pdBestFitTime = 0;
	*pdBestFitVal = -1;
	*pdBestFitFFE = 0;

	while  (!feof(pfRunFile))
	{
		s_line = Tools::sReadLine(pfRunFile);

		d_fit = d_get_marker_value(s_line, s_best_fit_marker);
		if  (d_fit == -1)  d_fit = d_get_marker_value(s_line, s_best_fit_marker_alt);

		d_ffe = d_get_marker_value(s_line, s_ffe_marker);
		d_time = d_get_marker_value(s_line, s_time_marker);

		if  (d_fit >= 0)
		{
			if  ( (d_fit < *pdBestFitVal)||(*pdBestFitVal == -1) )
			{
				*pdBestFitTime = d_time;
				*pdBestFitVal = d_fit;
				*pdBestFitFFE = d_ffe;
			}//if  ( (d_fit < *pdBestFitVal)||(*pdBestFitVal == -1) )
		}//if  (d_fit >= 0)
		
		
		//extract best fitness
		
	}//while  (!feof(pfRunFile))
}//void  v_extract_best_fit_time_val_ffe(FILE  *pfRunFile, double  *pdBestFitTime, double  *pdBestFitVal, double  *pdBestFitFFE)


CError  e_process_summary_file(FILE  *pfSummary, FILE  *pfDest, CString  sFolder)
{
	CError  c_err;


	CString  s_line, s_run_file_name, s_buf;
	double  d_best_fit_time, d_best_fit_val, d_best_fit_ffe;

	FILE  *pf_run_file;

	while  (!feof(pfSummary))
	{
		s_line = Tools::sReadLine(pfSummary);

		//::MessageBox(NULL, s_line, "LINE", MB_OK);

		//first extract run file name
		s_run_file_name = Tools::sExtractFromStringBackwards(s_line, s_line.GetLength());
		s_run_file_name.Replace(".\\", "");
		s_run_file_name.Replace("00.txt", "00_RUN.txt");

		if  (s_run_file_name.GetLength() > 2)
		{
			pf_run_file = fopen(sFolder + s_run_file_name, "r");
			if  (pf_run_file  !=  NULL)
			{
				v_extract_best_fit_time_val_ffe(pf_run_file, &d_best_fit_time, &d_best_fit_val, &d_best_fit_ffe);
				fclose(pf_run_file);

				s_buf.Format("%s \t Fit:\t%.8lf \t time:\t %.2lf \t FFE:\t %.2lf\n", s_run_file_name, d_best_fit_val, d_best_fit_time, d_best_fit_ffe);

				fprintf(pfDest, s_buf);
			}//if  (pf_run_file  !=  NULL)
			else
			{
				::MessageBox(NULL, sFolder + s_run_file_name, "not opened", MB_OK);
			}//else  if  (pf_run_file  !=  NULL)
		}//if  (s_run_file_name.GetLength() > 2)
		
		
	}//while  (!feof(pfSummary))



	return(c_err);
}//CError  e_process_summary_file(FILE  *pfSummary)



System::Void CSurvP2PMainForm::but_extract_best_result_time_Click(System::Object*  sender, System::EventArgs*  e)
{
	CError  c_err;
	CString  s_buf;
	CString  s_folder, s_file;


	try
	{
		s_folder = ::Application::ExecutablePath;
		s_folder.Replace("FiberFlowAssignment.exe", "");
		

		FILE  *pf_summary, *pf_dest;
		for  (int  i_serie = 0; i_serie < 7; i_serie++)
		{
			if  (i_serie  ==  0)  s_buf = "z_summary_0_k2.txt";
			if  (i_serie  ==  1)  s_buf = "z_summary_1_k4.txt";
			if  (i_serie  ==  2)  s_buf = "z_summary_2_k6.txt";
			if  (i_serie  ==  3)  s_buf = "z_summary_3_k8.txt";
			if  (i_serie  ==  4)  s_buf = "z_summary_4_k10.txt";
			if  (i_serie  ==  5)  s_buf = "z_summary_5_k20.txt";
			if  (i_serie  ==  6)  s_buf = "z_summary_6_k30.txt";

			s_file = s_folder + s_buf;
			pf_summary = fopen(s_file, "r");


			if  (pf_summary == NULL)
			{
				if  (i_serie  ==  0)  s_buf = "z_summary_0.txt";
				if  (i_serie  ==  1)  s_buf = "z_summary_1.txt";
				if  (i_serie  ==  2)  s_buf = "z_summary_2.txt";
				if  (i_serie  ==  3)  s_buf = "z_summary_3.txt";
				if  (i_serie  ==  4)  s_buf = "z_summary_4.txt";
				if  (i_serie  ==  5)  s_buf = "z_summary_5.txt";
				if  (i_serie  ==  6)  s_buf = "z_summary_6.txt";

				s_file = s_folder + s_buf;
				pf_summary = fopen(s_file, "r");			
			}//if  (pf_summary == NULL)


			if  (pf_summary == NULL)
			{
				::MessageBox(NULL, s_file, "NOT FOUND", MB_OK);
				
			}//if  (pf_summary == NULL)
			else
			{
				pf_dest = fopen(s_file + "time_best.txt", "w+");

				c_err = e_process_summary_file(pf_summary, pf_dest, s_folder);
				
				fclose(pf_dest);
				fclose(pf_summary);
			}//else if  (pf_summary == NULL)
			
		}//for  (int  i_serie = 0; i_serie < 7; i_serie++)

		
		
		if  (c_err)
		{
			c_err.vShowWindow();
			return;		
		}//if  (c_err)
	}//try
	catch(Exception *pex)
	{
		CError  c_err;
		s_buf = pex->ToString();
		s_buf = s_buf + "\nGeneral run serie exception";
		c_err.vPutError(s_buf);
	}//catch(Exception *pex)

	::MessageBox(NULL, "ok", "ok", MB_OK);
	
}//System::Void CSurvP2PMainForm::but_extract_best_result_time_Click(System::Object*  sender, System::EventArgs*  e)




System::Void CSurvP2PMainForm::but_load_files_Click(System::Object*  sender, System::EventArgs*  e)
{
	CError  c_err;
	CString  s_buf;

	/*OpenFileDialog* openFileDialog1 = new OpenFileDialog();
 
	openFileDialog1->Filter = "All files (*.*)|*.*";
    openFileDialog1->FilterIndex = 0;
    openFileDialog1->RestoreDirectory = true;
	openFileDialog1->Multiselect  =  false;

    if(openFileDialog1->ShowDialog() == DialogResult::OK)
    {
		Refresh();

		if  (File::Exists(openFileDialog1->FileNames[0])  ==  false)
		{
			s_buf  =  (CString) openFileDialog1->FileNames[0];
			s_buf.Format("The file does not exist (%s)", (LPCSTR) s_buf);
			c_err.vPutError(s_buf);
			c_err.vShowWindow();
			return;
		}//if  (File::Exists(openFileDialog1->FileNames[ii])  ==  false)*/

		try
		{
			//::MessageBox(NULL, "test", "test", MB_OK);*/
			//c_err  =  pc_system->eRunSerie("D:\\projekty_pwr\\FiberFlowAssignment\\release\\serie_test.txt", list_info);
			s_buf = ::Application::ExecutablePath;
			s_buf.Replace("FiberFlowAssignment.exe", "");
			s_buf += "serie.txt";
			//::MessageBox(NULL, s_buf, s_buf, MB_OK);
			//c_err  =  pc_system->eRunSerie("R:\\muppets_test_30\\serie_test.txt", list_info);
			c_err  =  pc_system->eRunSerie(s_buf, list_info);
			
			//c_err  =  pc_system->eRunSerie(openFileDialog1->FileNames[0], list_info);
			if  (c_err)
			{
				c_err.vShowWindow();
				return;		
			}//if  (c_err)
		}//try
		catch(Exception *pex)
		{
			CError  c_err;
			s_buf = pex->ToString();
			s_buf = s_buf + "\nGeneral run serie exception";
			c_err.vPutError(s_buf);
		}//catch(Exception *pex)

	//}//if(openFileDialog1->ShowDialog() == DialogResult::OK)*/

	::MessageBox(NULL, "ok", "ok", MB_OK);
}//System::Void CSurvP2PMainForm::but_load_files_Click(System::Object*  sender, System::EventArgs*  e)



System::Void CSurvP2PMainForm::but_analyze_pattern_length_Click(System::Object*  sender, System::EventArgs*  e)
{
	CError  c_err;
	CString  s_buf;
	FILE  *pf_buf;


	
	try
	{
		pf_buf  =  fopen("D:\\studia\\doktoranckie\\publikacje\\X 2013.02 MuPPetS and Fiber flow assignment\\2013.12.02 RESEARCH new comp machine\\ffe_test\\mup_30\\serie_test.txt","r");

		//getting files names
		CString  s_file_name, s_setting_name, s_buf;
		int  i_repetations;
		vector  <CString>  v_file_names;

		
		s_file_name  =  "a";
		for  (;(s_file_name != "")&&(feof(pf_buf) != true);)
		{
			s_file_name  =  Tools::sReadLine(pf_buf, &s_setting_name);
			i_repetations  =  atoi(s_setting_name);
			//::MessageBox(NULL, s_file_name + " " + s_setting_name, "", MB_OK);
			
			if  (s_file_name != "")
			{
				if  (i_repetations  <=  0)  i_repetations  =  1;

				for  (int  ii = 0; ii < i_repetations; ii++)
				{
					v_file_names.push_back(s_file_name);
					//::MessageBox(NULL, s_file_name, s_file_name, MB_OK);
				}//for  (int  ii = 0; ii < i_repetations; ii++)
			}//if  (s_file_name <> "")	
		}//for  (;(s_file_name <> "")&&(feof(pf_buf) != true);)

		
		for  (int  ii = 0; ii < v_file_names.size(); ii++)
		{
			//v_analyze_one_pattern_result(v_file_names.at(ii));
		}//for  (int  ii = 0; ii < v_file_names.at(ii); ii++)

		fclose(pf_buf);
	}//try
	catch(Exception *pex)
	{
		CError  c_err;
		s_buf = pex->ToString();
		s_buf = s_buf + "\nGeneral run serie exception";
		c_err.vPutError(s_buf);
	}//catch(Exception *pex)


	::MessageBox(NULL, "ok", "ok", MB_OK);
}//System::Void CSurvP2PMainForm::but_analyze_pattern_length_Click(System::Object*  sender, System::EventArgs*  e)


System::Void CSurvP2PMainForm::but_load_1_file_Click(System::Object*  sender, System::EventArgs*  e)
{
	CError  c_err;
	CString  s_buf;

	OpenFileDialog* openFileDialog1 = new OpenFileDialog();
 
    //openFileDialog1->InitialDirectory = S"D:\\praca dyplomowa - nowy interfejs\\publikacja\\FD_init" ;
	openFileDialog1->Filter = "All files (*.*)|*.*";
    openFileDialog1->FilterIndex = 0;
    openFileDialog1->RestoreDirectory = true;
	openFileDialog1->Multiselect  =  false;

    if(openFileDialog1->ShowDialog() == DialogResult::OK)
    {
		Refresh();

		if  (File::Exists(openFileDialog1->FileNames[0])  ==  false)
		{
			s_buf  =  (CString) openFileDialog1->FileNames[0];
			s_buf.Format("The file does not exist (%s)", (LPCSTR) s_buf);
			c_err.vPutError(s_buf);
			c_err.vShowWindow();
			return;
		}//if  (File::Exists(openFileDialog1->FileNames[ii])  ==  false)


		c_err  =  pc_system->eRun("",openFileDialog1->FileNames[0], list_info);
		if  (c_err)
		{
			c_err.vShowWindow();
			return;		
		}//if  (c_err)

	}//if(openFileDialog1->ShowDialog() == DialogResult::OK)

	::MessageBox(NULL, "ok", "ok", MB_OK);

}//System::Void CSurvP2PMainForm::but_load_1_file_Click(System::Object*  sender, System::EventArgs*  e)




System::Void CSurvP2PMainForm::button1_Click(System::Object *  sender, System::EventArgs *  e)
{
	/*CError  c_err;
	Random*  pc_random_gen  =  new  Random((int)DateTime::Now.Ticks);
	CmGA  c_mga(pc_random_gen);

	c_err  =  c_mga.eConfigureTemplate(30);
	if  (c_err)
	{
		c_err.vShowWindow();
		return;
	}//if  (c_err)

	
	c_err  =  c_mga.eConfigure
		(
		50, 
		40, 40, 40,
		1200, 0.5,
		0.015, 0.4, 
		0.1, 0.1, 0.1
		);

	if  (c_err)
	{
		c_err.vShowWindow();
		return;	
	}//if  (c_err)

	CDecFunc1Modal  c_fitness(5, 1, 0, 1,  &c_err);
	if  (c_err)
	{
		c_err.vShowWindow();
		return;	
	}//if  (c_err)


	CConcatDecFunc  c_con_dec;
	for  (int  ii = 0; ii < 5; ii++)
	{
		c_err  =  c_con_dec.eAddDecFunc(&c_fitness);
		if  (c_err)
		{
			c_err.vShowWindow();
			return;	
		}//if  (c_err)
	}//for  (int  ii = 0; ii < 4; ii++)

	
	c_err  =  c_mga.eSetFitness(&c_con_dec);
	if  (c_err)
	{
		c_err.vShowWindow();
		return;	
	}//if  (c_err)


	vector  <CMessyPattern  *>  v_best_found;

	//c_err  =  c_mga.eRun("D:\\mgat", &v_best_found);
	if  (c_err)
	{
		c_err.vShowWindow();
		return;	
	}//if  (c_err)

	::MessageBox(NULL, "ok", "ok", MB_OK);*/


};//System::Void CSurvP2PMainForm::button1_Click(System::Object *  sender, System::EventArgs *  e)