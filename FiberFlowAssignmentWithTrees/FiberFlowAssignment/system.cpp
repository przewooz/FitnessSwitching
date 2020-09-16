#include "stdafx.h"

#include "system.h"




void  CSystem::v_create_bb_vector(vector  <int>  *pvBBOrder,  CString  sBBOrder)
{
	CString  s_buf;

	int  i_coma, i_coma_prev;
	int  i_number;

	i_coma = 0;
	for  (; i_coma < sBBOrder.GetLength();)
	{
		i_coma_prev  =  i_coma;
		i_coma  =  sBBOrder.Find(',', i_coma + 1);

		if  (i_coma  ==  -1)  i_coma  =  sBBOrder.GetLength();

		s_buf = "";
		for  (int ii = 0; ii < i_coma - i_coma_prev; ii++)
			if  (sBBOrder.GetAt(i_coma_prev + ii)  !=  ',')
				s_buf  +=  sBBOrder.GetAt(i_coma_prev + ii);


		i_number  =  atoi(s_buf);

		pvBBOrder->push_back(i_number);

		/*s_buf.Format("%d  %d  %d", i_coma, i_number, i_coma_prev);
		::MessageBox(NULL, s_buf, "", MB_OK);*/	
	}//for  (; i_coma < sBBOrder.GetLength();)
	
}//CError  CSystem::e_create_bb_vector(vector  <int>  *pvBBOrder,  CString  sBBOrder)



void  CSystem::v_log(CString  sLog)
{
	FILE  *pf_log;

	pf_log = fopen("!log.txt", "a+");

	fprintf(pf_log, sLog + "\n");


	fclose(pf_log);
}//void  CSystem::v_log(CString  sLog)


CError  CSystem::eRun(CString  sMainDir, CString  sSettings, System::Windows::Forms::ListBox*  listInfo)
{
	CError  c_err;
	

	FILE  *pf_buf;
	CString  s_effective_settings = sMainDir + sSettings;
	CString  s_dir_for_files_from_settings = Tools::sGetDirectory(s_effective_settings);


	
	pf_buf  =  fopen(s_effective_settings,"r");
	if  (pf_buf  ==  NULL)
	{
		CString  s_buf;

		s_buf.Format("Couldnt open settings file (%s | %s)", sMainDir, sSettings);
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  (pf_buf  ==  NULL)


	//getting result file name
	CString  s_file_name, s_setting_name, s_buf;
	s_file_name  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_FILE_NAME)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_FILE_NAME, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	int  i_file_counter;
	CString  s_effective_file_name;
	i_file_counter  =  0;
	s_effective_file_name.Format("%s_%.2d", s_file_name, i_file_counter);
	s_effective_file_name +=  ".txt";

	while  (System::IO::File::Exists(s_effective_file_name)  ==  true)
	{
		i_file_counter++;
		s_effective_file_name.Format("%s_%.2d.txt", s_file_name, i_file_counter);
		//s_effective_file_name +=  ".txt";
	}//while  (System::IO::File::Exists(s_file_name)  ==  true)

	FILE  *pf_effective_result_file;
	pf_effective_result_file = fopen(s_effective_file_name, "w+");
	
	if  (pf_effective_result_file  ==  NULL)
	{
		s_buf.Format("Couldnt open result file (%s)", s_effective_file_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);
	}//if  (pf_effective_result_file  ==  NULL)

	fclose(pf_effective_result_file);//we JUST want to reserve a file name...
	

	//getting summarize results file name
	CString  s_file_summarize;
	s_file_summarize  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_SUMMARIZE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_SUMMARIZE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	if  (sInParameters !=  "")
	{
		s_file_summarize.Format("%s_out.txt", sInParameters);
	}//if  (sInParameters !=  "")





	//reading in the network model
	CString  s_net_file;
	s_net_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_NET_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_NET_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)
	s_net_file = s_dir_for_files_from_settings + s_net_file;


	CString  s_rep_file;
	s_rep_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_REP_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_REP_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)
	s_rep_file = s_dir_for_files_from_settings + s_rep_file;



	CString  s_dem_file;
	s_dem_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_DEMAND_UNICAST_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_DEMAND_UNICAST_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)
	if  (s_dem_file  !=  "")  s_dem_file = s_dir_for_files_from_settings + s_dem_file;



	CString  s_dea_file;
	s_dea_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_DEMAND_ANYCAST_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_DEMAND_ANYCAST_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)
	if  (s_dea_file != "")s_dea_file = s_dir_for_files_from_settings + s_dea_file;


	CString  s_dmc_file;
	s_dmc_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_DEMAND_MULTICAST_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_DEMAND_MULTICAST_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_DEMAND_MULTICAST_FILE)
	if  (s_dmc_file != "") s_dmc_file = s_dir_for_files_from_settings + s_dmc_file;


	CString  s_dea_file_1_dir;
	s_dea_file_1_dir  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_DEMAND_ANYCAST_1_DIR_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_DEMAND_ANYCAST_1_DIR_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_DEMAND_ANYCAST_1_DIR_FILE)
	if  (s_dea_file_1_dir != "")s_dea_file_1_dir = s_dir_for_files_from_settings + s_dea_file_1_dir;


	double  d_transfer_multiplier;
	d_transfer_multiplier  =  Tools::dReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_TRANSFER_MULTIPLIER)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_TRANSFER_MULTIPLIER, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_TRANSFER_MULTIPLIER)
	


	CString  s_path_file;
	s_path_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_PATHS_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_PATHS_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_MOD_FILE)
	s_path_file = s_dir_for_files_from_settings + s_path_file;



	CString  s_path_backup_file;
	s_path_backup_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_PATHS_BACKUP_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_PATHS_BACKUP_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_PATHS_BACKUP_FILE)
	s_path_backup_file = s_dir_for_files_from_settings + s_path_backup_file;



	CString  s_uni_spec_file;
	s_uni_spec_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_SPEC_FOR_UNI_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_SPEC_FOR_UNI_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_SPEC_FOR_UNI_FILE)
	s_uni_spec_file = s_dir_for_files_from_settings + s_uni_spec_file;


	CString  s_uni_spec_file_backup;
	s_uni_spec_file_backup  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_SPEC_FOR_UNI_FILE_BACKUP)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_SPEC_FOR_UNI_FILE_BACKUP, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_SPEC_FOR_UNI_FILE)
	s_uni_spec_file_backup = s_dir_for_files_from_settings + s_uni_spec_file_backup;




	CString  s_uni_reg_file;
	s_uni_reg_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_REG_FOR_UNI_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_REG_FOR_UNI_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_REG_FOR_UNI_FILE)
	if  (s_uni_reg_file != "") s_uni_reg_file = s_dir_for_files_from_settings + s_uni_reg_file;



	CString  s_tree_file;
	s_tree_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_TREES_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_TREES_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_TREES_FILE)
	if  (s_tree_file != "")  s_tree_file = s_dir_for_files_from_settings + s_tree_file;


	CString  s_tree_spec_file;
	s_tree_spec_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_SPEC_FOR_TREE_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_SPEC_FOR_TREE_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_SPEC_FOR_UNI_FILE)
	if  ((s_tree_file != "")||(s_tree_spec_file != ""))  s_tree_spec_file = s_dir_for_files_from_settings + s_tree_spec_file;


	CString  s_tree_reg_file;
	s_tree_reg_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_REG_FOR_TREE_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_REG_FOR_TREE_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_REG_FOR_UNI_FILE)
	if  ((s_tree_file != "")||(s_tree_spec_file != "")||(s_tree_reg_file != ""))  s_tree_reg_file = s_dir_for_files_from_settings + s_tree_reg_file;



	int  i_paths_to_use;
	i_paths_to_use  =  Tools::iReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_PATH_NUMBER)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_PATH_NUMBER, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_PATH_NUMBER)


	int  i_trees_to_use;
	i_trees_to_use  =  Tools::iReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_TREE_NUMBER)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_TREE_NUMBER, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_TREE_NUMBER)



	int  i_cores;
	i_cores =  Tools::iReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_CORE_NUMBER)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_CORE_NUMBER, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SYSTEM_PARAM_CORE_NUMBER)


	


	CString  s_mod_file;
	s_mod_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_MOD_FILE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_MOD_FILE, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)
	s_mod_file = s_dir_for_files_from_settings + s_mod_file;


	
	CString  s_solf_file;
	s_solf_file  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  ( (s_setting_name  !=  SYSTEM_PARAM_SOLF_FILE)&&(s_setting_name  !=  SYSTEM_PARAM_OPTIMAL) )
	{
		s_buf.Format("Error at reading (%s / %s) parameter read:(%s)", SYSTEM_PARAM_SOLF_FILE, SYSTEM_PARAM_OPTIMAL, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)
	//if  (s_solf_file  !=  "")  s_solf_file = /*s_dir_for_files_from_settings +*/ s_solf_file;


	double  d_optimal_value = -1;
	if  (s_setting_name  ==  SYSTEM_PARAM_OPTIMAL)
	{
		if  (s_solf_file.GetLength() > 0)
		{
			d_optimal_value = atof(s_solf_file);
		}//if  (s_optimal_value.GetLength() > 0)
	}//if  (s_setting_name  ==  SYSTEM_PARAM_OPTIMAL)



	CString  s_fit_func;
	s_fit_func  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_FIT_FUNC)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_FIT_FUNC, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	
	



	CString  s_algorithm_name;
	s_algorithm_name  =  Tools::sReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  SYSTEM_PARAM_ALGORITHM)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SYSTEM_PARAM_ALGORITHM, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)

		

	double  d_time;
	d_time  =  Tools::dReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_MAX_TIME)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_MAX_TIME, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	double  d_switch_time;
	d_switch_time  =  Tools::dReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_SWITCH_TIME)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_SWITCH_TIME, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)




	double  d_time_probing;
	d_time_probing  =  Tools::dReadLine(pf_buf, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_PROBING_TIME)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_PROBING_TIME, s_setting_name);
		c_err.vPutError(s_buf);
		fclose(pf_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)







	

	try
	{
		if  (s_algorithm_name  ==  SYSTEM_PARAM_ALGORITHM_GRASP)
		{
			CVirusGA  c_vga;
			CFiberNet  c_fiber_net;


			c_err = c_fiber_net.eSetCores(i_cores);
			if  (c_err)  
			{
				fclose(pf_buf);
				return(c_err);
			}//if  (c_err)  


			c_err = c_vga.eRandomSearchLoadSettings(pf_buf);
			if  (c_err)  
			{
				fclose(pf_buf);
				return(c_err);
			}//if  (c_err)  



			long  l_restricted_candidate_list_size; 
			double d_restricted_candidate_list_size_factor;
			int  i_value_based_candidate_list_size;  
			long  l_acceptable_distance_from_optimal;
			long  l_local_search_max_iterations;
			double  d_local_search_cooling_rate_factor;
			double  d_local_search_nr_of_demands_to_swap_factor;
			long  l_local_search_nr_of_demands_to_swap;
			double  d_local_search_initial_temperature;
			double  d_local_search_initial_distance_from_optimal;
			double  d_local_search_initial_probability_of_accepting_worse_solution;


			l_restricted_candidate_list_size  =  Tools::iReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_GRASP_lRestrictedCandidateListSize)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GRASP_lRestrictedCandidateListSize, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_GRASP_lRestrictedCandidateListSize)aa


			d_restricted_candidate_list_size_factor  =  Tools::dReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_GRASP_dRestrictedCandidateListSizeFactor)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GRASP_dRestrictedCandidateListSizeFactor, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_PRO_REP)


			i_value_based_candidate_list_size  =  Tools::iReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_GRASP_iValueBasedCandidateListSize)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GRASP_iValueBasedCandidateListSize, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_LIST_LIMIT_PERC)


			l_acceptable_distance_from_optimal  =  Tools::iReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_GRASP_lAcceptableDistanceFromOptimal)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GRASP_lAcceptableDistanceFromOptimal, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_NO_IMPROV_LIMIT_PERC)


			l_local_search_max_iterations  =  Tools::iReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_GRASP_lLocalSearchMaxIterations)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GRASP_lLocalSearchMaxIterations, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_WORSENING_FACTOR)


			d_local_search_cooling_rate_factor  =  Tools::dReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_GRASP_dLocalSearchCoolingRateFactor)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GRASP_dLocalSearchCoolingRateFactor, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_WORSENING_FACTOR)



			d_local_search_nr_of_demands_to_swap_factor  =  Tools::dReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_GRASP_dLocalSearchNrOfDemandsToSwapFactor)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GRASP_dLocalSearchNrOfDemandsToSwapFactor, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_WORSENING_FACTOR)


			l_local_search_nr_of_demands_to_swap  =  Tools::iReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_GRASP_lLocalSearchNrOfDemandsToSwap)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GRASP_lLocalSearchNrOfDemandsToSwap, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_WORSENING_FACTOR)


			d_local_search_initial_temperature  =  Tools::dReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_GRASP_dLocalSearchInitialTemperature)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GRASP_dLocalSearchInitialTemperature, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_WORSENING_FACTOR)


			d_local_search_initial_distance_from_optimal  =  Tools::dReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_GRASP_dLocalSearchInitialDistanceFromOptimal)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GRASP_dLocalSearchInitialDistanceFromOptimal, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_WORSENING_FACTOR)

			d_local_search_initial_probability_of_accepting_worse_solution  =  Tools::dReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_GRASP_dLocalSearchInitialProbabilityOfAcceptingWorseSolution)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GRASP_dLocalSearchInitialProbabilityOfAcceptingWorseSolution, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_WORSENING_FACTOR)
		

			fclose(pf_buf);
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadTopology(s_net_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTopology("!top_test.txt");
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eLoadPaths(s_path_file, s_path_backup_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePaths("!path_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadPathsSpec(s_uni_spec_file, s_uni_spec_file_backup, d_transfer_multiplier);
			if  (c_err)  return(c_err);
			
			c_err  = c_fiber_net.eLoadPathsReg(s_uni_reg_file);
			if  (c_err)  return(c_err);



			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsSpec("spec_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsReg("reg_test.txt");
			if  (c_err)  return(c_err);



			c_err  = c_fiber_net.eLoadTrees(s_tree_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTrees("!tree_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadTreesSpec(s_tree_spec_file, d_transfer_multiplier);
			if  (c_err)  return(c_err);
			
			c_err  = c_fiber_net.eLoadTreesReg(s_tree_reg_file);
			if  (c_err)  return(c_err);



			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTreesSpec("!tree_spec_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTreesReg("!tree_reg_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadReplicas(s_rep_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveReplicas("!rep_test.txt");
			if  (c_err)  return(c_err);


			

			c_err  = c_fiber_net.eLoadDemands(s_dem_file, s_dea_file, s_dmc_file, s_dea_file_1_dir, i_paths_to_use, i_trees_to_use);
			if  (c_err)  return(c_err);


			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveDemands("!dem_test.txt");
			if  (c_err)  return(c_err);


			
			
			CFiberSolution  c_sol(&c_fiber_net);
			c_sol.vInitStatic();
			
			c_vga.eSetFitness(&c_sol);
			if  (c_err)  return(c_err);




			if  (d_optimal_value  >  0)
				c_err  =  c_vga.eRunGRASP
					(
					d_time, s_effective_file_name,  s_file_summarize, "", &d_optimal_value, 
					
					l_restricted_candidate_list_size, 
					d_restricted_candidate_list_size_factor,
					i_value_based_candidate_list_size,  
					l_acceptable_distance_from_optimal,
					l_local_search_max_iterations,
					d_local_search_cooling_rate_factor,
					d_local_search_nr_of_demands_to_swap_factor,
					l_local_search_nr_of_demands_to_swap,
					d_local_search_initial_temperature,
					d_local_search_initial_distance_from_optimal,
					d_local_search_initial_probability_of_accepting_worse_solution,


					listInfo
					);
			else
				c_err  =  c_vga.eRunGRASP
					(
					d_time, s_effective_file_name,  s_file_summarize, s_solf_file, NULL,
					
					l_restricted_candidate_list_size, 
					d_restricted_candidate_list_size_factor,
					i_value_based_candidate_list_size,  
					l_acceptable_distance_from_optimal,
					l_local_search_max_iterations,
					d_local_search_cooling_rate_factor,
					d_local_search_nr_of_demands_to_swap_factor,
					l_local_search_nr_of_demands_to_swap,
					d_local_search_initial_temperature,
					d_local_search_initial_distance_from_optimal,
					d_local_search_initial_probability_of_accepting_worse_solution,


					listInfo
					);

			if  (c_err)  return(c_err);

			s_buf.Format("%s :: end", s_effective_file_name);
			v_log(s_buf);
			s_buf.Format("-----\n\n", s_effective_file_name);
			v_log(s_buf);


		}//if  (s_algorithm_name  ==  SYSTEM_PARAM_ALGORITHM_TABU)


		if  (s_algorithm_name  ==  SYSTEM_PARAM_ALGORITHM_TABU)
		{
			CVirusGA  c_vga;
			CFiberNet  c_fiber_net;

			c_err = c_fiber_net.eSetCores(i_cores);
			if  (c_err)  
			{
				fclose(pf_buf);
				return(c_err);
			}//if  (c_err)  



			c_err = c_vga.eRandomSearchLoadSettings(pf_buf);
			if  (c_err)  
			{
				fclose(pf_buf);
				return(c_err);
			}//if  (c_err)  

			double  d_pro_rep, d_pro_path;
			double  d_tabu_list_limit_perc, d_no_improv_limit_perc, d_worsening_factor;

			d_pro_rep  =  Tools::dReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_TABU_PRO_REP)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_TABU_PRO_REP, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_PRO_REP)


			d_pro_path  =  Tools::dReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_TABU_PRO_PATH)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_TABU_PRO_PATH, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_PRO_REP)


			d_tabu_list_limit_perc  =  Tools::dReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_TABU_LIST_LIMIT_PERC)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_TABU_LIST_LIMIT_PERC, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_LIST_LIMIT_PERC)


			d_no_improv_limit_perc  =  Tools::dReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_TABU_NO_IMPROV_LIMIT_PERC)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_TABU_NO_IMPROV_LIMIT_PERC, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_NO_IMPROV_LIMIT_PERC)


			d_worsening_factor  =  Tools::dReadLine(pf_buf, &s_setting_name);
			if  (s_setting_name  !=  VGA_PARAM_TABU_WORSENING_FACTOR)
			{
				s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_TABU_WORSENING_FACTOR, s_setting_name);
				c_err.vPutError(s_buf);
				fclose(pf_buf);
				return(c_err);	
			}//if  (s_setting_name  !=  VGA_PARAM_TABU_WORSENING_FACTOR)

		

			fclose(pf_buf);
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadTopology(s_net_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTopology("!top_test.txt");
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eLoadPaths(s_path_file, s_path_backup_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePaths("!path_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadPathsSpec(s_uni_spec_file, s_uni_spec_file_backup, d_transfer_multiplier);
			if  (c_err)  return(c_err);
			
			c_err  = c_fiber_net.eLoadPathsReg(s_uni_reg_file);
			if  (c_err)  return(c_err);



			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsSpec("spec_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsReg("reg_test.txt");
			if  (c_err)  return(c_err);



			c_err  = c_fiber_net.eLoadTrees(s_tree_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTrees("!tree_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadTreesSpec(s_tree_spec_file, d_transfer_multiplier);
			if  (c_err)  return(c_err);
			
			c_err  = c_fiber_net.eLoadTreesReg(s_tree_reg_file);
			if  (c_err)  return(c_err);



			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTreesSpec("!tree_spec_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTreesReg("!tree_reg_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadReplicas(s_rep_file);
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eSaveReplicas("!rep_test.txt");
			if  (c_err)  return(c_err);


			

			c_err  = c_fiber_net.eLoadDemands(s_dem_file, s_dea_file, s_dmc_file, s_dea_file_1_dir, i_paths_to_use, i_trees_to_use);
			if  (c_err)  return(c_err);


			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveDemands("!dem_test.txt");
			if  (c_err)  return(c_err);


			
			
			CFiberSolution  c_sol(&c_fiber_net);
			c_sol.vInitStatic();
			
			c_vga.eSetFitness(&c_sol);
			if  (c_err)  return(c_err);




			if  (d_optimal_value  >  0)
				c_err  =  c_vga.eRunTabuSearch
					(
					d_time, d_switch_time, s_effective_file_name,  s_file_summarize, "", &d_optimal_value, 
					d_pro_rep, d_pro_path,
					d_tabu_list_limit_perc, d_no_improv_limit_perc, d_worsening_factor,
					listInfo
					);
			else
				c_err  =  c_vga.eRunTabuSearch
					(
					d_time, d_switch_time, s_effective_file_name,  s_file_summarize, s_solf_file, NULL,
					d_pro_rep, d_pro_path,
					d_tabu_list_limit_perc, d_no_improv_limit_perc, d_worsening_factor,
					listInfo
					);

			if  (c_err)  return(c_err);

			s_buf.Format("%s :: end", s_effective_file_name);
			v_log(s_buf);
			s_buf.Format("-----\n\n", s_effective_file_name);
			v_log(s_buf);


		}//if  (s_algorithm_name  ==  SYSTEM_PARAM_ALGORITHM_TABU)



		if  (s_algorithm_name  ==  SYSTEM_PARAM_ALGORITHM_MUPPETS)
		{
			CVirusGA  c_vga;
			CFiberNet  c_fiber_net;


			c_err = c_fiber_net.eSetCores(i_cores);
			if  (c_err)  
			{
				fclose(pf_buf);
				return(c_err);
			}//if  (c_err)  



			c_err = c_vga.eLoadSettings(pf_buf);
			fclose(pf_buf);
			if  (c_err)  return(c_err);



			c_err  = c_fiber_net.eLoadTopology(s_net_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTopology("!top_test.txt");
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eLoadPaths(s_path_file, s_path_backup_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePaths("!path_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsSummarized("!path_test2.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadPathsSpec(s_uni_spec_file, s_uni_spec_file_backup, d_transfer_multiplier);
			if  (c_err)  return(c_err);
			
			c_err  = c_fiber_net.eLoadPathsReg(s_uni_reg_file);
			if  (c_err)  return(c_err);



			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsSpec("spec_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsReg("reg_test.txt");
			if  (c_err)  return(c_err);



			c_err  = c_fiber_net.eLoadTrees(s_tree_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTrees("!tree_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadTreesSpec(s_tree_spec_file, d_transfer_multiplier);
			if  (c_err)  return(c_err);
			
			c_err  = c_fiber_net.eLoadTreesReg(s_tree_reg_file);
			if  (c_err)  return(c_err);

			
			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTreesSpec("!tree_spec_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTreesReg("!tree_reg_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadReplicas(s_rep_file);
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eSaveReplicas("!rep_test.txt");
			if  (c_err)  return(c_err);

		

			c_err  = c_fiber_net.eLoadDemands(s_dem_file, s_dea_file, s_dmc_file, s_dea_file_1_dir, i_paths_to_use, i_trees_to_use);
			if  (c_err)  return(c_err);


			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveDemands("!dem_test.txt");
			if  (c_err)  return(c_err);

			//return(c_err);


			/*c_err  = c_fiber_net.eLoadModulation(s_mod_file);
			if  (c_err)  return(c_err);

			//c_err  =  c_fiber_net.eSaveModulation("!mod_test.txt");
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eLoadReplicas(s_rep_file);
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eSaveReplicas("!rep_test.txt");
			if  (c_err)  return(c_err);

					

			//c_err  = c_fiber_net.eSavePaths("!pat_test.txt");
			if  (c_err)  return(c_err);*/

			
			

			CFiberSolution  c_sol(&c_fiber_net);
			c_sol.vInitStatic();
			
			c_vga.eSetFitness(&c_sol);
			if  (c_err)  return(c_err);





			if  (d_optimal_value  >  0)
				c_err  =  c_vga.eRun(d_time, d_switch_time, d_time_probing, s_effective_file_name,  s_file_summarize, "", &d_optimal_value, listInfo);
			else
				c_err  =  c_vga.eRun(d_time, d_switch_time, d_time_probing, s_effective_file_name,  s_file_summarize, s_solf_file, NULL, listInfo);


			if  (c_err)  return(c_err);

			s_buf.Format("%s :: end", s_effective_file_name);
			v_log(s_buf);
			s_buf.Format("-----\n\n", s_effective_file_name);
			v_log(s_buf);

		}//if  (s_algorithm_name  ==  SYSTEM_PARAM_ALGORITHM_MUPPETS)





		if  (s_algorithm_name  ==  SYSTEM_PARAM_ALGORITHM_SIMPLE_GA)
		{
			CVirusGA  c_vga;
			CFiberNet  c_fiber_net;

			c_err = c_fiber_net.eSetCores(i_cores);
			if  (c_err)  
			{
				fclose(pf_buf);
				return(c_err);
			}//if  (c_err)  



			c_err = c_vga.eSgaLoadSettings(pf_buf);
			fclose(pf_buf);
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadTopology(s_net_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTopology("!top_test.txt");
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eLoadPaths(s_path_file, s_path_backup_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePaths("!path_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadPathsSpec(s_uni_spec_file, s_uni_spec_file_backup, d_transfer_multiplier);
			if  (c_err)  return(c_err);
			
			c_err  = c_fiber_net.eLoadPathsReg(s_uni_reg_file);
			if  (c_err)  return(c_err);



			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsSpec("spec_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsReg("reg_test.txt");
			if  (c_err)  return(c_err);



			c_err  = c_fiber_net.eLoadTrees(s_tree_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTrees("!tree_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadTreesSpec(s_tree_spec_file, d_transfer_multiplier);
			if  (c_err)  return(c_err);
			
			c_err  = c_fiber_net.eLoadTreesReg(s_tree_reg_file);
			if  (c_err)  return(c_err);



			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTreesSpec("!tree_spec_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTreesReg("!tree_reg_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadReplicas(s_rep_file);
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eSaveReplicas("!rep_test.txt");
			if  (c_err)  return(c_err);


			

			c_err  = c_fiber_net.eLoadDemands(s_dem_file, s_dea_file, s_dmc_file, s_dea_file_1_dir, i_paths_to_use, i_trees_to_use);
			if  (c_err)  return(c_err);


			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveDemands("!dem_test.txt");
			if  (c_err)  return(c_err);


			
			
			CFiberSolution  c_sol(&c_fiber_net);
			c_sol.vInitStatic();
			
			c_vga.eSetFitness(&c_sol);
			if  (c_err)  return(c_err);




			if  (d_optimal_value  >  0)
				c_err  =  c_vga.eRunSimpleGA(d_time, d_time_probing, s_effective_file_name,  s_file_summarize, "", &d_optimal_value, listInfo);
			else
				c_err  =  c_vga.eRunSimpleGA(d_time, d_time_probing, s_effective_file_name,  s_file_summarize, s_solf_file, NULL, listInfo);

			if  (c_err)  return(c_err);

			s_buf.Format("%s :: end", s_effective_file_name);
			v_log(s_buf);
			s_buf.Format("-----\n\n", s_effective_file_name);
			v_log(s_buf);

		}//if  (s_algorithm_name  ==  SYSTEM_PARAM_ALGORITHM_SIMPLE_GA)



		if  (s_algorithm_name  ==  SYSTEM_PARAM_ALGORITHM_RANDOM_SEARCH)
		{
			CVirusGA  c_vga;
			CFiberNet  c_fiber_net;


			c_err = c_fiber_net.eSetCores(i_cores);
			if  (c_err)  
			{
				fclose(pf_buf);
				return(c_err);
			}//if  (c_err)  

			c_err = c_vga.eRandomSearchLoadSettings(pf_buf);
			fclose(pf_buf);
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadTopology(s_net_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTopology("!top_test.txt");
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eLoadPaths(s_path_file, s_path_backup_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePaths("!path_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadPathsSpec(s_uni_spec_file, s_uni_spec_file_backup, d_transfer_multiplier);
			if  (c_err)  return(c_err);
			
			c_err  = c_fiber_net.eLoadPathsReg(s_uni_reg_file);
			if  (c_err)  return(c_err);



			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsSpec("spec_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsReg("reg_test.txt");
			if  (c_err)  return(c_err);



			c_err  = c_fiber_net.eLoadTrees(s_tree_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTrees("!tree_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadTreesSpec(s_tree_spec_file, d_transfer_multiplier);
			if  (c_err)  return(c_err);
			
			c_err  = c_fiber_net.eLoadTreesReg(s_tree_reg_file);
			if  (c_err)  return(c_err);



			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTreesSpec("!tree_spec_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTreesReg("!tree_reg_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadReplicas(s_rep_file);
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eSaveReplicas("!rep_test.txt");
			if  (c_err)  return(c_err);


			

			c_err  = c_fiber_net.eLoadDemands(s_dem_file, s_dea_file, s_dmc_file, s_dea_file_1_dir, i_paths_to_use, i_trees_to_use);
			if  (c_err)  return(c_err);


			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveDemands("!dem_test.txt");
			if  (c_err)  return(c_err);


			
			
			CFiberSolution  c_sol(&c_fiber_net);
			c_sol.vInitStatic();
			
			c_vga.eSetFitness(&c_sol);
			if  (c_err)  return(c_err);




			if  (d_optimal_value  >  0)
				c_err  =  c_vga.eRunRandomSearch(d_time, d_time_probing, s_effective_file_name,  s_file_summarize, "", &d_optimal_value, listInfo);
			else
				c_err  =  c_vga.eRunRandomSearch(d_time, d_time_probing, s_effective_file_name,  s_file_summarize, s_solf_file, NULL, listInfo);

			if  (c_err)  return(c_err);

			s_buf.Format("%s :: end", s_effective_file_name);
			v_log(s_buf);
			s_buf.Format("-----\n\n", s_effective_file_name);
			v_log(s_buf);


		}//if  (s_algorithm_name  ==  SYSTEM_PARAM_ALGORITHM_RANDOM_SEARCH)




		if  (s_algorithm_name  ==  SYSTEM_PARAM_ALGORITHM_SIMMULATED_ANNEALING)
		{
			CVirusGA  c_vga;
			CFiberNet  c_fiber_net;


			c_err = c_fiber_net.eSetCores(i_cores);
			if  (c_err)  
			{
				fclose(pf_buf);
				return(c_err);
			}//if  (c_err)  

			c_err = c_vga.eSimulatedAnnealingLoadSettings(pf_buf);
			fclose(pf_buf);
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadTopology(s_net_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTopology("!top_test.txt");
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eLoadPaths(s_path_file, s_path_backup_file);
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePaths("!path_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadPathsSpec(s_uni_spec_file, s_uni_spec_file_backup, d_transfer_multiplier);
			if  (c_err)  return(c_err);

			
			c_err  = c_fiber_net.eLoadPathsReg(s_uni_reg_file);
			if  (c_err)  return(c_err);



			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsSpec("spec_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSavePathsReg("reg_test.txt");
			if  (c_err)  return(c_err);



			c_err  = c_fiber_net.eLoadTrees(s_tree_file);
			if  (c_err)  return(c_err);


			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTrees("!tree_test.txt");
			if  (c_err)  return(c_err);


			c_err  = c_fiber_net.eLoadTreesSpec(s_tree_spec_file, d_transfer_multiplier);
			if  (c_err)  return(c_err);
			
			c_err  = c_fiber_net.eLoadTreesReg(s_tree_reg_file);
			if  (c_err)  return(c_err);


			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTreesSpec("!tree_spec_test.txt");
			if  (c_err)  return(c_err);

			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveTreesReg("!tree_reg_test.txt");
			if  (c_err)  return(c_err);



			c_err  = c_fiber_net.eLoadReplicas(s_rep_file);
			if  (c_err)  return(c_err);

			c_err  = c_fiber_net.eSaveReplicas("!rep_test.txt");
			if  (c_err)  return(c_err);




			c_err  = c_fiber_net.eLoadDemands(s_dem_file, s_dea_file, s_dmc_file, s_dea_file_1_dir, i_paths_to_use, i_trees_to_use);
			if  (c_err)  return(c_err);


			if  (DEBUG_SAVE)  c_err  = c_fiber_net.eSaveDemands("!dem_test.txt");
			if  (c_err)  return(c_err);


						
			CFiberSolution  c_sol(&c_fiber_net);
			c_sol.vInitStatic();
			
			c_vga.eSetFitness(&c_sol);
			if  (c_err)  return(c_err);




			if  (d_optimal_value  >  0)
				c_err  =  c_vga.eRunSimulatedAnnealing(d_time, d_time_probing, s_effective_file_name,  s_file_summarize, "", &d_optimal_value, listInfo);
			else
				c_err  =  c_vga.eRunSimulatedAnnealing(d_time, d_time_probing, s_effective_file_name,  s_file_summarize, s_solf_file, NULL, listInfo);

			if  (c_err)  return(c_err);

			//s_buf.Format("%s :: end", s_effective_file_name);
			//v_log(s_buf);
			//s_buf.Format("-----\n\n", s_effective_file_name);
			//v_log(s_buf);



		}//if  (s_algorithm_name  ==  SYSTEM_PARAM_ALGORITHM_SIMMULATED_ANNEALING)



	}//try
	catch (Exception  *p_ex)
	{
		s_buf = p_ex->ToString();
		s_buf = s_buf + "\n General muppets error";
		c_err.vPutError(s_buf);
	}//catch (Exception  p_ex)



	FILE  *pf_run_save;
	s_effective_file_name.Format("%s_%.2d_RUN.txt", s_file_name, i_file_counter);
	pf_run_save  =  fopen(s_effective_file_name,  "w+");
	if  (pf_run_save  !=  NULL)
	{
		for  (int  ii = 0; ii  <  listInfo->Items->get_Count();  ii++)
		{
			s_buf  =  listInfo->Items->get_Item(ii)->ToString();		
			fprintf(pf_run_save,  "%s\n", s_buf);
		}//for  (int  ii = 0; ii  <  listInfo->Items->get_Count();  ii++)
			
	
		fclose(pf_run_save);
	}//if  (pf_run_save  !=  NULL)
	else
	{
		s_buf.Format("Couldnt open (%s)", s_effective_file_name);
		c_err.vPutError(s_buf);
		return(c_err);
	}//else  if  (pf_run_save  !=  NULL)

	return(c_err);
}//CError  CSystem::eRun(CString  sSettings)






CError  CSystem::eRunSerie(CString  sFileList, System::Windows::Forms::ListBox*  listInfo)
{
	CError  c_err;

	FILE  *pf_buf;


	CString  s_dir = Tools::sGetDirectory(sFileList);
	
	pf_buf  =  fopen(sFileList,"r");
	if  (pf_buf  ==  NULL)
	{
		CString  s_buf;

		s_buf.Format("Couldnt open settings files series file (%s)", sFileList);
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  (pf_buf  ==  NULL)


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
				v_file_names.push_back(s_file_name);
		}//if  (s_file_name <> "")	
	}//for  (;(s_file_name <> "")&&(feof(pf_buf) != true);)
	fclose(pf_buf);

	

	for  (int  ii = 0; ii < (int) v_file_names.size(); ii++)
	{
		c_err  =  eRun(s_dir, v_file_names.at(ii), listInfo);
		if  (c_err)
		{
			c_err.vShowWindow();
			return(c_err);
		}//if  (c_err)	
		listInfo->Items->Clear();
	}//for  (int  ii = 0; ii < (int) v_file_names.size(); ii++)


	return(c_err);
};//CError  CSystem::eRunSerie(CString  sFileList, System::Windows::Forms::ListBox*  listInfo)