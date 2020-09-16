#include "stdafx.h"

#include "MessyGA.h"


using  namespace  MessyGA;
using namespace TimeCounters;
using namespace Tools;
using namespace std;


bool  bGlobalDebug = false;
bool  bGlobalDebug2 = false;
bool  bGlobalDebugConstructSol = false;


bool bPatternGreater (CMessyPattern *elem1, CMessyPattern *elem2);//predefinition


class CChannelSortPair
{
public:
	int  i_channel_offset;
	int  i_channel_start_slot;
};//class CChannelSortPair


//int  i_global_debug = 0; //prw remove_02.13

//---------------------------class  CVirusGA--------------------------------------
void  CTabuMove::vApply(CmGA  *pcCurSolution, CmGA  *pcCandidateSolution, CFiberSolution  *pcFitness, gcroot<Random*> pcRandomGen)
{
	pcCurSolution->vCopyTo(pcCandidateSolution);


	if  (i_move_type == TABU_MOVE_TYPE_DEMAND)
	{
		double  d_buf;
		d_buf = pcCandidateSolution->pc_competetive_template[i_demand_0].dGetOrder();
		pcCandidateSolution->pc_competetive_template[i_demand_0].d_order = pcCandidateSolution->pc_competetive_template[i_demand_1].dGetOrder();
		pcCandidateSolution->pc_competetive_template[i_demand_1].d_order = d_buf;
		pcCandidateSolution->d_templ_fitness = -1;		
	}//if  (i_move_type == TABU_MOVE_TYPE_DEMAND)



	if  (i_move_type == TABU_MOVE_TYPE_PATH)
	{
		pcCandidateSolution->pc_competetive_template[i_demand_for_path_change].pc_path = pc_path_to_change;
		pcCandidateSolution->d_templ_fitness = -1;	
	}//if  (i_move_type == TABU_MOVE_TYPE_PATH)



	if  (i_move_type == TABU_MOVE_TYPE_REPLICA)
	{
		int  i_demand_offset;

		i_demand_offset = pcCandidateSolution->pc_competetive_template[i_demand_for_replica_change].iGetDemNumber();
		//ktory wezel to relika

		vector<int>  v_path_offsets;
		if  (pcFitness->pcGetNet()->pcGetDemands()[i_demand_offset].iGetStartNode() == -1)
		{
			v_path_offsets = pcFitness->pcGetNet()->pvGetPathsBetweenNodes()[i_new_replica][pcCandidateSolution->pc_competetive_template[i_demand_for_replica_change].pcGetPath()->iGetEndNode()];
		}//if  (pcFitness->pcGetNet()->pcGetDemands()[i_demand_offset].iGetStartNode() == -1)
		else
		{
			v_path_offsets = pcFitness->pcGetNet()->pvGetPathsBetweenNodes()[pcCandidateSolution->pc_competetive_template[i_demand_for_replica_change].pcGetPath()->iGetEndNode()][i_new_replica];
		}//else  if  (pcFitness->pcGetNet()->pcGetDemands()[i_demand_offset].iGetStartNode() == -1)

		
		int  i_chosen_path = pcRandomGen->Next(0,  v_path_offsets.size());
		pcCandidateSolution->pc_competetive_template[i_demand_for_replica_change].pc_path = &(pcFitness->pcGetNet()->pcPaths()[v_path_offsets.at(i_chosen_path)]);

		pcCandidateSolution->d_templ_fitness = -1;	
	}//if  (i_move_type == TABU_MOVE_TYPE_REPLICA)*/
	
}//void  CTabuMove::vApply(CmGA  *pcCurSolution, CmGA  *pcCandidateSolution)




//---------------------------class  CVirusGA--------------------------------------
CVirusGA::CVirusGA()
{
	//
	srand(time(NULL));
	i_seed = ::rand();
	//i_seed = 16641;

	if  (DEBUG_SAVE_MESSY)  
	{
		FILE  *pf_seed = fopen("_seed.txt", "a");
		fprintf(pf_seed, "%d\n", i_seed);
		fclose(pf_seed);
	}//if  (DEBUG_SAVE)

	pc_random_gen = new Random(i_seed);

	pc_fitness = NULL;
	pv_population = NULL;
	pv_pattern_pool = new vector  <CMessyPattern  *>;

	i_bayesian_effect = 0;
	i_solution_construction_method = 0;
	
	i_virginity_rounds = 200;
}//CVirusGA::CVirusGA()




CVirusGA::~CVirusGA()
{
	CError  c_err;
	CString  s_buf;


	if  (pc_fitness != NULL) delete  pc_fitness;

	
	try
	{
		if  (pv_population  !=  NULL)
		{
			for  (int  ii = 0; ii < (int) pv_population->size(); ii++)
				delete  pv_population->at(ii);
			delete  pv_population;
		}//if  (pv_population  !=  NULL)
	}//try
	catch (Exception  *p_ex)
	{
		s_buf = p_ex->ToString();
		s_buf = s_buf + "\n deleting pv_population in CVirusGA";
		c_err.vPutError(s_buf);
	}//catch (Exception  p_ex)



	try
	{
		if  (pv_pattern_pool  !=  NULL)
		{
			for  (int  ii = 0; ii < (int) pv_pattern_pool->size(); ii++)
				delete  pv_pattern_pool->at(ii);
			delete  pv_pattern_pool;
		}//if  (pv_pattern_pool  !=  NULL)
	}//try
	catch (Exception  *p_ex)
	{
		s_buf = p_ex->ToString();
		s_buf = s_buf + "\n deleting pv_pattern_pool in CVirusGA";
		c_err.vPutError(s_buf);
	}//catch (Exception  p_ex)

}//CVirusGA::~CVirusGA()





CError  CVirusGA::eRandomSearchLoadSettings(FILE  *pfSettings)
{
	CError  c_err;
	CString  s_setting_name,  s_buf;



	i_solution_construction_method  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD)

	
	return(c_err);
}//CError  CVirusGA::eRandomSearchLoadSettings(FILE  *pfSettings)



CError  CVirusGA::eTabuSearchLoadSettings(FILE  *pfSettings)
{
	CError  c_err;
	CString  s_setting_name,  s_buf;



	i_solution_construction_method  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD)

	
	return(c_err);
}//CError  CVirusGA::eTabuSearchLoadSettings(FILE  *pfSettings)





CError  CVirusGA::eSimulatedAnnealingLoadSettings(FILE  *pfSettings)
{
	CError  c_err;
	CString  s_setting_name,  s_buf;



	i_solution_construction_method  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD)



	i_sa_max_iteration  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_SIMULATED_ANNEALING_MAX_ITER_NUMBER)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_SIMULATED_ANNEALING_MAX_ITER_NUMBER, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD)

	

	
	return(c_err);
}//CError  CVirusGA::eSimulatedAnnealingLoadSettings(FILE  *pfSettings)






CError  CVirusGA::eSgaLoadSettings(FILE  *pfSettings)
{
	CError  c_err;
	CString  s_setting_name,  s_buf;



	i_sga_pop_size  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  SGA_PARAM_POP_SIZE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SGA_PARAM_POP_SIZE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SGA_PARAM_POP_SIZE)


	d_sga_cross_prob  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  SGA_PARAM_PROB_CROSS)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SGA_PARAM_PROB_CROSS, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SGA_PARAM_PROB_CROSS)


	d_sga_mut_prob  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  SGA_PARAM_PROB_MUT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", SGA_PARAM_PROB_MUT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  SGA_PARAM_PROB_MUT)


	
	i_solution_construction_method  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD)

	
	return(c_err);
}//CError  CVirusGA::eSgaLoadSettings(FILE  *pfSettings)



CError  CVirusGA::eLoadSettings(FILE  *pfSettings)
{
	CError  c_err;
	CString  s_setting_name,  s_buf;


	i_pattern_max_number  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_PATTERN_POOL_SIZE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_PATTERN_POOL_SIZE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_PATTERN_POOL_SIZE)



	i_the_same_template_check  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_SAME_PATTERN_CHECK)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_SAME_PATTERN_CHECK, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_minimal_template_length  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_MINIMAL_PATTERN_LEN)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_MINIMAL_PATTERN_LEN, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)




	i_virus_gen  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_GENERATIONS)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_GENERATIONS, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_virus_pop_size  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_POP_SIZE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_POP_SIZE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_vir_prob_cut  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_PROB_CUT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_PROB_CUT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_vir_prob_splice  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_PROB_SPLICE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_PROB_SPLICE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_vir_prob_mut  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_PROB_MUT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_PROB_MUT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_prob_mut_rem_gene  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_REM_GENE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_REM_GENE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_prob_mut_add_gene  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_ADD_GENE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_ADD_GENE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)




	i_virginity_rounds  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIRGIN_ROUNDS)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIRGIN_ROUNDS, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_VIRGIN_ROUNDS)


	i_ct_add_remove_strategy  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_CT_ADD_REMOVE_STRATEGY)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_CT_ADD_REMOVE_STRATEGY, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_bayesian_effect  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_BAYESIAN_EFFECT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_BAYESIAN_EFFECT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_solution_construction_method  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD)


	
	
	return(c_err);




	

	i_ct_add_by_turn  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_CT_ADD_BY_TURN)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_CT_ADD_BY_TURN, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)

	i_ct_take_back_enable  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_CT_TAKE_BACK_ENABLED)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_CT_TAKE_BACK_ENABLED, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)

	i_divide_virus_population_among_ct  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_DIVIDE_VIRUS_AMONG_CT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_DIVIDE_VIRUS_AMONG_CT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)

	i_actualize_feno_and_genotype_after_tree_construction  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_ACTUALIZE_FENO_GENO_AFTER_TREE_CONST)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_ACTUALIZE_FENO_GENO_AFTER_TREE_CONST, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)




	return(c_err);
}//CError  CVirusGA::eLoadSettings(FILE  *pfSettings)




CError  CVirusGA::eSetFitness(CFiberSolution  *pcFitnessFunc)
{
	CError  c_err;

	if  (pc_fitness  !=  NULL)  
	{
		delete  pc_fitness;
		pc_fitness  =  NULL;
	}//if  (pc_fitness  !=  NULL)  

	pc_fitness  =  new  CFiberSolution(pcFitnessFunc);

	i_templ_length = pc_fitness->pcGetNet()->iGetDemandNumber();


	return(c_err);
}//CError  CVirusGA::eSetFitness(int  iFuncType)




void  CVirusGA::vSaveParameters(FILE  *pfReport)
{
	fprintf(pfReport, "%d\\\\%s\n",  i_minimal_template_length, VGA_PARAM_MINIMAL_PATTERN_LEN);
	
	fprintf(pfReport, "%d\\\\%s\n",  i_virus_gen, VGA_PARAM_VIR_GENERATIONS);
	fprintf(pfReport, "%d\\\\%s\n",  i_virus_pop_size, VGA_PARAM_VIR_POP);
	
	fprintf(pfReport, "%.6lf\\\\%s\n",  d_vir_prob_cut, VGA_PARAM_VIR_PROB_CUT);
	fprintf(pfReport, "%.6lf\\\\%s\n",  d_vir_prob_splice, VGA_PARAM_VIR_PROB_SPLICE);
	fprintf(pfReport, "%.6lf\\\\%s\n",  d_vir_prob_mut, VGA_PARAM_VIR_PROB_MUT);
	fprintf(pfReport, "%.6lf\\\\%s\n",  d_prob_mut_rem_gene, VGA_PARAM_VIR_REM_GENE);
	fprintf(pfReport, "%.6lf\\\\%s\n",  d_prob_mut_add_gene, VGA_PARAM_VIR_ADD_GENE);


	fprintf(pfReport, "\n\n\n\n\n");

}//void  CVirusGA::vSaveParameters(FILE  *pfReport)


void  CVirusGA::vSgaSaveParameters(FILE  *pfReport)
{
	fprintf(pfReport, "%d\\\\%s\n",  i_sga_pop_size, SGA_PARAM_POP_SIZE);

	fprintf(pfReport, "%.6lf\\\\%s\n",  d_sga_cross_prob, SGA_PARAM_PROB_CROSS);
	fprintf(pfReport, "%.6lf\\\\%s\n",  d_sga_mut_prob, SGA_PARAM_PROB_MUT);


	fprintf(pfReport, "\n\n\n\n\n");

}//void  CVirusGA::vSaveParameters(FILE  *pfReport)




bool  CVirusGA::b_the_same_templates_check(CMessyIndividual  *pcTemplCandidate)
{
	vector  <int>  v_templates_offsets,  v_chosen_templates_offsets;//for "the same templates" check
	int  i_chosen_template_offset;


	if  (i_the_same_template_check  >  0)
	{
		//now checking if the same template is not in random piece of the current templates
									
		v_chosen_templates_offsets.clear();

		//now choosing proper number of random templates
		if  ((int) pv_pattern_pool->size()  <=  i_the_same_template_check)
		{
			//just copy if we are to take all anyway...
			for  (int  ii = 0; ii < (int)  pv_pattern_pool->size();  ii++)
				v_chosen_templates_offsets.push_back(ii);
		}//if  (pv_pattern_pool->size()  <=  i_the_same_template_check)
		else
		{
			v_templates_offsets.clear();
			for  (int  ii = 0; ii < (int) pv_pattern_pool->size();  ii++)
				v_templates_offsets.push_back(ii);

			//we randomly choose offsets...
			for  (int  ii = 0; ii < i_the_same_template_check;  ii++)
			{
				i_chosen_template_offset  =  pc_random_gen->Next(0,  v_templates_offsets.size());
				v_chosen_templates_offsets.push_back(v_templates_offsets.at(i_chosen_template_offset));
				v_templates_offsets.erase(v_templates_offsets.begin()  +  i_chosen_template_offset);
			}//for  (int  ii = 0; ii < i_the_same_template_check;  ii++)

		
		}//else  if  (pv_pattern_pool->size()  <=  i_the_same_template_check)

	}//if  (i_the_same_template_check  >  0)
	
	for  (int  ii = 0; ii < (int) v_chosen_templates_offsets.size();  ii++)
	{

		if  (
			pcTemplCandidate->bCompareFenotypes
				(
				pv_pattern_pool->at(v_chosen_templates_offsets.at(ii))
				)
			==
			true
			)
		return(false);
	}//for

	return(true);
}//bool  CVirusGA::b_the_same_templates_check()



void  CVirusGA::v_pattern_debug_control(int  iNum, bool bBuf)
{
	/*for  (int  ii = 0; ii < pv_pattern_pool->size(); ii++)
	{
		CString  s_buf;
		s_buf.Format("pattern [%d]: %d  %d", ii, pv_pattern_pool->at(ii)->pc_fitness->pc_network->iGetTreeNum(), pv_pattern_pool->at(ii)->pc_fitness->pc_network->iGetNodesNum());
		::MessageBox(NULL, s_buf, s_buf, MB_OK);
	}//	for  (int  ii = 0; ii < pv_pattern_pool->size(); ii++)*/

	if  (pv_pattern_pool->size()  >=  2)
	{
		CString  s_buf;
		s_buf.Format("(%d) pattern [1]: %d", iNum, pv_pattern_pool->at(1)->pc_fitness->pcGetNet()->iGetDemandNumber());
		::MessageBox(NULL, s_buf, s_buf, MB_OK);	
	}//if  (pv_pattern_pool->size()  >=  2)
}//void  CVirusGA::v_pattern_debug_control()



void  CVirusGA::v_get_new_pattern_proposition_into_pattern_pool(CmGA  *pcCT)
{
	CMessyIndividual  *pc_pattern_proposition;
	pc_pattern_proposition = pcCT->pcGetPatternProposition();
	
	if  (pc_pattern_proposition  !=  NULL)
	{
		CMessyPattern  *pc_buf;
		pc_buf  =  new   CMessyPattern(pc_fitness);
		*pc_buf  =  *(pc_pattern_proposition);
		pc_buf->vSetFitness(pc_fitness);

		pv_pattern_pool->push_back(pc_buf);
	}//if  (pc_pattern_proposition  !=  NULL)

	pcCT->vRemovePatternProposition();
}//void  CVirusGA::v_get_new_pattern_proposition_into_pattern_pool(CmGA  *pcCT)



void  CVirusGA::v_get_new_patterns()
{
	for (int  ii = 0; ii < (int) pv_population->size();  ii++)
	{
		if  (i_ct_add_remove_strategy  ==  2)
		{
			v_get_new_pattern_proposition_into_pattern_pool(pv_population->at(ii));
		}//if  (i_ct_add_remove_strategy  ==  2)
		else
		{
			if  (pv_population->at(ii)->pcGetInfectionHistory()  !=  NULL)
			{
				if  (pv_population->at(ii)->pcGetInfectionHistory()->pvGetGenotype()->size()  >  0)
				{
					if  (
							(
							pv_population->at(ii)->dGetCompTemplateFitness()
							>
							pv_population->at(ii)->dGetCompTemplateFitnessBeforeCrossing()						
							)
							&&
							(
							i_minimal_template_length  <=
							pv_population->at(ii)->pcGetInfectionHistory()->iGetFenotypeLength()
							)
						)
					{
						if  (b_the_same_templates_check(pv_population->at(ii)->pcGetInfectionHistory())  ==  true)
						{
							CMessyPattern  *pc_buf;
							pc_buf  =  new   CMessyPattern(pc_fitness);
							*pc_buf  =  *(pv_population->at(ii)->pcGetInfectionHistory());
							pc_buf->vSetFitness(pc_fitness);//musi byc po porzednim wywolaniu, bo operator = przepisuje pc_fitness i jesli pozniej CmGA zostanie usuniety, a pc_fitnes nalezacy do CmGA razem z nim to pc_fitnes patternu bedzie wskazywal w kosmos
							pv_pattern_pool->push_back(pc_buf);
							pv_population->at(ii)->vSetInfectionHistory(NULL);
						}//if  (b_chosen_templates_check_passed  ==  true)

					}//if  (
				}//if  (pv_population->at(ii)->pcGetInfectionHistory()->pvGetGenotype()->size()  >  0)
			}//if  (pv_population->at(ii)->pcGetInfectionHistory()  !=  NULL)				
		}//else  if  (i_ct_add_remove_strategy  ==  2)

	}//for (int  ii = 0; ii < (int) pv_population->size();  ii++)
}//void  CVirusGA::v_get_new_templates()



inline bool comparePriority(int a, int b)
{
	return a>b ? true : false;
}


inline bool comparePaths(pair<int,int> a, pair<int,int> b)
{
	return a.second>b.second ? true : false;
}




int  CVirusGA::iGetFirstOffsetInEmployedPathsForTabu(int iStartNode, int  iEndNode)
{
	int  i_result;


	for  (int  ii = 0; ii < pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu()->size();  ii++)
	{
		if  (
			(pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu()->at(ii)->iGetStartNode() == iStartNode)&&
			(pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu()->at(ii)->iGetEndNode() == iEndNode)
			)
			return(ii);
	}//for  (int  ii = 0; ii < pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu()->size();  ii++)
	
	return(-1);
}//int  CVirusGA::iGetFirstOffsetInEmployedPathsForTabu(int iStartNode, int  iEndNode)





bool CVirusGA::IsTabu(CTabuMove *pcMove, vector<CTabuMove> *pvTabuList, vector<CTabuMove> *pvUsedMoves, CmGA *pcSolution)
{
    // przeszukanie TabuList
	//int q = 2*problem.qAnyGet() + problem.qUniGet()+problem.qMulti;
	int q = pc_fitness->pcGetNet()->iGetDemandNumber();//problem.qAnyGet();

	// przeszukaj Tabu List
	for(int i=0; i<pvTabuList->size(); i++)
		if(pvTabuList->at(i) == *pcMove)
			return true;

	// przeszukaj Used Moves
	for(int i=0; i<pvUsedMoves->size(); i++)
		if(pvUsedMoves->at(i) == *pcMove)
			return true;

	// sprawdz czy ruch zerowy
	//if(move.GetMoveType() == "p")
	if  (pcMove->i_move_type == TABU_MOVE_TYPE_PATH)
	{
		for(int i=0; i<q; i++)
		{
			//if(solution[i].IdGet() == move.param1Get())
			if  (pcSolution->pc_competetive_template[i].iGetDemNumber() == pcMove->i_demand_for_path_change)
			{
				//if(solution[i].PathIdGet() == move.param2Get())
				if  (pcSolution->pc_competetive_template[i].pcGetPath() == pcMove->pc_path_to_change)
					return true;
				else
					return false;
			}
		}
	}
	else
	if  (pcMove->i_move_type == TABU_MOVE_TYPE_REPLICA)
	{
		for(int i=0; i<q; i++)
		{
			//if(solution[i].IdGet() == move.param1Get())
			if  (pcSolution->pc_competetive_template[i].iGetDemNumber() == pcMove->i_demand_for_replica_change)
			{
				//if(solution[i].replica == move.param2Get())

				//sprawdzamy ktore wezel jest replika
				if  (pc_fitness->pcGetNet()->pcGetDemands()[pcSolution->pc_competetive_template[i].iGetDemNumber()].iGetStartNode() == -1)
				{
					if  (pcSolution->pc_competetive_template[i].pcGetPath()->iGetStartNode() == pcMove->i_new_replica)
						return true;
					else 
						return false;
				
				}//if  (pc_fitness->pcGetNet()->pcGetDemands()[pcSolution->pc_competetive_template[i].iGetDemNumber()].iGetStartNode() == -1)
				else
				{
					if  (pcSolution->pc_competetive_template[i].pcGetPath()->iGetEndNode() == pcMove->i_new_replica)
						return true;
					else 
						return false;				
				}//else  if  (pc_fitness->pcGetNet()->pcGetDemands()[pcSolution->pc_competetive_template[i].iGetDemNumber()].iGetStartNode() == -1)
				
			}				
		}
	}

	/*else if(move.GetMoveType() == "mp")
	{
		for(int i=0; i<q; i++)
		{
			if(solution[i].IdGet() == move.param1Get())
			{
				if(solution[i].p_mod == move.param2Get())
					return true;
				else 
					return false;
			}				
		}
	}
	
	else if(move.GetMoveType() == "mb")
	{
		for(int i=0; i<q; i++)
		{
			//if(solution[i].IdGet() == move.param1Get())
			if  (pcSolution->pc_competetive_template[i].iGetDemNumber()
			{
				if(solution[i].b_mod == move.param2Get())
					return true;
				else 
					return false;
			}				
		}
	}

	else if(move.GetMoveType() == "t")
	{
		for(int i=0; i<q; i++)
		{
			if(solution[i].IdGet() == move.param1Get())
			{
				if(solution[i].path == move.param2Get())
					return true;
				else 
					return false;
			}				
		}
	}*/

            
    return false;
}//bool CVirusGA::IsTabu(CTabuMove *pcMove, vector<TabuMove> *pvTabuList, vector<TabuMove> *pvUsedMoves, CmGA *pcSolution)







CTabuMove  CVirusGA::cGenerateMoveRandom(vector<CTabuMove> *pvTabuList,	vector<CTabuMove> *pvUsedMoves, CmGA *pcSolution)
{
	CString  s_buf;

	// dane problrmu
	int qMulti = pc_fitness->pcGetNet()->iGetDemandNumberMulti();
	int qAny = pc_fitness->pcGetNet()->iGetDemandNumberAny1Direction();
	int qUni = pc_fitness->pcGetNet()->iGetDemandNumberUni();
	int q = qUni + qAny + qMulti;
	int r = pc_fitness->pcGetNet()->iGetReplicesNum();
	int k = pc_fitness->pcGetNet()->pcGetDemands()[0].pvGetPossiblePaths()->size() / pc_fitness->pcGetNet()->iGetReplicesNum();
	int m = pc_fitness->pcGetNet()->iGetConnsNum();
	int n = pc_fitness->pcGetNet()->iGetNodesNum();
	//int kMulti = problem.kMulti;
	int kMulti = 0;
	//int mod = problem.modGet();

	// sterowanie petla
	int moveMax = (q*(q-1)/2 + qAny*(r-1) + (qAny+qUni)*(k-1) + qMulti * (kMulti-1)) / 3;
	int moveGenerated = 0;

	// generowanie wg prawdopodobienstwa
	CTabuMove move;
	int l1, l2;

	bool demand = q>1;
	bool path = ((qAny+qUni)>0 && k>1);
	bool replica = (qAny>0 && r>1);
	bool tree = (qMulti>0 && kMulti>1);

	// e - jak error, nie mozna nic wygenerowac
	if(!demand && !path && !replica && !tree)
		return CTabuMove();

	CString wybor = "m";

	for(int i=0; i<moveMax; i++)
	{
		// wylosuj rodzaj ruchu
		
		if(demand && path && replica && tree)
		{
			//l1 = int(4*rand()/(RAND_MAX + 1.0));
			l1 = pc_random_gen->Next(0, 4);
			//cout << l1 << endl;

			if(l1==0)
				wybor = "d";
			else if(l1==1)
				wybor = "p";
			else if(l1==2)
				wybor = "r";
			else 
				wybor = "t";
		}

		else if(demand && path && replica)
		{
			//l1 = int(3*rand()/(RAND_MAX + 1.0));
			l1 = pc_random_gen->Next(0, 3);

			if(l1==0)
				wybor = "d";
			else if(l1==1)
				wybor = "p";
			else
				wybor = "r";
		}

		else if(demand && replica && tree)
		{
			//l1 = int(3*rand()/(RAND_MAX + 1.0));
			l1 = pc_random_gen->Next(0, 3);

			if(l1==0)
				wybor = "d";
			else if(l1==1)
				wybor = "t";
			else
				wybor = "r";
		}

		else if(replica && tree && path)
		{
			//l1 = int(3*rand()/(RAND_MAX + 1.0));
			l1 = pc_random_gen->Next(0, 3);

			if(l1==0)
				wybor = "t";
			else if(l1==1)
				wybor = "p";
			else
				wybor = "r";
		}

		else if(demand && path && tree)
		{
			//l1 = int(3*rand()/(RAND_MAX + 1.0));
			l1 = pc_random_gen->Next(0, 3);

			if(l1==0)
				wybor = "d";
			else if(l1==1)
				wybor = "p";
			else
				wybor = "t";
		}

		else if(demand && path)
		{
			//l1 = int(2*rand()/(RAND_MAX + 1.0));
			l1 = pc_random_gen->Next(0, 2);

			if(l1==0)
				wybor = "d";
			else
				wybor = "p";
		}

		else if(demand && replica)
		{
			//l1 = int(2*rand()/(RAND_MAX + 1.0));
			l1 = pc_random_gen->Next(0, 2);

			if(l1==0)
				wybor = "d";
			else
				wybor = "r";
		}

		else if(demand && tree)
		{
			//l1 = int(2*rand()/(RAND_MAX + 1.0));
			l1 = pc_random_gen->Next(0, 2);

			if(l1==0)
				wybor = "d";
			else
				wybor = "t";
		}

		else if(path && tree)
		{
			//l1 = int(2*rand()/(RAND_MAX + 1.0));
			l1 = pc_random_gen->Next(0, 2);

			if(l1==0)
				wybor = "t";
			else
				wybor = "p";
		}

		else if(path && replica)
		{
			//l1 = int(2*rand()/(RAND_MAX + 1.0));
			l1 = pc_random_gen->Next(0, 2);

			if(l1==0)
				wybor = "r";
			else
				wybor = "p";
		}

		else if(replica && tree)
		{
			//l1 = int(2*rand()/(RAND_MAX + 1.0));
			l1 = pc_random_gen->Next(0, 2);

			if(l1==0)
				wybor = "r";
			else
				wybor = "t";
		}
		

		else if(tree)
			wybor = "t";

		else if(path)
			wybor = "p";

		else if(demand)
			wybor = "d";

		else if(replica)
			wybor = "r";


		// wygeneruj ruch odpowiedniego typu 

		if(wybor == "d")
		{
			int  i_r1, i_r2;
			//l1 = int((qUni+qAny)*rand()/(RAND_MAX + 1.0));
			//l2 = int((qUni+qAny)*rand()/(RAND_MAX + 1.0));

			l1 = pc_random_gen->Next(0, qUni+qAny);
			l2 = pc_random_gen->Next(0, qUni+qAny);
			

			
			while (l1 == l2)
				l2 = pc_random_gen->Next(0, qUni+qAny);
				//l2 = int((qUni+qAny)*rand()/(RAND_MAX + 1.0));

			//move.param1Set(l1);
			//move.param2Set(l2);
			//move.SetMoveType("d");
			move.i_demand_0 = l1;
			move.i_demand_1 = l2;
			move.i_move_type = TABU_MOVE_TYPE_DEMAND;


			int  i_dems_all;
			i_dems_all = pc_fitness->pcGetNet()->iGetDemandNumberMulti() + pc_fitness->pcGetNet()->iGetDemandNumberAny1Direction() + pc_fitness->pcGetNet()->iGetDemandNumberUni();

			if  ( 
				(l1 < 0)||(l2 < 0)||
				(l1 >= i_dems_all)||(l2 >= i_dems_all)
				)
			{
				s_buf.Format("%d %d  all_dems:%d (multi: %d  any1: %d  uni: %d)", l1, l2, i_dems_all,
					pc_fitness->pcGetNet()->iGetDemandNumberMulti(), pc_fitness->pcGetNet()->iGetDemandNumberAny1Direction(), pc_fitness->pcGetNet()->iGetDemandNumberUni());

				::vRepInFile("zzz_random.txt",s_buf,true);
				
				s_buf.Format("r1: %d r2: %d MAX_RAND: %d", i_r1, i_r2, RAND_MAX);
				::vRepInFile("zzz_random.txt",s_buf);

				s_buf.Format("qUni: %d +qAny: %d ", qUni, qAny);
				::vRepInFile("zzz_random.txt",s_buf);

				
				::vShow(s_buf);
			}//if  ( 




							
			if(!IsTabu(&move, pvTabuList, pvUsedMoves, pcSolution))
				return move;
		}

		else if(wybor == "r")
		{
			//move.param1Set(int(qAny*rand()/(RAND_MAX + 1.0)));
			//move.param2Set(repliki[int(r*rand()/(RAND_MAX + 1.0))]);
			//move.SetMoveType("r");
			move.i_demand_for_replica_change = pc_fitness->pcGetNet()->iGetDemandNumberUni() + pc_fitness->pcGetNet()->iGetDemandNumberMulti() +  pc_random_gen->Next(0, qAny); //int(qAny*rand()/(RAND_MAX + 1.0));
			//move.i_new_replica = pc_fitness->pcGetNet()->pcGetReplices()[int(r*rand()/(RAND_MAX + 1.0))]->iGetNodeId();
			move.i_new_replica = pc_fitness->pcGetNet()->pcGetReplices()[pc_random_gen->Next(0, r)]->iGetNodeId();
			move.i_move_type = TABU_MOVE_TYPE_REPLICA;
				
			if(!IsTabu(&move, pvTabuList, pvUsedMoves, pcSolution))
				return move;
		}

		else if(wybor == "p")
		{
			//move.param1Set(int((qUni+qAny)*rand()/(RAND_MAX + 1.0)));
			//move.param2Set(int(k*rand()/(RAND_MAX + 1.0)));
			//move.SetMoveType("p");
			//move.i_demand_for_path_change =  int((qUni+qAny)*rand()/(RAND_MAX + 1.0));
			move.i_demand_for_path_change =  pc_random_gen->Next(0, qUni+qAny);
			move.pc_path_to_change = 
				pc_fitness->pcGetNet()->pcGetDemands()[move.i_demand_for_path_change].pvGetPossiblePaths()->at
				(
					pc_random_gen->Next(0, pc_fitness->pcGetNet()->pcGetDemands()[move.i_demand_for_path_change].pvGetPossiblePaths()->size())
					//(pc_fitness->pcGetNet()->pcGetDemands()[move.i_demand_for_path_change].pvGetPossiblePaths()->size() * rand()) / (RAND_MAX + 1.0)
				);
			move.i_move_type = TABU_MOVE_TYPE_PATH;
				
			if(!IsTabu(&move, pvTabuList, pvUsedMoves, pcSolution))
				return move;
		}

		/*else if(wybor == "t")
		{
			move.param1Set(int((qUni+2*qAny)*rand()/(RAND_MAX + 1.0)));
			move.param2Set(int(kMulti*rand()/(RAND_MAX + 1.0)));
			move.SetMoveType("t");
				
			if(! IsTabu(move,TabuList,UsedMoves,problem,solution))
				return move;
		}*/
	}


	// wybierz najstarszy ruch z TabuList lub UsedMoves i zwroc go
	if(pvTabuList->size() > 0)
		return pvTabuList->at(0);
	else if(pvUsedMoves->size() > 0)
		return pvUsedMoves->at(0);
	else
	{
		CTabuMove  c_no_move;
		return c_no_move;
	}//else
}






CTabuMove  CVirusGA::cGenerateMoveIntelligent
	(
	vector<CTabuMove> *pvTabuList,	vector<CTabuMove> *pvUsedMoves, CmGA *pcSolution, double dProRep, double dProPath
	)
{
	CString  s_buf;

	int qAny = pc_fitness->pcGetNet()->iGetDemandNumberAny1Direction();//problem.qAnyGet();
	int qUni = pc_fitness->pcGetNet()->iGetDemandNumberUni();//problem.qUniGet();
	int q = qUni + qAny;
	int r = pc_fitness->pcGetNet()->iGetReplicesNum();//problem.rGet();
	int kAll = pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu()->size();
	int k = pc_fitness->pcGetNet()->pcGetDemands()[0].pvGetPossiblePaths()->size() / pc_fitness->pcGetNet()->iGetReplicesNum();
	int m = pc_fitness->pcGetNet()->iGetConnsNum();
	int n = pc_fitness->pcGetNet()->iGetNodesNum();
	
	int slicesLimit = pcSolution->iGetMaximumSlot();

	vector<CFiberPath *>  *pv_employed_paths;
	pv_employed_paths = pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu();


	// sterowanie petla
	int moveGenerated = 0;

	// generowanie wg prawdopodobienstwa
  //  srand(time(NULL));
	int l1;
	int repPriority=0;

	// wylicz kryteria dla krawedzi i sciezek
	
	int* linksUsage;
	try
	{
		linksUsage = new int[m];
	}//try
	catch(Exception *pex)
	{
		s_buf.Format("m=%d", m);
		::vShow(s_buf);
	}//catch(Exception *pex)

	int* pathMaxUsage = new int[pv_employed_paths->size()];

	// tworzenie tablicy alokacji
	int demId, pathNumber, finalPathNumber;



	
	// wyyliczenie zajetosci za kazdej krawedzi 
	for(int i=0; i<m; i++)
	{
		linksUsage[i] = 0;
		for(int j=slicesLimit-1; j>=0; j--)
		{
			//if(slicesAllocated[i][j] == true)
			if  (pc_fitness->pcGetNet()->pcGetConns()[i].bSlotInUse(j) == true)
			{
				linksUsage[i]++;
				//break;
			}
		}
	}


	// dla kazdej sciezki wyliczenie najwiekszej i sredniej wielkosci zajetosci 
	//int pom = 0;

	bool  b_is_link_in_path;
	for(int i=0; i<pv_employed_paths->size(); i++)
	{
		pathMaxUsage[i] = 0;
	//	pom = 0;

		for(int j=0; j<m; j++)
		{
			b_is_link_in_path = pv_employed_paths->at(i)->bIsConnPartOfPath(&(pc_fitness->pcGetNet()->pcGetConns()[j]));
			if  (b_is_link_in_path  ==  false)  b_is_link_in_path = pv_employed_paths->at(i)->pcGetSlavePath()->bIsConnPartOfPath(&(pc_fitness->pcGetNet()->pcGetConns()[j]));

			if  (b_is_link_in_path == true)
			{
				if  (pathMaxUsage[i] <= linksUsage[j])  pathMaxUsage[i] = linksUsage[j];
			}//if  (b_is_link_in_path == true)

			//if(primaryPath[i][j] || backupPath[i][j])
			//	pathMaxUsage[i] = pathMaxUsage[i]>linksUsage[j] ? pathMaxUsage[i] : linksUsage[j];
		}
	}


	delete[] linksUsage;

	// dla kazdej sciezki jest juz wyliczone 
	vector<CTabuMove> rep;

	// tabela spriorytetyzowanych ruchow 
	vector<CTabuMove> all[2];





	// dla replik
	if(r > 1 && qAny > 0 && dProRep != 0)
	{
		vector<int>* demPerRep= new vector<int>[r];

		for(int i=0; i<q; i++)
		{
			//if(solution[i].IdGet() < qAny)
			if  (pc_fitness->pcGetNet()->pcGetDemands()[pcSolution->pc_competetive_template[i].iGetDemNumber()].iDemandType() == DEMAND_TYPE_ANYCAST)
			{
				for(int j=0; j<r; j++)
				{
					if  (
							(
							pc_fitness->pcGetNet()->pcGetReplices()[j]->iGetNodeId()
							==
							pcSolution->pc_competetive_template[i].pcGetPath()->iGetStartNode()
							)
							||
							(
							pc_fitness->pcGetNet()->pcGetReplices()[j]->iGetNodeId()
							==
							pcSolution->pc_competetive_template[i].pcGetPath()->iGetEndNode()
							)
						)
					{
						demPerRep[j].push_back(pc_fitness->pcGetNet()->pcGetDemands()[pcSolution->pc_competetive_template[i].iGetDemNumber()].iGetId());
					}//if  (
					
					
					//if(repliki[j] == solution[i].replica)
					//	demPerRep[j].push_back(solution[i].IdGet());
				}
			}
		}

		int* sumDemPerRep = new int[r];
		for(int i=0; i<r; i++)
			sumDemPerRep[i] = demPerRep[i].size();

		// znajdz maks 
		int max = -1, min = q+1;
		int minIndeks = -1, maxIndeks = -1;

		for(int i=0; i<r; i++)
		{
			if(sumDemPerRep[i] < min)
			{
				min = sumDemPerRep[i];
				minIndeks = i;
			}

			if(sumDemPerRep[i] > max)
			{
				max = sumDemPerRep[i];
				maxIndeks = i;
			}
		}

		// wrzucamy do wektora
		double wsk = min / (min+max);
		repPriority = 5;

		if(wsk <= dProRep)
			repPriority = 0;
		else if(wsk <= 2*dProRep)
			repPriority = 1;

		if(repPriority < 2 && maxIndeks >= 0 && minIndeks >= 0)
		{
			for(int i=0; i<max; i++)
				rep.push_back(CTabuMove(TABU_MOVE_TYPE_REPLICA, demPerRep[maxIndeks][i], pc_fitness->pcGetNet()->pcGetReplices()[minIndeks]->iGetNodeId()));
		}

		// usun tablice dynamiczne 
		delete[] demPerRep;
		delete[] sumDemPerRep;
	}





	if(dProPath > 0)
	{
		int pathMoveCounter = 0;
		pair<int,int>* pathsPerDem = new pair<int,int>[k];

		// priorytety dla sciezek
		int* pomPriority = new int[kAll];
		for(int i=0; i<kAll; i++)
			pomPriority[i] = pathMaxUsage[i];
		sort(pomPriority, pomPriority+kAll, comparePriority);

		// granice podzialu 
		int procent = dProPath * kAll;
		if(procent == 0)
			procent == 1;

		int pathPriority[5];
		for(int i=0; i<5; i++)
			pathPriority[i] = pomPriority[i*procent+1];
		delete[] pomPriority;

		// dla kazdego zadania 
		for(int i=0; i<q; i++)
		{
			//demId = solution[i].IdGet();
			demId = pcSolution->pc_competetive_template[i].iGetDemNumber();

			// anycast dn
			//if(demId < qAny)
			//if  (pc_fitness->pcGetNet()->pcGetDemands()[pcSolution->pc_competetive_template[i].iGetDemNumber()].iDemandType() == DEMAND_TYPE_ANYCAST)
			//pathNumber = solution[i].PathIdGet();
			pathNumber = pcSolution->pc_competetive_template[i].pcGetPath()->iGetId();
			//finalPathNumber = k * GetNodesPairNumber(solution[i].replica, solution[i].dem->ClientNodeGet(), n);

			//zwraca offset pierwszej sceizki od wezla start do wezla end w pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu()
			finalPathNumber = iGetFirstOffsetInEmployedPathsForTabu(pcSolution->pc_competetive_template[i].pcGetPath()->iGetStartNode(), pcSolution->pc_competetive_template[i].pcGetPath()->iGetEndNode());

				
			// wrzuc do wektora
			for(int j=0; j<k; j++)
			{
				pathsPerDem[j].first = j;
				pathsPerDem[j].second = pathMaxUsage[finalPathNumber + j];
			}

			sort(pathsPerDem, pathsPerDem+k, comparePaths);

			for(int j=0; j<k; j++)
			{
				if(pathsPerDem[j].first == pathNumber)
				{
					if(j != k-1)
					{
						// priorytet 1
						if(pathsPerDem[j].second >= pathPriority[0])
						{
							all[0].push_back(CTabuMove(demId, pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu()->at(finalPathNumber + k-1)));
							//all[0].push_back(TabuMove(demId, k-1, "p"));
							pathMoveCounter++;
						}

						// priorytet 2
						else if(pathsPerDem[j].second >= pathPriority[1])
						{
							all[1].push_back(CTabuMove(demId, pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu()->at(finalPathNumber + k-1)));
							//all[1].push_back(TabuMove(demId, k-1, "p"));
							pathMoveCounter++;
						}

						// koniec przeszukiwan
						break;
					}
				}
			}

		}

			delete[] pathsPerDem;

	}



	// wrzucenie odpowiednien ilosci do koncowego wektora 

	// repliki
	for(int i=0; i<rep.size(); i++)
		all[repPriority].push_back(rep[i]);

	delete[] pathMaxUsage;
	// losowanie ruchu

	// prioruytet 0
	bool Tabu = false;
	while(all[0].size() > 0)
	{
		l1 = int(all[0].size() * rand() / (RAND_MAX + 1.0));
		Tabu = false;

		if(IsTabu(&(all[0][l1]),pvTabuList,pvUsedMoves,pcSolution))
		{
			Tabu = true;
			break;
		}

		if(!Tabu)
		{
			CTabuMove movePom =  all[0][l1];
			all[0].clear();
			all[1].clear();
			return movePom;
		}

		all[0].erase(all[0].begin() + l1);
	}

	// priorytet 1
	while(all[1].size() > 0)
	{
		//l1 = rand() % all[1].size();
		l1 = int(all[1].size() * rand() / (RAND_MAX + 1.0));
		Tabu = false;

		if(IsTabu(&(all[1][l1]),pvTabuList,pvUsedMoves,pcSolution))
		{
			Tabu = true;
			break;
		}

		if(!Tabu)
		{
			CTabuMove movePom =  all[1][l1];
			all[0].clear();
			all[1].clear();
			return movePom;
		}

		all[1].erase(all[1].begin() + l1);
	}

	CTabuMove  c_result;
	c_result = cGenerateMoveRandom(pvTabuList,pvUsedMoves, pcSolution);

	return(c_result);
}//CTabuMove  CVirusGA::cGenerateMoveIntelligent




void  CVirusGA::v_pattern_number_control_new()
{
	while  ((int)  pv_pattern_pool->size()  >  i_pattern_max_number)
	{
		int  i_indiv;

		i_indiv  =  pc_random_gen->Next(0,  pv_pattern_pool->size());

		delete  pv_pattern_pool->at(i_indiv);
		pv_pattern_pool->erase(pv_pattern_pool->begin()  +  i_indiv);
	}//while  ((int)  pv_new_pattern_pool->size()  <  i_max_infections_considered_at_crossing)

	return;
}//void  CVirusGA::v_pattern_number_control_new(bool  bLengthEntrophy)




void  CVirusGA::v_remove_the_same_ct()
{
	for (int  ii = 0; ii < (int) pv_population->size()-1;  ii++)
	{
		for (int  ij = ii+1; ij < (int) pv_population->size();  ij++)
		{
			if  (
				pv_population->at(ii)->bIsTheSame(pv_population->at(ij))
				==  true				
				)
			{
				delete  pv_population->at(ij);
				pv_population->erase(pv_population->begin() + ij);
				ij--;//the next is going to have the offset as we have removed one			
			}//if  (		
		}//for (int  ij = ii+1; ij < (int) pv_population->size();  ij++)	
	}//for (int  ii = 0; ii < (int) pv_population->size();  ii++)
}//void  CVirusGA::v_remove_the_same_ct()




void  CVirusGA::v_remove_all_ct_except_best()
{
	double  d_best = 0;

	for (int  ii = 0; ii < (int) pv_population->size();  ii++)
	{
		if  (d_best  <  pv_population->at(ii)->dGetCompTemplateFitness())  
		{
			d_best  =  pv_population->at(ii)->dGetCompTemplateFitness();
		}//if  (d_best  <  pv_population->at(ii)->dGetFOMLevel())  
	}//for (int  ii = 0; ii < (int) pv_population->size()-1;  ii++)

	//CString  s_buf;

	//s_buf.Format("best: %.8lf", d_best);
	//::MessageBox(NULL, s_buf, s_buf, MB_OK);

	for (int  ii = 0; ii < (int) pv_population->size();  ii++)
	{
		//s_buf.Format("best: %.8lf  actual:%.8lf", d_best, pv_population->at(ii)->dGetFOMLevel());
		//::MessageBox(NULL, s_buf, s_buf, MB_OK);

		if  (d_best  !=  pv_population->at(ii)->dGetCompTemplateFitness())  
		{
			//::MessageBox(NULL, "usuwamy", "usuwamy", MB_OK);
			delete  pv_population->at(ii);
			pv_population->erase(pv_population->begin() + ii);
			ii=-1;//the next is going to have the offset as we have removed one
		}//if  (		
	}//for (int  ii = 0; ii < (int) pv_population->size();  ii++)
}//void  CVirusGA::v_remove_the_same_ct()




void  CVirusGA::v_create_simple_ga_pop(vector  <CmGA  *>  *pvPopulationToMake)
{
	for  (int  ii = 0; ii < i_sga_pop_size; ii++)
	{
		pvPopulationToMake->push_back(new  CmGA(pc_random_gen, i_bayesian_effect, i_solution_construction_method));

		pvPopulationToMake->at(pvPopulationToMake->size() - 1)->eSetFitness(pc_fitness, true);
		pvPopulationToMake->at(pvPopulationToMake->size() - 1)->eConfigureTemplate();
		pvPopulationToMake->at(pvPopulationToMake->size() - 1)->eVirGenerateRandomTemplate();
	}//for  (int  ii = 0; ii < i_sga_pop_size; ii++)
	
}//void  CVirusGA::v_create_simple_ga_pop()




void  CVirusGA::v_add_new_ct(System::Windows::Forms::ListBox*  listInfo, double  *pdAverage, double *pdBestFitness)
{
	if  (i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_MIX)
	{
		if  (pv_population->size() > 0)
		{
			if  (pv_population->at(pv_population->size()-1)->i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_MIX)
				pv_population->push_back(new  CmGA(pc_random_gen, i_bayesian_effect, SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION));
		}//if  (pv_population->size() > 0)
		else
			pv_population->push_back(new  CmGA(pc_random_gen, i_bayesian_effect, i_solution_construction_method));
	}//if  (i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_MIX)
	else
		pv_population->push_back(new  CmGA(pc_random_gen, i_bayesian_effect, i_solution_construction_method));

	pv_population->at(pv_population->size() - 1)->eSetFitness(pc_fitness);
	

	pv_population->at(pv_population->size() - 1)->eVirConfigure
		(
		i_virus_gen,  i_virus_pop_size,  
		d_vir_prob_cut,  d_vir_prob_splice,  d_vir_prob_mut,
		d_prob_mut_add_gene, d_prob_mut_rem_gene
		);

	pv_population->at(pv_population->size() - 1)->eConfigureTemplate();
	pv_population->at(pv_population->size() - 1)->eVirGenerateRandomTemplate();


	if  (i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_ONLINE_SWITCHING)
	{
		CString  s_buf;
		double  d_avr;
		int  i_solution_constrution_to_set;

		if  (pv_population->at(0)->i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION)
			i_solution_constrution_to_set = SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION;
		else
			i_solution_constrution_to_set = SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION;


		if  (i_solution_constrution_to_set == SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION)
			s_buf.Format("SWITCHING TO FULL:");
		else
			s_buf.Format("SWITCHING TO SIMPLE:");

		listInfo->Items->Add((String *) s_buf);
		listInfo->Refresh();

		*pdAverage = 0;
		*pdBestFitness = 0;
		double  d_after_switch, d_after_restoring;
		double  d_ct_fit_before, d_ct_fit_after;
		for  (int  i_ct = 0; i_ct < pv_population->size(); i_ct++)
		{
			d_after_switch = -2;
			d_after_restoring = -2;
			if  (pv_population->at(i_ct)->i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_ONLINE_SWITCHING)
				d_ct_fit_before = -1;
			else
				d_ct_fit_before = pv_population->at(i_ct)->dGetCompTemplateFitness();

			if  (i_solution_constrution_to_set == SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION)  
				pv_population->at(i_ct)->eSwitchToFull(&d_after_switch, &d_after_restoring);
			else
				pv_population->at(i_ct)->eSwitchToSimple();
			/*if  (i_solution_constrution_to_set == SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION)  
				pv_population->at(i_ct)->eMakePreciseGentoypeFromFluent();
			pv_population->at(i_ct)->i_solution_construction_method = i_solution_constrution_to_set;

			pv_population->at(i_ct)->d_templ_fitness = -1;
			d_ct_fit_after = pv_population->at(i_ct)->dGetCompTemplateFitness();*/

			d_ct_fit_after = pv_population->at(i_ct)->dGetCompTemplateFitness();
			s_buf.Format("POPULATION %d: Before Fit=%.8lf  AFTER Fit=%.8lf  Restoring=%.8lf", i_ct, 1.0/d_ct_fit_before, 1.0/d_ct_fit_after, 1/d_after_restoring);
			listInfo->Items->Add((String *) s_buf);
			listInfo->Refresh();

			//if  (pv_population->at(i_ct)->dGetCompTemplateFitness()  !=  d_after_restoring)  ::Tools::vShow("WRONG");

			*pdAverage +=  d_ct_fit_after;
			if  (*pdBestFitness < d_ct_fit_after)  *pdBestFitness = d_ct_fit_after;
		}//for  (int  i_ct = 0; i_ct < pv_population->size(); i_ct++)
	}//if  (i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_ONLINE_SWITCHING)

}//void  CVirusGA::v_add_new_cmga()




CmGA*  CVirusGA::pc_get_parent_tournament(bool  bBetterTakesAll  /*=  false*/)
{
	return(pc_get_parent_tournament_normal(2, bBetterTakesAll));
}//CmGA*  CVirusGA::pc_get_parent_tournament(bool  bBetterTakesAll  /*=  false*/)



CmGA*  CVirusGA::pc_get_parent_tournament_normal(int  iIndiv1Offset,  int  iIndiv2Offset,  bool  bBetterTakesAll  /*=  false*/)
{
	CmGA  *pc_candidate_1,  *pc_candidate_2;

	pc_candidate_1  =  pv_population->at(iIndiv1Offset);
	pc_candidate_2  =  pv_population->at(iIndiv2Offset);

	double  d_fit_1,  d_fit_2;

	d_fit_1  =  pc_candidate_1->dGetCompTemplateFitness();
	d_fit_1  =  Math::Round(d_fit_1, 2);
	d_fit_2  =  pc_candidate_2->dGetCompTemplateFitness();
	d_fit_2  =  Math::Round(d_fit_2, 2);

	
	if  (bBetterTakesAll  ==  true)
	{
		if  (d_fit_1  >  d_fit_2)  
			return(pc_candidate_1);
		else
			return(pc_candidate_2);
	
	}//if  (bBetterTakesAll  ==  true)
	else
	{
		double  d_fit_sum;
		d_fit_sum  =  d_fit_1  +  d_fit_2;
		d_fit_sum  =  d_fit_sum  *  pc_random_gen->NextDouble();

		if  (d_fit_sum  <  d_fit_1)
			return(pc_candidate_1);
		else
			return(pc_candidate_2);	
	}//else  if  (bBetterTakesAll  ==  true)

}//CmGA*  CVirusGA::pc_get_parent_tournament(int  iIndiv1Offset,  int  iIndiv2Offset,  bool  bBetterTakesAll  /*=  false*/)



CmGA*  CVirusGA::pc_get_parent_tournament_normal(int  iTournamentSize,  bool  bBetterTakesAll  /*= false*/)
{
	if  (iTournamentSize  ==  2)
	{
		int  i_indiv_1,  i_indiv_2;

		i_indiv_1  =  pc_random_gen->Next(0,  pv_population->size());
		i_indiv_2  =  pc_random_gen->Next(0,  pv_population->size());

		
		while  ( (i_indiv_1  ==  i_indiv_2)&&(pv_population->size()  >  1) )
			i_indiv_2  =  pc_random_gen->Next(0,  pv_population->size());


		return(pc_get_parent_tournament_normal(i_indiv_1,  i_indiv_2,  bBetterTakesAll));
	}//if  (iTournamentSize  ==  2)

	return(NULL);
}//CmGA*  CVirusGA::pc_get_parent_tournament(int  iTournamentSize,  bool  bBetterTakesAll)







CmGA*  CVirusGA::pc_get_parent_tournament_normal_for_pattern_different(CMessyPattern  *pcPattern,  CmGA  *pcParentTaker)
{
	if  (pv_population  ==  NULL)  return(NULL);
	if  (pcPattern  ==  NULL)  return(pc_get_parent_tournament());

	double  *pd_template_diffr_fitness;
	double  d_diffr_fitness,  d_diffr_fitness_sum;

	pd_template_diffr_fitness  =  new  double[pv_population->size()];

	d_diffr_fitness_sum  =  0;
	for  (int  ii = 0; ii < (int) pv_population->size(); ii++)
	{
		d_diffr_fitness  =  pcParentTaker->iGetNumberOfDiffrGenes(pcPattern,  pv_population->at(ii));
		//d_diffr_fitness  *=  pv_population->at(ii)->dGetCompTemplateFitness();


		/*if  (pcParentTaker  !=  pv_population->at(ii))
		{
			FILE  *pf_test;

			pf_test = fopen("D:\\projekty_pwr\\FiberFlowAssignment\\release\\aa_test.txt", "w+");

			fprintf(pf_test, "pattern genes: %d \n", pcPattern->pvGetGenotype()->size());
			fprintf(pf_test, "diffr%.2lf \n\n", d_diffr_fitness);
			

			fprintf(pf_test, "Genes(%d):\n\n", i_templ_length);
			for  (int  ig = 0; ig < i_templ_length; ig++)
			{
				fprintf(pf_test, "(%.2lf | %d)", pcParentTaker->pc_competetive_template[ig].dGetOrder(), pcParentTaker->pc_competetive_template[ig].pcGetPath()->iGetId());
				if  (pcParentTaker->pc_competetive_template[ig]  !=  pv_population->at(ii)->pc_competetive_template[ig])
					fprintf(pf_test, "  !=  ");
				else
					fprintf(pf_test, "  ==  ");

				fprintf(pf_test, "(%.2lf | %d)\n", pv_population->at(ii)->pc_competetive_template[ig].dGetOrder(), pv_population->at(ii)->pc_competetive_template[ig].pcGetPath()->iGetId());
			}//for  (int  ii = 0; ii < i_templ_length; ii++)


			fprintf(pf_test, "\n\n\n\n\n");

			int  i_dem_buf;
			for  (int  ig = 0; ig < pcPattern->pvGetGenotype()->size(); ig++)
			{
				i_dem_buf = pcPattern->pvGetGenotype()->at(ig)->iGetDemNumber();
				fprintf(pf_test, "pattern [%d]:", i_dem_buf);
				fprintf(pf_test, "(%.2lf | %d)", pcParentTaker->pc_competetive_template[i_dem_buf].dGetOrder(), pcParentTaker->pc_competetive_template[i_dem_buf].pcGetPath()->iGetId());
				if  (pcParentTaker->pc_competetive_template[i_dem_buf]  !=  pv_population->at(ii)->pc_competetive_template[i_dem_buf])
					fprintf(pf_test, "  !=  ");
				else
					fprintf(pf_test, "  ==  ");

				fprintf(pf_test, "(%.2lf | %d)\n", pv_population->at(ii)->pc_competetive_template[i_dem_buf].dGetOrder(), pv_population->at(ii)->pc_competetive_template[i_dem_buf].pcGetPath()->iGetId());
			
			}//for  (int  ig = 0; ig < pcPattern->pvGetGenotype()->size(); ig++)



			fclose(pf_test);
			vShow("File test created");
		}//if  (pcParentTaker  !=  pv_population->at(ii))*/


		d_diffr_fitness  =  Math::Round(d_diffr_fitness, 2);

		pd_template_diffr_fitness[ii]  =  d_diffr_fitness;
		d_diffr_fitness_sum  +=  d_diffr_fitness;
	}//for  (int  ii = 0; ii < (int) pv_population->size(); ii++)

	if  (d_diffr_fitness_sum  ==  0)
	{
		delete  []  pd_template_diffr_fitness;	
		return(NULL);
	}//if  (d_diffr_fitness_sum  ==  0)

	double  d_rand, d_buf;
	d_rand  =  pc_random_gen->NextDouble()  *  d_diffr_fitness_sum;

	d_buf  =  0;
	for  (int  ii = 0; ii < (int) pv_population->size(); ii++)
	{
		d_buf  +=  pd_template_diffr_fitness[ii];
		
		//found!
		if  (d_buf  >  d_rand)
		{
			delete  []  pd_template_diffr_fitness;
			return(pv_population->at(ii));
		}//if  (d_buf  >  d_rand)
	
	}//for  (int  ii = 0; ii < (int) pv_population->size(); ii++)


	delete  []  pd_template_diffr_fitness;
	return(pv_population->at(pv_population->size() - 1));
}//CmGA*  CVirusGA::pc_get_parent_tournament_normal_for_pattern_different(CMessyPattern  *pcPattern,  CMessyIndividual  *pcParentTaker)




CMessyPattern*  CVirusGA::pc_get_random_pattern()
{
	if  (pv_pattern_pool->size()  ==  0)  return(NULL);

	int  i_pattern;

	i_pattern  =  pc_random_gen->Next(0,  pv_pattern_pool->size());
	return(pv_pattern_pool->at(i_pattern));
}//CMessyPattern*  CVirusGA::pc_get_random_pattern()



CError  CVirusGA::e_load_init_solution(CString sReferenceSolution)
{
	CError  c_err;

	FILE  *pf_solf;

	c_err = pc_fitness->eLoadSolutionWalk(sReferenceSolution);
	//c_err = pc_fitness->eLoadSolutionAibin(sReferenceSolution);

	return(c_err);
}//CError  CVirusGA::e_load_init_solution(CString sReferenceSolution)



CmGA  *CVirusGA::pc_sga_get_parent_roulette(double  dFitnessSum)
{
	double  d_choice;
	d_choice = pc_random_gen->NextDouble();
	d_choice *= dFitnessSum;

	double  d_fit_sum_buf;
	for  (int  ii = 0; ii < pv_population->size(); ii++)
	{
		d_fit_sum_buf += pv_population->at(ii)->dGetCompTemplateFitness();
		if  (d_fit_sum_buf  >=  d_choice)  return(pv_population->at(ii));
	}//for  (int  ii = 0; ii < pv_population->size(); ii++)
	
	return(pv_population->at(pv_population->size() - 1));
}//CmGA  *CVirusGA::pc_sga_get_parent_roulette(double  dFitnessSum)




CmGA*  CVirusGA::pc_sga_get_best_individual(vector  <CmGA  *>  *pvPopulation)
{
	double  d_buf;
	CmGA  *pc_best_individual = pvPopulation->at(0);

	//first choose best individual
	for (int  ii = 0; ii < (int) pv_population->size();  ii++)
	{
		d_buf  =  pvPopulation->at(ii)->dGetCompTemplateFitness();

		if  (d_buf  >  pc_best_individual->dGetCompTemplateFitness())  
		{
			pc_best_individual = pvPopulation->at(ii);
		}//if  (d_buf  >  pc_best_individual->dGetCompTemplateFitness())  
	}//for (int  ii = 0; ii < i_pop_size;  ii++)

	return(pc_best_individual);
}//CmGA*  CVirusGA::pc_sga_get_best_individual(vector  <CmGA  *>  *pvPopulation)




CError  CVirusGA::eRunSimulatedAnnealing(double  dMaxTime, double  dProbingTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, System::Windows::Forms::ListBox*  listInfo)
{
	CError  c_err;
	
	CString  s_buf, s_buf2;
	s_buf.Format
					(
					"Iterations max: %d\n", i_sa_max_iteration
					);
				listInfo->Items->Add((String *) s_buf);
	bool  b_clever_paths = true;

	if  (pc_fitness  ==  NULL)
	{
		c_err.vPutError("No fitness function defined");
		return(c_err);	
	}//if  (pc_fitness  ==  NULL)

		

	pc_fitness->vZeroEval();


	

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initializing...");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	

	if  (pv_population  !=  NULL)
	{
		for  (int  ii = 0; ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);	
		pv_population->clear();
	}//if  (pv_population  !=  NULL)
	else
		pv_population  =  new  vector  <CmGA *>;

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initting...1");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	CmGA  *pc_best_solution, *pc_new_solution, *pc_current_solution;

	pc_best_solution = new  CmGA(pc_random_gen,  i_bayesian_effect, i_solution_construction_method);
	pc_new_solution = new  CmGA(pc_random_gen,  i_bayesian_effect, i_solution_construction_method);
	pc_current_solution = new  CmGA(pc_random_gen,  i_bayesian_effect, i_solution_construction_method);

	pc_best_solution->eSetFitness(pc_fitness, true);
	pc_new_solution->eSetFitness(pc_fitness, true);
	pc_current_solution->eSetFitness(pc_fitness, true);

	pc_best_solution->eConfigureTemplate();
	pc_new_solution->eConfigureTemplate();
	pc_current_solution->eConfigureTemplate();

	// todo: different ways of generating initial solution
	pc_best_solution->eVirGenerateRandomTemplate();	

	//::MessageBox(NULL, "a", "a", MB_OK);


	if  (sReferenceSolution  !=  "")
	{
		listInfo->Items->Add(S"Loading initial solution...");
		listInfo->Refresh();
		//::MessageBox(NULL, sReferenceSolution, sReferenceSolution, MB_OK);

		
		c_err = e_load_init_solution(sReferenceSolution);
		if  (c_err)  return(c_err);

		bool  b_const = true;
		double  d_afa;
		d_afa = pc_fitness->dCompCost(&b_const);


		pc_best_solution->eMakeGentoypeFromSolution(pc_fitness);
		pc_fitness->pcGetNet()->eResetNet();

		double  d_ct_fit;
		d_ct_fit = pc_best_solution->dGetCompTemplateFitness();


		s_buf.Format("AFA=%.2lf  CT=%.2lf", d_afa, 1/d_ct_fit);
		listInfo->Items->Add((String *) s_buf);
		listInfo->Refresh();
	}//if  (s_solf_file  !=  "");





	pc_best_solution->vCopyTo(pc_current_solution);

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initting...2");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	CTimeCounter  c_time_counter;

	
	FILE  *pf_report;
	pf_report  =  fopen(sReportFile,  "w+");
	vSgaSaveParameters(pf_report);
	c_time_counter.vSetStartNow();

	int  i_probe_num;
	double  d_buf,  d_best_fitness,  d_prev_best_fitness, d_prev_average,  d_average,  d_time_passed, d_last_probe;
	double  d_time_passed_till_max, d_fitness_eval_till_max;

	double  d_fitness_sum;
	bool  b_optimal_soluion_reached;
	b_optimal_soluion_reached = false;

	d_time_passed  =  0;
	d_last_probe = 0;
	i_probe_num = 0;

	d_prev_average  =  0;
	d_average  =  0;
	d_best_fitness  =  0;
	d_prev_best_fitness = -1;
	double best = 1.0 / pc_best_solution->dGetCompTemplateFitness();
	double temperaturaPoczatkowa = (double)best/80; //2.5;// 					   // temperatura pocz¹tkowa
	double t = temperaturaPoczatkowa;
	double current = best;
	
	
	if  (listInfo  !=  NULL)  listInfo->Items->Add(S"Started..");
	// todo: temp
	// todo: timeMax
	for (int  i_cur_gen = 0; (d_time_passed < dMaxTime) /*&& (i_cur_gen < i_sa_max_iteration)*/ && (b_optimal_soluion_reached ==  false);  i_cur_gen++)
	{
		if  (d_time_passed - d_last_probe > dProbingTime)
		{
			double  d_best_ct, d_ct_fit_cur;
			d_best_ct = 0;
			CMessyIndividual  c_best_ct_fit(pc_fitness, i_bayesian_effect, i_solution_construction_method);

			d_ct_fit_cur  =  c_best_ct_fit.dComputeFitness(pc_best_solution->pc_competetive_template);	
			if  (d_best_ct < d_ct_fit_cur)  d_best_ct = d_ct_fit_cur;

			bool  b_const;
			double d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);
			

			FILE  *pf_summarized_rep;
			pf_summarized_rep  =  fopen(sSummarizedReport, "a");
			fprintf
				(pf_summarized_rep, 

				"PROBE \t %d \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.1lf \t %d \t %s \n", 

				i_probe_num,//pc_fitness->pcGetNet()->pcGetDemands()[0].pvGetPossiblePaths()->size();
				d_best_ct, d_templ_fitness_no_pen, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, sReportFile
				);
			fclose(pf_summarized_rep);

			d_last_probe = d_time_passed;
			i_probe_num++;		
		}//if  (d_time_passed - d_last_probe > dProbingTime)


		//::MessageBox(NULL, "0", "0", MB_OK);
		if  (i_cur_gen == 0)
		{
			fprintf(pf_report,  "Started at: %.8lf   fiber fitness:%.8lf\n", pc_best_solution->dGetCompTemplateFitness(), 1.0 / pc_best_solution->dGetCompTemplateFitness());

			if  (listInfo  !=  NULL)
			{
				s_buf.Format
					(
					"Started at: %.8lf   fiber fitness:%.8lf\n", pc_best_solution->dGetCompTemplateFitness(), 1.0 / pc_best_solution->dGetCompTemplateFitness()
					);
				listInfo->Items->Add((String *) s_buf);
				listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
				listInfo->Refresh();

				System::Windows::Forms::Application::DoEvents();
			}//if  (listInfo  !=  NULL)
		}//if  (i_cur_gen == 0)

		pc_current_solution->vCopyTo(pc_new_solution);
		pc_new_solution->eVirGenerateRandomSwap();
		c_time_counter.bGetTimePassed(&d_time_passed);

		if  (pc_best_solution->dGetCompTemplateFitness()  <  pc_new_solution->dGetCompTemplateFitness())
		{
			pc_new_solution->vCopyTo(pc_best_solution);
			pc_new_solution->vCopyTo(pc_current_solution);
			current = pc_new_solution->dGetCompTemplateFitness();
			if  (listInfo  !=  NULL)
			{
				s_buf.Format
					(
					"Generation: %d new best fitness: %.8lf  fiber best fitness: %.8lf   [fitness evaluations:%.0lf] [time passed:%.4lf] [temperature%.8lf]",  
					i_cur_gen,  
					pc_best_solution->dGetCompTemplateFitness(), 1.0 / pc_best_solution->dGetCompTemplateFitness(),
					pc_fitness->dGetFuncEval(),
					d_time_passed,
					t
					);
				listInfo->Items->Add((String *) s_buf);
				listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
				listInfo->Refresh();

				System::Windows::Forms::Application::DoEvents();
			}//if  (listInfo  !=  NULL)
		
		}//if  (pc_best_solution->dGetCompTemplateFitness()  <  pc_new_solution->eVirGenerateRandomTemplate())
		else {
			double delta = pc_new_solution->dGetCompTemplateFitness() - current;
			double x = rand() / double(RAND_MAX);
				if ( x < exp((-1.0 * (double)delta)/t))	// to z pewnym prawdopodobieñstwem to rozwi¹zanie jest zapisywane jako obecne
				{
					pc_new_solution->vCopyTo(pc_current_solution);
					current = pc_new_solution->dGetCompTemplateFitness();
				}
		}
			
		System::Windows::Forms::Application::DoEvents();

		//if  (pdOptimalValue  !=  NULL)
		//{
		//	if  (d_best_fitness  >  0)
		//	{
		//		if  ((1.0 / d_best_fitness) + 1  <=  *pdOptimalValue)  b_optimal_soluion_reached = true;
		//	}//if  (d_best_fitness  >  0)
		//}//if  (pdOptimalValue  !=  NULL)	
		t = 0.99992 * t; // zmniejszamy temperaturê

		//::MessageBox(NULL, "k", "k", MB_OK);
	}//for (int  ii = 0; ii < i_gen;  ii++)


	


	fprintf(pf_report,  "\n\n\n");
	fprintf(pf_report,  "\n\nFITNESS EVALUATIONS: %.0lf\n\n\n\n", pc_fitness->dGetFuncEval());
	fclose(pf_report);

	for  (int  ii = 0; ii < (int) pv_pattern_pool->size();  ii++)
		delete  pv_pattern_pool->at(ii);
	pv_pattern_pool->clear();

		
	FILE  *pf_summarized_rep;
	
	double  d_templ_fitness;
	bool  b_const;
	double  d_ref_solution;

	if  (sReferenceSolution  !=  "")
	{
		//vShow("("+sReferenceSolution+")");
		pc_fitness->eLoadSolution(sReferenceSolution);
		pc_fitness->eApplySolution();
		d_ref_solution = pc_fitness->dCompCost(&b_const);
	}//if  (sReferenceSolution  !=  "")
	else
		d_ref_solution = 1;

	CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	d_templ_fitness  =  c_buf.dComputeFitness(pc_best_solution->pc_competetive_template);	
	FILE  *pf_solution;
	pf_solution  =  fopen(sReportFile + "_solf.txt",  "w+");
	pc_fitness->eSaveSolution(pf_solution);
	fclose(pf_solution);

	
	

	double d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);
	if  (d_templ_fitness_no_pen  <  d_templ_fitness)
		b_const = true;
	else
		b_const = false;

	pf_summarized_rep  =  fopen(sSummarizedReport, "a");
	fprintf
		(pf_summarized_rep, 
		"%.8lf \t %.8lf \t %.8lf \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.0lf \t %d \t %s \n", 

		d_templ_fitness, d_templ_fitness_no_pen, d_ref_solution,  d_templ_fitness / d_ref_solution, d_templ_fitness_no_pen / d_ref_solution, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, sReportFile
		);
	fclose(pf_summarized_rep);


	return(c_err);
}//CError  CVirusGA::eRunSimulatedAnnealing(double  dMaxTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, System::Windows::Forms::ListBox*  listInfo)







CError  CVirusGA::eRunRandomSearch(double  dMaxTime, double  dProbingTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, System::Windows::Forms::ListBox*  listInfo)
{
	CError  c_err;

	CString  s_buf, s_buf2;
	
	bool  b_clever_paths = true;

	if  (pc_fitness  ==  NULL)
	{
		c_err.vPutError("No fitness function defined");
		return(c_err);	
	}//if  (pc_fitness  ==  NULL)

		

	pc_fitness->vZeroEval();


	

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initializing...");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	

	if  (pv_population  !=  NULL)
	{
		for  (int  ii = 0; ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);	
		pv_population->clear();
	}//if  (pv_population  !=  NULL)
	else
		pv_population  =  new  vector  <CmGA *>;

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initting...1");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	CmGA  *pc_best_solution, *pc_new_solution;

	pc_best_solution = new  CmGA(pc_random_gen,  i_bayesian_effect, i_solution_construction_method);
	pc_new_solution = new  CmGA(pc_random_gen,  i_bayesian_effect, i_solution_construction_method);

	pc_best_solution->eSetFitness(pc_fitness, true);
	pc_new_solution->eSetFitness(pc_fitness, true);
	pc_best_solution->eConfigureTemplate();
	pc_new_solution->eConfigureTemplate();
	pc_best_solution->eVirGenerateRandomTemplate();
	pc_new_solution->eVirGenerateRandomTemplate();

	//::MessageBox(NULL, "a", "a", MB_OK);


	if  (sReferenceSolution  !=  "")
	{
		listInfo->Items->Add(S"Loading initial solution...");
		listInfo->Refresh();
		//::MessageBox(NULL, sReferenceSolution, sReferenceSolution, MB_OK);
		c_err = e_load_init_solution(sReferenceSolution);
		if  (c_err)  return(c_err);

		bool  b_const = true;
		double  d_afa;
		d_afa = pc_fitness->dCompCost(&b_const);

		/*bool  b_const = true;
		double  d_afa;
		d_afa = pc_fitness->dCompCost(&b_const);
		s_buf.Format("%.2lf", d_afa);
		//::MessageBox(NULL, s_buf, s_buf, MB_OK);

		FILE  *pf_afa;
		pf_afa = fopen("afa.txt", "a");
		fprintf(pf_afa, "%s \t %.2lf\n", sReferenceSolution, d_afa);
		fclose(pf_afa);*/

		pc_best_solution->eMakeGentoypeFromSolution(pc_fitness);
		pc_fitness->pcGetNet()->eResetNet();

		double  d_ct_fit;
		d_ct_fit = pc_best_solution->dGetCompTemplateFitness();
	}//if  (s_solf_file  !=  "");



	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initting...2");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	CTimeCounter  c_time_counter;

	
	FILE  *pf_report;
	pf_report  =  fopen(sReportFile,  "w+");
	vSgaSaveParameters(pf_report);
	c_time_counter.vSetStartNow();

	int  i_probe_num;
	double  d_buf,  d_best_fitness,  d_prev_best_fitness, d_prev_average,  d_average,  d_time_passed, d_last_probe;
	double  d_time_passed_till_max, d_fitness_eval_till_max;

	double  d_fitness_sum;
	bool  b_optimal_soluion_reached;
	b_optimal_soluion_reached = false;

	d_time_passed  =  0;
	d_last_probe = 0;
	i_probe_num = 0;

	d_prev_average  =  0;
	d_average  =  0;
	d_best_fitness  =  0;
	d_prev_best_fitness = -1;
	


	if  (listInfo  !=  NULL)  listInfo->Items->Add(S"Started..");
	for (int  i_cur_gen = 0; (d_time_passed < dMaxTime) && (b_optimal_soluion_reached ==  false);  i_cur_gen++)
	{
		if  (d_time_passed - d_last_probe > dProbingTime)
		{
			double  d_best_ct, d_ct_fit_cur;
			d_best_ct = 0;
			CMessyIndividual  c_best_ct_fit(pc_fitness, i_bayesian_effect, i_solution_construction_method);

			d_ct_fit_cur  =  c_best_ct_fit.dComputeFitness(pc_best_solution->pc_competetive_template);	
			if  (d_best_ct < d_ct_fit_cur)  d_best_ct = d_ct_fit_cur;

			bool  b_const;
			double d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);
			

			FILE  *pf_summarized_rep;
			pf_summarized_rep  =  fopen(sSummarizedReport, "a");
			fprintf
				(pf_summarized_rep, 

				"PROBE \t %d \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.1lf \t %d \t %s \n", 

				i_probe_num,//pc_fitness->pcGetNet()->pcGetDemands()[0].pvGetPossiblePaths()->size();
				d_best_ct, d_templ_fitness_no_pen, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, sReportFile
				);
			fclose(pf_summarized_rep);

			d_last_probe = d_time_passed;
			i_probe_num++;		
		}//if  (d_time_passed - d_last_probe > dProbingTime)


		//::MessageBox(NULL, "0", "0", MB_OK);
		if  (i_cur_gen == 0)
		{
			fprintf(pf_report,  "Started at: %.8lf   fiber fitness:%.8lf\n", pc_best_solution->dGetCompTemplateFitness(), 1.0 / pc_best_solution->dGetCompTemplateFitness());

			if  (listInfo  !=  NULL)
			{
				s_buf.Format
					(
					"Started at: %.8lf   fiber fitness:%.8lf\n", pc_best_solution->dGetCompTemplateFitness(), 1.0 / pc_best_solution->dGetCompTemplateFitness()
					);
				listInfo->Items->Add((String *) s_buf);
				listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
				listInfo->Refresh();

				System::Windows::Forms::Application::DoEvents();
			}//if  (listInfo  !=  NULL)
		}//if  (i_cur_gen == 0)

		
		pc_new_solution->eVirGenerateRandomTemplate();
		c_time_counter.bGetTimePassed(&d_time_passed);

		if  (pc_best_solution->dGetCompTemplateFitness()  <  pc_new_solution->dGetCompTemplateFitness())
		{
			pc_new_solution->vCopyTo(pc_best_solution);

			if  (listInfo  !=  NULL)
			{
				s_buf.Format
					(
					"Generation: %d new best fitness: %.8lf  fiber best fitness: %.8lf   [fitness evaluations:%.0lf] [time passed:%.4lf]",  
					i_cur_gen,  
					pc_best_solution->dGetCompTemplateFitness(), 1.0 / pc_best_solution->dGetCompTemplateFitness(),
					pc_fitness->dGetFuncEval(),
					d_time_passed
					);
				listInfo->Items->Add((String *) s_buf);
				listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
				listInfo->Refresh();

				System::Windows::Forms::Application::DoEvents();
			}//if  (listInfo  !=  NULL)
		
		}//if  (pc_best_solution->dGetCompTemplateFitness()  <  pc_new_solution->eVirGenerateRandomTemplate())
		/*else
		{
			if  (listInfo  !=  NULL)
			{
				s_buf.Format
					(
					"Generation: %d candidate fitness: %.8lf  fiber candidate fitness: %.8lf   [fitness evaluations:%.0lf] [time passed:%.4lf]",  
					i_cur_gen,  
					pc_new_solution->dGetCompTemplateFitness(), 1.0 / pc_new_solution->dGetCompTemplateFitness(),
					pc_fitness->dGetFuncEval(),
					d_time_passed
					);
				listInfo->Items->Add((String *) s_buf);
				listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
				listInfo->Refresh();

				System::Threading::Thread::Sleep(1000);
				System::Windows::Forms::Application::DoEvents();
			}//if  (listInfo  !=  NULL)
		}//else  if  (pc_best_solution->dGetCompTemplateFitness()  <  pc_new_solution->eVirGenerateRandomTemplate())*/
		
			
		System::Windows::Forms::Application::DoEvents();

		if  (pdOptimalValue  !=  NULL)
		{
			if  (d_best_fitness  >  0)
			{
				if  ((1.0 / d_best_fitness) + 1  <=  *pdOptimalValue)  b_optimal_soluion_reached = true;
			}//if  (d_best_fitness  >  0)
		}//if  (pdOptimalValue  !=  NULL)	

		//::MessageBox(NULL, "k", "k", MB_OK);
	}//for (int  ii = 0; ii < i_gen;  ii++)


	


	fprintf(pf_report,  "\n\n\n");
	fprintf(pf_report,  "\n\nFITNESS EVALUATIONS: %.0lf\n\n\n\n", pc_fitness->dGetFuncEval());
	fclose(pf_report);

	for  (int  ii = 0; ii < (int) pv_pattern_pool->size();  ii++)
		delete  pv_pattern_pool->at(ii);
	pv_pattern_pool->clear();

		
	FILE  *pf_summarized_rep;
	
	double  d_templ_fitness;
	bool  b_const;
	double  d_ref_solution;

	if  (sReferenceSolution  !=  "")
	{
		//vShow("("+sReferenceSolution+")");
		pc_fitness->eLoadSolution(sReferenceSolution);
		pc_fitness->eApplySolution();
		d_ref_solution = pc_fitness->dCompCost(&b_const);
	}//if  (sReferenceSolution  !=  "")
	else
		d_ref_solution = 1;

	CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	d_templ_fitness  =  c_buf.dComputeFitness(pc_best_solution->pc_competetive_template);	
	FILE  *pf_solution;
	pf_solution  =  fopen(sReportFile + "_solf.txt",  "w+");
	pc_fitness->eSaveSolution(pf_solution);
	fclose(pf_solution);

	
	

	double d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);
	if  (d_templ_fitness_no_pen  <  d_templ_fitness)
		b_const = true;
	else
		b_const = false;

	pf_summarized_rep  =  fopen(sSummarizedReport, "a");
	fprintf
		(pf_summarized_rep, 
		"%.8lf \t %.8lf \t %.8lf \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.0lf \t %d \t %s \n", 

		d_templ_fitness, d_templ_fitness_no_pen, d_ref_solution,  d_templ_fitness / d_ref_solution, d_templ_fitness_no_pen / d_ref_solution, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, sReportFile
		);
	fclose(pf_summarized_rep);


	return(c_err);
}//CError  CVirusGA::eRunRandomSearch(double  dMaxTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, System::Windows::Forms::ListBox*  listInfo)




CError  CVirusGA::eRunGRASP
	(
	double  dMaxTime, 
	CString  sReportFile,  
	CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, 

	long  lRestrictedCandidateListSize, 
	double dRestrictedCandidateListSizeFactor,
	int  iValueBasedCandidateListSize,  
	long  lAcceptableDistanceFromOptimal,
	long  lLocalSearchMaxIterations,
	double  dLocalSearchCoolingRateFactor,
	double  dLocalSearchNrOfDemandsToSwapFactor,
	long  lLocalSearchNrOfDemandsToSwap,
	double  dLocalSearchInitialTemperature,
	double  dLocalSearchInitialDistanceFromOptimal,
	double  dLocalSearchInitialProbabilityOfAcceptingWorseSolution,

	System::Windows::Forms::ListBox*  listInfo
	)
{
	CError  c_err;
	CString  s_buf;


	if  (pc_fitness  ==  NULL)
	{
		c_err.vPutError("No fitness function defined");
		return(c_err);	
	}//if  (pc_fitness  ==  NULL)

	pc_fitness->vZeroEval();

	

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initializing...");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)

	if  (pv_population  !=  NULL)
	{
		for  (int  ii = 0; ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);	
		pv_population->clear();
	}//if  (pv_population  !=  NULL)
	else
		pv_population  =  new  vector  <CmGA *>;

	if (listInfo != NULL)
	{
		listInfo->Items->Add(S"Initting..1"); // initing time counter and file to output the results
		listInfo->Refresh();
	}

	CTimeCounter  c_time_counter;

	double  d_time_passed = 0;
	FILE  *pf_report;
	pf_report  =  fopen(sReportFile,  "w+");
	
	//------------------- start grasp
	// evaluate grasp parameters
	int rclSize = 0;
	if (iValueBasedCandidateListSize != 0)
	{
		// case not supported
		CError error;
		error.vPutError("case not implemented. set iValueBasedCandidateListSize to 0");
		return error;
	} else 
	{
		if (lRestrictedCandidateListSize != 0)
		{
			if (dRestrictedCandidateListSizeFactor != 0.0)
			{ 
				listInfo->Items->Add(S"dRestrictedCandidateListSizeFactor is set, however it is ignored, because lRestrictedCandidateListSize is set");
				listInfo->Refresh();
			}
			rclSize = (int) lRestrictedCandidateListSize;
		} else
		{ // evaluate RCL based on factor
			rclSize = (int) ceil(pc_fitness->pcGetNet()->iGetDemandNumber() / dRestrictedCandidateListSizeFactor);
			if (rclSize = 0) rclSize = 1;
		}
		if (rclSize <= 0)
		{
			CError error;
			error.vPutError("rcl size have to be greater than 0");
			return error;
		}
	}
	// get initial temperature
	double temperature = 0;
	if (dLocalSearchInitialTemperature != 0.0)
	{
		if (dLocalSearchInitialDistanceFromOptimal != 0.0 || dLocalSearchInitialProbabilityOfAcceptingWorseSolution != 0.0)
		{
			listInfo->Items->Add(S"dLocalSearchInitialDistanceFromOptimal or dLocalSearchInitialProbabilityOfAcceptingWorseSolution are not 0, however they are ignored, because dLocalSearchInitialTemperature is set");
			listInfo->Refresh();
		}
		temperature = dLocalSearchInitialTemperature;
	} else
	{
		// temperature can be calculate as -psi/ln(phi)
		// then initially algorithm accpets solutions which are far about
		// psi from the optimal solution with the probability phi
		temperature = -1 * dLocalSearchInitialDistanceFromOptimal / log(dLocalSearchInitialProbabilityOfAcceptingWorseSolution);
	}
	if (temperature <= 0.0)
	{
		CError error;
		error.vPutError("temperature is below 0");
		return error;
	}
	int lsSwapSize = 0;
	if (lLocalSearchNrOfDemandsToSwap != 0)
	{
		if (dLocalSearchNrOfDemandsToSwapFactor != 0.0)
		{
			listInfo->Items->Add(S"dLocalSearchNrOfDemandsToSwapFactor is set, however they are ignored, because lLocalSearchNrOfDemandsToSwap is set");
			listInfo->Refresh();
		}
		lsSwapSize = lLocalSearchNrOfDemandsToSwap;	
	} else
	{
		lsSwapSize = (int) ceil(pc_fitness->pcGetNet()->iGetDemandNumber() / dLocalSearchNrOfDemandsToSwapFactor);
		if (lsSwapSize <= 0) lsSwapSize = 1;
	}
	if (lsSwapSize > pc_fitness->pcGetNet()->iGetDemandNumber() / 2)
	{
		CError error;
		error.vPutError("swap size cannot be larger than half of the size of demands");
		return error;
	}

	/*if (i_solution_construction_method != SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION)
	{
		CError error;
		error.vPutError("Grasp should be run with fluent optimization");
		return error;
	}*/

	listInfo->Items->Add(S"I am running algorithm with following properties");
	s_buf.Format("valuBasedRCL: %d", iValueBasedCandidateListSize); 
	listInfo->Items->Add((String *) s_buf);
	s_buf.Format("rclSize: %d", rclSize); 
	listInfo->Items->Add((String *) s_buf);
	s_buf.Format("lsMaxIter: %d", lLocalSearchMaxIterations); 
	listInfo->Items->Add((String *) s_buf);
	s_buf.Format("lsInitTemp: %1.6f", temperature); 
	listInfo->Items->Add((String *) s_buf);
	s_buf.Format("lsInitDistFromOpt: %1.6f", dLocalSearchInitialDistanceFromOptimal); 
	listInfo->Items->Add((String *) s_buf);
	s_buf.Format("lsInitProbAcceptWorseSol: %1.6f", dLocalSearchInitialProbabilityOfAcceptingWorseSolution); 
	listInfo->Items->Add((String *) s_buf);
	s_buf.Format("lsCoolingRate: %1.6f", dLocalSearchCoolingRateFactor); 
	listInfo->Items->Add((String *) s_buf);
	s_buf.Format("lsSwapSize: %d", lsSwapSize); 
	listInfo->Items->Add((String *) s_buf);
	listInfo->Refresh();
	
	c_time_counter.vSetStartNow();

	// some variables required for algorithms execution
	CRankedSelection *rankedSelection = CRankedSelection::newInstance(pc_random_gen);
	CGrasp grasp(pc_fitness, pc_random_gen, rankedSelection);
	//grasp.vSetDebug(true);
	grasp.vSetDebug(false);
	CPermutationRandomGenerator *permutationRandomGen = CPermutationRandomGenerator::pcGetInstance(pc_random_gen, grasp.iGetNrOfDemands());
	CGraspSolution cBestSolution(pc_random_gen, i_bayesian_effect, i_solution_construction_method, pc_fitness);
	CGraspSolution cCurrentSolution(pc_random_gen, i_bayesian_effect, i_solution_construction_method, pc_fitness);
	CGraspSolution cCurrentLocalSearchSolution(pc_random_gen, i_bayesian_effect, i_solution_construction_method, pc_fitness);
	CGraspSolution cCandidateLocalSearchSolution(pc_random_gen, i_bayesian_effect, i_solution_construction_method, pc_fitness);
	double d_best_fitness = 0.0, d_current_fitness = 0.0, d_current_local_search_fitness = 0.0,
		d_candidate_local_search_fitness = 0.0;
	double solutionDifference;	
	
	s_buf.Format("graspDebug: %d", grasp.iGetDebug());
	listInfo->Items->Add((String *) s_buf);
	listInfo->Refresh();

	if (listInfo != NULL) listInfo->Items->Add(S"Started..");

	for (int  i_cur_gen = 0; (d_time_passed < dMaxTime) ;  i_cur_gen++)
	{
		c_time_counter.bGetTimePassed(&d_time_passed);
	
		if (i_cur_gen == 0) // creating initial solution, first iteration works differently
		{
			grasp.vFillSolutionWithMostSubcarriersFirst(&cBestSolution);
			d_best_fitness = cBestSolution.dGetFitness();
			cBestSolution.vCopySolutionTo(&cCurrentSolution);
			if (grasp.iGetDebug())
			{
				grasp.logToDebugFile("best:");
				grasp.logToDebugFile(cBestSolution.iGetSolutionOrderHash());
			}

			if (listInfo != NULL) 
			{
				s_buf.Format("Fitness first sol: %.81f", cBestSolution.dGetFitness());
				listInfo->Items->Add((String *) s_buf);
				listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
				listInfo->Refresh();
				System::Windows::Forms::Application::DoEvents();
			}
		} else {
			grasp.vRandomGreedyConstruction(cCurrentSolution, rclSize);
			if (grasp.iGetDebug())
			{
				grasp.logToDebugFile("rcl:");
				grasp.logToDebugFile(cCurrentSolution.iGetSolutionOrderHash());
			}
			if  (listInfo  !=  NULL)
			{
				s_buf.Format("iter %d: ", i_cur_gen);
				listInfo->Items->Add((String *) s_buf);
				s_buf.Format("Fitness after RCL creation: %.81f", cCurrentSolution.dGetFitness());
				listInfo->Items->Add((String *) s_buf);
				listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
				listInfo->Refresh();
			}
		}//if (i_cur_gen == 0)
		// here call local search
		cCurrentSolution.vCopySolutionTo(&cCurrentLocalSearchSolution);
		d_current_fitness = cCurrentSolution.dGetFitness();
		for (int lsIter = 0; (lsIter < lLocalSearchMaxIterations) && (d_time_passed < dMaxTime); lsIter++)
		{
			cCurrentLocalSearchSolution.vCopySolutionTo(&cCandidateLocalSearchSolution);
			// get kDistinct random indices
			vector<int> *subview = permutationRandomGen->pvGenerateKDistinctIntegers(lsSwapSize * 2);
			// swap
			cCandidateLocalSearchSolution.vSwapDemands(subview->begin(), subview->begin() + lsSwapSize,
				subview->begin() + lsSwapSize, subview->end());
			delete subview;
			solutionDifference = cCandidateLocalSearchSolution.dGetFitness() - d_current_fitness;
			if (solutionDifference < 0)
			{ // accept new solution as the best one (currentSolution in GRASP)
				cCandidateLocalSearchSolution.vCopySolutionTo(&cCurrentSolution);
				cCandidateLocalSearchSolution.vCopySolutionTo(&cCurrentLocalSearchSolution);
				d_current_fitness = cCandidateLocalSearchSolution.dGetFitness();
			} else
			{
				// accept worse solution as current solution with 
				// probability related to the temperature
				// e^(-solDiff / temp)
				// this equation has always result within range (0, 1)
				// when temperature decreases there is lower probability that the worse
				// solution will be accepted
				if (exp(-solutionDifference / temperature) > pc_random_gen->NextDouble())
				{
					cCandidateLocalSearchSolution.vCopySolutionTo(&cCurrentLocalSearchSolution);
				}
			}
			temperature = temperature * dLocalSearchCoolingRateFactor;

			c_time_counter.bGetTimePassed(&d_time_passed);
		}//for (int lsIter = 0; (lsIter < lLocalSearchMaxIterations) && (d_time_passed < dMaxTime); lsIter++)
		d_current_fitness = cCurrentSolution.dGetFitness();
		if (grasp.iGetDebug())
		{
			grasp.logToDebugFile("ls:");
			grasp.logToDebugFile(cCurrentSolution.iGetSolutionOrderHash());
		}
		if  (listInfo  !=  NULL)
		{
			s_buf.Format("Cost after LS: %.81f", cCurrentSolution.dGetFitness());
			listInfo->Items->Add((String *) s_buf);
			listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
			listInfo->Refresh();
		}
		if (d_current_fitness < d_best_fitness)
		{ // if current solution better then best so far, store current as best
			d_best_fitness = d_current_fitness;
			cCurrentSolution.vCopySolutionTo(&cBestSolution);
		} else 
		{ // restore current solution from best one
			cBestSolution.vCopySolutionTo(&cCurrentSolution);
		}
		System::Windows::Forms::Application::DoEvents();		
		
		c_time_counter.bGetTimePassed(&d_time_passed);
	}//for (int  i_cur_gen = 0; (d_time_passed < dMaxTime) && (b_optimal_soluion_reached ==  false);  i_cur_gen++)

	if  (listInfo  !=  NULL)
	{
		if  (listInfo  !=  NULL)
		{
			s_buf.Format("Best solution found: %.81f", cBestSolution.dGetFitness());
			listInfo->Items->Add((String *) s_buf);
			listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
			listInfo->Refresh();
		}

		System::Windows::Forms::Application::DoEvents();
	}//if  (listInfo  !=  NULL)

	CPermutationRandomGenerator::vClearCache();
	CRankedSelection::deleteRankedSelection();

	fclose(pf_report);


	bool  b_const = false;
	double  d_templ_fitness;
	d_templ_fitness = cBestSolution.pcGetFiberSol()->dCompCost(&b_const);	
	//d_templ_fitness  =  pc_fitness->dCompCost(&b_const);
	FILE  *pf_solution;
	pf_solution  =  fopen(sReportFile + "_solf.txt",  "w+");
	cBestSolution.pcGetFiberSol()->eSaveSolution(pf_solution);
	fclose(pf_solution);


	
	double d_templ_fitness_no_pen  =  cBestSolution.pcGetFiberSol()->dCompCost(&b_const, true);

	double  d_ref_solution = 1;


	FILE  *pf_summarized_rep;
	pf_summarized_rep  =  fopen(sSummarizedReport, "a");
	fprintf
		(pf_summarized_rep, 

		"%d \t %d \t %d \t %.8lf \t %.8lf \t %.8lf \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.0lf \t %d \t %s \n", 

		pc_fitness->pcGetNet()->iGetDemandNumberUni(), pc_fitness->pcGetNet()->iGetDemandNumberAny(), pc_fitness->pcGetNet()->iGetDemandNumberMulti(), 
		//c_num.sToStringFirstDgitsDotted(2), c_num.sToString10Pow(),
		//pc_fitness->pcGetNet()->pcGetDemands()[0].pvGetPossiblePaths()->size();
		d_templ_fitness, d_templ_fitness_no_pen, d_ref_solution,  d_templ_fitness / d_ref_solution, d_templ_fitness_no_pen / d_ref_solution, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, sReportFile
		);
	fclose(pf_summarized_rep);//*/


	return(c_err);
}//CError  CVirusGA::eRunGRASP



void  CVirusGA::v_switch_coding_of_cm_GA(CString  sName, CmGA *pcSolution, System::Windows::Forms::ListBox*  listInfo)
{
	CError  c_err;
	CString  s_buf;

	double  d_ct_fit_before, d_ct_fit_after;
	d_ct_fit_before = pcSolution->dGetCompTemplateFitness();


	c_err = pcSolution->eMakePreciseGentoypeFromFluent();
	pcSolution->i_solution_construction_method = SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION;

	pcSolution->d_templ_fitness = -1;
	d_ct_fit_after = pcSolution->dGetCompTemplateFitness();

	s_buf.Format("%s: Before Fit=%.8lf  CT=%.8lf  AFTER Fit=%.8lf  CT=%.8lf", sName, d_ct_fit_before, 1.0/d_ct_fit_before, d_ct_fit_after, 1.0/d_ct_fit_after);
	listInfo->Items->Add((String *) s_buf);
	listInfo->Refresh();
}//void  CVirusGA::v_switch_coding_of_cm_GA(CmGA *pcSolution, System::Windows::Forms::ListBox*  listInfo)




CError  CVirusGA::eRunTabuSearch
	(
	double  dMaxTime, double  dSimpleTime,
	CString  sReportFile,  
	CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, 
	double dProRep, double dProPath,
	double dTabuListLimitPercent, double dNoImprovementLimitPercent, double dWorseningFactor,
	System::Windows::Forms::ListBox*  listInfo
	)
{
	/*int  *pi_buf;

	int  ***pi_tab;
	int  i_test;
	
	pi_buf = &i_test;
	pi_tab[2][3] = pi_buf;

	(&i_test)[0] = *pi_buf;
	(**pi_tab)[1] = *pi_buf;

	(&pi_tab)[0][1] = &pi_buf;
	*((*(pi_tab + 1))[1]) = *pi_buf;*/



	CError  c_err;


	CString  s_buf, s_buf2;	
	bool  b_clever_paths = true;

	if  (pc_fitness  ==  NULL)
	{
		c_err.vPutError("No fitness function defined");
		return(c_err);	
	}//if  (pc_fitness  ==  NULL)

	pc_fitness->vZeroEval();

	

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initializing...");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	

	if  (pv_population  !=  NULL)
	{
		for  (int  ii = 0; ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);	
		pv_population->clear();
	}//if  (pv_population  !=  NULL)
	else
		pv_population  =  new  vector  <CmGA *>;

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initting...1");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)



	CmGA  cBestSolution(pc_random_gen, i_bayesian_effect, i_solution_construction_method);
	CmGA  cCurSolution(pc_random_gen, i_bayesian_effect, i_solution_construction_method);
	CmGA  cCandidateSolution(pc_random_gen, i_bayesian_effect, i_solution_construction_method);

	cBestSolution.eSetFitness(pc_fitness, true);
	cBestSolution.eConfigureTemplate();
	cBestSolution.eVirGenerateRandomTemplate();

	cCurSolution.eSetFitness(pc_fitness, true);
	cCurSolution.eConfigureTemplate();
	cCurSolution.eVirGenerateRandomTemplate();

	cCandidateSolution.eSetFitness(pc_fitness, true);
	cCandidateSolution.eConfigureTemplate();
	cCandidateSolution.eVirGenerateRandomTemplate();

	

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initting...2");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	CTimeCounter  c_time_counter;

	
	FILE  *pf_report;
	pf_report  =  fopen(sReportFile,  "w+");
	c_time_counter.vSetStartNow();

	int  i_probe_num;
	double  d_buf,  d_best_fitness,  d_prev_best_fitness, d_prev_average,  d_average,  d_time_passed, d_last_probe;
	double  d_time_passed_till_max, d_fitness_eval_till_max;

	double  d_fitness_sum;
	bool  b_optimal_soluion_reached;
	b_optimal_soluion_reached = false;

	d_time_passed  =  0;
	d_last_probe = 0;
	i_probe_num = 0;

	d_prev_average  =  0;
	d_average  =  0;
	d_best_fitness  =  0;
	d_prev_best_fitness = -1;


	vector<CTabuMove> vTabuList;
	vector<CTabuMove> vUsedMoves;


	//int TabuListLimit = TabuListLimitPercent * (q+(k+t)*r);
	int qAny = pc_fitness->pcGetNet()->iGetDemandNumberAny1Direction();//problem.qAnyGet();
	int qUni = pc_fitness->pcGetNet()->iGetDemandNumberUni();//problem.qUniGet();

	int q = qUni + qAny;
	int r = pc_fitness->pcGetNet()->iGetReplicesNum();//problem.rGet();
	int k = pc_fitness->pcGetNet()->pcGetDemands()[0].pvGetPossiblePaths()->size() / pc_fitness->pcGetNet()->iGetReplicesNum();
	int t = 0;
	int iTabuListLimit = dTabuListLimitPercent * (q+(k+t)*r);

	int  iNoImprovementLimit;
	//int totalMoves = (r-1)*qAny + (qAny+qAny+qUni)*(k-1) + q*(q-1)/2 + qMulti * (t-1);
	int totalMoves = (r-1)*qAny + (qAny+qAny+qUni)*(k-1) + q*(q-1)/2;
	totalMoves /= 3;

	iNoImprovementLimit = dNoImprovementLimitPercent * (q+r*(k+t));
	if  (iNoImprovementLimit  >  totalMoves)
		iNoImprovementLimit = totalMoves;
	if  (iNoImprovementLimit <= 0)  iNoImprovementLimit = 1;

	
	int iNoImprovement = 0;

	CTabuMove c_move;
	double  d_goal_cur_fitness;
	d_goal_cur_fitness = cCurSolution.dGetCompTemplateFitness();
	//pv_population->at(0)->vCopyTo(pc_best_individual_global);
	if  (listInfo  !=  NULL)  listInfo->Items->Add(S"Started..");
	bool  b_improved;

	for (int  i_cur_gen = 0; (d_time_passed < dMaxTime) && (b_optimal_soluion_reached ==  false);  i_cur_gen++)
	{
		b_improved = false;
		//do the switch...
		if  ( (d_time_passed  > dSimpleTime)&&(i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION) )
		{
			s_buf.Format("Time: %.2lf/%.2lf SWITCHING TO FLUENT OPTIMIZATION", d_time_passed, dMaxTime);
			listInfo->Items->Add((String *) s_buf);
			listInfo->Refresh();


			v_switch_coding_of_cm_GA("BEST:", &cBestSolution, listInfo);
			v_switch_coding_of_cm_GA("CurSol:", &cCurSolution, listInfo);
			v_switch_coding_of_cm_GA("Candidate:", &cCandidateSolution, listInfo);



			s_buf.Format("END OF SWITCHING");
			listInfo->Items->Add((String *) s_buf);
			listInfo->Refresh();
			

			
			i_solution_construction_method = SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION;
		}//if  ( (d_time_passed  > dSimpleTime)&&(i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION) )


		c_time_counter.bGetTimePassed(&d_time_passed);

		c_move = cGenerateMoveIntelligent(&vTabuList, &vUsedMoves, &cCurSolution, dProRep, dProPath);

		//if(move == errorMove)  break;
		if  (c_move.i_move_type != TABU_MOVE_TYPE_NO_MOVE)
		{
			c_move.vApply(&cCurSolution, &cCandidateSolution, pc_fitness, pc_random_gen);	

			//if(candidateGoalValue.MaxLinkSpec < curGoalValue.MaxLinkSpec)
			//if  (cCandidateSolution.dGetCompTemplateFitness() > cCurSolution.dGetCompTemplateFitness())
			if  (cCandidateSolution.dGetCompTemplateFitness() > d_goal_cur_fitness)
			{
				//if(candidateGoalValue.MaxLinkSpec < bestGoalValue.MaxLinkSpec)
				if  (cCandidateSolution.dGetCompTemplateFitness() > cBestSolution.dGetCompTemplateFitness())
				{
					b_improved = true;
					cCandidateSolution.vCopyTo(&cBestSolution);
					cCandidateSolution.vCopyTo(&cCurSolution);
					d_goal_cur_fitness = cCurSolution.dGetCompTemplateFitness();

					vUsedMoves.clear();
					vTabuList.push_back(c_move);

					while (vTabuList.size() > iTabuListLimit)  vTabuList.erase(vTabuList.begin());
				}//if  (cCurSolution.dGetCompTemplateFitness() < cBestSolution.dGetCompTemplateFitness())
				else
				{
					cCandidateSolution.vCopyTo(&cCurSolution);
					d_goal_cur_fitness = cCurSolution.dGetCompTemplateFitness();
					iNoImprovement = 0;
				}//else  if  (cCurSolution.dGetCompTemplateFitness() < cBestSolution.dGetCompTemplateFitness())

			
			}//if  (cCurSolution.dGetCompTemplateFitness() < cCandidateSolution.dGetCompTemplateFitness())
			else
			{
				vUsedMoves.push_back(c_move);
				iNoImprovement++;

				if(iNoImprovement >= iNoImprovementLimit)
				{
					iNoImprovement = 0;

					//curGoalValue.MaxLinkSpec += worseningFactor * curGoalValue.MaxLinkSpec;
					d_goal_cur_fitness = 1.0 / cCurSolution.dGetCompTemplateFitness();
					d_goal_cur_fitness += d_goal_cur_fitness * dWorseningFactor;
					d_goal_cur_fitness = 1.0 / d_goal_cur_fitness;

					vUsedMoves.clear();
				}//if(iNoImprovement >= noImprovementLimit)
			}//else  if  (cCurSolution.dGetCompTemplateFitness() < cCandidateSolution.dGetCompTemplateFitness())

			c_time_counter.bGetTimePassed(&d_time_passed);

			if  (b_improved == true)
			{
				if  (listInfo  !=  NULL)
				{
					s_buf.Format
						(
						"Generation: %d (best: %.8lf) current: %.8lf [fitness evaluations:%.0lf] [time passed:%.4lf]",  
						i_cur_gen,  1 / cBestSolution.dGetCompTemplateFitness(), 1 / cCurSolution.dGetCompTemplateFitness(),
						pc_fitness->dGetFuncEval(),
						d_time_passed
						);
					listInfo->Items->Add((String *) s_buf);
					listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
					listInfo->Refresh();

					System::Windows::Forms::Application::DoEvents();
				}//if  (listInfo  !=  NULL)
			}//if  (iNoImprovement == 0)

		}//if  (c_move.i_move_type != TABU_MOVE_TYPE_NO_MOVE)
	}//for (int  i_cur_gen = 0; (d_time_passed < dMaxTime) && (b_optimal_soluion_reached ==  false);  i_cur_gen++)
	

	CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	double d_templ_fitness  =  c_buf.dComputeFitness(cBestSolution.pc_competetive_template);	
	//d_templ_fitness  =  pc_fitness->dCompCost(&b_const);
	FILE  *pf_solution;
	pf_solution  =  fopen(sReportFile + "_solf.txt",  "w+");
	pc_fitness->eSaveSolution(pf_solution);
	fclose(pf_solution);

	bool  b_const = false;
	double d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);

	double  d_ref_solution = 1;


	FILE  *pf_summarized_rep;
	pf_summarized_rep  =  fopen(sSummarizedReport, "a");
	fprintf
		(pf_summarized_rep, 

		"%d \t %d \t %d \t %.8lf \t %.8lf \t %.8lf \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.0lf \t %d \t %s \t timeTillBest: \t %.2lf \t FfeTillBest: \t %.0lf\n", 

		pc_fitness->pcGetNet()->iGetDemandNumberUni(), pc_fitness->pcGetNet()->iGetDemandNumberAny(), pc_fitness->pcGetNet()->iGetDemandNumberMulti(), 
		//c_num.sToStringFirstDgitsDotted(2), c_num.sToString10Pow(),
		//pc_fitness->pcGetNet()->pcGetDemands()[0].pvGetPossiblePaths()->size();
		d_templ_fitness, d_templ_fitness_no_pen, d_ref_solution,  d_templ_fitness / d_ref_solution, d_templ_fitness_no_pen / d_ref_solution, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, sReportFile,
		pc_fitness->dGetTimeTillBest(), pc_fitness->dGetFfeTillBest()
		);
	fclose(pf_summarized_rep);//*/

	/*

	if  (listInfo  !=  NULL)  listInfo->Items->Add(S"Started..");
	for (int  i_cur_gen = 0; (d_time_passed < dMaxTime) && (b_optimal_soluion_reached ==  false);  i_cur_gen++)
	{
		if  (d_time_passed - d_last_probe > dProbingTime)
		{
			double  d_best_ct, d_ct_fit_cur;
			double d_templ_fitness_no_pen;
			bool  b_const;
			CMessyIndividual  c_best_ct_fit(pc_fitness, i_bayesian_effect, i_solution_construction_method);
			d_best_ct = 0;
			d_templ_fitness_no_pen = 0;

			for  (int  i_ct = 0; i_ct < pv_population->size(); i_ct++)
			{
				d_ct_fit_cur  =  c_best_ct_fit.dComputeFitness(pv_population->at(i_ct)->pc_competetive_template);	
				if  (d_best_ct < d_ct_fit_cur)
				{
					d_best_ct = d_ct_fit_cur;
					d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);
				}//if  (d_best_ct < d_ct_fit_cur)
			}//for  (int  i_ct = 0; i_ct < pv_population->size(); i_ct++)


			FILE  *pf_summarized_rep;
			pf_summarized_rep  =  fopen(sSummarizedReport, "a");
			fprintf
				(pf_summarized_rep, 

				"PROBE \t %d \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.1lf \t %d \t %s \n", 

				i_probe_num,//pc_fitness->pcGetNet()->pcGetDemands()[0].pvGetPossiblePaths()->size();
				d_best_ct, d_templ_fitness_no_pen, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, sReportFile
				);
			fclose(pf_summarized_rep);

			d_last_probe = d_time_passed;
			i_probe_num++;		
		}//if  (d_time_passed - d_last_probe > dProbingTime)
		//::MessageBox(NULL, "0", "0", MB_OK);

		fprintf(pf_report,  "Generation: %d \n", i_cur_gen);

		d_prev_average  =  d_average;
		d_prev_best_fitness  =  d_best_fitness;
		

		d_best_fitness  =  0;
		d_average  =  0;
		

		d_fitness_sum = 0;
		//first get fit summ
		for (int  ii = 0; ii < (int) pv_population->size();  ii++)
		{
			//pv_population->at(ii)->d_templ_fitness = -1;
			d_buf  =  pv_population->at(ii)->dGetCompTemplateFitness();
			d_fitness_sum  +=  d_buf;
		}//for (int  ii = 0; ii < i_pop_size;  ii++)
		d_average = d_fitness_sum / (int) pv_population->size();
		pc_best_individual_current_pop = pc_sga_get_best_individual(pv_population);

		if  (pc_best_individual_global->dGetCompTemplateFitness() < pc_best_individual_current_pop->dGetCompTemplateFitness())
		{
			pc_best_individual_current_pop->vCopyTo(pc_best_individual_global);
		}//if  (pc_best_individual_global->dGetCompTemplateFitness() < pc_best_individual_current_pop->dGetCompTemplateFitness())
		d_best_fitness = pc_best_individual_global->dGetCompTemplateFitness();//

		//::MessageBox(NULL, "1", "1", MB_OK);

		c_time_counter.bGetTimePassed(&d_time_passed);
		fprintf(pf_report,  "\n\nFITNESS EVALUATIONS: %.0lf  \t  time passed:%.4lf \n\n\n\n", pc_fitness->dGetFuncEval(), d_time_passed);

		if  (listInfo  !=  NULL)
		{
			s_buf.Format
				(
				"Generation: %d ct number: %d (best fitness: %.8lf) average: %.8lf best - average:%.8lf  [fitness evaluations:%.0lf] [time passed:%.4lf]",  
				i_cur_gen,  pv_population->size(), 1 / d_best_fitness, d_average,
				d_best_fitness  - d_average,
				pc_fitness->dGetFuncEval(),
				d_time_passed
				);
			listInfo->Items->Add((String *) s_buf);
			listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
			listInfo->Refresh();

			System::Windows::Forms::Application::DoEvents();
		}//if  (listInfo  !=  NULL)

		
		
		CmGA  *pc_parent_0, *pc_parent_1;
		CmGA  *pc_child_0, *pc_child_1;
		for  (int  ii = 0; ii < this->pv_population->size() / 2; ii++)
		{
			pc_parent_0 = pc_sga_get_parent_roulette(d_fitness_sum);
			pc_parent_1 = pc_sga_get_parent_roulette(d_fitness_sum);

			pc_parent_0->vSgaCrossSinglePoint(pc_parent_1, &(pv_next_population->at(ii*2)), &(pv_next_population->at(ii*2 + 1)), d_sga_cross_prob);

			//vShow(pv_next_population->at(ii*2)->d_templ_fitness);
		}//while  (pv_next_population->size() <  i_sga_pop_size)

		//::MessageBox(NULL, "2", "2", MB_OK);

		pc_best_individual_global->vCopyTo(pv_next_population->at(0));


		for  (int  ii = 0;  ii  <  pv_next_population->size();  ii++)
		{
			if  (pc_random_gen->NextDouble()  <  d_sga_mut_prob)
				pv_next_population->at(ii)->vSgaMutate();
		
		}//for  (int  ii = 0;  ii  <  pv_next_population->size();  ii++)

		pc_best_individual_current_pop = pc_sga_get_best_individual(pv_next_population);
		if  (pc_best_individual_global->dGetCompTemplateFitness() < pc_best_individual_current_pop->dGetCompTemplateFitness())
		{
			pc_best_individual_current_pop->vCopyTo(pc_best_individual_global);
		}//if  (pc_best_individual_global->dGetCompTemplateFitness() < pc_best_individual_current_pop->dGetCompTemplateFitness())
		d_best_fitness = pc_best_individual_global->dGetCompTemplateFitness();//


		
		//::MessageBox(NULL, "3", "3", MB_OK);

		pv_pop_buf = pv_population;
		pv_population = pv_next_population;
		pv_next_population = pv_pop_buf;

		//::MessageBox(NULL, "4", "4", MB_OK);

		
		fprintf(pf_report,  "\n\n\n");
		System::Windows::Forms::Application::DoEvents();

		if  (pdOptimalValue  !=  NULL)
		{
			if  (d_best_fitness  >  0)
			{
				if  ((1.0 / d_best_fitness) + 1  <=  *pdOptimalValue)  b_optimal_soluion_reached = true;
			}//if  (d_best_fitness  >  0)
		}//if  (pdOptimalValue  !=  NULL)	

		//::MessageBox(NULL, "k", "k", MB_OK);
	}//for (int  ii = 0; ii < i_gen;  ii++)*/


	
/*

	fprintf(pf_report,  "\n\n\n");
	fprintf(pf_report,  "\n\nFITNESS EVALUATIONS: %.0lf\n\n\n\n", pc_fitness->dGetFuncEval());
	fclose(pf_report);

	for  (int  ii = 0; ii < (int) pv_pattern_pool->size();  ii++)
		delete  pv_pattern_pool->at(ii);
	pv_pattern_pool->clear();

	if  (pc_best_individual_global  ==  NULL)
	{
		pc_best_individual_global = pc_best_individual_current_pop->pcSgaClone();
	}//if  (pc_best_individual_global  ==  NULL)
	else
	{
		if  (pc_best_individual_global->dGetCompTemplateFitness() < pc_best_individual_current_pop->dGetCompTemplateFitness())
		{
			delete  pc_best_individual_global;
			pc_best_individual_global = pc_best_individual_current_pop->pcSgaClone();			
		}//if  (pc_best_individual_global->dGetCompTemplateFitness() < pc_best_individual_current_pop->dGetCompTemplateFitness())
	}//else  if  (pc_best_individual_global  ==  NULL)
	d_best_fitness = pc_best_individual_global->dGetCompTemplateFitness();


	
	FILE  *pf_summarized_rep;
	
	double  d_templ_fitness;
	bool  b_const;
	double  d_ref_solution;

	if  (sReferenceSolution  !=  "")
	{
		//vShow("("+sReferenceSolution+")");
		pc_fitness->eLoadSolution(sReferenceSolution);
		pc_fitness->eApplySolution();
		d_ref_solution = pc_fitness->dCompCost(&b_const);
	}//if  (sReferenceSolution  !=  "")
	else
		d_ref_solution = 1;

	CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	d_templ_fitness  =  c_buf.dComputeFitness(pc_best_individual_global->pc_competetive_template);	
	FILE  *pf_solution;
	pf_solution  =  fopen(sReportFile + "_solf.txt",  "w+");
	pc_fitness->eSaveSolution(pf_solution);
	fclose(pf_solution);

	
	

	double d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);
	if  (d_templ_fitness_no_pen  <  d_templ_fitness)
		b_const = true;
	else
		b_const = false;

	pf_summarized_rep  =  fopen(sSummarizedReport, "a");
	fprintf
		(pf_summarized_rep, 
		"%.8lf \t %.8lf \t %.8lf \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.0lf \t %d \t %s \n", 

		d_templ_fitness, d_templ_fitness_no_pen, d_ref_solution,  d_templ_fitness / d_ref_solution, d_templ_fitness_no_pen / d_ref_solution, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, sReportFile
		);
	fclose(pf_summarized_rep);*/


	return(c_err);
}//CError  CVirusGA::eRunTabuSearch(double  dMaxTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, System::Windows::Forms::ListBox*  listInfo)








CError  CVirusGA::eRunSimpleGA(double  dMaxTime, double  dProbingTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, System::Windows::Forms::ListBox*  listInfo)
{
	CError  c_err;

	CString  s_buf, s_buf2;
	
	bool  b_clever_paths = true;

	if  (pc_fitness  ==  NULL)
	{
		c_err.vPutError("No fitness function defined");
		return(c_err);	
	}//if  (pc_fitness  ==  NULL)

		

	pc_fitness->vZeroEval();



	//::MessageBox(NULL, sReferenceSolution, sReferenceSolution, MB_OK);

	

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initializing...");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	

	if  (pv_population  !=  NULL)
	{
		for  (int  ii = 0; ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);	
		pv_population->clear();
	}//if  (pv_population  !=  NULL)
	else
		pv_population  =  new  vector  <CmGA *>;

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initting...1");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	vector  <CmGA  *>  *pv_next_population, *pv_pop_buf;
	pv_next_population = new vector  <CmGA  *>();

	v_create_simple_ga_pop(pv_population);
	v_create_simple_ga_pop(pv_next_population);	


	if  (sReferenceSolution  !=  "")
	{
		listInfo->Items->Add(S"Loading initial solution...");
		listInfo->Refresh();
		//::MessageBox(NULL, sReferenceSolution, sReferenceSolution, MB_OK);
		c_err = e_load_init_solution(sReferenceSolution);
		if  (c_err)  return(c_err);

		bool  b_const = true;
		double  d_afa;
		d_afa = pc_fitness->dCompCost(&b_const);

		/*bool  b_const = true;
		double  d_afa;
		d_afa = pc_fitness->dCompCost(&b_const);
		s_buf.Format("%.2lf", d_afa);
		//::MessageBox(NULL, s_buf, s_buf, MB_OK);

		FILE  *pf_afa;
		pf_afa = fopen("afa.txt", "a");
		fprintf(pf_afa, "%s \t %.2lf\n", sReferenceSolution, d_afa);
		fclose(pf_afa);*/

		c_err = pv_population->at(0)->eMakeGentoypeFromSolution(pc_fitness);
		pc_fitness->pcGetNet()->eResetNet();

		double  d_ct_fit;
		d_ct_fit = pv_population->at(0)->dGetCompTemplateFitness();

		s_buf.Format("AFA=%.2lf  CT=%.2lf", d_afa, 1/d_ct_fit);
		listInfo->Items->Add((String *) s_buf);
		listInfo->Refresh();
	}//if  (s_solf_file  !=  "");

	

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initting...2");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	CTimeCounter  c_time_counter;

	
	FILE  *pf_report;
	pf_report  =  fopen(sReportFile,  "w+");
	vSgaSaveParameters(pf_report);
	c_time_counter.vSetStartNow();

	int  i_probe_num;
	double  d_buf,  d_best_fitness,  d_prev_best_fitness, d_prev_average,  d_average,  d_time_passed, d_last_probe;
	double  d_time_passed_till_max, d_fitness_eval_till_max;

	double  d_fitness_sum;
	bool  b_optimal_soluion_reached;
	b_optimal_soluion_reached = false;

	d_time_passed  =  0;
	d_last_probe = 0;
	i_probe_num = 0;

	d_prev_average  =  0;
	d_average  =  0;
	d_best_fitness  =  0;
	d_prev_best_fitness = -1;
	
	CmGA  *pc_best_individual_global, *pc_best_individual_current_pop;

	pc_best_individual_global = new CmGA(pc_random_gen, i_bayesian_effect, i_solution_construction_method);
	pc_best_individual_global->eSetFitness(pc_fitness, true);
	pc_best_individual_global->eConfigureTemplate();
	pc_best_individual_global->eVirGenerateRandomTemplate();

	pv_population->at(0)->vCopyTo(pc_best_individual_global);
	
	


	if  (listInfo  !=  NULL)  listInfo->Items->Add(S"Started..");
	for (int  i_cur_gen = 0; (d_time_passed < dMaxTime) && (b_optimal_soluion_reached ==  false);  i_cur_gen++)
	{
		if  (d_time_passed - d_last_probe > dProbingTime)
		{
			double  d_best_ct, d_ct_fit_cur;
			double d_templ_fitness_no_pen;
			bool  b_const;
			CMessyIndividual  c_best_ct_fit(pc_fitness, i_bayesian_effect, i_solution_construction_method);
			d_best_ct = 0;
			d_templ_fitness_no_pen = 0;

			for  (int  i_ct = 0; i_ct < pv_population->size(); i_ct++)
			{
				d_ct_fit_cur  =  c_best_ct_fit.dComputeFitness(pv_population->at(i_ct)->pc_competetive_template);	
				if  (d_best_ct < d_ct_fit_cur)
				{
					d_best_ct = d_ct_fit_cur;
					d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);
				}//if  (d_best_ct < d_ct_fit_cur)
			}//for  (int  i_ct = 0; i_ct < pv_population->size(); i_ct++)


			FILE  *pf_summarized_rep;
			pf_summarized_rep  =  fopen(sSummarizedReport, "a");
			fprintf
				(pf_summarized_rep, 

				"PROBE \t %d \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.1lf \t %d \t %s \n", 

				i_probe_num,//pc_fitness->pcGetNet()->pcGetDemands()[0].pvGetPossiblePaths()->size();
				d_best_ct, d_templ_fitness_no_pen, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, sReportFile
				);
			fclose(pf_summarized_rep);

			d_last_probe = d_time_passed;
			i_probe_num++;		
		}//if  (d_time_passed - d_last_probe > dProbingTime)
		//::MessageBox(NULL, "0", "0", MB_OK);

		fprintf(pf_report,  "Generation: %d \n", i_cur_gen);

		d_prev_average  =  d_average;
		d_prev_best_fitness  =  d_best_fitness;
		

		d_best_fitness  =  0;
		d_average  =  0;
		

		d_fitness_sum = 0;
		//first get fit summ
		for (int  ii = 0; ii < (int) pv_population->size();  ii++)
		{
			//pv_population->at(ii)->d_templ_fitness = -1;
			d_buf  =  pv_population->at(ii)->dGetCompTemplateFitness();
			d_fitness_sum  +=  d_buf;
		}//for (int  ii = 0; ii < i_pop_size;  ii++)
		d_average = d_fitness_sum / (int) pv_population->size();
		pc_best_individual_current_pop = pc_sga_get_best_individual(pv_population);

		if  (pc_best_individual_global->dGetCompTemplateFitness() < pc_best_individual_current_pop->dGetCompTemplateFitness())
		{
			pc_best_individual_current_pop->vCopyTo(pc_best_individual_global);
		}//if  (pc_best_individual_global->dGetCompTemplateFitness() < pc_best_individual_current_pop->dGetCompTemplateFitness())
		d_best_fitness = pc_best_individual_global->dGetCompTemplateFitness();//*/

		//::MessageBox(NULL, "1", "1", MB_OK);

		c_time_counter.bGetTimePassed(&d_time_passed);
		fprintf(pf_report,  "\n\nFITNESS EVALUATIONS: %.0lf  \t  time passed:%.4lf \n\n\n\n", pc_fitness->dGetFuncEval(), d_time_passed);

		if  (listInfo  !=  NULL)
		{
			s_buf.Format
				(
				"Generation: %d ct number: %d (best fitness: %.8lf) average: %.8lf best - average:%.8lf  [fitness evaluations:%.0lf] [time passed:%.4lf]",  
				i_cur_gen,  pv_population->size(), 1 / d_best_fitness, d_average,
				d_best_fitness  - d_average,
				pc_fitness->dGetFuncEval(),
				d_time_passed
				);
			listInfo->Items->Add((String *) s_buf);
			listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
			listInfo->Refresh();

			System::Windows::Forms::Application::DoEvents();
		}//if  (listInfo  !=  NULL)

		
		
		CmGA  *pc_parent_0, *pc_parent_1;
		CmGA  *pc_child_0, *pc_child_1;
		for  (int  ii = 0; ii < this->pv_population->size() / 2; ii++)
		{
			pc_parent_0 = pc_sga_get_parent_roulette(d_fitness_sum);
			pc_parent_1 = pc_sga_get_parent_roulette(d_fitness_sum);

			pc_parent_0->vSgaCrossSinglePoint(pc_parent_1, &(pv_next_population->at(ii*2)), &(pv_next_population->at(ii*2 + 1)), d_sga_cross_prob);

			//vShow(pv_next_population->at(ii*2)->d_templ_fitness);
		}//while  (pv_next_population->size() <  i_sga_pop_size)*/

		//::MessageBox(NULL, "2", "2", MB_OK);

		pc_best_individual_global->vCopyTo(pv_next_population->at(0));


		for  (int  ii = 0;  ii  <  pv_next_population->size();  ii++)
		{
			if  (pc_random_gen->NextDouble()  <  d_sga_mut_prob)
				pv_next_population->at(ii)->vSgaMutate();
		
		}//for  (int  ii = 0;  ii  <  pv_next_population->size();  ii++)

		pc_best_individual_current_pop = pc_sga_get_best_individual(pv_next_population);
		if  (pc_best_individual_global->dGetCompTemplateFitness() < pc_best_individual_current_pop->dGetCompTemplateFitness())
		{
			pc_best_individual_current_pop->vCopyTo(pc_best_individual_global);
		}//if  (pc_best_individual_global->dGetCompTemplateFitness() < pc_best_individual_current_pop->dGetCompTemplateFitness())
		d_best_fitness = pc_best_individual_global->dGetCompTemplateFitness();//*/


		
		//::MessageBox(NULL, "3", "3", MB_OK);

		pv_pop_buf = pv_population;
		pv_population = pv_next_population;
		pv_next_population = pv_pop_buf;

		//::MessageBox(NULL, "4", "4", MB_OK);

		
		fprintf(pf_report,  "\n\n\n");
		System::Windows::Forms::Application::DoEvents();

		if  (pdOptimalValue  !=  NULL)
		{
			if  (d_best_fitness  >  0)
			{
				if  ((1.0 / d_best_fitness) + 1  <=  *pdOptimalValue)  b_optimal_soluion_reached = true;
			}//if  (d_best_fitness  >  0)
		}//if  (pdOptimalValue  !=  NULL)	

		//::MessageBox(NULL, "k", "k", MB_OK);
	}//for (int  ii = 0; ii < i_gen;  ii++)


	


	fprintf(pf_report,  "\n\n\n");
	fprintf(pf_report,  "\n\nFITNESS EVALUATIONS: %.0lf\n\n\n\n", pc_fitness->dGetFuncEval());
	fclose(pf_report);

	for  (int  ii = 0; ii < (int) pv_pattern_pool->size();  ii++)
		delete  pv_pattern_pool->at(ii);
	pv_pattern_pool->clear();

	if  (pc_best_individual_global  ==  NULL)
	{
		pc_best_individual_global = pc_best_individual_current_pop->pcSgaClone();
	}//if  (pc_best_individual_global  ==  NULL)
	else
	{
		if  (pc_best_individual_global->dGetCompTemplateFitness() < pc_best_individual_current_pop->dGetCompTemplateFitness())
		{
			delete  pc_best_individual_global;
			pc_best_individual_global = pc_best_individual_current_pop->pcSgaClone();			
		}//if  (pc_best_individual_global->dGetCompTemplateFitness() < pc_best_individual_current_pop->dGetCompTemplateFitness())
	}//else  if  (pc_best_individual_global  ==  NULL)
	d_best_fitness = pc_best_individual_global->dGetCompTemplateFitness();


	
	FILE  *pf_summarized_rep;
	
	double  d_templ_fitness;
	bool  b_const;
	double  d_ref_solution;

	if  (sReferenceSolution  !=  "")
	{
		//vShow("("+sReferenceSolution+")");
		pc_fitness->eLoadSolution(sReferenceSolution);
		pc_fitness->eApplySolution();
		d_ref_solution = pc_fitness->dCompCost(&b_const);
	}//if  (sReferenceSolution  !=  "")
	else
		d_ref_solution = 1;

	CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	d_templ_fitness  =  c_buf.dComputeFitness(pc_best_individual_global->pc_competetive_template);	
	FILE  *pf_solution;
	pf_solution  =  fopen(sReportFile + "_solf.txt",  "w+");
	pc_fitness->eSaveSolution(pf_solution);
	fclose(pf_solution);

	
	

	double d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);
	if  (d_templ_fitness_no_pen  <  d_templ_fitness)
		b_const = true;
	else
		b_const = false;

	pf_summarized_rep  =  fopen(sSummarizedReport, "a");
	fprintf
		(pf_summarized_rep, 
		"%.8lf \t %.8lf \t %.8lf \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.0lf \t %d \t %s \n", 

		d_templ_fitness, d_templ_fitness_no_pen, d_ref_solution,  d_templ_fitness / d_ref_solution, d_templ_fitness_no_pen / d_ref_solution, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, sReportFile
		);
	fclose(pf_summarized_rep);


	return(c_err);
}//CError  CVirusGA::eRunSimpleGA(double  dMaxTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, System::Windows::Forms::ListBox*  listInfo)




CError  CVirusGA::eRun(double  dMaxTime, double  dSimpleTime, double  dProbingTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, System::Windows::Forms::ListBox*  listInfo)
{
	CError  c_err;
	CString  s_buf, s_buf2;

	
	bool  b_clever_paths = true;

	if  (pc_fitness  ==  NULL)
	{
		c_err.vPutError("No fitness function defined");
		return(c_err);	
	}//if  (pc_fitness  ==  NULL)

		

	pc_fitness->vZeroEval();
	pc_fitness->vInitStatic();


	

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initializing...");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	

	//first generate population of messy individuals
	if  (pv_population  !=  NULL)
	{
		for  (int  ii = 0; ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);	
		pv_population->clear();
	}//if  (pv_population  !=  NULL)
	else
		pv_population  =  new  vector  <CmGA *>;

	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initting...1");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	double  d_place_holder, d_place_holder2;
	v_add_new_ct(listInfo, &d_place_holder, &d_place_holder2);

	if  (sReferenceSolution  !=  "")
	{
		listInfo->Items->Add(S"Loading initial solution...");
		listInfo->Refresh();
		//::MessageBox(NULL, sReferenceSolution, sReferenceSolution, MB_OK);
		c_err = e_load_init_solution(sReferenceSolution);
		if  (c_err)  return(c_err);
		//::MessageBox(NULL, "init loaded", "init loaded", MB_OK);

		bool  b_const = true;
		double  d_afa;
		d_afa = pc_fitness->dCompCost(&b_const);

		s_buf.Format("Initial solution value: %lf", d_afa);
		listInfo->Items->Add((String *) s_buf);
		listInfo->Refresh();

		/*bool  b_const = true;
		double  d_afa;
		d_afa = pc_fitness->dCompCost(&b_const);
		s_buf.Format("%.2lf", d_afa);
		//::MessageBox(NULL, s_buf, s_buf, MB_OK);

		FILE  *pf_afa;
		pf_afa = fopen("afa.txt", "a");
		fprintf(pf_afa, "%s \t %.2lf\n", sReferenceSolution, d_afa);
		fclose(pf_afa);*/

		c_err = pv_population->at(0)->eMakeGentoypeFromSolution(pc_fitness);
		pc_fitness->pcGetNet()->eResetNet();

		double  d_ct_fit;
		d_ct_fit = pv_population->at(0)->dGetCompTemplateFitness();

		s_buf.Format("AFA=%.2lf  CT=%.2lf", d_afa, 1/d_ct_fit);
		listInfo->Items->Add((String *) s_buf);
		listInfo->Refresh();

		//::MessageBox(NULL, s_buf, s_buf, MB_OK);
	}//if  (s_solf_file  !=  "");



	if  (listInfo  !=  NULL)
	{
		listInfo->Items->Add(S"Initting...2");
		listInfo->Refresh();
	}//if  (listInfo  !=  NULL)


	CTimeCounter  c_time_counter;

	
	FILE  *pf_report;
	pf_report  =  fopen(sReportFile,  "w+");
	vSaveParameters(pf_report);
	c_time_counter.vSetStartNow();

	int  i_probe_num;
	double  d_buf,  d_best_fitness,  d_prev_best_fitness, d_prev_average,  d_average,  d_time_passed, d_last_probe;
	double  d_time_passed_till_max, d_fitness_eval_till_max;

	int  i_vir_init_done,  i_break_at_the_same_base,  i_break_at_immediate_infect;

	bool  b_optimal_soluion_reached;
	b_optimal_soluion_reached = false;
	d_time_passed  =  0;
	d_last_probe = 0;
	i_probe_num = 0;

	i_break_at_the_same_base  =  0;
	i_break_at_immediate_infect  =  0;
	d_prev_average  =  0;
	d_average  =  0;
	d_best_fitness  =  0;
	d_prev_best_fitness = -1;

	

	
	if  (listInfo  !=  NULL)  listInfo->Items->Add(S"Started..");
	for (int  i_cur_gen = 0; (d_time_passed < dMaxTime) && (b_optimal_soluion_reached ==  false);  i_cur_gen++)
	{
		//do the switch...
		if  ( (d_time_passed  > dSimpleTime)&&(i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION) )
		{
			for  (int  i_ct = 0; i_ct < pv_population->size(); i_ct++)
			{
				s_buf.Format("Time: %.2lf/%.2lf SWITCHING TO FLUENT OPTIMIZATION", d_time_passed, dMaxTime);
				listInfo->Items->Add((String *) s_buf);
				listInfo->Refresh();

				double  d_ct_fit_before, d_ct_fit_after;
				d_ct_fit_before = pv_population->at(i_ct)->dGetCompTemplateFitness();

				c_err = pv_population->at(i_ct)->eMakePreciseGentoypeFromFluent();
				pv_population->at(i_ct)->i_solution_construction_method = SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION;

				pv_population->at(i_ct)->d_templ_fitness = -1;
				d_ct_fit_after = pv_population->at(i_ct)->dGetCompTemplateFitness();

				s_buf.Format("POPULATION %d: Before Fit=%.8lf  CT=%.8lf  AFTER Fit=%.8lf  CT=%.8lf", i_ct, d_ct_fit_before, 1.0/d_ct_fit_before, d_ct_fit_after, 1.0/d_ct_fit_after);
				listInfo->Items->Add((String *) s_buf);
				listInfo->Refresh();


				s_buf.Format("END OF SWITCHING");
				listInfo->Items->Add((String *) s_buf);
				listInfo->Refresh();
			}//for  (int  i_ct = 0; i_ct < pv_population->size(); i_ct++)

			i_solution_construction_method = SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION;
		}//if  ( (d_time_passed  > dSimpleTime)&&(i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION) )


		
		if  (d_time_passed - d_last_probe > dProbingTime)
		{
			double  d_best_ct, d_ct_fit_cur;
			double d_templ_fitness_no_pen;
			bool  b_const;
			CMessyIndividual  c_best_ct_fit(pc_fitness, i_bayesian_effect, i_solution_construction_method);
			d_best_ct = 0;
			d_templ_fitness_no_pen = 0;

			for  (int  i_ct = 0; i_ct < pv_population->size(); i_ct++)
			{
				d_ct_fit_cur  =  c_best_ct_fit.dComputeFitness(pv_population->at(i_ct)->pc_competetive_template);	
				if  (d_best_ct < d_ct_fit_cur)
				{
					d_best_ct = d_ct_fit_cur;
					d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);
				}//if  (d_best_ct < d_ct_fit_cur)
			}//for  (int  i_ct = 0; i_ct < pv_population->size(); i_ct++)
			

			FILE  *pf_summarized_rep;
			pf_summarized_rep  =  fopen(sSummarizedReport, "a");
			fprintf
				(pf_summarized_rep, 

				"PROBE \t %d \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.1lf \t %d \t %s \t %d\n", 

				i_probe_num,//pc_fitness->pcGetNet()->pcGetDemands()[0].pvGetPossiblePaths()->size();
				d_best_ct, d_templ_fitness_no_pen, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, sReportFile, pv_population->size()
				);
			fclose(pf_summarized_rep);

			d_last_probe = d_time_passed;
			i_probe_num++;		
		}//if  (d_time_passed - d_last_probe > dProbingTime)

		fprintf(pf_report,  "Generation: %d \n", i_cur_gen);

		if  (d_prev_best_fitness  ==  d_best_fitness)
		{
			if  (d_average  <=  d_prev_average)
			{
				v_remove_the_same_ct();//we remove cts that are exactly the same
				
				s_buf.Format
					(
					"ct number refresh; number after refresh: %d    patterns number: %d",  
					pv_population->size(), pv_pattern_pool->size()
					);
				listInfo->Items->Add((String *) s_buf);
				listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
				listInfo->Refresh();

				v_add_new_ct(listInfo, &d_average, &d_best_fitness);//we add new ct to population if we are stucked
			}//if  (d_average  ==  d_prev_average)
		}//if  (d_prev_best_fitness  ==  d_best_fitness)
		else
		{
			if  (pv_population->size()  >  1)
			{
				if  (i_ct_add_remove_strategy  ==  1)
					v_remove_all_ct_except_best();
				else
					v_remove_the_same_ct();//we remove cts that are exactly the same


		
				s_buf.Format
					(
					"ct best fitness increased - reducing CT number to: %d",  
					pv_population->size()
					);
				listInfo->Items->Add((String *) s_buf);
				listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
				listInfo->Refresh();

			}//if  (pv_population->size()  >  1)

		}//else  if  (d_prev_best_fitness  ==  d_best_fitness)

		d_prev_average  =  d_average;
		d_prev_best_fitness  =  d_best_fitness;


		int  i_virginity_current_round;
		int  i_vir_pop_temp_size;
		i_vir_pop_temp_size = i_virus_pop_size;
		bool  b_removing_virginity;
		//first we remove virginity
		for (int  ii = 0; ii < (int) pv_population->size();  ii++)
		{

			//for  (int  i_virginity_init = 0; i_virginity_init < 2; i_virginity_init++)
			{
				i_virginity_current_round = 0;
				b_removing_virginity = false;
				
				while  ( (pv_population->at(ii)->bGetVirgin()  ==  true)&&(d_time_passed < dMaxTime)&&(i_virginity_current_round < i_virginity_rounds) )
				{
					b_removing_virginity = true;

					if  (i_virginity_current_round == 0)
					{
						c_time_counter.bGetTimePassed(&d_time_passed);
						s_buf.Format
							(
							"removing virginity start fitness: %.8lf [evaluations:%.0lf] (solBTime:%.4lf) [clePathFitEval:%.0lf] [clePathFitInit:%.0lf] [time passed:%.4lf]",  
							1 / pv_population->at(ii)->dGetCompTemplateFitness(),
							pc_fitness->dGetFuncEval(), pc_fitness->dGetConstructionTime(), pc_fitness->dGetCleverPathsEval(), pc_fitness->dGetCleverPathsInits(),
							pc_fitness->dGetFullBetterThanSimple(),
							d_time_passed
							);

						listInfo->Items->Add((String *) s_buf);
						listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
						listInfo->Refresh();
					}//if  (i_virginity_current_round == 0)

					i_virginity_current_round++;
					pv_population->at(ii)->eVirGeneratePopulation(this, &i_vir_pop_temp_size, b_clever_paths);

					if  (i_ct_add_remove_strategy  ==  2)
					{
						c_err  =  pv_population->at(ii)->eMuppetsVirRun(pv_pattern_pool, this,  &i_vir_init_done, 0, 1);
						
						if  (pv_population->at(ii)->pcGetPatternProposition()  ==  NULL)
							s_buf2.Format(" [no pattern][pool:%d]", pv_pattern_pool->size());
						else
							s_buf2.Format(" [%d/%d][pool:%d]", pv_population->at(ii)->pcGetPatternProposition()->pvGetGenotype()->size(), pc_fitness->pcGetNet()->iGetDemandNumber(), pv_pattern_pool->size());

						

						v_get_new_pattern_proposition_into_pattern_pool(pv_population->at(ii));
					}//if  (i_ct_add_remove_strategy  ==  2)
					else
						c_err  =  pv_population->at(ii)->eMuppetsVirRun(pv_pattern_pool, this,  &i_vir_init_done, 0, 0, listInfo);

					
					c_time_counter.bGetTimePassed(&d_time_passed);
					s_buf.Format
						(
						"removing virginity fitness: %.8lf [evaluations:%.0lf] (solBTime:%.4lf) [clePathFitEval:%.0lf] [clePathFitInit:%.0lf] [full>simple:%.0lf] [time passed:%.4lf]",  
						1 / pv_population->at(ii)->dGetCompTemplateFitness(),
						pc_fitness->dGetFuncEval(), pc_fitness->dGetConstructionTime(), pc_fitness->dGetCleverPathsEval(), pc_fitness->dGetCleverPathsInits(),
						pc_fitness->dGetFullBetterThanSimple(),
						d_time_passed
						);

					if  (i_ct_add_remove_strategy  ==  2)
					{
						s_buf = s_buf + s_buf2;				
					}//if  (i_ct_add_remove_strategy  ==  2)


					listInfo->Items->Add((String *) s_buf);
					listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
					listInfo->Refresh();

					//v_get_new_patterns();
					System::Windows::Forms::Application::DoEvents();
			
					//prw remove_02.13
					//i_virginity_current_round = i_virginity_rounds;
					if  (WATCH_FITNESS_LEVELS)  i_virginity_current_round = i_virginity_rounds;
				}//while  (pv_population->at(ii)->bGetVirgin()  ==  true)

				pv_population->at(ii)->b_virgin_init = false;


				//we do the switch AFTER virginity removal
				if  ( (i_solution_construction_method  == SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FIRST_FULL_LATER)&&(b_removing_virginity == true) )
				{
					s_buf.Format("CT: %d - SWITCHING TO FLUENT OPTIMIZATION", ii);
					listInfo->Items->Add((String *) s_buf);
					listInfo->Refresh();

					double  d_ct_fit_before, d_ct_fit_after;
					d_ct_fit_before = pv_population->at(ii)->dGetCompTemplateFitness();

					c_err = pv_population->at(ii)->eMakePreciseGentoypeFromFluent();
					pv_population->at(ii)->i_solution_construction_method = SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION;

					pv_population->at(ii)->d_templ_fitness = -1;
					d_ct_fit_after = pv_population->at(ii)->dGetCompTemplateFitness();

					s_buf.Format("POPULATION %d: Before Fit=%.8lf  CT=%.8lf  AFTER Fit=%.8lf  CT=%.8lf", ii, d_ct_fit_before, 1.0/d_ct_fit_before, d_ct_fit_after, 1.0/d_ct_fit_after);
					listInfo->Items->Add((String *) s_buf);
					listInfo->Refresh();


					s_buf.Format("END OF SWITCHING");
					listInfo->Items->Add((String *) s_buf);
					listInfo->Refresh();
				}//if  ( (i_solution_construction_method  == SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FIRST_FULL_LATER)&&(b_removing_virginity == true) )

				/*if  (i_virginity_init == 0)
				{
					i_virginity_current_round = 0;
					pv_population->at(ii)->b_virgin_init = true;

					double  d_ct_fit_before, d_ct_fit_after;
					pv_population->at(ii)->d_templ_fitness = -1;

					//prw remove_02.13
					//i_global_debug = 1;
					d_ct_fit_before = pv_population->at(ii)->dGetCompTemplateFitness();

					//c_err = pv_population->at(ii)->eMakeGentoypeFromSolution(pv_population->at(ii)->pc_fitness);

					c_err = pv_population->at(ii)->eMakePreciseGentoypeFromFluent();					
					pv_population->at(ii)->i_solution_construction_method = 1;

					pv_population->at(ii)->d_templ_fitness = -1;
					d_ct_fit_after = pv_population->at(ii)->dGetCompTemplateFitness();

					s_buf.Format("SWITCHING TO FLUENT OPTIMIZATION: Before Fit=%.8lf  CT=%.8lf  AFTER Fit=%.8lf  CT=%.8lf", d_ct_fit_before, 1.0/d_ct_fit_before, d_ct_fit_after, 1.0/d_ct_fit_after);
					listInfo->Items->Add((String *) s_buf);
					listInfo->Refresh();				
				}//if  (i_virginity_current_round == 0)*/
			}//for  (int  i_virginity_init = 0; i_virginity_init < 2; i_virginity_init++)
		
		}//for (int  ii = 0; ii < (int) pv_population->size();  ii++)


		d_best_fitness  =  0;
		d_average  =  0;
		//int  i_vir_pop_temp_size;
		//i_vir_pop_temp_size = i_virus_pop_size / pv_population->size();
		for (int  ii = 0; ii < (int) pv_population->size();  ii++)
		{
			pv_population->at(ii)->eVirGeneratePopulation(this, &i_vir_pop_temp_size, b_clever_paths);

			if  (i_ct_add_remove_strategy  ==  2)
				c_err  =  pv_population->at(ii)->eMuppetsVirRun(pv_pattern_pool, this,  &i_vir_init_done, 1, 1, listInfo);
			else
				c_err  =  pv_population->at(ii)->eMuppetsVirRun(pv_pattern_pool, this,  &i_vir_init_done, 0, 0, listInfo);


			if  (i_vir_init_done  ==  1)  i_break_at_the_same_base++;
			if  (i_vir_init_done  ==  2)  i_break_at_immediate_infect++;

			if  (c_err)
			{
				//fclose(pf_report);
				return(c_err);
			}//if  (c_err)
		
		

			
			/*if  (pv_population->at(ii)->pcGetLastInfection()  ==  NULL)
				fprintf(pf_report,  "Individual: %d (STUCKED!)\n", ii);
			else
				fprintf(pf_report,  "Individual: %d \n", ii);
			pv_population->at(ii)->eReportShort(pf_report);
			fprintf(pf_report,  "\n\n");*/

			d_buf  =  pv_population->at(ii)->dGetCompTemplateFitness();
			if  (d_buf  >  d_best_fitness)  
			{
				c_time_counter.bGetTimePassed(&d_time_passed_till_max);
				d_fitness_eval_till_max  =  pc_fitness->dGetFuncEval();
				d_best_fitness  =  d_buf;
			}//if  (d_buf  >  d_best_fitness)  
			d_average  +=  d_buf;

		}//for (int  ii = 0; ii < i_pop_size;  ii++)

		c_time_counter.bGetTimePassed(&d_time_passed);
		fprintf(pf_report,  "\n\nFITNESS EVALUATIONS: %.0lf  \t  time passed:%.4lf \n\n\n\n", pc_fitness->dGetFuncEval(), d_time_passed);

		if  (listInfo  !=  NULL)
		{
			s_buf.Format
				(
				"Generation: %d ct number: %d (best fitness: %.8lf) average: %.8lf best - average:%.8lf  [evaluations:%.0lf] (solBTime:%.4lf) [clePathFitEval:%.0lf] [clePathFitInit:%.0lf] [full>simple:%.0lf] [time passed:%.4lf]",  
				i_cur_gen,  pv_population->size(), 1 / d_best_fitness, d_average/pv_population->size(),
				d_best_fitness  - d_average/pv_population->size(),
				pc_fitness->dGetFuncEval(), pc_fitness->dGetConstructionTime(), pc_fitness->dGetCleverPathsEval(), pc_fitness->dGetCleverPathsInits(),
				pc_fitness->dGetFullBetterThanSimple(),
				d_time_passed
				);
			listInfo->Items->Add((String *) s_buf);
			listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
			listInfo->Refresh();

			System::Windows::Forms::Application::DoEvents();
		}//if  (listInfo  !=  NULL)

		bGlobalDebug = true;

		//now we save infection histories as templates
		v_get_new_patterns();
		//v_get_new_patterns_multiple();
		

		//template number control - we delete the overnumbered templates
		v_pattern_number_control_new();  
		
		
		fprintf(pf_report,  "\n\n\n");
		System::Windows::Forms::Application::DoEvents();

		
		if  (pdOptimalValue  !=  NULL)
		{
			if  (d_best_fitness  >  0)
			{
				if  ((1.0 / d_best_fitness) + 1  <=  *pdOptimalValue)  b_optimal_soluion_reached = true;
			}//if  (d_best_fitness  >  0)
		}//if  (pdOptimalValue  !=  NULL)	

		if  (WATCH_FITNESS_LEVELS)  d_time_passed = dMaxTime + 1;
	}//for (int  ii = 0; ii < i_gen;  ii++)


	std::sort(pv_pattern_pool->begin(),  pv_pattern_pool->end(), bPatternGreater);


	fprintf(pf_report,  "\n\n\nBREAK AT THE SAME BASE: %d\n", i_break_at_the_same_base);
	fprintf(pf_report,  "\n\n\nBREAK AT IMMEDIATE INFECT: %d\n", i_break_at_immediate_infect);
	

	fprintf(pf_report,  "\n\n\nTEMPLATES: %d (%d)\n", pv_pattern_pool->size(), i_pattern_max_number);
	for  (int  ii = 0;  ii < (int) pv_pattern_pool->size();  ii++)
	{
		fprintf(pf_report,  "\n\n\nTEMPLATES: %d\n", ii);
		pv_pattern_pool->at(ii)->eReport(pf_report,  true);	
	}//for  (int  ii = 0;  ii < (int) v_best_found.size();  ii++)


	fprintf(pf_report,  "\n\n\n");
	fprintf(pf_report,  "\n\nFITNESS EVALUATIONS: %.0lf\n\n\n\n", pc_fitness->dGetFuncEval());
	//fprintf(pf_report,  "\n\nCLEVER PATHS EVALUATIONS: %.0lf\n\n\n\n", pc_fitness->dGetCleverPathsEval());
	//fprintf(pf_report,  "\n\nCLEVER PATHS INITS: %.0lf\n\n\n\n", pc_fitness->dGetCleverPathsInits());
	//fprintf(pf_report,  "\n\nMODEL CONSTRUCTION TIME: %.8lf\n\n\n\n", pc_fitness->dGetConstructionTime());
	
	
	fclose(pf_report);

	for  (int  ii = 0; ii < (int) pv_pattern_pool->size();  ii++)
		delete  pv_pattern_pool->at(ii);
	pv_pattern_pool->clear();


	/*FILE  *pf_summarized_rep;
	pf_summarized_rep  =  fopen(sSummarizedReport, "a");
	fprintf(pf_summarized_rep, "%.8lf \t %.2lf\t%.2lf\n", d_best_fitness,  d_fitness_eval_till_max, d_time_passed_till_max);
	fclose(pf_summarized_rep);*/


	v_remove_all_ct_except_best();
	FILE  *pf_summarized_rep;
	
	double  d_templ_fitness;
	bool  b_const;
	double  d_ref_solution;

	if  (sReferenceSolution  !=  "")
	{
		//vShow("("+sReferenceSolution+")");
		pc_fitness->eLoadSolution(sReferenceSolution);
		pc_fitness->eApplySolution();
		d_ref_solution = pc_fitness->dCompCost(&b_const);
	}//if  (sReferenceSolution  !=  "")
	else
		d_ref_solution = 1;

	CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, pv_population->at(0)->i_solution_construction_method);
	d_templ_fitness  =  c_buf.dComputeFitness(pv_population->at(0)->pc_competetive_template);	
	//d_templ_fitness  =  pc_fitness->dCompCost(&b_const);
	FILE  *pf_solution;
	pf_solution  =  fopen(sReportFile + "_solf.txt",  "w+");
	pc_fitness->eSaveSolution(pf_solution);
	fclose(pf_solution);

	/*FILE  *pf_test;
	pf_test = fopen("D:\\test.txt", "w+");
	pc_fitness->eSaveSolution(pf_test);
	c_buf.eReportFenotype(pf_test, pv_population->at(0)->pc_competetive_template);
	fclose(pf_test);//*/
	

	double d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);
	if  (d_templ_fitness_no_pen  <  d_templ_fitness)
		b_const = true;
	else
		b_const = false;

	LargeNum::CNumber  c_num, c_buf_num;
	c_num.vSetValue(1);

	//potegi najpierw...
	for  (int  i_dem = 0; i_dem < pc_fitness->pcGetNet()->iGetDemandNumber(); i_dem++)
	{
		c_buf_num.vSetValue(pc_fitness->pcGetNet()->pcGetDemands()[i_dem].pvGetPossiblePaths()->size());
		c_num = c_num.cMul(&c_buf_num);
	}//for  (int  i_dem = 0; i_dem < pc_fitness->pcGetNet()->iGetDemandNumber(); i_dem++)

	//potem silnia
	for  (int  i_dem = pc_fitness->pcGetNet()->iGetDemandNumber(); i_dem > 0; i_dem--)
	{
		c_buf_num.vSetValue(i_dem);
		c_num = c_num.cMul(&c_buf_num);	
	}//for  (int  i_dem = pc_fitness->pcGetNet()->iGetDemandNumber(); i_dem > 0; i_dem--)


	CString  s_ffe_levels;
	if  (WATCH_FITNESS_LEVELS)
		s_ffe_levels = pv_population->at(0)->pc_fitness->sFitnessDiversity();

	pf_summarized_rep  =  fopen(sSummarizedReport, "a");
	fprintf
		(pf_summarized_rep, 

		"%d \t %d \t %d \t %s \t %s \t %.8lf \t %.8lf \t %.8lf \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.0lf \t %d \t %s \t %s \t timeTillBest: \t %.2lf FfeTillBest: \t %.0lf \t fullBetThanSimple: \t %.0lf \t seed: \t %d\n", 

		pc_fitness->pcGetNet()->iGetDemandNumberUni(), pc_fitness->pcGetNet()->iGetDemandNumberAny(), pc_fitness->pcGetNet()->iGetDemandNumberMulti(), 
		c_num.sToStringFirstDgitsDotted(2), c_num.sToString10Pow(),
		//pc_fitness->pcGetNet()->pcGetDemands()[0].pvGetPossiblePaths()->size();
		pc_fitness->dGetBest(), d_templ_fitness_no_pen, d_ref_solution,  d_templ_fitness / d_ref_solution, d_templ_fitness_no_pen / d_ref_solution, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, 
		sReportFile,
		s_ffe_levels, 
		pc_fitness->dGetTimeTillBest(), pc_fitness->dGetFfeTillBest(),
		pc_fitness->dGetFullBetterThanSimple(),
		i_seed
		);
	fclose(pf_summarized_rep);


	
	

	/*//nowe summarize


	FILE  *pf_report;
	pf_report  =  fopen(sReportFile,  "w+");
	eReport(pf_report);
	fclose(pf_report);

	pf_report  =  fopen(sReportFile + "_trans.txt",  "w+");
	eReportTransparent(pf_report);
	fclose(pf_report);


	double  d_ref_solution;
	bool  b_const;
	if  (sReferenceSolution  !=  "")
	{
		pc_fitness->eLoadSolution(sReferenceSolution);
		d_ref_solution = pc_fitness->dCompCost(&b_const);
	}//if  (sReferenceSolution  !=  "")
	else
		d_ref_solution = 1;


	FILE  *pf_summarized_rep;
	
	CMessyIndividual  c_buf(pc_fitness);
	d_templ_fitness  =  c_buf.dComputeFitness(pc_competetive_template);	
	d_templ_fitness  =  pc_fitness->dCompCost(&b_const);
	double d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);
	int  i_c_links, i_c_nodes, i_c_isp, i_flow_const;
	i_c_links = pc_fitness->iGetCommonLinks();
	i_c_nodes = pc_fitness->iGetCommonNodes();
	i_c_isp = pc_fitness->iGetCommonIspLinks();
	i_flow_const = pc_fitness->iGetFlowConstraint();

	pf_summarized_rep  =  fopen(sSummarizedReport, "a");
	fprintf
		(pf_summarized_rep, 
		"%.8lf \t %.8lf \t %.8lf \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.0lf \t %d \t %d \t %d \t %d \t %d \t %s \n", 

		d_templ_fitness, d_templ_fitness_no_pen, d_ref_solution,  d_templ_fitness / d_ref_solution, d_templ_fitness_no_pen / d_ref_solution, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, i_c_links, i_c_nodes, i_c_isp, i_flow_const, sReportFile
		);
	fclose(pf_summarized_rep);*/

	return(c_err);
}//CError  CVirusGA::eRun()





//---------------------------class  CmGA--------------------------------------

CmGA::CmGA(gcroot<Random*> pcRandomGen, int  iBayesianEffect, int iSolutionConstructionMethod)
{
	pc_random_gen  =  pcRandomGen;
	
	pc_fitness  =  NULL;

	pc_competetive_template  =  NULL;
	pc_ordered_template  =  NULL;
	i_templ_length  =  0;

	d_templ_fitness  =  0;
	d_templ_fittness_before_crossing  =  0;
	pc_best_found  =  NULL;
	pc_infection_history = NULL;

	pv_population  =  NULL;


	d_templ_fittness_before_insert  =  0;
	pc_competetive_template_before_insert  =  NULL;

	d_templ_fittness_full_before_switch_to_simple = 0;
	pc_competetive_template_before_switch_from_full_to_simple = NULL;

	pc_ct_channels  =  NULL;
	pc_best_virus_channels  =  NULL;
	pc_pattern_proposition = NULL;

	i_bayesian_effect = iBayesianEffect;
	i_solution_construction_method = iSolutionConstructionMethod;

	d_prob_cut  =  0;
	d_prob_splice  =  0;
	d_prob_mut  =  0;

	i_generations  =  0;

	i_prim_gen_without_pop_red  =  0;//population is not reduced
	i_prim_gen_reduce_pop_every_2nd  =  0;  //population reduced every other generation
	i_prim_gen_sized_but_not_reduced  =  0;  //population is sized at iPopSize, but primordial phase is still going on


	i_pop_size  =  0;
	d_pop_reduction_primordial  =  0;
	d_pop_reduction_juxtapositional  =  1.0;

	d_fmga_primordial_red_rate  =  1.0;
	i_fmga_primordial_red_period  = 0;
	i_fmga_primordial_number_of_indiv_to_check  =  1;
	d_threshold_multiplier  =  1;

	b_virgin_init  =  true;
};//CmGA::CmGA()



CmGA::~CmGA()
{
	CError  c_err;
	CString  s_buf;

	
	if  (pc_fitness  !=  NULL)  delete  pc_fitness;
	if  (pc_competetive_template  !=  NULL)  delete [] pc_competetive_template;
	if  (pc_ordered_template  !=  NULL)  delete [] pc_ordered_template;
	if  (pc_competetive_template_before_insert  !=  NULL)  delete [] pc_competetive_template_before_insert;
	if  (pc_competetive_template_before_switch_from_full_to_simple  !=  NULL)  delete [] pc_competetive_template_before_switch_from_full_to_simple;


	if  (pc_ct_channels  !=  NULL)  delete  [] pc_ct_channels;
	if  (pc_best_virus_channels  !=  NULL)  delete  []  pc_best_virus_channels;

	try
	{
		if  (pc_pattern_proposition  !=  NULL)  delete  pc_pattern_proposition;
	}//try
	catch (Exception  *p_ex)
	{
		s_buf = p_ex->ToString();
		s_buf = s_buf + "\n deleting pc_pattern_proposition";
		c_err.vPutError(s_buf);
	}//catch (Exception  p_ex)

	
	
	try
	{
		if  (pv_population  !=  NULL)
		{
			for  (int  ii = 0; ii < (int) pv_population->size(); ii++)
				delete  pv_population->at(ii);
			delete  pv_population;
		}//if  (pv_population  !=  NULL)
	}//try
	catch (Exception  *p_ex)
	{
		s_buf = p_ex->ToString();
		s_buf = s_buf + "\n deleting population";
		c_err.vPutError(s_buf);
	}//catch (Exception  p_ex)


	try
	{
		if  (pc_best_found  !=  NULL)  delete  pc_best_found;
	}//try
	catch (Exception  *p_ex)
	{
		s_buf = p_ex->ToString();
		s_buf = s_buf + "\n deleting pc_best_found";
		c_err.vPutError(s_buf);
	}//catch (Exception  p_ex)


	try
	{
		if  (pc_infection_history  !=  NULL)  delete  pc_infection_history;
	}//try
	catch (Exception  *p_ex)
	{
		s_buf = p_ex->ToString();
		s_buf = s_buf + "\n deleting pc_infection_history";
		c_err.vPutError(s_buf);
	}//catch (Exception  p_ex)

	//::MessageBox(NULL, "cnga", "cnga", MB_OK);
};//CmGA::CmGA()



int  CmGA::iCountGenes()
{
	int  i_res;

	i_res  =  0;
	for (int  ii = 0;  ii < (int) pv_population->size();  ii++)
	{
		i_res  +=  (int)  pv_population->at(ii)->pvGetGenotype()->size();		
	}//for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)

	return(i_res);
}//int  CmGA::iCountGenes()





CError  CmGA::eLoadSettings(FILE  *pfSettings)
{
	CError  c_err;
	CString  s_setting_name,  s_buf;


	i_generations  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_GENERATIONS)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_GENERATIONS, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_pop_size  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_POP_SIZE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_POP_SIZE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_prob_cut  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_PROB_CUT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_PROB_CUT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_prob_splice  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_PROB_SPLICE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_PROB_SPLICE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_prob_mut  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_PROB_MUT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_PROB_MUT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_prob_mut_rem_gene  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_REM_GENE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_REM_GENE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_prob_mut_add_gene  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_ADD_GENE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_ADD_GENE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	

	return(c_err);
}//CError  CmGA::eLoadSettings(FILE  *pfSettings)




CError  CmGA::eReportTransparent(FILE  *pfReport)
{
	CError  c_err;

	fprintf(pfReport,  "Competetive template:\n");
	fprintf(pfReport,  "Fitness: %.10lf\n",  1/d_templ_fitness);


	int  i_feno_len = pc_fitness->pcGetNet()->iGetDemandNumber();
	for  (int ii = 0; ii < i_feno_len; ii++)
	{
		fprintf(pfReport, "%d", ii);
		pc_competetive_template[ii].vReportGene(pfReport);
		fprintf(pfReport, "\n\n", ii);
	}//for  (int ii = 0; ii < i_feno_len; ii++)


	CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	d_templ_fitness  =  c_buf.dComputeFitness(pc_competetive_template);	

	fprintf(pfReport,  "\n\n\n");
	pc_fitness->eSaveSolution(pfReport);


	return(c_err);
}//CError  CmGA::eReport(FILE  *pfReport)




CError  CmGA::eReport(FILE  *pfReport)
{
	CError  c_err;


	CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	d_templ_fitness  =  c_buf.dComputeFitness(pc_competetive_template);	

	fprintf(pfReport,  "\n\n\n");
	pc_fitness->eSaveSolution(pfReport);


	return(c_err);
}//CError  CmGA::eReport(FILE  *pfReport)




CError  CmGA::eReportShort(FILE  *pfReport)
{
	CError  c_err;


	CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	d_templ_fitness  =  c_buf.dComputeFitness(pc_competetive_template);	

	fprintf(pfReport,  "\n\n\n");
	pc_fitness->eSaveSolution(pfReport);

	for  (int  ii = 0; ii < i_templ_length; ii++)
	{
		fprintf(pfReport, "%.8lf\t", pc_competetive_template[ii].d_order);		
	}//for  (int  ii = 0; ii < i_feno_len; ii++)

	return(c_err);
}//CError  CmGA::eReport(FILE  *pfReport)



CError  CmGA::eReportIndividuals(FILE  *pfReport)
{
	CError  c_err;

	CString  s_buf;

	c_err  =  eReport(pfReport);
	if  (c_err)  return(c_err);

	fprintf(pfReport,  "\n\n\n");
	fprintf(pfReport,  "Population(%d):\n",  pv_population->size());
	for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)
	{
		s_buf.Format("Individual %d",  ii);
		fprintf(pfReport,  "\n%s\n",  s_buf);
		
		c_err  =  pv_population->at(ii)->eReport(pfReport,  pc_competetive_template);
		if  (c_err)  return(c_err);
	}//for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)


	return(c_err);
}//CError  CmGA::eReport(FILE  *pfReport)





bool  CmGA::bIsTheSame(CMessyPattern  *pcPattern, CmGA  *pcOther)
{
	if  ( (pc_competetive_template  ==  NULL) )  return(false);
	if  ( (pcOther->pc_competetive_template  ==  NULL) )  return(false);

	for  (int  ii = 0; ii < (int) pcPattern->pvGetGenotype()->size();  ii++)
	{
		//PRWpotem
		//porownywanie musi byc na podstawie solution, czyli wlasciwego fenotypu, a nie sekwencji genow
	}//for  (int  ii = 0; ii < i_templ_length;  ii++)

	return(true);
}//bool  CmGA::bIsTheSame(CMessyPattern  *pcPattern, CmGA  *pcOther)



bool  CmGA::bIsTheSame(CmGA  *pcOther)
{
	/*if  ( (pc_competetive_template  ==  NULL) )  return(false);
	if  ( (pcOther->pc_competetive_template  ==  NULL) )  return(false);

	for  (int  ii = 0; ii < i_templ_length;  ii++)
	{
		if  (!(pc_competetive_template[ii]  ==  pcOther->pc_competetive_template[ii]))  return(false);	
	}//for  (int  ii = 0; ii < i_templ_length;  ii++)

	if  (dGetCompTemplateFitness()  ==  pcOther->dGetCompTemplateFitness())
		return(true);
	else
		return(false);//*/

	if  ( (pc_ordered_template  ==  NULL) )  return(false);
	if  ( (pcOther->pc_ordered_template  ==  NULL) )  return(false);

	for  (int  ii = 0; ii < i_templ_length;  ii++)
	{
		if  (pc_ordered_template[ii].pcGetPath()->iGetId()  !=  pcOther->pc_ordered_template[ii].pcGetPath()->iGetId())  return(false);	
	}//for  (int  ii = 0; ii < i_templ_length;  ii++)*/

	return(true);
}//bool  CmGA::bIsTheSame(CmGA  *pcOther)



int  CmGA::iGetNumberOfDiffrGenes(CMessyPattern  *pcPattern,  CmGA  *pcOther)
{
	if  ( (pc_competetive_template  ==  NULL) )  return(0);
	if  ( (pcOther->pc_competetive_template  ==  NULL) )  return(0);

	int  i_result  = 0;

	for  (int  ii = 0; ii < (int) pcPattern->pvGetGenotype()->size();  ii++)
	{
		//PRWpotem
		//to samo - trzeba zrobic porownanie na podstawie solution, a nie genotpyu
		if  (
			pc_competetive_template[pcPattern->pvGetGenotype()->at(ii)->i_demand_number]  
			!=  
			pcOther->pc_competetive_template[pcPattern->pvGetGenotype()->at(ii)->i_demand_number])  
			i_result++;
	}//for  (int  ii = 0; ii < i_templ_length;  ii++)

	return(i_result);
}//int  CmGA::iGetNumberOfDiffrGenes(CmGA  *pcOther)



int  CmGA::iGetNumberOfDiffrGenes(CmGA  *pcOther)
{
	if  ( (pc_competetive_template  ==  NULL)&&(i_templ_length  !=  0) )  return(-1);

	int  i_result  =  0;

	for  (int  ii = 0; ii < i_templ_length;  ii++)
	{
		//PRWpotem
		//porownanie na podstawie solution, a nie genotpyu
		if  (pc_competetive_template[ii]  !=  pcOther->pc_competetive_template[ii])  i_result++;	
	}//for  (int  ii = 0; ii < i_templ_length;  ii++)

	return(i_result);
}//int  CmGA::iGetNumberOfDiffrGenes(CmGA  *pcOther)









void  CmGA::vCrossStatic
	(
	CmGA  *pcOther,
	CmGA  *pcChild,
	CMessyIndividual  *pcTemplate
	)
{
	CMessyGene  *pc_templ_buffer;
	int *pi_infection_tool;
	pc_templ_buffer  =  new  CMessyGene[i_templ_length];

	pi_infection_tool  =  new  int[i_templ_length];


	if  ( pcTemplate  ==  NULL )
	{
		::MessageBox(NULL, "no template for crossing",  "EXCEPTION", MB_OK);
		return;
	}//if  ( (d_fit_1  ==  0)||(d_fit_2) )

	//first copying templates...
	for  (int  ii = 0; ii < i_templ_length; ii++)
	{
		pc_templ_buffer[ii]  =  pc_competetive_template[ii];
		pi_infection_tool[ii]  =  0;
	}//for  (int  ii = 0; ii < i_templ_length; ii++)
	

	//now crossing the individuals into templates
	for  (int  ii = 0; ii < (int) pcTemplate->pvGetGenotype()->size(); ii++)
	{

		if  (
			pc_competetive_template[pcTemplate->pvGetGenotype()->at(ii)->i_demand_number]
			!=
			pcOther->pc_competetive_template[pcTemplate->pvGetGenotype()->at(ii)->i_demand_number]
			)
			pi_infection_tool[pcTemplate->pvGetGenotype()->at(ii)->i_demand_number]  =  1;


		pc_templ_buffer[pcTemplate->pvGetGenotype()->at(ii)->i_demand_number]
			=
		pcOther->pc_competetive_template[pcTemplate->pvGetGenotype()->at(ii)->i_demand_number];	
	}//for  (int  ii = 0; ii < (int) pc_template->pvGetGenotype()->size(); ii++)

	//copying templates into the new individuals
	pcChild->eSetTemplate(pc_templ_buffer,  i_templ_length);



	//now setting the infection history into a new start
	vector  <CMessyGene  *>  v_infection_genotype;

	for  (int  ii = 0; ii < i_templ_length; ii++)
	{
		if  (pi_infection_tool[ii]  ==  1)
			v_infection_genotype.push_back
				(
				new CMessyGene(pc_templ_buffer[ii].i_demand_number, pc_templ_buffer[ii].d_order, pc_templ_buffer[ii].pc_path)
				);
	}//for  (int  ii = 0; ii < i_templ_length; ii++)


	if  (pc_infection_history  ==  NULL)  pc_infection_history  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	pc_infection_history->vSetGenotype(&v_infection_genotype);
	

	pcChild->vSetInfectionHistory(pc_infection_history);


	for  (int  ii = 0; ii < (int)  v_infection_genotype.size(); ii++)
		delete  v_infection_genotype.at(ii);



	delete  []  pc_templ_buffer;
	delete  []  pi_infection_tool;

	
}//void  CmGA::vCross(CmGA  *pcOther,  int iMaxToConsider)




void  CmGA::vCopyTo(CmGA*  pcDest)
{
	pcDest->d_templ_fitness = d_templ_fitness;

	for  (int  ii = 0;  ii < i_templ_length;  ii++)
	{
		pcDest->pc_competetive_template[ii] = pc_competetive_template[ii];
	}//for  (int  ii = i_cross_point;  ii < i_templ_length;  ii++)	
}//void  CmGA::vCopyTo(CmGA*  pcDest)




CmGA*  CmGA::pcSgaClone()
{
	CmGA  *pc_clone;


	pc_clone = new CmGA(pc_random_gen, i_bayesian_effect, i_solution_construction_method);

	pc_clone->eSetFitness(pc_fitness);
	pc_clone->eConfigureTemplate();
	pc_clone->eVirGenerateRandomTemplate();
	pc_clone->d_templ_fitness = d_templ_fitness;
	
	for  (int  ii = 0;  ii < i_templ_length;  ii++)
	{
		pc_clone->pc_competetive_template[ii] = pc_competetive_template[ii];
	}//for  (int  ii = i_cross_point;  ii < i_templ_length;  ii++)	


	return(pc_clone);
}//CmGA*  CmGA::pcSgaClone()




void  CmGA::vSgaMutate()
{
	d_templ_fitness = -1;

	int  i_mutated_gene;
	i_mutated_gene  =  pc_random_gen->Next(0, i_templ_length);


	double  d_rand;

	d_rand = pc_random_gen->NextDouble();
	if  (d_rand < 0.5)
	{
		pc_competetive_template[i_mutated_gene].pc_path = pc_fitness->pcGetCleverPath(pc_competetive_template[i_mutated_gene].i_demand_number, pc_random_gen);
	}//if  (pcRandomGen->NextDouble() < 0.33)
	else
	{
		pc_competetive_template[i_mutated_gene].d_order = pc_random_gen->NextDouble();
	}//else  if  (d_rand < 0.5)
	
	d_templ_fitness = -1;


	
}//void  CmGA::vSgaMutate()




void  CmGA::vSgaCrossSinglePoint
	(
	CmGA  *pcFather,
	CmGA  **pcChild0, CmGA  **pcChild1,
	double  dCrossProbability
	)
{
	CString  s_buf;

	if  (pc_random_gen->NextDouble()  <  dCrossProbability)
	{
		int  i_cross_point;
		i_cross_point = pc_random_gen->Next(i_templ_length);


		for  (int  ii = 0;  ii < i_cross_point;  ii++)
		{
			(*pcChild0)->pc_competetive_template[ii] = pc_competetive_template[ii];
			(*pcChild1)->pc_competetive_template[ii] = pcFather->pc_competetive_template[ii];
		}//for  (int  ii = 0;  ii < i_cross_point;  ii++)


		for  (int  ii = i_cross_point;  ii < i_templ_length;  ii++)
		{
			(*pcChild0)->pc_competetive_template[ii] = pcFather->pc_competetive_template[ii];
			(*pcChild1)->pc_competetive_template[ii] = pc_competetive_template[ii];
		}//for  (int  ii = i_cross_point;  ii < i_templ_length;  ii++)
	
		(*pcChild0)->d_templ_fitness = -1;
		(*pcChild1)->d_templ_fitness = -1;

		//s_buf.Format("Par0: %.4lf  Par1: %.4lf   Child0: %.4lf  Child1: %.4lf",  1.0 / dGetCompTemplateFitness(), 1.0 / pcFather->dGetCompTemplateFitness(), 1.0 / (*pcChild0)->dGetCompTemplateFitness(), 1.0 / (*pcChild1)->dGetCompTemplateFitness());
		//vShow(s_buf);
	}//if  (pc_random_gen->NextDouble()  <  dCrossProbability)
	else
	{
		for  (int  ii = 0;  ii < i_templ_length;  ii++)
		{
			(*pcChild0)->pc_competetive_template[ii] = pc_competetive_template[ii];
			(*pcChild1)->pc_competetive_template[ii] = pcFather->pc_competetive_template[ii];		
		}//for  (int  ii = i_cross_point;  ii < i_templ_length;  ii++)	

		(*pcChild0)->d_templ_fitness = d_templ_fitness;
		(*pcChild1)->d_templ_fitness = pcFather->d_templ_fitness;
	}//else  if  (pc_random_gen->NextDouble()  <  dCrossProbability)


}//void  CmGA::vCrossSinglePoint




void  CmGA::vCross
	(
	CmGA  *pcOther,
	CmGA  *pcChild1, CmGA  *pcChild2,
	CMessyIndividual  *pcTemplate
	)
{

	CMessyGene  *pc_templ_buffer_1, *pc_templ_buffer_2;
	int   *pi_infection_tool;
	pc_templ_buffer_1  =  new  CMessyGene[i_templ_length];
	pc_templ_buffer_2  =  new  CMessyGene[i_templ_length];
	pi_infection_tool  =  new  int[i_templ_length];


	if  ( pcTemplate  ==  NULL )
	{
		::MessageBox(NULL, "no template for crossing",  "EXCEPTION", MB_OK);
		return;
	}//if  ( (d_fit_1  ==  0)||(d_fit_2) )

	//first copying templates...
	for  (int  ii = 0; ii < i_templ_length; ii++)
	{
		pc_templ_buffer_1[ii]  =  pc_competetive_template[ii];
		pc_templ_buffer_2[ii]  =  pcOther->pc_competetive_template[ii];

		pi_infection_tool[ii]  =  0;
	}//for  (int  ii = 0; ii < i_templ_length; ii++)
	

	//now crossing the individuals into templates
	for  (int  ii = 0; ii < (int) pcTemplate->pvGetGenotype()->size(); ii++)
	{
		if  (
			pc_competetive_template[pcTemplate->pvGetGenotype()->at(ii)->i_demand_number]
			!=
			pcOther->pc_competetive_template[pcTemplate->pvGetGenotype()->at(ii)->i_demand_number]
			)
			pi_infection_tool[pcTemplate->pvGetGenotype()->at(ii)->i_demand_number]  =  1;


		pc_templ_buffer_1[pcTemplate->pvGetGenotype()->at(ii)->i_demand_number]
			=
			pcOther->pc_competetive_template[pcTemplate->pvGetGenotype()->at(ii)->i_demand_number];


		pc_templ_buffer_2[pcTemplate->pvGetGenotype()->at(ii)->i_demand_number]
			=
			pc_competetive_template[pcTemplate->pvGetGenotype()->at(ii)->i_demand_number];
	}//for  (int  ii = 0; ii < (int) pc_template->pvGetGenotype()->size(); ii++)

	//copying templates into the new individuals
	pcChild1->eSetTemplate(pc_templ_buffer_1,  i_templ_length);
	pcChild2->eSetTemplate(pc_templ_buffer_2,  i_templ_length);


	//now setting the infection history into a new start
	vector  <CMessyGene  *>  v_infection_genotype;

	for  (int  ii = 0; ii < i_templ_length; ii++)
	{
		if  (pi_infection_tool[ii]  ==  1)
			v_infection_genotype.push_back
				(
				new CMessyGene(pc_templ_buffer_1[ii].i_demand_number, pc_templ_buffer_1[ii].d_order, pc_templ_buffer_1[ii].pc_path)
				);
	}//for  (int  ii = 0; ii < i_templ_length; ii++)


	if  (pc_infection_history  ==  NULL)  pc_infection_history  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	pc_infection_history->vSetGenotype(&v_infection_genotype);
	

	pcChild1->vSetInfectionHistory(pc_infection_history);
	pcChild2->vSetInfectionHistory(pc_infection_history);


	for  (int  ii = 0; ii < (int)  v_infection_genotype.size(); ii++)
		delete  v_infection_genotype.at(ii);



	delete  []  pc_templ_buffer_1;
	delete  []  pc_templ_buffer_2;
	delete  []  pi_infection_tool;


	
}//void  CmGA::vCross(CmGA  *pcOther,  int iMaxToConsider)



bool  CmGA::b_repair_necessary_()
{
	CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	int  i_solution_repaired = 0;
	c_buf.dComputeFitness(pc_competetive_template, &i_solution_repaired);	
	
	if  (i_solution_repaired  !=  0)  return(true);
	return(false);
}//bool  CmGA::b_repair_necessary()




int  CmGA::iGetMaximumSlot()
{
	double  d_sol_fitness = dGetCompTemplateFitness();
	int slicesLimit = (int) Math::Round(1.0 / d_sol_fitness);

	return(slicesLimit);
}//int  CmGA::iGetMaximumSlot()



double  CmGA::dGetCompTemplateFitness()
{
	if  (d_templ_fitness  <=  0)//not computed...
	{
		//::MessageBox(NULL, "0", "0", MB_OK);

		CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
		int  i_solution_repaired = 0;
		d_templ_fitness  =  c_buf.dComputeFitness(pc_competetive_template, &i_solution_repaired);	

		if  (i_solution_repaired  !=  0)
		{
			//::MessageBox(NULL, "1", "1", MB_OK);
			//CString  s_buf;
			//s_buf.Format("fit: %.2lf  repaired: %d", 1.0 / d_templ_fitness, i_solution_repaired);
			//vShow(s_buf);
			c_buf.vInfect(pc_competetive_template);
			d_templ_fitness = -1;
			dGetCompTemplateFitness();

			//s_buf.Format("fit repaired: %.2lf", 1.0 / d_templ_fitness);
			//vShow(s_buf);
		}//if  (b_solution_repaired  ==  true)

		c_buf.vCopyOrderedFenotype(pc_ordered_template);
	}//if  (d_templ_fitness  <=  0)//not computed...


	return(d_templ_fitness);
}//double  CmGA::dGetCompTemplateFitness()


CError  CmGA::eMuppetsVirRun(vector  <CMessyPattern  *>  *pvInfectionHistory,  CVirusGA  *pcVGA,  int  *piInitDone, int iRunVirusesOnlyAfterCTImprove, int iPatternExtractionBasedOnLocalOpt, System::Windows::Forms::ListBox*  listInfo)
{
	CError  c_err;
	CString  s_buf;

	if  (b_virgin_init  ==  false)
	{
		/*double  d_t_buf = d_templ_fitness;
		d_templ_fitness = -1;
		dGetCompTemplateFitness();
		double  d_t_recomp = d_templ_fitness;*/

		

		v_try_pattern_and_template_insert(pcVGA,  piInitDone, iRunVirusesOnlyAfterCTImprove);

		//double  d_t_ater_pattern_ins = dGetCompTemplateFitness();
		//s_buf.Format("CT start: %.2lf recomputed: %.2lf pattern ins: %.2lf", 1.0 / d_t_buf, 1.0 / d_t_recomp, 1.0 / d_t_ater_pattern_ins);
		//vShow(s_buf);

		if  (iRunVirusesOnlyAfterCTImprove  ==  0)
		{
			if  (*piInitDone  !=  0)  return(c_err);
		}//if  (iRunVirusesOnlyAfterCTImprove  ==  0)
	}//if  (b_virgin_init  ==  false)

	int  i_buf;

	/*CString  s_buf;
	if  ( (iRunVirusesOnlyAfterCTImprove  ==  1)&&(iPatternExtractionBasedOnLocalOpt ==  1) )
	{
		s_buf.Format("Normal run. Init done: %d", *piInitDone);
		vShow(s_buf);
	}//if  ( (iRunVirusesOnlyAfterCTImprove  ==  1)&&(iPatternExtractionBasedOnLocalOpt ==  1) )*/

	if  (iRunVirusesOnlyAfterCTImprove  ==  1)
	{
		if  (*piInitDone  ==  2)
		{
			//s_buf.Format("vir run after imporve. current fit: %.8lf", dGetCompTemplateFitness());
			//vShow(s_buf);
			c_err  =  e_juxtapositional_phase(&i_buf, pvInfectionHistory, iPatternExtractionBasedOnLocalOpt);
			//s_buf.Format("after e_juxtapositional_phase. current fit: %.8lf", dGetCompTemplateFitness());
			//vShow(s_buf);
			if  (c_err)  return(c_err);
		}//if  (*piInitDone  ==  2)
		//WE DO NOT DO TAKEBACK IN THIS CASE - THE CT WAS NOT INFECTED!
		//else
		//	v_take_back_ct_genotype_changes();
	}//if  (iRunVirusesOnlyAfterCTImprove  ==  1)
	else
	{
		if  ( (*piInitDone  ==  0)||(b_virgin_init  ==  true) )
		{
			c_err  =  e_juxtapositional_phase(&i_buf, pvInfectionHistory, iPatternExtractionBasedOnLocalOpt);
			if  (c_err)  return(c_err);
			
			if  (b_virgin_init == false)
			{
				CString  s_buf;

				//d_templ_fitness = -1;
				//double  d_before_repair = dGetCompTemplateFitness();
				
				if  (d_templ_fittness_before_insert  >  dGetCompTemplateFitness())  v_take_back_ct_genotype_changes();
				
				//d_templ_fitness = -1;
				//double  d_after_repair = dGetCompTemplateFitness();

				//s_buf.Format("before repair: %.2lf after repair: %.2lf", 1.0 / d_before_repair, 1.0 / d_after_repair);
				//vShow(s_buf);
			}//if  (b_virgin_init == false)
		}//if  (*piInitDone  ==  0)
	}//else  if  (iRunVirusesOnlyAfterCTImprove  ==  1)





	//we switch to full to obtain appropriate path suggestions, then we set simple encoding (MIX) back again
	if  (i_solution_construction_method  == SOLUTION_CONSTRUCTION_METHOS_MIX)
	{
		if  (listInfo  !=  NULL)
		{
			s_buf.Format("CT: SWITCHING TO FULL encoding to get path suggestions");
			//listInfo->Items->Add((String *) s_buf);
			//listInfo->Refresh();
		}//if  (listInfo  !=  NULL)

		double  d_ct_fit_before, d_ct_fit_after;
		d_ct_fit_before = dGetCompTemplateFitness();

		c_err = eMakePreciseGentoypeFromFluent();
		i_solution_construction_method = SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION;

		d_templ_fitness = -1;
		d_ct_fit_after = dGetCompTemplateFitness();

		if  (listInfo  !=  NULL)
		{
			s_buf.Format("POPULATION: Before Fit=%.8lf  CT=%.8lf  AFTER Fit=%.8lf  CT=%.8lf", d_ct_fit_before, 1.0/d_ct_fit_before, d_ct_fit_after, 1.0/d_ct_fit_after);
			//listInfo->Items->Add((String *) s_buf);
			//listInfo->Refresh();
		}//if  (listInfo  !=  NULL)

		i_solution_construction_method = SOLUTION_CONSTRUCTION_METHOS_MIX;


		if  (listInfo  !=  NULL)
		{
			s_buf.Format("END OF SWITCHING");
			//listInfo->Items->Add((String *) s_buf);
			//listInfo->Refresh();
		}//if  (listInfo  !=  NULL)
	}//if  ( (i_solution_construction_method  == SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FIRST_FULL_LATER)&&(b_removing_virginity == true) )

	
	return(c_err);
}//CError  CmGA::eMuppetsVirRun()



void  CmGA::v_try_pattern_and_template_insert(CVirusGA  *pcVGA,  int  *piInitDone, int iCrossOnlyIfImproves)
{
	*piInitDone  =  0;
	
	vector  <CMessyGene  *>  v_genotype,  v_genotype_for_pure_ct_and_pattern;

	CMessyPattern  *pc_pattern;
	CmGA  *pc_parent_ct;

	

	pc_pattern  =  pcVGA->pc_get_random_pattern();
	pc_parent_ct  =  pcVGA->pc_get_parent_tournament_normal_for_pattern_different(pc_pattern, this);

	if  ( (pc_parent_ct  ==  NULL)||(pc_pattern == NULL) )  
	{
		*piInitDone  =  1;
		return;
	}//if  ( (pc_parent_ct  ==  NULL)||(pc_pattern == NULL) )  

	/*if  (pc_pattern == NULL)  
		vShow("pc_pattern == NULL");
	else
		vShow("pc_pattern ok");

	if  (pc_parent_ct == NULL)  
		vShow("pc_parent_ct == NULL");
	else
		vShow("pc_parent_ct ok");*/

	if  (pc_competetive_template_before_insert  ==  NULL)  pc_competetive_template_before_insert  =  new CMessyGene[i_templ_length];
	for  (int  ii = 0;  ii < i_templ_length; ii++)
	{
		pc_competetive_template_before_insert[ii]  =  pc_competetive_template[ii];
	}//for  (int  ii = 0;  ii < i_templ_length; ii++)



	if  ( (pc_pattern  !=  NULL)&&(pc_parent_ct  !=  NULL) )
	{
		//we create base proposition
		for  (int  ij = 0; ij < (int) pc_pattern->pvGetGenotype()->size(); ij++)
		{
			v_genotype.push_back
				(
				new  CMessyGene
					(
					pc_pattern->pvGetGenotype()->at(ij)->i_demand_number,
					pc_parent_ct->pc_competetive_template[pc_pattern->pvGetGenotype()->at(ij)->i_demand_number].d_order,
					pc_parent_ct->pc_competetive_template[pc_pattern->pvGetGenotype()->at(ij)->i_demand_number].pc_path
					)
				);
		}//for  (int  ij = 0; ij < (int) pc_template->pvGetGenotype()->size(); ij++)


		//we check if the base proposition makes any changes - if it doesnt no computation wille be done AND
		bool  b_base_proposition_does_not_make_changes  =  true;
		for  (int  ij = 0; (ij < (int) v_genotype.size())&&(b_base_proposition_does_not_make_changes  ==  true); ij++)
		{
			if  (*(v_genotype.at(ij)) !=  pc_competetive_template[v_genotype.at(ij)->i_demand_number])
				b_base_proposition_does_not_make_changes  =  false;	
		}//for  (int  ij = 0; (ij < (int) v_genotype.size())&&(b_base_proposition_does_not_make_changes  ==  true); ij++)

		if  (b_base_proposition_does_not_make_changes  ==  true)
		{
			//we clear the genotype buffer
			for  (int  ij = 0; ij < (int) v_genotype.size(); ij++)
				delete  v_genotype.at(ij);
			v_genotype.clear();

			*piInitDone  =  1;
			return;
		}//if  (b_base_proposition_does_not_make_changes  ==  true)


		//we also check if the base proposition gives any enhancement - if it does we input it and dont do any computation too
		CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
		c_buf.vSetGenotype(&v_genotype);

		//we clear the genotype buffer
		for  (int  ij = 0; ij < (int) v_genotype.size(); ij++)
			delete  v_genotype.at(ij);
		v_genotype.clear();

		
		double  d_new_fit  =  c_buf.dComputeFitness(pc_competetive_template);
		c_buf.vRemoveUnimportantGenes(pc_competetive_template);

		if  (pc_infection_history  ==  NULL)
			pc_infection_history  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
		
		*pc_infection_history  =  c_buf;


		/*CString  s_buf;
		s_buf.Format("Before: %.8lf after: %.8lf", dGetCompTemplateFitness(), d_new_fit);
		::vShow(s_buf);//*/

		if  (d_new_fit  >  dGetCompTemplateFitness())
		{
			d_templ_fittness_before_insert  =  dGetCompTemplateFitness();
			v_infect_competetive_template(&c_buf);//it MUST be here because v_infect_competetive_template affects dGetCompTemplateFitness

			//vShow("improved!");

			*piInitDone  =  2;
			return;	
		}//if  (d_base_fit  >  dGetCompTemplateFitness())
		else
		{
			if  (iCrossOnlyIfImproves  ==  0)
			{
				d_templ_fittness_before_insert  =  dGetCompTemplateFitness();
				//vShow((int) (1.0/d_templ_fittness_before_insert));
				v_infect_competetive_template(&c_buf);
			}//if  (iCrossOnlyIfImproves  ==  0)
			else
			{
				*piInitDone  =  1;
			}//else  if  (iCrossOnlyIfImproves  ==  0)

		}//else  if  (d_base_fit  >  dGetCompTemplateFitness())*/


	}//if  ( (pc_pattern  !=  NULL)&&(pc_competetive_template  !=  NULL) )
	else
	{
		*piInitDone  =  1;
	}//else  if  ( (pc_pattern  !=  NULL)&&(pc_competetive_template  !=  NULL) )

}//void  CmGA::v_try_pattern_and_template_insert()


void  CmGA::vRemovePatternProposition()
{
	if  (pc_pattern_proposition  !=  NULL)
	{
		delete  pc_pattern_proposition;
		pc_pattern_proposition = NULL;
	}//if  (pc_pattern_proposition  !=  NULL)
}//void  CmGA::vRemovePatternProposition()



CError  CmGA::eVirRun(double  dMaxTime, CString  sReportFile,  CString  sSummarizedReport, CString sReferenceSolution,  System::Windows::Forms::ListBox*  listInfo)
{
	CError  c_err;
	bool  b_const;

	pc_fitness->vZeroEval();

	CTimeCounter  c_time_counter;


	CString  s_buf;
	c_time_counter.vSetStartNow();
	double  d_time_passed = 0;


	for  (;d_time_passed  < dMaxTime;)
	{
		c_err = eVirGeneratePopulation(pc_fitness->pcGetNet()->iGetDemandNumber());
		if  (c_err)  return(c_err);

		c_err  =  e_juxtapositional_phase(NULL, NULL);
		if  (c_err)  return(c_err);

		c_time_counter.bGetTimePassed(&d_time_passed);

		
		s_buf.Format
				(
				"current template fitness: %.8lf  [time:%.2lf]",  1 / d_templ_fitness, d_time_passed
				);
		listInfo->Items->Add((String *) s_buf);
		listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
		listInfo->Refresh();

		System::Windows::Forms::Application::DoEvents();
	}//for  (d_time_passed  < dMaxTime)


	FILE  *pf_report;
	pf_report  =  fopen(sReportFile,  "w+");
	eReport(pf_report);
	fclose(pf_report);

	pf_report  =  fopen(sReportFile + "_trans.txt",  "w+");
	eReportTransparent(pf_report);
	fclose(pf_report);


	double  d_ref_solution;
	
	if  (sReferenceSolution  !=  "")
	{
		pc_fitness->eLoadSolution(sReferenceSolution);
		d_ref_solution = pc_fitness->dCompCost(&b_const);
	}//if  (sReferenceSolution  !=  "")
	else
		d_ref_solution = 1;


	FILE  *pf_summarized_rep;
	
	CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	d_templ_fitness  =  c_buf.dComputeFitness(pc_competetive_template);	
	d_templ_fitness  =  pc_fitness->dCompCost(&b_const);
	double d_templ_fitness_no_pen  =  pc_fitness->dCompCost(&b_const, true);

	pf_summarized_rep  =  fopen(sSummarizedReport, "a");
	fprintf
		(pf_summarized_rep, 
		"%.8lf \t %.8lf \t %.8lf \t %.8lf \t %.8lf \t %.0lf \t %.1lf \t %.0lf \t %d \t %s \n", 

		d_templ_fitness, d_templ_fitness_no_pen, d_ref_solution,  d_templ_fitness / d_ref_solution, d_templ_fitness_no_pen / d_ref_solution, pc_fitness->dGetFuncEval(), d_time_passed, dMaxTime, b_const, sReportFile
		);
	fclose(pf_summarized_rep);

	return(c_err);
}//CError  CmGA::eVirRun()



CError  CmGA::eRunFastMessyGa
			(
			vector  <int> *pvBBLengths,  int  iRandomTemplateStart,
			CString  sReportFile,  CString  sSummarizedReport,  System::Windows::Forms::ListBox*  listInfo
			)
{
	CError  c_err;
	return(c_err);

	/*if  (pc_fitness  ==  NULL)
	{
		c_err.vPutError("No fitness function defined");
		return(c_err);	
	}//if  (pc_fitness  ==  NULL)


	//for parameter check...
	c_err  =  eConfigure
		(
		i_generations,

		i_prim_gen_without_pop_red,//population is not reduced
		i_prim_gen_reduce_pop_every_2nd,  //population reduced every other generation
		i_prim_gen_sized_but_not_reduced,  //population is sized at iPopSize, but primordial phase is still going on

		i_pop_size,
		d_pop_reduction_primordial,

		d_prob_cut,  d_prob_splice,
		d_prob_mut,
		d_prob_mut_rem_gene,
		d_prob_mut_add_gene
		);
	if  (c_err)  return(c_err);


	//first prepare random template for the first iteration...
	for  (int  ii = 0;  ii < i_templ_length; ii++)
		if  (iRandomTemplateStart  ==  0)
			pi_competetive_template[ii]  =  0;
		else
			pi_competetive_template[ii]  =  pc_random_gen->Next(0,2);
		

	dGetCompTemplateFitness();


	pc_fitness->vZeroEval();
	CTimeCounter  c_time_counter;
	c_time_counter.vSetStartNow();

	FILE  *pf_report;
	pf_report  =  fopen(sReportFile,  "w+");
	if  (pf_report  ==  NULL)
	{
		c_err.vPutError("Unable to open report file");
		return(c_err);	
	}//if  (pf_report  ==  NULL)

	CString  s_buf;
	double  d_unitation_perc,  d_fitness_eval_till_max,  d_time_passed_till_max;
	double  d_max_value;
//	d_max_value  =  pc_fitness->dGetMaxFuncVal();


	for  (int  ii = 0;  (ii < (int) pvBBLengths->size())&&(dGetCompTemplateFitness() <  d_max_value); ii++)
	{
		fprintf(pf_report,  "\nSTART FOR BB: %d\n\n\n",  pvBBLengths->at(ii));

		if  (listInfo  !=  NULL)
		{
			s_buf.Format("START FOR BB: %d",  pvBBLengths->at(ii));
			listInfo->Items->Add((String *) s_buf);
			listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
			listInfo->Refresh();

			System::Windows::Forms::Application::DoEvents();
		}//if  (listInfo  !=  NULL)


		c_err  =  e_run_iteration_fast_mga(pf_report,  pvBBLengths->at(ii), NULL, d_fmga_primordial_red_rate,  i_fmga_primordial_red_period,  listInfo);
		if  (c_err)
		{
			fclose(pf_report);
			return(c_err);		
		}//if  (c_err)

		fprintf(pf_report,  "\nEND FOR BB: %d\n",  pvBBLengths->at(ii));

		//eReportIndividuals(pf_report);


		d_unitation_perc  =  iGetUnitation();
		d_unitation_perc  =  d_unitation_perc  /  i_templ_length;
		c_time_counter.bGetTimePassed(&d_time_passed_till_max);
		d_fitness_eval_till_max  =  pc_fitness->dGetFuncEval();

		if  (listInfo  !=  NULL)
		{
			s_buf.Format("found: %.8lf  untation: %.2lf  [fitness evaluations:%.0lf] [time passed:%.4lf]",  dGetCompTemplateFitness(),  d_unitation_perc, pc_fitness->dGetFuncEval(),  d_time_passed_till_max);
			listInfo->Items->Add((String *) s_buf);
			listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
			listInfo->Refresh();

			System::Windows::Forms::Application::DoEvents();
		}//if  (listInfo  !=  NULL)
	}//for  (int  ii = 0;  ii < i_iterations; ii++)


//	pc_fitness->eCreateReportSeparateFunc(pf_report);

	fclose(pf_report);

	FILE  *pf_summarized_rep;
	pf_summarized_rep  =  fopen(sSummarizedReport, "a");
	fprintf(pf_summarized_rep, "%.8lf \t%.2lf \t %.2lf\t%.2lf\n", dGetCompTemplateFitness(),  d_unitation_perc, d_fitness_eval_till_max, d_time_passed_till_max);
	fclose(pf_summarized_rep);

	return(c_err);*/
}//CError  eRunFastMessyGa(CString  sReportFile,  vector  <CMessyPattern  *>  *pvBestFound)



CError  CmGA::eRun(CString  sReportFile,  vector  <CMessyPattern  *>  *pvBestFound, vector  <int> *pvBBLengths)
{
	CError  c_err;

	if  (pc_fitness  ==  NULL)
	{
		c_err.vPutError("No fitness function defined");
		return(c_err);	
	}//if  (pc_fitness  ==  NULL)


	//for parameter check...
	c_err  =  eConfigure
		(
		i_generations,

		i_prim_gen_without_pop_red,//population is not reduced
		i_prim_gen_reduce_pop_every_2nd,  //population reduced every other generation
		i_prim_gen_sized_but_not_reduced,  //population is sized at iPopSize, but primordial phase is still going on

		i_pop_size,
		d_pop_reduction_primordial,

		d_prob_cut,  d_prob_splice,
		d_prob_mut,
		d_prob_mut_rem_gene,
		d_prob_mut_add_gene
		);
	if  (c_err)  return(c_err);


	//PRWpotem
	//przygotowanie template'a
	//first prepare random template for the first iteration...
	/*for  (int  ii = 0;  ii < i_templ_length; ii++)
		//pi_competetive_template[ii]  =  pc_random_gen->Next(0,2);
		pi_competetive_template[ii]  =  0;*/

	dGetCompTemplateFitness();

		

	CString  s_buf;
	for  (int  ii = 0;  ii < (int) pvBBLengths->size(); ii++)
	{
		s_buf.Format("%d.txt", ii);

		FILE  *pf_report;
		pf_report  =  fopen(sReportFile + s_buf,  "w+");
		if  (pf_report  ==  NULL)
		{
			c_err.vPutError("Unable to open report file");
			return(c_err);	
		}//if  (pf_report  ==  NULL)


		c_err  =  e_run_iteration(pf_report,  pvBBLengths->at(ii), pvBestFound);
		if  (c_err)
		{
			fclose(pf_report);
			return(c_err);		
		}//if  (c_err)

		//c_err  =  eReportIndividuals(pf_report);
		//fprintf(pf_report,  "\nEND OF PHASE: %d\n\n\n",  ii);

		fclose(pf_report);
	}//for  (int  ii = 0;  ii < i_iterations; ii++)


	//fclose(pf_report);
	return(c_err);
}//CError  CmGA::eRun(CString  sReportFile)




CError  CmGA::e_run_iteration_fast_mga
	(
	FILE  *pfReport,  int  iIterationNum,  vector  <CMessyPattern  *>  *pvBestFound, 
	double  dReductionRate,  int  iReductionPeriod,
	System::Windows::Forms::ListBox*  listInfo
	)
{
	CError  c_err;

	if  (iIterationNum  <  0)
	{
		c_err.vPutError("Iteration number below 0");
		return(c_err);
	}//if  (iIterationNum  <  0)


	int  i_gen_num  =  0;
	c_err  =  e_primordial_phase_fast_mga(&i_gen_num,  iIterationNum,  dReductionRate,  iReductionPeriod,  listInfo);
	if  (c_err)  return(c_err);

	//c_err  =  eReportIndividuals(pfReport);
	c_err  =  eReport(pfReport);
	fprintf(pfReport,  "\nEND OF PRIMORDIAL\n");
	fprintf(pfReport,  "FITNESS EVALUATIONS: %.0lf\n\n", pc_fitness->dGetFuncEval());
	
//	c_err  =  e_juxtapositional_phase(&i_gen_num,  pvBestFound);
	if  (c_err)  return(c_err);
	//c_err  =  eReportIndividuals(pfReport);
	c_err  =  eReport(pfReport);
	fprintf(pfReport,  "\nEND OF JUXTAPOSITIONAL\n");
	fprintf(pfReport,  "FITNESS EVALUATIONS: %.0lf\n\n", pc_fitness->dGetFuncEval());


	return(c_err);
}//CError  CmGA::e_run_iteration_fast_mga(pf_report,  ii, pvBestFound, dReductionRate,  iReductionPeriod)




CError  CmGA::e_run_iteration(FILE  *pfReport,  int  iIterationNum,  vector  <CMessyPattern  *>  *pvBestFound)
{
	CError  c_err;

	if  (iIterationNum  <  0)
	{
		c_err.vPutError("Iteration number below 0");
		return(c_err);
	}//if  (iIterationNum  <  0)


	int  i_gen_num  =  0;
	c_err  =  e_primordial_phase(&i_gen_num,  iIterationNum);
	if  (c_err)  return(c_err);

	c_err  =  eReportIndividuals(pfReport);
	fprintf(pfReport,  "\nEND OF PRIMORDIAL\n\n\n");

//	c_err  =  e_juxtapositional_phase(&i_gen_num,  pvBestFound);
	if  (c_err)  return(c_err);
	c_err  =  eReportIndividuals(pfReport);
	fprintf(pfReport,  "\nEND OF JUXTAPOSITIONAL\n\n\n");


	return(c_err);
}//CError  CmGA::e_run_iteration(FILE  *pfReport,  int  iIterationNum)



void  CmGA::v_infect_competetive_template(CMessyIndividual  *pcBestFound)
{
	int  i_feno_len = pc_fitness->pcGetNet()->iGetDemandNumber();

	d_templ_fitness = pcBestFound->dComputeFitness(pc_competetive_template);
	pcBestFound->vInfect(pc_competetive_template);
	d_templ_fitness = -1;

	for  (int  ii = 0; ii < (int) pv_population->size(); ii++)
	{
		pv_population->at(ii)->b_fenotype_actual = false;
	}//for  (int  ii = 0; pv_population->size(); ii++)
}//void  CmGA::v_infect_competetive_template(CMessyIndividual  *pcBestFound)




void  CmGA::v_find_best_indiv()
{
	for  (int  ii = 0;  ii < (int)  pv_population->size();  ii++)
	{
		if  (pc_best_found  ==  NULL)
		{
			if  (d_templ_fitness  <  pv_population->at(ii)->dComputeFitness(pc_competetive_template))
			{
				pc_best_found  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);

				*pc_best_found  =  *(pv_population->at(ii));
			}//if  (d_templ_fitness  <  pv_population->at(ii)->dComputeFitness(pi_competetive_template,  pc_fitness))
		}//if  (pc_best_found  ==  NULL)
		else
		{
			if  (
                pc_best_found->dComputeFitness(pc_competetive_template)
				<
				pv_population->at(ii)->dComputeFitness(pc_competetive_template)
				)
				*pc_best_found  =  *(pv_population->at(ii));
		
		}//else  if  (pc_best_found  ==  NULL)
	
	}//for  (int  ii = 0;  ii < (int)  pv_population->size();  ii++)

}//void  CmGA::v_find_best_indiv()



CError  CmGA::e_juxtapositional_phase(int  *piGenNum,  vector  <CMessyPattern  *>  *pvInfectionHistory, int  iPatternExtractionBasedOnLocOpt)
{
	CError  c_err;
	
	CMessyIndividual  *pc_parent_1,  *pc_parent_2;
	int  i_parent_1_child_num,  i_parent_2_child_num;
	CMessyIndividual  *pc_par_1_child_1,  *pc_par_1_child_2;
	CMessyIndividual  *pc_par_2_child_1,  *pc_par_2_child_2;

	vector  <CMessyIndividual  *>  *pv_new_population;

	d_templ_fitness  =  dGetCompTemplateFitness();
	if  (pv_population  ==  NULL)  return(c_err);//if population does not exist we just simply go on and leave...


	if  (pc_best_found  !=  NULL)  
	{
		delete  pc_best_found;
		pc_best_found  =  NULL;	
	}//if  (pc_best_found  !=  NULL)  

	double  d_current_pop_size;
	//d_current_pop_size  =  i_pop_size;
	d_current_pop_size  =  pv_population->size();
	for  (int  i_gen = 0; (i_gen <  i_generations)&&(pc_best_found == NULL); i_gen++)
	{
		//we do not reduce for the first generation...
		if  (i_gen  !=  0)  d_current_pop_size  *=  d_pop_reduction_juxtapositional;

		pv_new_population  =  new  vector  <CMessyIndividual  *>;

		for  (;(int) pv_new_population->size()  <  d_current_pop_size;)
		{

//			if  (b_repair_necessary()  ==  true)  vShow("rep = 0.02");
			pc_parent_1  =  pc_get_parent_tournament(2);
//			if  (b_repair_necessary()  ==  true)  vShow("rep = 0.03");
			pc_parent_2  =  pc_get_parent_tournament(2);
//			if  (b_repair_necessary()  ==  true)  vShow("rep = 0.04");

			pc_par_1_child_1  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
			pc_par_1_child_2  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
			pc_par_2_child_1  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
			pc_par_2_child_2  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);

			c_err  =  pc_parent_1->eCut(d_prob_cut,  pc_random_gen, pc_par_1_child_1,  pc_par_1_child_2,  &i_parent_1_child_num);
			if  (c_err)
			{
				delete  pc_par_1_child_1;
				delete  pc_par_1_child_2;
				delete  pc_par_2_child_1;
				delete  pc_par_2_child_2;

				return(c_err);
			}//if  (c_err)

			c_err  =  pc_parent_2->eCut(d_prob_cut,  pc_random_gen, pc_par_2_child_1,  pc_par_2_child_2,  &i_parent_2_child_num);
			if  (c_err)
			{
				delete  pc_par_1_child_1;
				delete  pc_par_1_child_2;
				delete  pc_par_2_child_1;
				delete  pc_par_2_child_2;

				return(c_err);
			}//if  (c_err)

			
			bool  b_spliced;
			if  ( (i_parent_1_child_num  ==  2)&&(i_parent_2_child_num  ==  2) )
			{
				pc_par_1_child_1->eSplice(d_prob_splice,  pc_random_gen,  pc_par_2_child_2,  &b_spliced);

				if  (b_spliced  ==  true)
				{
					pc_par_2_child_1->eSplice(d_prob_splice,  pc_random_gen,  pc_par_1_child_2,  &b_spliced);

					if  (b_spliced  == true)
					{
						pv_new_population->push_back(pc_par_1_child_1);
						pv_new_population->push_back(pc_par_2_child_1);


						delete  pc_par_1_child_2;
						delete  pc_par_2_child_2;
					}//if  (b_spliced  == true)
					else
					{
						pv_new_population->push_back(pc_par_1_child_1);
						pv_new_population->push_back(pc_par_1_child_2);
						pv_new_population->push_back(pc_par_2_child_1);


						delete  pc_par_2_child_2;
					}//else  if  (b_spliced  == true)
				
				}//if  (b_spliced  ==  true)
				else
				{
					pc_par_2_child_2->eSplice(d_prob_splice,  pc_random_gen,  pc_par_2_child_1,  &b_spliced);

					if  (b_spliced  == true)
					{
						pv_new_population->push_back(pc_par_1_child_1);
						pv_new_population->push_back(pc_par_1_child_2);
						pv_new_population->push_back(pc_par_2_child_2);


						delete  pc_par_2_child_1;
					}//if  (b_spliced  == true)
					else
					{
						pc_par_2_child_1->eSplice(d_prob_splice,  pc_random_gen,  pc_par_1_child_2,  &b_spliced);

						if  (b_spliced  == true)
						{
							pv_new_population->push_back(pc_par_1_child_1);
							pv_new_population->push_back(pc_par_2_child_1);
							pv_new_population->push_back(pc_par_2_child_2);

							delete  pc_par_1_child_2;
						}//if  (b_spliced  == true)
						else
						{
							pv_new_population->push_back(pc_par_1_child_1);
							pv_new_population->push_back(pc_par_1_child_2);
							pv_new_population->push_back(pc_par_2_child_1);
							pv_new_population->push_back(pc_par_2_child_2);

						}//else  if  (b_spliced  == true)

					}//else  if  (b_spliced  == true)
				
				}//else  if  (b_spliced  ==  true)
			
			}//if  ( (i_parent_1_child_num  ==  2)&&(i_parent_2_child_num  ==  2) )


			if  ( (i_parent_1_child_num  ==  2)&&(i_parent_2_child_num  ==  1) )
			{
				delete  pc_par_2_child_2;
							

				pc_par_1_child_1->eSplice(d_prob_splice,  pc_random_gen,  pc_par_2_child_1,  &b_spliced);

				if  (b_spliced  ==  true)
				{
					pv_new_population->push_back(pc_par_1_child_1);
					pv_new_population->push_back(pc_par_1_child_2);

					
					delete  pc_par_2_child_1;
				}//if  (b_spliced  ==  true)
				else
				{
					pc_par_2_child_1->eSplice(d_prob_splice,  pc_random_gen,  pc_par_1_child_2,  &b_spliced);
					if  (b_spliced  ==  true)
					{
						pv_new_population->push_back(pc_par_1_child_1);
						pv_new_population->push_back(pc_par_2_child_1);

							
						delete  pc_par_1_child_2;
					}//if  (b_spliced  ==  true)
					else
					{
						pv_new_population->push_back(pc_par_1_child_1);
						pv_new_population->push_back(pc_par_1_child_2);
						pv_new_population->push_back(pc_par_2_child_1);

					}//else  if  (b_spliced  ==  true)*/
				
				}//else  if  (b_spliced  ==  true)

			
			}//if  ( (i_parent_1_child_num  ==  2)&&(i_parent_2_child_num  ==  1) )


			if  ( (i_parent_1_child_num  ==  1)&&(i_parent_2_child_num  ==  2) )
			{
				delete  pc_par_1_child_2;

				pc_par_1_child_1->eSplice(d_prob_splice,  pc_random_gen,  pc_par_2_child_2,  &b_spliced);

				if  (b_spliced  ==  true)
				{
					pv_new_population->push_back(pc_par_1_child_1);
					pv_new_population->push_back(pc_par_2_child_1);

					delete  pc_par_2_child_2;
				}//if  (b_spliced  ==  true)
				else
				{
					pc_par_2_child_1->eSplice(d_prob_splice,  pc_random_gen,  pc_par_2_child_2,  &b_spliced);
					if  (b_spliced  ==  true)
					{
						pv_new_population->push_back(pc_par_1_child_1);
						pv_new_population->push_back(pc_par_2_child_1);

						delete  pc_par_2_child_2;
					}//if  (b_spliced  ==  true)
					else
					{
						pc_par_2_child_1->eSplice(d_prob_splice,  pc_random_gen,  pc_par_1_child_1,  &b_spliced);
						if  (b_spliced  ==  true)
						{
							pv_new_population->push_back(pc_par_2_child_1);
							pv_new_population->push_back(pc_par_2_child_2);

				
							delete  pc_par_1_child_1;
						}//if  (b_spliced  ==  true)
						else
						{
							pv_new_population->push_back(pc_par_1_child_1);
							pv_new_population->push_back(pc_par_2_child_1);
							pv_new_population->push_back(pc_par_2_child_2);

						}//else  if  (b_spliced  ==  true)
					}//else  if  (b_spliced  ==  true)
				
				}//else  if  (b_spliced  ==  true)

			}//if  ( (i_parent_1_child_num  ==  1)&&(i_parent_2_child_num  ==  2) )



			if  ( (i_parent_1_child_num  ==  1)&&(i_parent_2_child_num  ==  1) )
			{
				delete  pc_par_1_child_2;
				delete  pc_par_2_child_2;
				
				pc_par_1_child_1->eSplice(d_prob_splice,  pc_random_gen,  pc_par_2_child_1,  &b_spliced);

				if  (b_spliced  ==  true)
				{
					pv_new_population->push_back(pc_par_1_child_1);
						
					delete  pc_par_2_child_1;
				}//if  (b_spliced  ==  true)
				else
				{
					pc_par_2_child_1->eSplice(d_prob_splice,  pc_random_gen,  pc_par_1_child_1,  &b_spliced);

					if  (b_spliced  ==  true)
					{
						pv_new_population->push_back(pc_par_2_child_1);
							
						delete  pc_par_1_child_1;
					}//if  (b_spliced  ==  true)
					else
					{
						pv_new_population->push_back(pc_par_1_child_1);
						pv_new_population->push_back(pc_par_2_child_1);
					}//else if  (b_spliced  ==  true)
				}//else  if  (b_spliced  ==  true)
			}//if  ( (i_parent_1_child_num  ==  1)&&(i_parent_2_child_num  ==  1) )
		}//for  (;pv_new_population->size()  <  i_pop_size;)

		//replace old population with new
		for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);
		pv_population->clear();
		delete  pv_population;

		pv_population  =  pv_new_population;

//		if  (b_repair_necessary()  ==  true)  vShow("rep = 0.1");

		v_find_best_indiv();


		/*if  (pc_best_found  !=  NULL)  
		{
			if  (pc_best_found->dComputeFitness(pc_competetive_template)  >  d_templ_fitness)
			{
				v_infect_competetive_template(pc_best_found);
				i_gen = 0;
			}//if  (pc_best_found->dComputeFitness(pc_competetive_template)  >  d_templ_fitness)
		}//if  (pc_best_found  !=  NULL)
		else
			b_virgin_init  =  false;*/


		

		//mutate population
		if  (d_prob_mut_rem_gene  >  0)
			for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)
				pv_population->at(ii)->vMutateRemGene(d_prob_mut_rem_gene,  pc_random_gen);

		if  (d_prob_mut  >  0)
			for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)
				pv_population->at(ii)->vMutate(d_prob_mut,  pc_random_gen);

		if  (d_prob_mut_add_gene  >  0)
			for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)
				pv_population->at(ii)->vMutateAddGene(d_prob_mut_add_gene,  pc_random_gen,  i_templ_length);

	
//		if  (b_repair_necessary()  ==  true)  vShow("rep = 0.2");
		v_find_best_indiv();
//		if  (b_repair_necessary()  ==  true)  vShow("rep = 0.5");

	}//for  (; *piGenNum <  i_generations; (*piGenNum)++)

	v_find_best_indiv();
//	if  (b_repair_necessary()  ==  true)  vShow("rep = 1");

	if  (pc_pattern_proposition  !=  NULL)
	{
		delete  pc_pattern_proposition;
		pc_pattern_proposition = NULL;
	}//if  (pc_pattern_proposition  !=  NULL)


	if  (pc_best_found  !=  NULL)  
	{
//		if  (b_repair_necessary()  ==  true)  vShow("rep = 2");
		if  (iPatternExtractionBasedOnLocOpt  ==  0)
		{
			pc_best_found->vRemoveUnimportantGenes(pc_competetive_template);

			CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
			//double d_templ_fitness_test  =  1.0 / dGetCompTemplateFitness();
			//double  d_test = 1.0 / pc_best_found->dComputeFitness(pc_competetive_template);
			//d_templ_fitness = -1;
			//dGetCompTemplateFitness();
			v_infect_competetive_template(pc_best_found);
			//vShow("infect");
			pc_fitness->vSetPathFitness();
			//d_templ_fitness = -1;
			//dGetCompTemplateFitness();

			//CString  s_buf;
			//s_buf.Format("after infect: %.2lf", 1.0/d_templ_fitness);
			//vShow(s_buf);
			/*d_templ_fitness = -1;
			double d_templ_fitness_test2  =  1.0 / dGetCompTemplateFitness();
			pc_fitness->vSetPathFitness();
			d_templ_fitness = -1;
			double d_templ_fitness_test3  =  1.0 / dGetCompTemplateFitness();

			//if  (d_templ_fitness > d_templ_fitness_test2)
			{
				CString  s_buf;
				s_buf.Format("temp old:%.8lf, best %.8lf temp new:%.8lf temp paths: %.8lf", d_templ_fitness_test, d_test, d_templ_fitness_test2, d_templ_fitness_test3);
				::vShow(s_buf);
			}//if  (d_templ_fitness > d_templ_fitness_test2)*/


			if  (pc_infection_history  ==  NULL)
			{
				pc_infection_history  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
				pc_infection_history->vSplice(pc_best_found);
			}//if  (pc_best_found  ==  NULL)
			else
			{
				pc_infection_history->vSplice(pc_best_found);			
			}//else  if  (pc_best_found  ==  NULL)
		}//if  (iPatternExtractionBasedOnLocOpt  ==  0)
		else
		{
			//pattern extraction based on difference between optimized result of CT and infecting virus
			CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
			double d_templ_fitness_test  =  c_buf.dComputeFitness(pc_competetive_template);
			pc_fitness->vCopyChannelsState(pc_ct_channels);

			pc_best_found->b_fenotype_actual = false;
			double  d_test = pc_best_found->dComputeFitness(pc_competetive_template);
			pc_fitness->vCopyChannelsState(pc_best_virus_channels);

			v_infect_competetive_template(pc_best_found);

			vector<CMessyGene*> v_pattern_genotype;

			for  (int  ii = 0; ii < pc_fitness->pcGetNet()->iGetDemandNumber(); ii++)
			{
				if  (pc_ct_channels[ii]  !=  pc_best_virus_channels[ii])
				{
					//only demand number is important - this is a gene offset in classic GA genotype  order and path are gene value
					v_pattern_genotype.push_back(new CMessyGene(ii, pc_best_found->pc_fenotype[ii]->d_order, pc_best_found->pc_fenotype[ii]->pc_path));
				}//if  (pc_ct_channels[ii]  !=  pc_best_virus_channels[ii])			
			}//for  (int  ii = 0; ii < pc_fitness->pcGetNet()->iGetDemandNumber(); ii++)
		
			//CString  s_buf;
			//s_buf.Format("Pattern length: %d / %d;  templ:%.8lf, best %.8lf", v_pattern_genotype.size(), pc_fitness->pcGetNet()->iGetDemandNumber(), d_templ_fitness_test, d_test);
			//::vShow(s_buf);

			if  (v_pattern_genotype.size() > 0)
			{
				if  (pc_pattern_proposition  !=  NULL)  delete  pc_pattern_proposition;
				pc_pattern_proposition = new CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
				pc_pattern_proposition->vSetGenotype(&v_pattern_genotype);
				pc_pattern_proposition->vSetFitness(pc_fitness);
			}//if  (v_pattern_genotype.size() > 0)*/

			
			

			for  (int  ii = 0; ii < v_pattern_genotype.size(); ii++)
				delete  v_pattern_genotype.at(ii);

			
		}//else  if  (iPatternExtractionBasedOnLocOpt  ==  0)
	
		
	}//if  (pc_best_found  !=  NULL)
	else
		b_virgin_init  =  false;


	return(c_err);
}//CError  CmGA::e_juxtapositional_phase(int  *piGenNum)



void  CmGA::v_take_back_ct_genotype_changes()
{
	//we check if fitness hasnt droped if it did then we restore the previous (better) template
	if  ( (pc_competetive_template_before_insert  !=  NULL)&&(d_templ_fittness_before_insert  >  dGetCompTemplateFitness()) )
	{
		//first we check if the modification was NOT a takeback of insert operation (pattern*ct in VirRegeneratePopulation)
		bool  b_take_back  =  true;
		for  (int  ii = 0;  (ii < i_templ_length)&&(b_take_back  ==  true); ii++)
		{
			if  (pc_competetive_template[ii]  !=  pc_competetive_template_before_insert[ii])
				b_take_back  =  false;
		}//for  (int  ii = 0;  (ii < i_templ_length)&&(b_take_back  ==  false); ii++)

		if  (b_take_back  ==  true)
		{
			delete  pc_infection_history;
			pc_infection_history  =  NULL;	
		}//if  (b_take_back  ==  true)
			


		for  (int  ii = 0;  ii < i_templ_length; ii++)
			pc_competetive_template[ii]  =  pc_competetive_template_before_insert[ii];


		CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
		d_templ_fitness  =  c_buf.dComputeFitness(pc_competetive_template);
		c_buf.vCopyOrderedFenotype(pc_ordered_template);
	}//if  ( (pi_competetive_template_before_insert  !=  NULL)&&(d_templ_fittness_before_insert  >  dGetCompTemplateFitness()) )*/
}//void  CmGA::v_take_back_ct_genotype_changes()




void  CmGA::v_generate_genotype_fmga(vector <CMessyGene  *>  *pvGenotype,  int iVirusInitLength)
{
	/*vector  <int>  v_gene_positions;

	for  (int  ii = 0;  ii < i_templ_length;  ii++)
		v_gene_positions.push_back(ii);

	int  i_chosen;
	for  (int  ii = 0; ii < iVirusInitLength; ii++)
	{
		i_chosen  =  pc_random_gen->Next(0, v_gene_positions.size());

		pvGenotype->push_back(new  CMessyGene(pc_random_gen->Next(0, 2), v_gene_positions.at(i_chosen) ));

		v_gene_positions.erase(v_gene_positions.begin() + i_chosen);			
	}//for  (int  ii = 0; ii < iVirusInitLength; ii++)*/

}//void  CmGA::v_generate_genotype_fmga(vector <CMessyGene  *>  *pvGenotype,  int iVirusInitLength)



double  CmGA::d_compute_threshold(int  iCurVirLength)
{
	double  d_result,  d_buf;

	d_buf  =  i_templ_length;
	d_buf  *=  i_templ_length;
	d_buf  *=  (i_templ_length - 1);

	d_buf  =  sqrt(d_buf);

	d_result  =  iCurVirLength;
	d_result  *=  i_templ_length - iCurVirLength;

	d_result  =  d_result / d_buf;
	d_result  *=  d_threshold_multiplier;

	d_buf  =  iCurVirLength;
	d_buf  *=  iCurVirLength;
	d_buf  =  d_buf / i_templ_length;


	d_result  +=  d_buf;

	return(d_result);
}//double  CmGA::d_compute_threshold()



CError  CmGA::e_primordial_phase_fast_mga
	(
	int  *piGenNum,  int  iBBLength,  double  dReductionRate,  int  iReductionPeriod,
	System::Windows::Forms::ListBox*  listInfo
	)
{
	CError  c_err;

	//first generate population of messy individuals
	if  (pv_population  !=  NULL)
	{
		for  (int  ii = 0; ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);	
		pv_population->clear();
	}//if  (pv_population  !=  NULL)
	else
		pv_population  =  new  vector  <CMessyIndividual *>;


	vector  <CMessyIndividual  *>  *pv_new_population,  *pv_population_buffer;
	pv_new_population  =  new  vector  <CMessyIndividual *>;




	CMessyIndividual  *pc_indiv;
	CMessyIndividual  *pc_new_indiv,  *pc_new_indiv_2;
	vector  <CMessyGene  *>  v_genotype;
	CString  s_buf;

	int  i_virus_init_length  =  i_templ_length  -  iBBLength;

	//first filling the population
	for  (int  ii = 0; ii < i_pop_size;  ii++)
	{

		v_generate_genotype_fmga(&v_genotype, i_virus_init_length);

		/*for  (int  ij = 0; ij < i_virus_init_length; ij++)
			v_genotype.push_back(new  CMessyGene(pc_random_gen->Next(0, 2), pc_random_gen->Next(0, i_templ_length)));*/

		pc_indiv  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
		pv_population->push_back(pc_indiv);
		
		pc_indiv->vSetGenotype(&v_genotype);

		for  (int  ij = 0; ij < (int) v_genotype.size(); ij++)
			delete  v_genotype.at(ij);
		v_genotype.clear();

		//the other buffer...
		pc_new_indiv_2  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
		*pc_new_indiv_2  =  *pc_indiv;
		pv_new_population->push_back(pc_new_indiv_2);
	}//for  (int  ii = 0; ii < (int) pv_population->size();  ii++)

	
	if  (listInfo  !=  NULL)
	{
		s_buf.Format("created %d individuals [fitness evaluations:%.0lf]",  i_pop_size, pc_fitness->dGetFuncEval());
		listInfo->Items->Add((String *) s_buf);
		listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
		listInfo->Refresh();

		System::Windows::Forms::Application::DoEvents();
	}//if  (listInfo  !=  NULL)



	int  i_parent_offset_1, i_parent_offset_2;
	double  d_number_of_the_same_genes;
	
	int  i_cur_vir_length  =  i_virus_init_length;
	int  i_next_vir_length;
	for  (;i_cur_vir_length !=  0;)
	{
		d_number_of_the_same_genes  =  d_compute_threshold(i_cur_vir_length);

		for  (int  ii = 0; ii < iReductionPeriod;  ii++)
		{
			//pv_new_population  =  new  vector  <CMessyIndividual  *>;

			for  (int  ij = 0;  ij < (int) pv_population->size() / 2;  ij++)
			{
				//pc_new_indiv  =  new  CMessyIndividual(i_templ_length);
				//pc_new_indiv_2  =  new  CMessyIndividual(i_templ_length);
				pc_new_indiv  =  pv_new_population->at(ij * 2);
				pc_new_indiv_2  =  pv_new_population->at(ij * 2 + 1);

				i_parent_offset_1  =  pc_random_gen->Next(0, pv_population->size());
				i_parent_offset_2  =  -1;

				for  (
					 int  ik = 0; 
					 (ik < i_fmga_primordial_number_of_indiv_to_check)
					 &&
					 (i_parent_offset_2  ==  -1);
					 ik++
					 )
				{
					/*i_parent_offset_2  =  pc_random_gen->Next(0, pv_population->size());
					if  (
						pv_population->at(i_parent_offset_1)->iCompareAlles(pv_population->at(i_parent_offset_2))
						<
						d_number_of_the_same_genes
						)
						i_parent_offset_2  =  -1;*/

				}//for

				if  (i_parent_offset_2  !=  -1)
				{
					*pc_new_indiv  =  *pc_get_parent_tournament(i_parent_offset_1,  i_parent_offset_2,  true);
					*pc_new_indiv_2  =  *pc_new_indiv;
				}//if  (i_parent_offset_2  !=  -1)
				else
				{
					*pc_new_indiv  =  *(pv_population->at(i_parent_offset_1));
					*pc_new_indiv_2  =  *pc_new_indiv;				
				}//else  if  (i_parent_offset_2  !=  -1)
				

				//pv_new_population->push_back(pc_new_indiv);
				//pv_new_population->push_back(pc_new_indiv_2);

			}//for  (int  ij = 0;  ij < (int) pv_population->size() / 2;  ij++)

			//replace old population with new
			/*for  (int  ij = 0;  ij < (int) pv_population->size();  ij++)
				delete  pv_population->at(ij);
			pv_population->clear();
			delete  pv_population;*/

			pv_population_buffer  =  pv_population;
			pv_population  =  pv_new_population;
			pv_new_population  =  pv_population_buffer;

		}//for  (int  ii = 0; ii < iReductionPeriod;  ii++)


		if  (i_cur_vir_length  !=  iBBLength)
		{
			if  (dReductionRate  >  0)
				i_next_vir_length  =  (int) Math::Round(dReductionRate  *  i_cur_vir_length);
			else
				i_next_vir_length  =  i_cur_vir_length - 1;

			if  (i_next_vir_length  >=  i_cur_vir_length)  i_next_vir_length  =  i_cur_vir_length - 1;
			if  (i_next_vir_length  <  iBBLength)  i_next_vir_length  =  iBBLength;

			for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)
			{
				for  (int  ij = 0; ij < i_cur_vir_length - i_next_vir_length; ij++)
					pv_population->at(ii)->vRemoveRandomGene(pc_random_gen);		
			}//for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)

			if  (listInfo  !=  NULL)
			{
				s_buf.Format("end of primordial for length: %d (threshold: %.2lf) [fitness evaluations:%.0lf]",  i_cur_vir_length, d_number_of_the_same_genes, pc_fitness->dGetFuncEval());
				listInfo->Items->Add((String *) s_buf);
				listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
				listInfo->Refresh();

				System::Windows::Forms::Application::DoEvents();
			}//if  (listInfo  !=  NULL)

			i_cur_vir_length  =  i_next_vir_length;
		}//if  (i_cur_vir_length  !=  iIterationNum + 1)
		else
		{
			if  (listInfo  !=  NULL)
			{
				s_buf.Format("end of primordial for length: %d (threshold: %.2lf) [fitness evaluations:%.0lf]",  i_cur_vir_length, d_number_of_the_same_genes, pc_fitness->dGetFuncEval());
				listInfo->Items->Add((String *) s_buf);
				listInfo->SelectedIndex  =  listInfo->Items->Count - 1;
				listInfo->Refresh();

				System::Windows::Forms::Application::DoEvents();
			}//if  (listInfo  !=  NULL)

			i_cur_vir_length  =  0;
		}//else  if  (i_cur_vir_length  !=  iIterationNum + 1)*/
		
	}//for  (;i_cur_vir_length < i_virus_init_length;)

	for  (int  ij = 0;  ij < (int) pv_new_population->size();  ij++)
		delete  pv_new_population->at(ij);
	pv_new_population->clear();
	delete  pv_new_population;


	return(c_err);
}//CError  CmGA::e_primordial_phase_fast_mga(int  *piGenNum,  int  iIterationNum,  double  dReductionRate,  int  iReductionPeriod)




CError  CmGA::e_primordial_phase(int  *piGenNum,  int  iIterationNum)
{
	CError  c_err;

	//first generate population of messy individuals
	if  (pv_population  !=  NULL)
	{
		for  (int  ii = 0; ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);	
		pv_population->clear();
	}//if  (pv_population  !=  NULL)
	else
		pv_population  =  new  vector  <CMessyIndividual *>;


	vector  <CMessyGene  *>  v_generator;
	v_fill_generator(&v_generator,  iIterationNum+1,  0,  0);

	for  (int  ii = 0; ii < (int) v_generator.size();  ii++)
		delete  v_generator.at(ii);	
	v_generator.clear();


	//second we run primordial without pop reduction
	vector  <CMessyIndividual  *>  *pv_new_population;
	CMessyIndividual  *pc_new_indiv,  *pc_new_indiv_2;

	for  (int  ii = 0; ii < i_prim_gen_without_pop_red;  ii++)
	{
		(*piGenNum)++;

		if  (ii >  0)  v_shuffle_population();

		pv_new_population  =  new  vector  <CMessyIndividual  *>;

		for  (int  ii = 0;  ii < (int) pv_population->size() / 2;  ii++)
		{
			pc_new_indiv  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
			pc_new_indiv_2  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);

			*pc_new_indiv  =  *pc_get_parent_tournament(ii*2,  ii*2 + 1,  true);
			*pc_new_indiv_2  =  *pc_new_indiv;

			pv_new_population->push_back(pc_new_indiv);
			pv_new_population->push_back(pc_new_indiv_2);
		}//for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)

		//replace old population with new
		for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);
		pv_population->clear();
		delete  pv_population;

		pv_population  =  pv_new_population;
	}//for  (int  ii = 0; ii < i_prim_phase_gen_without_pop_reduction;  ii++)


	//third reduce population at every other generation, but STOP if the population size is proper
	bool  b_reduce_pop  =  true;

	int  i_next_pop_size;
	for  (
		int  ii = 0; 
		(ii < i_prim_gen_reduce_pop_every_2nd)
		&&
		(i_pop_size  <  (int) pv_population->size());
		ii++
		)
	{
		(*piGenNum)++;
		v_shuffle_population();

		pv_new_population  =  new  vector  <CMessyIndividual  *>;

		if  (b_reduce_pop  ==  false)
		{
			i_next_pop_size  =  (int) pv_population->size();
			b_reduce_pop  =  true;
		}//if  (b_reduce_pop  ==  false)
		else
		{
			i_next_pop_size  =  (int) Math::Round(d_pop_reduction_primordial  *  pv_population->size());
			b_reduce_pop  =  false;		
		}//else  if  (b_reduce_pop  ==  false)
		

		if  (i_next_pop_size  <  i_pop_size)  i_next_pop_size  =  i_pop_size;


		//create new population...
		int  i_offset_iterator  =  0;
		for  (int  ii = 0;  ii < i_next_pop_size;  ii++)
		{
			pc_new_indiv  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);

			if  (i_offset_iterator*2  >=  (int) pv_population->size())  i_offset_iterator  =  0;

			if  (i_offset_iterator*2 + 1  <  (int) pv_population->size())
			{
				*pc_new_indiv  =  *pc_get_parent_tournament(i_offset_iterator*2,  i_offset_iterator*2+1,  true);

				//s_buf.Format("%d %d ii:%d", i_offset_iterator*2,  i_offset_iterator*2+1,  ii);
				//System::Diagnostics::Debug::WriteLine((String *)s_buf);
			}//if  (i_offset_iterator*2 + 1  <  (int) pv_population->size())
			else
				*pc_new_indiv  =  *pc_get_parent_tournament(i_offset_iterator*2,  0,  true);

			i_offset_iterator++;
			pv_new_population->push_back(pc_new_indiv);
		}//for  (int  ii = 0;  ii < i_next_pop_size;  ii++)



		//replace old population with new
		for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);
		pv_population->clear();
		delete  pv_population;

		pv_population  =  pv_new_population;
			
	}//for  (;i_pop_size  <  (int) pv_population->size();)



	//4th reduce population at every generation, but STOP if the population size is proper
	for  (;i_pop_size  <  (int) pv_population->size();)
	{
		(*piGenNum)++;
		v_shuffle_population();

		pv_new_population  =  new  vector  <CMessyIndividual  *>;
		i_next_pop_size  =  (int) Math::Round(d_pop_reduction_primordial  *  pv_population->size());

		if  (i_next_pop_size  <  i_pop_size)  i_next_pop_size  =  i_pop_size;


		//create new population...
		int  i_offset_iterator  =  0;
		for  (int  ii = 0;  ii < i_next_pop_size;  ii++)
		{
			pc_new_indiv  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);

			if  (i_offset_iterator*2  >=  (int) pv_population->size())  i_offset_iterator  =  0;

			if  (i_offset_iterator*2 + 1  <  (int) pv_population->size())
			{
				*pc_new_indiv  =  *pc_get_parent_tournament(i_offset_iterator*2,  i_offset_iterator*2+1,  true);

				//s_buf.Format("%d %d ii:%d", i_offset_iterator*2,  i_offset_iterator*2+1,  ii);
				//System::Diagnostics::Debug::WriteLine((String *)s_buf);
			}//if  (i_offset_iterator*2 + 1  <  (int) pv_population->size())
			else
				*pc_new_indiv  =  *pc_get_parent_tournament(i_offset_iterator*2,  0,  true);

			i_offset_iterator++;
			pv_new_population->push_back(pc_new_indiv);
		}//for  (int  ii = 0;  ii < i_next_pop_size;  ii++)





		//replace old population with new
		for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);
		pv_population->clear();
		delete  pv_population;

		pv_population  =  pv_new_population;
			
	}//for  (;i_pop_size  <  (int) pv_population->size();)



	//5th DONT reduce just evluate...
	for  (int  ii = 0; ii < i_prim_gen_sized_but_not_reduced;  ii++)
	{
		(*piGenNum)++;

		if  (ii >  0)  v_shuffle_population();

		pv_new_population  =  new  vector  <CMessyIndividual  *>;

		for  (int  ii = 0;  ii < (int) pv_population->size() / 2;  ii++)
		{
			pc_new_indiv  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
			pc_new_indiv_2  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);

			*pc_new_indiv  =  *pc_get_parent_tournament(ii*2,  ii*2 + 1,  true);
			*pc_new_indiv_2  =  *pc_new_indiv;

			pv_new_population->push_back(pc_new_indiv);
			pv_new_population->push_back(pc_new_indiv_2);
		}//for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)

		//replace old population with new
		for  (int  ii = 0;  ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);
		pv_population->clear();
		delete  pv_population;

		pv_population  =  pv_new_population;
	}//for  (int  ii = 0; ii < i_prim_phase_gen_without_pop_reduction;  ii++)


	return(c_err);
}//CError  CmGA::e_primordial_phase(int  *piGenNum)



CMessyIndividual*  CmGA::pc_get_parent_tournament(int  iIndiv1Offset,  int  iIndiv2Offset,  bool  bBetterTakesAll  /*=  false*/)
{
	CMessyIndividual  *pc_candidate_1,  *pc_candidate_2;

	pc_candidate_1  =  pv_population->at(iIndiv1Offset);
	pc_candidate_2  =  pv_population->at(iIndiv2Offset);

	double  d_fit_1,  d_fit_2;
	int  i_test = 11;

//	if  (b_repair_necessary()  ==  true)  vShow("rep = 0.025");
	d_fit_1  =  pc_candidate_1->dComputeFitness(pc_competetive_template, &i_test);
//	if  (b_repair_necessary()  ==  true)  vShow("rep = 0.026");
	d_fit_1  =  Math::Round(d_fit_1, 2);
	d_fit_2  =  pc_candidate_2->dComputeFitness(pc_competetive_template);
//	if  (b_repair_necessary()  ==  true)  vShow("rep = 0.027");
	d_fit_2  =  Math::Round(d_fit_2, 2);



	if  (bBetterTakesAll  ==  true)
	{
		if  (d_fit_1  >  d_fit_2)  
			return(pc_candidate_1);
		else
			return(pc_candidate_2);
	
	}//if  (bBetterTakesAll  ==  true)
	else
	{
		double  d_fit_sum;
		d_fit_sum  =  d_fit_1  +  d_fit_2;
		d_fit_sum  =  d_fit_sum  *  pc_random_gen->NextDouble();

		if  (d_fit_sum  <  d_fit_1)
			return(pc_candidate_1);
		else
			return(pc_candidate_2);	
	}//else  if  (bBetterTakesAll  ==  true)

}//CMessyIndividual*  CmGA::pc_get_parent_tournament(int  iIndiv1Offset,  int  iIndiv2Offset)



CMessyIndividual*  CmGA::pc_get_parent_tournament(int  iTournamentSize,  bool  bBetterTakesAll  /*=  false*/)
{
	if  (iTournamentSize  ==  2)
	{
		int  i_indiv_1,  i_indiv_2;

		i_indiv_1  =  pc_random_gen->Next(0,  pv_population->size());
		i_indiv_2  =  pc_random_gen->Next(0,  pv_population->size());

		
		while  ( (i_indiv_1  ==  i_indiv_2)&&(pv_population->size()  >  1) )
			i_indiv_2  =  pc_random_gen->Next(0,  pv_population->size());

		//CString  s_buf;
		//s_buf.Format("%d %d %d", i_indiv_1,  i_indiv_2,  (int) pv_population->size());
		//System::Diagnostics::Debug::WriteLine((String *)s_buf);

		return(pc_get_parent_tournament(i_indiv_1,  i_indiv_2,  bBetterTakesAll));
	}//if  (iTournamentSize  ==  2)

	return(NULL);
}//CMessyIndividual*  CmGA::pc_get_parent_tournament(int  iTournamentSize)



void  CmGA::v_shuffle_population()
{
	vector  <CMessyIndividual  *>  *pv_shuffled_population;
	pv_shuffled_population  =  new  vector  <CMessyIndividual  *>;
	int  i_indiv_offset;

	FILE  *pf_test;

	pf_test  =  fopen("D:\\zztest.txt", "w+");

	for  (; 0 <  (int) pv_population->size(); )
	{
		i_indiv_offset  =  pc_random_gen->Next(0,  pv_population->size());
		pv_shuffled_population->push_back(pv_population->at(i_indiv_offset));
		pv_population->erase(pv_population->begin()  +  i_indiv_offset);

		fprintf(pf_test, "%d\n", i_indiv_offset);
	}//for  (int  ii = 0; ii <  (int) pv_population->size(); ii++)

	fclose(pf_test);

	delete  pv_population;
	pv_population  =  pv_shuffled_population;
}//void  CmGA::v_shuffle_population()



void	CmGA::v_fill_generator
	(
	vector  <CMessyGene  *>  *pvGenerator,  
	int  iIterationNum,  
	int  iCurrentBit,  int  iFilledBits
	)
{
	/*if  (iCurrentBit  <=  i_templ_length)
	{
		if  (iFilledBits  <  iIterationNum)
		{
			pvGenerator->push_back(new  CMessyGene(0, iCurrentBit));
			v_fill_generator(pvGenerator,  iIterationNum, iCurrentBit+1,  iFilledBits+1);

			pvGenerator->at(iFilledBits)->i_gene_val  =  1;
			v_fill_generator(pvGenerator,  iIterationNum, iCurrentBit+1,  iFilledBits+1);

			delete  pvGenerator->at(iFilledBits);
			pvGenerator->erase(pvGenerator->begin()  +  iFilledBits);
			v_fill_generator(pvGenerator,  iIterationNum, iCurrentBit+1,  iFilledBits);
		}//if  (iFilledBits  <  iIterationNum)
		else
		{
			//population insert
			CMessyIndividual  *pc_indiv;
			pc_indiv  =  new  CMessyIndividual(i_templ_length);
			pc_indiv->vSetGenotype(pvGenerator);
			pv_population->push_back(pc_indiv);		
		}//else  if  (iFilledBits  <  iIterationNum)
	}//if  (iCurrentBit  <  i_bit_length)*/
}//void	CmGA::v_fill_generator(int  *piGennerator,  int  iCurrentBit)




void  CmGA::vSetInfectionHistory(CMessyIndividual  *pcInfectionHistory)
{
	if  (pcInfectionHistory  !=  NULL)
	{
		if  (pc_infection_history  ==  NULL)
		{
			pc_infection_history  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
			*pc_infection_history  =  *pcInfectionHistory;
		}//if  (pc_best_found  ==  NULL)
		else
		{
			*pc_infection_history  =  *pcInfectionHistory;
		}//else  if  (pc_best_found  ==  NULL)
	}//if  (pcInfectionHistory  !=  NULL)
	else
	{
		if  (pc_infection_history  !=  NULL)
		{
			delete  pc_infection_history;
			pc_infection_history  =  NULL;		
		}//if  (pc_infection_history  !=  NULL)
	}//else  if  (pcInfectionHistory  !=  NULL)
}//CError  CmGA::eSetInfectionHistory(CMessyIndividual  *pcInfectionHistory)





CError  CmGA::eSetFitness(CFiberSolution  *pcFitnessFunc, bool  bSGAMode)
{
	CError  c_err;


	if  (bSGAMode  == false)
	{
		if  (pc_fitness  !=  NULL)  
		{
			delete  pc_fitness;
			pc_fitness  =  NULL;
		}//if  (pc_fitness  !=  NULL)  
	}//if  (bSGAMode  == false)

	if  (pc_ct_channels  !=  NULL)
	{
		delete  [] pc_ct_channels;
		pc_ct_channels = NULL;
	}//if  (pc_ct_channels  !=  NULL)

	if  (pc_best_virus_channels  !=  NULL)
	{
		delete  [] pc_best_virus_channels;
		pc_best_virus_channels = NULL;
	}//if  (pc_ct_best_virus_channels  !=  NULL)


	//prw-remove
	if  (bSGAMode  == false)
		pc_fitness  =  new  CFiberSolution(pcFitnessFunc);
	else
		pc_fitness  =  pcFitnessFunc;

	pc_ct_channels = new CFiberSolutionChannel[pcFitnessFunc->pcGetNet()->iGetDemandNumber()];
	pc_best_virus_channels = new CFiberSolutionChannel[pcFitnessFunc->pcGetNet()->iGetDemandNumber()];


	return(c_err);
}//CError  CmGA::eSetFitness(int  iFuncType)



CError  CmGA::eVirConfigure
	(
	int  iVirusGen,  int  iVirusPopSize,
	double  dVirProbCut,  double  dVirProbSplice,  double  dVirProbMut,
	double  dVirAddGene, double  dVirRemGene
	)
{
	CError  c_err;


	i_generations  =  iVirusGen;
	i_pop_size  =  iVirusPopSize;
	d_prob_cut  =  dVirProbCut;
	d_prob_splice  =  dVirProbSplice;
	d_prob_mut  =  dVirProbMut;
	d_prob_mut_rem_gene  =  dVirAddGene;
	d_prob_mut_add_gene  =  dVirRemGene;

	return(c_err);
}//CError  CmGA::eVirConfigure


CError  CmGA::eConfigure
	(
	int  iGenMax,  

	int  iPrimGenWithoutPopRed,//population is not reduced
	int  iPrimGenReducePopEvery2nd,  //population reduced every other generation
	int  iPrimGenSizedButNotReduced,  //population is sized at iPopSize, but primordial phase is still going on
	
	int  iPopSize, 
	double  dPopReductionPrimordial,
	double  dProbCut,  double  dProbSplice,
	double  dProbMut,
	double  dProbMutRemGene,
	double  dProbMutAddGene
	)
{
	CError  c_err;


	if  (iGenMax  <  0)
	{
		c_err.vPutError("Generation number below 0");
		return(c_err);	
	}//if  (iGenMax  <  0)

	if  (iPrimGenWithoutPopRed  <  0)
	{
		c_err.vPutError("Generation number for primordial without pop reduction below 0");
		return(c_err);	
	}//if  (iGenMax  <  0)


   	
	if  (iPopSize  <  0)
	{
		c_err.vPutError("Population size below 0");
		return(c_err);	
	}//if  (iGenMax  <  0)

	if  ( (dPopReductionPrimordial  <  0)||(dPopReductionPrimordial  >  1) )
	{
		c_err.vPutError("Population reduction primor exceeds bounds");
		return(c_err);	
	}//if  (iGenMax  <  0)

	if  ( (dProbCut  <  0)||(dProbCut  >  1) )
	{
		c_err.vPutError("Cut probability exceeds bounds");
		return(c_err);	
	}//if  (iGenMax  <  0)

	if  ( (dProbSplice  <  0)||(dProbSplice  >  1) )
	{
		c_err.vPutError("Splice probability exceeds bounds");
		return(c_err);	
	}//if  (iGenMax  <  0)

	if  ( (dProbMut  <  0)||(dProbMut  >  1) )
	{
		c_err.vPutError("Mutation probability exceeds bounds");
		return(c_err);	
	}//if  (iGenMax  <  0)


	if  ( (dProbMutRemGene  <  0)||(dProbMutRemGene  >  1) )
	{
		c_err.vPutError("Rem gene mutation probability exceeds bounds");
		return(c_err);	
	}//if  (iGenMax  <  0)


	if  ( (dProbMutAddGene  <  0)||(dProbMutAddGene  >  1) )
	{
		c_err.vPutError("Add gene mutation probability exceeds bounds");
		return(c_err);	
	}//if  (iGenMax  <  0)



	d_prob_cut  =  dProbCut;
	d_prob_splice  =  dProbSplice;
	d_prob_mut  =  dProbMut;
	d_prob_mut_rem_gene  =  dProbMutRemGene;
	d_prob_mut_add_gene  =  dProbMutAddGene;

	i_generations  =  iGenMax;
	i_pop_size  =  iPopSize;
	d_pop_reduction_primordial  =  dPopReductionPrimordial;

	i_prim_gen_without_pop_red  =  iPrimGenWithoutPopRed;
	i_prim_gen_reduce_pop_every_2nd  =  iPrimGenReducePopEvery2nd;
	i_prim_gen_sized_but_not_reduced  =  iPrimGenSizedButNotReduced;

	
	return(c_err);
}//CError  CmGA::eConfigure


CError  CmGA::eVirGenerateRandomTemplate()
{
	CError  c_err;
	CString  s_buf;

	vector  <CFiberPath  *>  *pv_possible_paths;
	
	for  (int  i_demand = 0;  i_demand < pc_fitness->pcGetNet()->iGetDemandNumber();  i_demand++)
	{
		//::Tools::vShow(i_demand);
		pv_possible_paths = pc_fitness->pcGetNet()->pcGetDemands()[i_demand].pvGetPossiblePaths();

		
		
		s_buf.Format
			(
			"%d->%d    possible paths size: %d  ALL PATHS: %d", 
			pc_fitness->pcGetNet()->pcGetDemands()[i_demand].iGetStartNode(),
			pc_fitness->pcGetNet()->pcGetDemands()[i_demand].iGetEndNode(),
			pv_possible_paths->size(), pc_fitness->pcGetNet()->iGetPathNum()
			);//ASD
		//::Tools::vShow(s_buf);
		if  (DEBUG_SAVE_MESSY)  ::Tools::vRepInFile("zzz_dems.txt",s_buf);//*/

		pc_competetive_template[i_demand].i_demand_number = i_demand;
		pc_competetive_template[i_demand].pc_path = pv_possible_paths->at(pc_random_gen->Next(0, pv_possible_paths->size()));
		pc_competetive_template[i_demand].d_order = pc_random_gen->NextDouble();
	}//for  (int  i_tree = 0;  i_tree < pc_fitness->pc_network->iGetTreeNum();  i_tree++)



	int  i_size;
	for  (int  i_first_node = 0; i_first_node < pc_fitness->pcGetNet()->iGetNodesNum(); i_first_node++)
	{
		for  (int  i_second_node = 0; i_second_node < pc_fitness->pcGetNet()->iGetNodesNum(); i_second_node++)
		{
			if  (i_first_node !=  i_second_node)
			{
				i_size = pc_fitness->pcGetNet()->pvGetPathsBetweenNodes()[i_first_node][i_second_node].size();

				s_buf.Format
					(
					"%d->%d    possible paths size: %d  ", i_first_node,  i_second_node, i_size
					);//ASD

				if  (DEBUG_SAVE_MESSY)  ::Tools::vRepInFile("zzz_paths.txt",s_buf);//*/
			
			}//if  (i_first_node !=  i_second_node)
		}//for  (int  i_second_node = 0; i_second_node < pc_fitness->pcGetNet()->iGetNodesNum(); i_second_node++)	
	}//for  (int  i_first_node = 0; i_first_node < pc_fitness->pcGetNet()->iGetNodesNum(); i_first_node++)



	for  (int  i_path = 0; i_path < pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu()->size(); i_path++)
	{
		s_buf.Format
			(
			"[%d] %d->%d", 
			pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu()->at(i_path)->iGetId(),
			pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu()->at(i_path)->iGetStartNode(),
			pc_fitness->pcGetNet()->pvGetEmployedPathsForTabu()->at(i_path)->iGetEndNode()
			);//ASD

		if  (DEBUG_SAVE_MESSY)  ::Tools::vRepInFile("zzz_tabu_paths.txt",s_buf);//*/
		
	}//for  (int  i_first_node = 0; i_first_node < pc_fitness->pcGetNet()->iGetNodesNum(); i_first_node++)




	//::Tools::vShow("end");

	d_templ_fitness = -1;

	return(c_err);
}//CError  CmGA::eVirGenerateRandomTemplate()



CError  CmGA::eVirGenerateRandomSwap()
{
	CError  c_err;
	
	// select two random orders and swap them
	int first = pc_random_gen->Next(0, pc_fitness->pcGetNet()->iGetDemandNumber());
	int second = pc_random_gen->Next(0, pc_fitness->pcGetNet()->iGetDemandNumber());
	cout << second;
	double ord = pc_competetive_template[first].d_order;
	cout << ord;
	   pc_competetive_template[first].d_order = pc_competetive_template[second].d_order;
	pc_competetive_template[second].d_order = ord;
	d_templ_fitness = -1;

	return(c_err);
}//CError  CmGA::eVirGenerateRandomSwap()



CError  CmGA::eVirGeneratePopulation(int  iVirusInitLength)
{
	CError  c_err;

	CMessyIndividual  *pc_indiv;
	vector  <CMessyGene  *>  v_genotype;


	if  (pv_population  ==  NULL)  
		pv_population  =  new  vector<CMessyIndividual  *>;
	else
	{
		for  (int  ii = 0; ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);
		delete  pv_population;
		pv_population  =  new  vector<CMessyIndividual  *>;
	}//else  if  (pv_population  ==  NULL)  
	

	for  (int  ij = 0; ij < iVirusInitLength; ij++)
		v_genotype.push_back(new  CMessyGene(0, 0, NULL));	
	

	vector  <CFiberPath  *>  *pv_possible_paths;
	
	for  (int  ii = 0; ii < i_pop_size;  ii++)
	{
		for  (int  ij = 0; ij < iVirusInitLength; ij++)
		{
			v_genotype.at(ij)->i_demand_number  =  pc_random_gen->Next(0, pc_fitness->pcGetNet()->iGetDemandNumber());
			pv_possible_paths = pc_fitness->pcGetNet()->pcGetDemands()[v_genotype.at(ij)->i_demand_number].pvGetPossiblePaths();

			v_genotype.at(ij)->pc_path  =  pv_possible_paths->at(pc_random_gen->Next(0, pv_possible_paths->size()));
			v_genotype.at(ij)->d_order =  pc_random_gen->NextDouble();
		}//for  (int  ij = 0; ij < iVirusInitLength; ij++)		

		pc_indiv  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
		pc_indiv->vSetGenotype(&v_genotype);

		//now make the individual randomly shorter
		int  i_genes_to_remove;
		i_genes_to_remove = pc_random_gen->Next(0, iVirusInitLength);
		i_genes_to_remove--;

		while (pc_indiv->pvGetGenotype()->size() > (unsigned int) iVirusInitLength - i_genes_to_remove)
		{
			delete  pc_indiv->pvGetGenotype()->at(0);
			pc_indiv->pvGetGenotype()->erase(pc_indiv->pvGetGenotype()->begin());
		}//while (pc_indiv->pvGetGenotype()->size() > iVirusInitLength - i_genes_to_remove)*/

		
		pv_population->push_back(pc_indiv);	
	}//for  (int  ii = 0; ii < i_pop_size;  ii++)


	for  (int  ij = 0; ij < iVirusInitLength; ij++)
		delete  v_genotype.at(ij);
		
	return(c_err);
}//CError  CmGA::eVirGeneratePopulation(int  iVirusInitLength)




CError  CmGA::eVirGeneratePopulation(CVirusGA  *pcVGA, int  *piPopSize, bool bCleverPaths)
{
	CError  c_err;

	CMessyIndividual  *pc_indiv;
	vector  <CMessyGene  *>  v_genotype;


	if  (pv_population  ==  NULL)  
		pv_population  =  new  vector<CMessyIndividual  *>;
	else
	{
		for  (int  ii = 0; ii < (int) pv_population->size();  ii++)
			delete  pv_population->at(ii);
		delete  pv_population;
		pv_population  =  new  vector<CMessyIndividual  *>;
	}//else  if  (pv_population  ==  NULL)  

	CMessyPattern  *pc_pattern;
	CmGA  *pc_other_ct;	

		
	int  i_effective_pop_size;
	if  (piPopSize  !=  NULL)
		i_effective_pop_size = *piPopSize;
	else 
		i_effective_pop_size = i_pop_size;
	
//	int  i_gene_pos,  i_gene_val;//tools for virus init, when bVirInitDifferentToTemplate ==  true

	
	if  (bCleverPaths  ==  true)
	{
		d_templ_fitness = -1;
		d_templ_fitness = dGetCompTemplateFitness();
		pc_fitness->vSetPathFitness();
	}//if  (bCleverPaths  ==  true)


	int  i_virus_init_length;
	for  (int  ii = 0; ii < i_effective_pop_size;  ii++)
	{
		if  (b_virgin_init  ==  false)
		{
			pc_pattern  =  pcVGA->pc_get_random_pattern();
			pc_other_ct  =  pcVGA->pc_get_parent_tournament_normal_for_pattern_different(pc_pattern,  this);
		}//if  (b_virgin_init  ==  false)
		else
		{
			pc_pattern  =  NULL;
			pc_other_ct  =  NULL;
		}//else  if  (b_virgin_init  ==  false)

		i_virus_init_length  =  pc_random_gen->Next(1, i_templ_length + 1);

		
		if  ( (pc_pattern  !=  NULL)&&(pc_other_ct  !=  NULL)&&(b_virgin_init  ==  false) )
		{

			for  (int  ij = 0; ij < i_virus_init_length; ij++)
			{
				v_genotype.push_back(new  CMessyGene(0, 0, 0));	

				v_genotype.at(ij)->i_demand_number  =  pc_random_gen->Next(0, pc_fitness->pcGetNet()->iGetDemandNumber());
				if  (bCleverPaths  ==  true)
				{
					v_genotype.at(ij)->pc_path  =  pc_fitness->pcGetCleverPath(v_genotype.at(ij)->i_demand_number, pc_random_gen);
				}//if  (bCleverPaths  ==  true)
				else
				{
					v_genotype.at(ij)->pc_path  =  pc_fitness->pcGetRandomPath(v_genotype.at(ij)->i_demand_number, pc_random_gen);
				}//else  if  (bCleverPaths  ==  true)
				
				v_genotype.at(ij)->d_order =  pc_random_gen->NextDouble();
			}//for  (int  ij = 0; ij < i_virus_init_length; ij++)

			
			
			for  (int  ij = 0; ij < (int) pc_pattern->pvGetGenotype()->size(); ij++)
			{
				v_genotype.push_back
					(
					new  CMessyGene
						(
						pc_pattern->pvGetGenotype()->at(ij)->i_demand_number,
						pc_other_ct->pc_competetive_template[pc_pattern->pvGetGenotype()->at(ij)->i_demand_number].d_order,
						pc_other_ct->pc_competetive_template[pc_pattern->pvGetGenotype()->at(ij)->i_demand_number].pc_path
						)
					);

				//v_genotype_for_pure_ct_and_pattern.push_back(v_genotype.at(v_genotype.size() - 1));
			}//for  (int  ij = 0; ij < (int) pc_template->pvGetGenotype()->size(); ij++)

		}//if  (pcTemplate  !=  NULL)
		else
		{
			for  (int  ij = 0; ij < i_virus_init_length; ij++)
			{
				v_genotype.push_back(new  CMessyGene(0, 0, NULL));

				v_genotype.at(ij)->i_demand_number  =  pc_random_gen->Next(0, pc_fitness->pcGetNet()->iGetDemandNumber());
				v_genotype.at(ij)->d_order  =  pc_random_gen->NextDouble();

				if  (bCleverPaths  ==  true)
				{
					v_genotype.at(ij)->pc_path  =  pc_fitness->pcGetCleverPath(v_genotype.at(ij)->i_demand_number, pc_random_gen);
				}//if  (bCleverPaths  ==  true)
				else
				{
					v_genotype.at(ij)->pc_path  =  pc_fitness->pcGetRandomPath(v_genotype.at(ij)->i_demand_number, pc_random_gen);
				}//else  if  (bCleverPaths  ==  true)
			}//for  (int  ij = 0; ij < i_virus_init_length; ij++)
		}//else  if  (pcTemplate  !=  NULL)

		if  (ii  <  (int)  pv_population->size())
			pc_indiv  =  pv_population->at(ii);
		else
		{
			pc_indiv  =  new  CMessyIndividual(pc_fitness, i_bayesian_effect, i_solution_construction_method);
			pv_population->push_back(pc_indiv);
		}//else  if  (ii  <  (int)  pv_population->size())
		
		
		pc_indiv->vSetGenotype(&v_genotype);
		
		for  (int  ij = 0; ij < (int) v_genotype.size(); ij++)
			delete  v_genotype.at(ij);
		v_genotype.clear();

	}//for  (int  ii = 0; ii < i_pop_size;  ii++)


	
	
	return(c_err);
}//CError  CmGA::eVirGeneratePopulation(CVirusGA  *pcVGA)







CError  CmGA::eSetTemplate(CMessyGene  *pcCompetetiveTemplate,  int  iBitLength)
{
	CError  c_err;

	c_err  =  eConfigureTemplate();
	if  (c_err)  return(c_err);

	if  (i_templ_length  !=  iBitLength)
	{
		c_err.vPutError("Template leghts differs");
		return(c_err);	
	}//if  (i_templ_legth  !=  iBitLength)

	for  (int  ii = 0;  ii < i_templ_length;  ii++)
		pc_competetive_template[ii]  =  pcCompetetiveTemplate[ii];


	return(c_err);
}//CError  CmGA::eSetTemplate(int  *piCompetetiveTemplate,  int  iBitLength)



CError  CmGA::eConfigureTemplate()
{
	CError  c_err;

	if  (pc_fitness  ==  NULL)
	{
		c_err.vPutError("No fitness defined - cannot do eConfigureTemplate");
		return(c_err);
	}//if  (pc_fitness  ==  NULL)

	i_templ_length = pc_fitness->pcGetNet()->iGetDemandNumber();

	if  (i_templ_length  <  1)
	{
		c_err.vPutError("Bit length below 1");
		return(c_err);
	}//if  (iBitLength  <  0)


	if  (pc_competetive_template  !=  NULL)
	{
		delete  [] pc_competetive_template;
		delete  [] pc_ordered_template;
	}//if  (pi_competetive_template  !=  NULL)

	pc_competetive_template  =  new  CMessyGene[i_templ_length];
	pc_ordered_template  =  new  CMessyGene[i_templ_length];

	return(c_err);
};//CError  CmGA::eConfigureTemplate(int  iBitLength)



int i_comp_channels (const void * a, const void * b)
{
	if  ( ((CChannelSortPair*)a)->i_channel_start_slot == ((CChannelSortPair*)b)->i_channel_start_slot)  return(0);

	if  ( ((CChannelSortPair*)a)->i_channel_start_slot < ( (CChannelSortPair*)b)->i_channel_start_slot)  
		return(-1);
	else
		return(1);
}//int i_comp_genes (const void * a, const void * b)


CError  CmGA::eSwitchToFull(double  *pdAfterSwitch,  double *pdRestoring)
{
	CError  c_err;

	double  d_fitness_after_switch;
	eMakePreciseGentoypeFromFluent();
	i_solution_construction_method = SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION;

	d_templ_fitness = -1;
	d_fitness_after_switch = dGetCompTemplateFitness();


	*pdAfterSwitch = d_fitness_after_switch;
	*pdRestoring = -1;

	if  (pc_competetive_template_before_switch_from_full_to_simple !=  NULL)
	{
		if  (d_templ_fittness_full_before_switch_to_simple > d_fitness_after_switch)
		{
			for  (int  ii = 0;  ii < i_templ_length; ii++)
				pc_competetive_template[ii]  =  pc_competetive_template_before_switch_from_full_to_simple[ii];

			d_templ_fitness = -1;
			*pdRestoring = dGetCompTemplateFitness();
		}//if  (d_templ_fittness_full_before_switch_to_simple > d_fitness_after_switch)
	}//if  (pc_competetive_template_before_switch_from_full_to_simple !=  NULL)
	
	return(c_err);
}//CError  CmGA::eSwitchToFull()



CError  CmGA::eSwitchToSimple()
{
	CError  c_err;

	if  (i_solution_construction_method  ==  SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION)
	{
		if  (pc_competetive_template_before_switch_from_full_to_simple  ==  NULL)  pc_competetive_template_before_switch_from_full_to_simple  =  new CMessyGene[i_templ_length];
		for  (int  ii = 0;  ii < i_templ_length; ii++)
			pc_competetive_template_before_switch_from_full_to_simple[ii]  =  pc_competetive_template[ii];

		d_templ_fittness_full_before_switch_to_simple  =  dGetCompTemplateFitness();
	}//if  (i_solution_construction_method  ==  SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION)


	i_solution_construction_method = SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION;

	d_templ_fitness = -1;
	dGetCompTemplateFitness();

	return(c_err);
}//CError  CmGA::eSwitchToSimple()



CError  CmGA::eMakePreciseGentoypeFromFluent()
{
	CError  c_err;


	CMessyIndividual  c_buf(pc_fitness, i_bayesian_effect, i_solution_construction_method);
	int  i_solution_repaired = 0;
	d_templ_fitness  =  c_buf.dComputeFitness(pc_competetive_template, &i_solution_repaired, true);	

	if  (i_solution_repaired  !=  0)
	{
		//::MessageBox(NULL, "1", "1", MB_OK);
		//CString  s_buf;
		//s_buf.Format("fit: %.2lf  repaired: %d", 1.0 / d_templ_fitness, i_solution_repaired);
		//vShow(s_buf);
		c_buf.vInfect(pc_competetive_template);
		d_templ_fitness = -1;
		dGetCompTemplateFitness();

		//s_buf.Format("fit repaired: %.2lf", 1.0 / d_templ_fitness);
		//vShow(s_buf);
	}//if  (b_solution_repaired  ==  true)

	c_buf.vCopyOrderedFenotype(pc_ordered_template);

	i_solution_construction_method = SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION;


	return(c_err);
}//CError  CmGA::eMakePreciseGentoypeFromFluent()


CError  CmGA::eMakeGentoypeFromSolution(CFiberSolution *pcSolution)
{
	CError  c_err;

	CChannelSortPair  *pc_sort_table;

	pc_sort_table = new CChannelSortPair[pcSolution->pcGetNet()->iGetDemandNumber()];

	for  (int  ii = 0; ii < pcSolution->pcGetNet()->iGetDemandNumber(); ii++)
	{
		pc_sort_table[ii].i_channel_offset = ii;
		pc_sort_table[ii].i_channel_start_slot = pcSolution->pcGetChannels()[ii].iFirstSlotNumber;
	}//for  (int  ii = 0; ii < pcSolution->pcGetNet()->iGetDemandNumber(); ii++)


	qsort(pc_sort_table, pcSolution->pcGetNet()->iGetDemandNumber(), sizeof(CChannelSortPair),  i_comp_channels);

	/*FILE  *pf_test;
	pf_test = fopen("atest.txt", "w+");

	for  (int  ii = 0; ii < pcSolution->pcGetNet()->iGetDemandNumber(); ii++)
	{
		fprintf(pf_test, "%d  %d\n", pc_sort_table[ii].i_channel_start_slot, pc_sort_table[ii].i_channel_offset);
	}//for  (int  ii = 0; ii < pcSolution->pcGetNet()->iGetDemandNumber(); ii++)

	fclose(pf_test);

	::MessageBox(NULL, "", "", MB_OK);*/

	int  i_offset;
	for  (int  ii = 0; ii < pcSolution->pcGetNet()->iGetDemandNumber(); ii++)
	{
		i_offset = pc_sort_table[ii].i_channel_offset;
		pc_competetive_template[i_offset].pc_path = pcSolution->pcGetChannels()[i_offset].pcPath;
		pc_competetive_template[i_offset].d_order = ii;
		pc_competetive_template[i_offset].d_order = pc_competetive_template[i_offset].d_order / pcSolution->pcGetNet()->iGetDemandNumber();
	}//for  (int  ii = 0; ii < pcSolution->pcGetNet()->iGetDemandNumber(); ii++)

	d_templ_fitness = -1;

	/*FILE  *pf_test;
	pf_test = fopen("a_CT_test.txt", "w+");

	for  (int  ii = 0; ii < pcSolution->pcGetNet()->iGetDemandNumber(); ii++)
	{
		fprintf(pf_test, "%d  %.2lf %d\n", ii, pc_competetive_template[ii].d_order, pc_competetive_template[ii].pcGetPath()->iGetId());
	}//for  (int  ii = 0; ii < pcSolution->pcGetNet()->iGetDemandNumber(); ii++)

	fclose(pf_test);

	::MessageBox(NULL, "", "", MB_OK);//*/
	

	return(c_err);
}//CError  CmGA::eMakeGentoypeFromSolution(CFiberSolution *pcSolution)



//---------------------------class  CMessyIndividual--------------------------------------

CMessyIndividual::CMessyIndividual(CFiberSolution  *pcFitness, int  iBayesianEffect, int iSolutionConstructionMethod)
{
	i_global_feno_len = 0;
	b_fenotype_actual  =  false;

	pc_fenotype = NULL;
	pc_ordered_fenotype = NULL;

	d_fitness  =  0;

	i_bayesian_effect =  iBayesianEffect;
	i_solution_construction_method  =  iSolutionConstructionMethod;

	vSetFitness(pcFitness);
}//CMessyIndividual::CMessyIndividual()


CMessyIndividual::~CMessyIndividual()
{
	vFlushGentoype();

	if  (pc_fenotype  !=  NULL)  delete  []  pc_fenotype;
	if  (pc_ordered_fenotype  !=  NULL)  delete  []  pc_ordered_fenotype;
};//CMessyIndividual::~CMessyIndividual()


void  CMessyIndividual::vSetGenotype(vector  <CMessyGene  *>  *pvNewGenotype)
{
	vFlushGentoype();
	for  (int  ii = 0;  ii < (int) pvNewGenotype->size();  ii++)
		v_genotype.push_back(new  CMessyGene(pvNewGenotype->at(ii)));
}//CMessyIndividual::vSetGenotype(vector  <CMessyGene  *>  *pvNewGenotype)


void  CMessyIndividual::vAddGenotype(vector  <CMessyGene  *>  *pvNewGenotype)
{
	b_fenotype_actual  =  false;

	for  (int  ii = 0;  ii < (int) pvNewGenotype->size();  ii++)
		v_genotype.push_back(new  CMessyGene(pvNewGenotype->at(ii)));

}//CMessyIndividual::vSetGenotype(vector  <CMessyGene  *>  *pvNewGenotype)



	
void  CMessyIndividual::vFlushGentoype()
{
	b_fenotype_actual  =  false;

	for  (int  ii = 0;  ii < (int) v_genotype.size();  ii++)
		delete  v_genotype.at(ii);
	v_genotype.clear();
}//CMessyIndividual::vFlushGentoype()



void  CMessyIndividual::vSplice(CMessyIndividual  *pcSplicedIndividual)
{
	for  (int  ii = 0; ii < (int) pcSplicedIndividual->v_genotype.size();  ii++)
		v_genotype.push_back(pcSplicedIndividual->v_genotype.at(ii));

	b_fenotype_actual  =  false;
	pcSplicedIndividual->b_fenotype_actual = false;

	//now removing spliced pointers from the source...
	pcSplicedIndividual->v_genotype.clear();
}//void  CMessyIndividual::vSplice(CMessyIndividual  *pcSplicedIndividual)


CError  CMessyIndividual::eSplice //this individual will be a splice result...
	(
	double  dSpliceProb,  
	gcroot<Random*> pcRandomGen,  
	CMessyIndividual  *pcSplicedIndividual,
	bool  *pbSpliced
	)
{
	CError  c_err;
	
	*pbSpliced  =  false;

	if  (pcRandomGen->NextDouble()  <  dSpliceProb)
	{
		*pbSpliced  =  true;

		vSplice(pcSplicedIndividual);
	}//if  (pcRandomGen->NextDouble()  <  dSpliceProb)*/


	return(c_err);
}//CError  CMessyIndividual::eSplice //this individual will be a splice result...




CError  CMessyIndividual::eCut
	(
	double  dCutProb,  
	gcroot<Random*> pcRandomGen,
	CMessyIndividual  *pcPart1,  CMessyIndividual  *pcPart2,
	int  *piReturnedPartsNumber
	)
{
	CError  c_err;

	double  d_effective_cut_prob;
	d_effective_cut_prob  =  dCutProb;
	d_effective_cut_prob  *=  v_genotype.size();

	//*piReturnedPartsNumber  =  1;
	//*pcPart1  =  *this;	


	if  ( (pcRandomGen->NextDouble()  <  d_effective_cut_prob)&&(v_genotype.size()  >  1) )
	{
		*piReturnedPartsNumber  =  2;

		vector  <CMessyGene  *>  v_genotype_1,  v_genotype_2;
		int  i_cut_point;

		i_cut_point  =  pcRandomGen->Next(0, v_genotype.size()-1);  // the numbers returned: 1,2,3,..., size()-1
		i_cut_point++;

		for  (int  ii = 0;  ii <  i_cut_point;  ii++)
			v_genotype_1.push_back(v_genotype.at(ii));

		for  (int  ii = i_cut_point;  ii <  (int) v_genotype.size();  ii++)
			v_genotype_2.push_back(v_genotype.at(ii));

		pcPart1->vSetGenotype(&v_genotype_1);
		pcPart2->vSetGenotype(&v_genotype_2);

		pcPart1->b_fenotype_actual = false;
		pcPart2->b_fenotype_actual = false;
	}//if  (pcRandomGen->NextDouble()  <  dCutProb)
	else
	{
		*piReturnedPartsNumber  =  1;
		*pcPart1  =  *this;
	}//else  if  (pcRandomGen->NextDouble()  <  dCutProb)

	return(c_err);
}//CError  CMessyIndividual::eCut(double  dCutProb,  CMessyIndividual  **pcPart1,  CMessyIndividual  **pcPart2)




void  CMessyIndividual::vMutateRemGene(double  dProbMutRemGene,  gcroot<Random*> pcRandomGen)
{
	if  ((int) v_genotype.size()  <=  0)  return;

	if  (pcRandomGen->NextDouble()  <  dProbMutRemGene)
	{
		vRemoveRandomGene(pcRandomGen);
	}//if  (pcRandomGen->NextDouble()  <  dProbMutRemGene)
}//void  CMessyIndividual::vMutateRemGene(double  dProbMutRemGene,  gcroot<Random*> pcRandomGen)




void  CMessyIndividual::vRemoveRandomGene(gcroot<Random*> pcRandomGen)
{
	int  i_removed_gene;

	i_removed_gene  =  pcRandomGen->Next(0, v_genotype.size());

	delete  v_genotype.at(i_removed_gene);
	v_genotype.erase(v_genotype.begin()  +  i_removed_gene);

	b_fenotype_actual  =  false;
}//void  CMessyIndividual::vRemoveRandomGene()



void  CMessyIndividual::vMutate(double  dProbMut,  gcroot<Random*> pcRandomGen)
{
	if  ((int) v_genotype.size()  <=  0)  return;

	if  (pcRandomGen->NextDouble()  <  dProbMut)
	{
		int  i_mutated_gene;

		i_mutated_gene  =  pcRandomGen->Next(0, v_genotype.size());

		double  d_rand = pcRandomGen->NextDouble();

		
		if  (d_rand < 0.25)
		{
			int  i_new_demand_number;
			i_new_demand_number = pcRandomGen->Next(0, pc_fitness->pcGetNet()->iGetDemandNumber());

			v_genotype.at(i_mutated_gene)->i_demand_number = i_new_demand_number;
			v_genotype.at(i_mutated_gene)->pc_path = pc_fitness->pcGetCleverPath(i_new_demand_number, pcRandomGen);
				
		}//if  (pcRandomGen->NextDouble() < 0.33)
		else
		{
			d_rand = pcRandomGen->NextDouble();
			if  (d_rand < 0.5)
			{
				v_genotype.at(i_mutated_gene)->pc_path = pc_fitness->pcGetCleverPath(v_genotype.at(i_mutated_gene)->i_demand_number, pcRandomGen);
			}//if  (pcRandomGen->NextDouble() < 0.33)
			else
			{
				v_genotype.at(i_mutated_gene)->d_order = pcRandomGen->NextDouble();
			}//else  if  (pcRandomGen->NextDouble() < 0.33)
		
		}//else  if  (pcRandomGen->NextDouble() < 0.33)
				
		b_fenotype_actual  =  false;			
	}//if  (pcRandomGen->NextDouble()  <  dProbMutRemGene)

}//void  CMessyIndividual::vMutate(double  dProbMut,  gcroot<Random*> pcRandomGen)




void  CMessyIndividual::vMutateAddGene(double  dProbMutAddGene,  gcroot<Random*> pcRandomGen,  int  iTemplLength)
{
	if  (pcRandomGen->NextDouble()  <  dProbMutAddGene)
	{
		int  i_new_demand_number;
		vector  <CFiberPath  *>  *pv_possible_paths;
		
		i_new_demand_number = pcRandomGen->Next(0, pc_fitness->pcGetNet()->iGetDemandNumber());
		pc_fitness->pcGetNet()->pcGetDemands()[i_new_demand_number].pvGetPossiblePaths();

		v_genotype.push_back
			(
			new  CMessyGene
				(
				i_new_demand_number,
				pcRandomGen->NextDouble(),
				pc_fitness->pcGetCleverPath(i_new_demand_number, pcRandomGen)
				)
			);
		
				
		b_fenotype_actual  =  false;
	}//if  (pcRandomGen->NextDouble()  <  dProbMutRemGene)
}//void  CMessyIndividual::vMutateAddGene(double  dProbMutAddGene,  gcroot<Random*> pcRandomGen)



void  CMessyIndividual::operator=(CMessyIndividual  &cOther)
{
	vSetGenotype(&cOther.v_genotype);
	pc_fitness = cOther.pc_fitness;

	d_fitness = cOther.d_fitness;
	b_fenotype_actual = false;
}//void  CMessyIndividual::operator=(CMessyIndividual  *pcOther)



bool  CMessyIndividual::bCompareFenotypes(CMessyIndividual  *pcOther)
{

	v_create_fenotype();
	/*if  (bGlobalDebug)
	{
		::MessageBox(NULL, "enabling debug", "enabling debug", MB_OK);
		bGlobalDebug2 = true;
	}//if  (bGlobalDebug)*/
	pcOther->v_create_fenotype();
	//bGlobalDebug2 = false;


	int  i_templ_length;
	i_templ_length = pc_fitness->pcGetNet()->iGetDemandNumber();

	for  (int  ii = 0; ii < i_templ_length;  ii++)
	{
		if  ( (pc_fenotype[ii]  !=  NULL)&&(pcOther->pc_fenotype[ii] != NULL) )
		{
			if  (*(pc_fenotype[ii])  !=  *(pcOther->pc_fenotype[ii]))  return(false);
		}//if  ( (pc_fenotype[ii]  !=  NULL)&&(pcOther->pc_fenotype[ii] != NULL) )
		else
		{
			if  (pc_fenotype[ii]  !=  pcOther->pc_fenotype[ii])  return(false);
		}//else  if  ( (pc_fenotype[ii]  !=  NULL)&&(pcOther->pc_fenotype[ii] != NULL) )
		
	}//for  (int  ii = 0; ii < i_templ_length;  ii++)
		

	return(true);
}//bool  CMessyIndividual::bCompareFenotypes(CMessyIndividual  *pcOther)





CError  CMessyIndividual::eReport(FILE  *pfReport,CMessyGene  *pcCompetetiveTemplate)
{
	CError  c_err;


	v_create_fenotype();
	v_construct_solution_order_queue(pcCompetetiveTemplate);
	pc_fitness->eSaveSolution(pfReport);

	return(c_err);
}//CError  CMessyIndividual::eReport(FILE  *pfReport)




CError  CMessyIndividual::eReportFenotype
	(
	FILE  *pfReport,
	CMessyGene  *pcCompetetiveTemplate
	)
{
	CError  c_err;

	v_create_fenotype();

	int  i_feno_len = pc_fitness->pcGetNet()->iGetDemandNumber();
	

	for  (int  ii = 0; ii < i_feno_len; ii++)
	{
		pc_fenotype[ii]->vReportGene(pfReport);	
		fprintf(pfReport, "\n");
	}//for  (int  ii = 0; ii < i_feno_len; ii++)


	return(c_err);
}//CError  CMessyIndividual::eReportFenotype





void  CMessyIndividual::vSetFitness(CFiberSolution  *pcFitness)
{
	b_fenotype_actual = false;

	pc_fitness = pcFitness;

	if  (pc_fenotype  !=  NULL)  delete  []  pc_fenotype;
	pc_fenotype = new CMessyGene*[pc_fitness->pcGetNet()->iGetDemandNumber()];


	if  (pc_ordered_fenotype  !=  NULL)  delete  []  pc_ordered_fenotype;
	pc_ordered_fenotype = new CMessyGene*[pc_fitness->pcGetNet()->iGetDemandNumber()];

}//void  CMessyIndividual::vSetFitness(CSolution  *pcFitness)







void  CMessyIndividual::v_create_fenotype()
{
	if  (b_fenotype_actual  ==  true)  return;

	int  i_feno_len = pc_fitness->pcGetNet()->iGetDemandNumber();

	CError  c_err;
	CString  s_buf;
	int  ii;
	try
	{
		//first we flash fenotype create tool
		for  (ii = 0; ii < i_feno_len; ii++)
			pc_fenotype[ii] = NULL;
	}//try
	catch (Exception  *p_ex)
	{
		s_buf = p_ex->ToString();
		s_buf = s_buf + "\n CMessyIndividual::v_create_fenotype   for pc_fenotype[ii] = NULL;";
		CString  s_ii;
		s_ii.Format("/nii = %d", ii);
		s_buf = s_buf + s_ii;
		c_err.vPutError(s_buf);
	}//catch (Exception  p_ex)


	if  (i_feno_len  !=  i_global_feno_len)
	{
		if  (i_global_feno_len  !=  0)
		{
			CString  s_buf;
			s_buf.Format("fenotype length error:  flen:%d fnewLen:%d", i_global_feno_len, i_feno_len);
			::MessageBox(NULL, s_buf, s_buf, MB_OK);
		}//if  (i_global_feno_len  !=  0)
		else
			i_global_feno_len = i_feno_len;	
	}//if  (i_feno_len  !=  i_global_feno_len)
	
	
	int  i_gene_feno_offset;
	try
	{
		for  (ii = 0;  ii < (int) v_genotype.size();  ii++)
		{
			pc_fenotype[v_genotype.at(ii)->i_demand_number] = v_genotype.at(ii);
		}//for  (int  ii = 0;  ii < (int) v_genotype.size();  ii++)
	}//try
	catch (Exception  *p_ex)
	{
		s_buf = p_ex->ToString();
		s_buf = s_buf + "\n CMessyIndividual::v_create_fenotype   for pc_fenotype[v_genotype.at(ii)->i_demand_number] = v_genotype.at(ii);";
		CString  s_ii;
		s_ii.Format("/nii = %d, demand_number: %d   feno len: %d", ii, v_genotype.at(ii)->i_demand_number, i_feno_len);
		s_buf = s_buf + s_ii;
		c_err.vPutError(s_buf);
	}//catch (Exception  p_ex)


	b_fenotype_actual  =  true;
}//void  CMessyIndividual::v_create_fenotype()




int  CMessyIndividual::iGetFenotypeLength()
{
	v_create_fenotype();
	int  i_result = 0;
	int  i_feno_len = pc_fitness->pcGetNet()->iGetDemandNumber();

	for  (int  ii = 0; ii < i_feno_len; ii++)
		if  (pc_fenotype[ii] != NULL)  i_result++;
	
	return(i_result);
}//int  CMessyIndividual::iGetFenotypeLength()




void  CMessyIndividual::vRemoveUnimportantGenes(CMessyGene  *pcCompetetiveTemplate)
{
	v_create_fenotype();

	for  (int  ii = 0;  ii < (int) v_genotype.size();  ii++)
	{
		if  (
			*(pc_fenotype[v_genotype.at(ii)->i_demand_number])  ==  pcCompetetiveTemplate[v_genotype.at(ii)->i_demand_number]
			)
		{
			delete  v_genotype.at(ii);
			v_genotype.erase(v_genotype.begin()  +  ii);
			ii--;
			b_fenotype_actual  =  false;
		}//if  (
	}//for  (int  ii = 0;  ii < (int) v_genotype.size();  ii++)

}//void  CMessyIndividual::vRemoveUnimportantGenes(int  *piCompetetiveTemplate)



void  CMessyIndividual::vInfect(CMessyGene  *pcCompetetiveTemplate)
{
	b_fenotype_actual = false;
	v_create_fenotype();
	v_construct_solution_order_queue(pcCompetetiveTemplate);

	int  i_test = 0;
	//::Tools::vShow("void  CMessyIndividual::vInfect(CMessyGene  *pcCompetetiveTemplate)");
	//we infect with SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION because it gives both - weights and suggested paths
	v_construct_solution(pc_ordered_fenotype, SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION, &i_test, pcCompetetiveTemplate);	//if the pcCompetetiveTemplate is given then it is infected
}//void  CMessyIndividual::vInfect(CMessyGene  *pcCompetetiveTemplate)



double  CMessyIndividual::dComputeFitness(CMessyGene  *pcCompetetiveTemplate, int  *piSolutionRepaired, bool bSelfUpgradeToPreciseEncoding)
{
	if  (b_fenotype_actual  ==  true)
	{
		return(d_fitness);
	}//if  (b_fenotype_actual  ==  true)

	
	if  (i_bayesian_effect == BAYESIAN_EFECT_NONE)
	{
		d_compute_fitness(pcCompetetiveTemplate, piSolutionRepaired, bSelfUpgradeToPreciseEncoding);
	}//if  (i_bayesian_effect == BAYESIAN_EFECT_NONE)

	
	if  (i_bayesian_effect == BAYESIAN_EFECT_BRUTAL)
	{
		double  d_best_fitness;
		d_best_fitness = d_compute_fitness(pcCompetetiveTemplate, piSolutionRepaired);

		int  i_feno_len = pc_fitness->pcGetNet()->iGetDemandNumber();
		CMessyGene  **pc_bayesian_fenotype;
		pc_bayesian_fenotype = new CMessyGene*[i_feno_len];


		for  (int  ii = 0; ii < i_feno_len; ii++)
			pc_bayesian_fenotype[ii] = new CMessyGene(pc_ordered_fenotype[ii]);
	
		bool  b_const;
		double  d_fitness_buf;
		CFiberDemand  *pc_demands;
		pc_demands = pc_fitness->pcGetNet()->pcGetDemands();
		CFiberPath  *pc_best_path, *pc_current_path;

		for  (int  ii = 0; ii < i_feno_len; ii++)
		{
			pc_best_path = pc_ordered_fenotype[ii]->pcGetPath();
			for  (int  i_path = 0;  i_path < pc_demands[pc_bayesian_fenotype[ii]->iGetDemNumber()].pvGetPossiblePaths()->size(); i_path++)
			{
				pc_current_path = pc_demands[pc_bayesian_fenotype[ii]->iGetDemNumber()].pvGetPossiblePaths()->at(i_path);
				if  (pc_current_path  !=  pc_ordered_fenotype[ii]->pcGetPath())
				{
					pc_bayesian_fenotype[ii]->pc_path = pc_current_path;

					//compute fitness
					v_construct_solution(pc_bayesian_fenotype, SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION);
					d_fitness_buf = pc_fitness->dCompCost(&b_const);
					d_fitness_buf = 1 / d_fitness_buf;

					if  (d_fitness_buf  >  d_best_fitness)
					{
						d_best_fitness = d_fitness_buf;
						pc_best_path  =  pc_current_path;					
					}//if  (d_fitness_buf  >  d_best_fitness)
				
				}//if  (pc_current_path  !=  pc_ordered_fenotype[ii]->pcGetPath())			
			}//for  (int  i_path = 0;  i_path < pc_demands[pc_bayesian_fenotype[ii]->iGetDemNumber()].v_possible_paths.size(); i_path++)

			pc_bayesian_fenotype[ii]->pc_path = pc_best_path;
		}//for  (int  ii = 0; ii < i_feno_len; ii++)

		d_fitness = d_best_fitness;

		for  (int  ii = 0; ii < i_feno_len; ii++)
			delete  pc_bayesian_fenotype[ii];
		delete  pc_bayesian_fenotype;

	}//if  (i_bayesian_effect == BAYESIAN_EFECT_BRUTAL)

	
	if  (i_bayesian_effect == BAYESIAN_EFECT_CLEVER)
	{
	
	}//if  (i_bayesian_effect == BAYESIAN_EFECT_CLEVER)

	return(d_fitness);
}//double  CMessyIndividual::dComputeFitness(CMessyGene  *pcCompetetiveTemplate, int  *piSolutionRepaired)



double  CMessyIndividual::d_compute_fitness(CMessyGene  *pcCompetetiveTemplate, int  *piSolutionRepaired, bool bSelfUpgradeToPreciseEncodingS)
{
	if  (b_fenotype_actual  ==  true)
	{
		return(d_fitness);
	}//if  (b_fenotype_actual  ==  true)


	//if none of the structures is actual...
	v_create_fenotype();
	if  ( (piSolutionRepaired !=  NULL)&&(*piSolutionRepaired  ==  2) )  return(0);
	v_construct_solution_order_queue(pcCompetetiveTemplate);
	if  ( (piSolutionRepaired !=  NULL)&&(*piSolutionRepaired  ==  3) )  return(0);

	//CTimeCounter  c_time_counter;
	//c_time_counter.vSetStartNow();

	if  (i_solution_construction_method  ==  SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION_BOTH)
	{
		double  d_fitness_simple, d_fitness_full;

		//SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION
		if  (bSelfUpgradeToPreciseEncodingS == true)
			v_construct_solution(pc_ordered_fenotype, SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION, piSolutionRepaired, NULL, pcCompetetiveTemplate);
		else
			v_construct_solution(pc_ordered_fenotype, SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION, piSolutionRepaired);

		if  ( (piSolutionRepaired !=  NULL)&&(*piSolutionRepaired  ==  4) )  return(0);

		bool  b_const;
		d_fitness_simple = pc_fitness->dCompCost(&b_const);
		double d_one = 1;
		d_fitness_simple = d_one / d_fitness_simple;



		//SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION
		if  (bSelfUpgradeToPreciseEncodingS == true)
			v_construct_solution(pc_ordered_fenotype, SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION, piSolutionRepaired, NULL, pcCompetetiveTemplate);
		else
			v_construct_solution(pc_ordered_fenotype, SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION, piSolutionRepaired);

		if  ( (piSolutionRepaired !=  NULL)&&(*piSolutionRepaired  ==  4) )  return(0);

		d_fitness_full = pc_fitness->dCompCost(&b_const);
		d_fitness_full = d_one / d_fitness_full;

		if  (d_fitness_full  >  d_fitness_simple)  
			d_fitness = d_fitness_full;
		else
		{
			pc_fitness->vIncFullBetterThanSimple();
			d_fitness = d_fitness_simple;
		}//else  if  (d_fitness_full  >  d_fitness_simple)  

		return(d_fitness);		
	}//if  (i_solution_construction_method  ==  SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION_BOTH)
	else
	{
		if  (bSelfUpgradeToPreciseEncodingS == true)
			v_construct_solution(pc_ordered_fenotype, i_solution_construction_method, piSolutionRepaired, NULL, pcCompetetiveTemplate);
		else
			v_construct_solution(pc_ordered_fenotype, i_solution_construction_method, piSolutionRepaired);

		if  ( (piSolutionRepaired !=  NULL)&&(*piSolutionRepaired  ==  4) )  return(0);

		bool  b_const;
		d_fitness = pc_fitness->dCompCost(&b_const);
		double d_one = 1;
		d_fitness = d_one / d_fitness;

		/*vShow("3. reconstructed");

		pc_fitness->eSaveSolution("!sol_reconstruted.txt");
		vShow("4. reconstructed saved!");//*/

		return(d_fitness);
	}//else  if  (i_solution_construction_method  ==  SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION_BOTH)

	//double  d_time_passed;
	//c_time_counter.bGetTimePassed(&d_time_passed);
	//pc_fitness->vAddConstructionTime(d_time_passed);



	//pc_fitness->eApplySolution();
	//pc_fitness->eSaveSolution("zz_test_save.txt");
	/*vShow("1. constructed!");
	pc_fitness->eSaveSolution("!sol_constructed.txt");
	vShow("2. saved");//*/	
}//double  CMessyIndividual::dComputeFitness(int  *piCompetetiveTemplate,  CDecFunc  *pcFitness)



int i_comp_genes (const void * a, const void * b)
{
	if  ( (*((CMessyGene**)a))->dGetOrder() == ( (*(CMessyGene**)b))->dGetOrder())  return(0);

	if  ( (*((CMessyGene**)a))->dGetOrder() < ( (*(CMessyGene**)b))->dGetOrder())  
		return(-1);
	else
		return(1);
}//int i_comp_genes (const void * a, const void * b)




void  CMessyIndividual::vCopyOrderedFenotype(CMessyGene  *pcDest)
{
	int  i_feno_len = pc_fitness->pcGetNet()->iGetDemandNumber();

	for  (int  ii = 0; ii < i_feno_len; ii++)
		pcDest[ii]  =  pc_ordered_fenotype[ii];
}//void  CMessyIndividual::vCopyOrderedFenotype(CMessyGene  *pcDest)



void  CMessyIndividual::v_construct_solution_order_queue(CMessyGene  *pcCompetetiveTemplate)
{
	int  i_feno_len = pc_fitness->pcGetNet()->iGetDemandNumber();

	for  (int  ii = 0; ii < i_feno_len; ii++)
	{
		if  (pc_fenotype[ii] == NULL)
			pc_fenotype[ii] = &(pcCompetetiveTemplate[ii]);

		pc_ordered_fenotype[ii] = pc_fenotype[ii];
	}//for  (int  ii = 0; ii < i_feno_len; ii++)


	qsort(pc_ordered_fenotype, i_feno_len, sizeof(CMessyGene *),  i_comp_genes);

	/*//test
	for  (int  ii = 0; ii < i_feno_len; ii++)
	{
		CString  s_buf;

		s_buf.Format("%.8lf  tree: %d    node: %d", pc_ordered_fenotype[ii]->d_order, pc_ordered_fenotype[ii]->i_tree_number, pc_ordered_fenotype[ii]->i_node_number);

		::MessageBox(NULL, s_buf, s_buf, MB_OK);
	
	}//for  (int  ii = 0; ii < i_feno_len; ii++)*/
}//void  CMessyIndividual::v_construct_solution_order_queue(CMessyGene  *pcCompetetiveTemplate)




void  CMessyIndividual::v_construct_solution_with_fluent_optimization_simple(CMessyGene  **pcOrderedFenotype, int  *piSolutionRepaired, CMessyGene  *pcCompetetiveTemplateToInfect, CMessyGene  *pcGenotypeToSelfPrecisionUpgrade)
{
	CError  c_err;
	CString  s_buf;

	c_err = pc_fitness->pcGetNet()->eResetNet();
	if (c_err)
	{
		::MessageBox(NULL, "error while reseting net", "error", MB_OK);
		return;
	}//if (c_err)

	if  ( (piSolutionRepaired !=  NULL)&&(*piSolutionRepaired  ==  10) )  return;


	CFiberPath  *pc_effective_path;
	int  i_effective_path_start_slot, i_effective_path_slots;
	int  i_effective_path_backup_start_slot, i_effective_backup_path_slots;
	int  i_effective_path_pessimistic_start_slot, i_effective_pessimistic_path_slots;

	int  i_feno_len;
	CFiberDemand  *pc_demands;
	CFiberPath  *pc_checked_path;
	int  i_checked_path_start_slot, i_checked_path_slots;
	int  i_checked_path_backup_start_slot, i_checked_path_backup_slots;
	int  i_checked_path_pessimistic_start_slot, i_checked_path_pessimistic_slots;


	pc_demands = pc_fitness->pcGetNet()->pcGetDemands();
	i_feno_len = pc_fitness->pcGetNet()->iGetDemandNumber();	
	for  (int  ii = 0;  ii < i_feno_len; ii++)
	{
		//pc_effective_path = pcOrderedFenotype[ii]->pc_path;

		
		if  (pcCompetetiveTemplateToInfect  !=  NULL)
		{
			pcCompetetiveTemplateToInfect[pcOrderedFenotype[ii]->i_demand_number].d_order = pcOrderedFenotype[ii]->d_order;
			pcCompetetiveTemplateToInfect[pcOrderedFenotype[ii]->i_demand_number].i_demand_number = pcOrderedFenotype[ii]->i_demand_number;
			pcCompetetiveTemplateToInfect[pcOrderedFenotype[ii]->i_demand_number].pc_path = pcOrderedFenotype[ii]->pc_path;
		}//if  (pcCompetetiveTemplateToInfect  !=  NULL)

		if  (pcGenotypeToSelfPrecisionUpgrade  !=  NULL)
		{
			pcGenotypeToSelfPrecisionUpgrade[pcOrderedFenotype[ii]->i_demand_number].d_order = pcOrderedFenotype[ii]->d_order;
			pcGenotypeToSelfPrecisionUpgrade[pcOrderedFenotype[ii]->i_demand_number].i_demand_number = pcOrderedFenotype[ii]->i_demand_number;
			pcGenotypeToSelfPrecisionUpgrade[pcOrderedFenotype[ii]->i_demand_number].pc_path = pcOrderedFenotype[ii]->pc_path;		
		}//if  (pcGenotypeToSelfPrecisionUpgrade  !=  NULL)
		


		bool  b_new_best;
		pc_effective_path = NULL;
		for  (int  i_path = 0; i_path < pc_demands[pcOrderedFenotype[ii]->i_demand_number].pvGetPossiblePaths()->size(); i_path++)
		//for  (int  i_path = 0; i_path < 1; i_path++)
		{
			pc_checked_path = pc_demands[pcOrderedFenotype[ii]->i_demand_number].pvGetPossiblePaths()->at(i_path);
			c_err = pc_fitness->eFindChanel(pcOrderedFenotype[ii]->i_demand_number, pc_checked_path, &i_checked_path_start_slot, &i_checked_path_slots);
			c_err = pc_fitness->eFindChanel(pcOrderedFenotype[ii]->i_demand_number, pc_checked_path->pcGetSlavePath(), &i_checked_path_backup_start_slot, &i_checked_path_backup_slots);


			i_checked_path_pessimistic_start_slot = i_checked_path_start_slot;
			i_checked_path_pessimistic_slots = i_checked_path_slots;
			//first get the worst of two: regular path and backup
			if  (i_checked_path_start_slot + i_checked_path_slots  <  i_checked_path_backup_start_slot + i_checked_path_backup_slots)
			{
				i_checked_path_pessimistic_start_slot = i_checked_path_backup_start_slot;
				i_checked_path_pessimistic_slots = i_checked_path_backup_slots;
			}//if  (i_checked_path_start_slot + i_checked_path_slots  <  i_checked_path_backup_start_slot + i_checked_path_backup_slots)
			else
			{
				if  (
					(i_checked_path_start_slot + i_checked_path_slots  ==  i_checked_path_backup_start_slot + i_checked_path_backup_slots)&&
					(i_checked_path_slots  <  i_checked_path_backup_slots)
					)
				{
					i_checked_path_pessimistic_start_slot = i_checked_path_backup_start_slot;
					i_checked_path_pessimistic_slots = i_checked_path_backup_slots;				
				}//if  (
			}//else  if  (i_checked_path_start_slot + i_checked_path_slots  <  i_checked_path_backup_start_slot + i_checked_path_backup_slots)



			/*//prw remove_02.13
			if  ( (pcOrderedFenotype[ii]->i_demand_number == 962)&&(i_global_debug == 1) )
			{
				if  (pc_checked_path->iGetId() == 4201)
				{
					s_buf.Format("ORDER: %d dem: %d orig: %d checked: %d", ii, pcOrderedFenotype[ii]->i_demand_number, pcOrderedFenotype[ii]->pc_path->iGetId(), pc_checked_path->iGetId() );
					::Tools::vRepInFile("zz_constr_simple.txt", s_buf);

					s_buf.Format("ORDER: %d i_checked_path_start_slot: %d [%d] i_checked_path_backup_start_slot: %d [%d]", ii, i_checked_path_start_slot, i_checked_path_slots, i_checked_path_backup_start_slot,  i_checked_path_backup_slots);
					::Tools::vRepInFile("zz_constr_simple.txt", s_buf);
				}//if  (pc_checked_path->iGetId() == 4201)

				if  (pc_checked_path->iGetId() == 960)
				{
					s_buf.Format("ORDER: %d dem: %d orig: %d checked: %d", ii,  pcOrderedFenotype[ii]->i_demand_number, pcOrderedFenotype[ii]->pc_path->iGetId(), pc_checked_path->iGetId() );
					::Tools::vRepInFile("zz_constr_simple.txt", s_buf);

					s_buf.Format("ORDER: %d  i_checked_path_start_slot: %d [%d] i_checked_path_backup_start_slot: %d [%d]", ii, i_checked_path_start_slot, i_checked_path_slots, i_checked_path_backup_start_slot,  i_checked_path_backup_slots);
					::Tools::vRepInFile("zz_constr_simple.txt", s_buf);
				}//if  (pc_checked_path->iGetId() == 960)
				
			}//if  ( (pcOrderedFenotype[ii]->i_demand_number == 962)&&(i_global_debug == 1) )*/


				



			b_new_best = false;
			if  (pc_effective_path  ==  NULL)  
				b_new_best = true;
			else
			{
				if  (i_checked_path_pessimistic_start_slot + i_checked_path_pessimistic_slots  <  i_effective_path_pessimistic_start_slot + i_effective_pessimistic_path_slots)  b_new_best = true;

				if  (
					(i_checked_path_pessimistic_start_slot + i_checked_path_pessimistic_slots  ==  i_effective_path_pessimistic_start_slot + i_effective_pessimistic_path_slots)&&
					(i_checked_path_pessimistic_slots  <  i_effective_pessimistic_path_slots)
					)
					b_new_best = true;

			}//else if  (pc_effective_path  ==  NULL)  


			if  (b_new_best == true)
			{
				pc_effective_path  =  pc_checked_path;

				i_effective_path_pessimistic_start_slot = i_checked_path_pessimistic_start_slot;
				i_effective_pessimistic_path_slots = i_checked_path_pessimistic_slots;

				i_effective_path_start_slot = i_checked_path_start_slot;
				i_effective_path_slots = i_checked_path_slots;			

				i_effective_path_backup_start_slot = i_checked_path_backup_start_slot;
				i_effective_backup_path_slots = i_checked_path_backup_slots;
			}//if  (b_new_best == true)
				
		}//for  (int  i_path = 0; i_path < pc_demands[pcOrderedFenotype[ii]->i_demand_number].pvGetPossiblePaths()->size(); i_path++)


		if  (pcGenotypeToSelfPrecisionUpgrade  !=  NULL)
		{
			pcGenotypeToSelfPrecisionUpgrade[pcOrderedFenotype[ii]->i_demand_number].pc_path = pc_effective_path;		
		}//if  (pcGenotypeToSelfPrecisionUpgrade  !=  NULL)
				
		c_err = pc_fitness->eSetChanel(pcOrderedFenotype[ii]->i_demand_number, pc_effective_path, i_effective_path_start_slot, i_effective_path_slots);
		if  (c_err)
		{
			s_buf.Format("  //v_construct_solution()  demand: %d", pcOrderedFenotype[ii]->i_demand_number);
			c_err.csMessage += s_buf;
			c_err.vShowWindow();
		}//if  (c_err)


		c_err = pc_fitness->eSetChanel(pcOrderedFenotype[ii]->i_demand_number, pc_effective_path->pcGetSlavePath(), i_effective_path_backup_start_slot, i_effective_backup_path_slots);
		if  (c_err)
		{
			s_buf.Format("  //v_construct_solution()  demand: %d", pcOrderedFenotype[ii]->i_demand_number);
			c_err.csMessage += s_buf;
			c_err.vShowWindow();
		}//if  (c_err)
	}//for  (int  ii = 0;  ii < i_feno_len; ii++)

}//void  CMessyIndividual::v_construct_solution_with_fluent_optimization_simple(CMessyGene  **pcOrderedFenotype, int  *piSolutionRepaired, CMessyGene  *pcCompetetiveTemplateToInfect)





void  CMessyIndividual::v_construct_solution_with_fluent_optimization(CMessyGene  **pcOrderedFenotype, int  *piSolutionRepaired, CMessyGene  *pcCompetetiveTemplateToInfect)
{
	CError  c_err;
	CString  s_buf;

	c_err = pc_fitness->pcGetNet()->eResetNet();
	if (c_err)
	{
		::MessageBox(NULL, "error while reseting net", "error", MB_OK);
		return;
	}//if (c_err)

	if  ( (piSolutionRepaired !=  NULL)&&(*piSolutionRepaired  ==  10) )  return;



	CFiberPath  *pc_effective_path;
	int  i_effective_path_start_slot, i_effective_path_slots;
	int  i_effective_path_backup_start_slot, i_effective_backup_path_slots;
	int  i_effective_path_pessimistic_start_slot, i_effective_pessimistic_path_slots;

	int  i_feno_len;
	CFiberDemand  *pc_demands;
	CFiberPath  *pc_checked_path;

	int  i_checked_path_start_slot, i_checked_path_slots;
	int  i_checked_path_backup_start_slot, i_checked_path_backup_slots;
	int  i_checked_path_pessimistic_start_slot, i_checked_path_pessimistic_slots;


	

	pc_demands = pc_fitness->pcGetNet()->pcGetDemands();
	i_feno_len = pc_fitness->pcGetNet()->iGetDemandNumber();	
	for  (int  ii = 0;  ii < i_feno_len; ii++)
	{
		//pc_effective_path = pcOrderedFenotype[ii]->pc_path;

		
		if  (pcCompetetiveTemplateToInfect  !=  NULL)
		{
			pcCompetetiveTemplateToInfect[pcOrderedFenotype[ii]->i_demand_number].d_order = pcOrderedFenotype[ii]->d_order;
			pcCompetetiveTemplateToInfect[pcOrderedFenotype[ii]->i_demand_number].i_demand_number = pcOrderedFenotype[ii]->i_demand_number;
			pcCompetetiveTemplateToInfect[pcOrderedFenotype[ii]->i_demand_number].pc_path = pcOrderedFenotype[ii]->pc_path;
		}//if  (pcCompetetiveTemplateToInfect  !=  NULL)


		bool  b_new_best;
		pc_effective_path = NULL;
		for  (int  i_path = 0; i_path < pc_demands[pcOrderedFenotype[ii]->i_demand_number].pvGetPossiblePaths()->size(); i_path++)
		//for  (int  i_path = 0; i_path < 1; i_path++)
		{
			pc_checked_path = pc_demands[pcOrderedFenotype[ii]->i_demand_number].pvGetPossiblePaths()->at(i_path);
			c_err = pc_fitness->eFindChanel(pcOrderedFenotype[ii]->i_demand_number, pc_checked_path, &i_checked_path_start_slot, &i_checked_path_slots);
			c_err = pc_fitness->eFindChanel(pcOrderedFenotype[ii]->i_demand_number, pc_checked_path->pcGetSlavePath(), &i_checked_path_backup_start_slot, &i_checked_path_backup_slots);

			i_checked_path_pessimistic_start_slot = i_checked_path_start_slot;
			i_checked_path_pessimistic_slots = i_checked_path_slots;
			//first get the worst of two: regular path and backup
			if  (i_checked_path_start_slot + i_checked_path_slots  <  i_checked_path_backup_start_slot + i_checked_path_backup_slots)
			{
				i_checked_path_pessimistic_start_slot = i_checked_path_backup_start_slot;
				i_checked_path_pessimistic_slots = i_checked_path_backup_slots;
			}//if  (i_checked_path_start_slot + i_checked_path_slots  <  i_checked_path_backup_start_slot + i_checked_path_backup_slots)
			else
			{
				if  (
					(i_checked_path_start_slot + i_checked_path_slots  ==  i_checked_path_backup_start_slot + i_checked_path_backup_slots)&&
					(i_checked_path_slots  <  i_checked_path_backup_slots)
					)
				{
					i_checked_path_pessimistic_start_slot = i_checked_path_backup_start_slot;
					i_checked_path_pessimistic_slots = i_checked_path_backup_slots;				
				}//if  (
			}//else  if  (i_checked_path_start_slot + i_checked_path_slots  <  i_checked_path_backup_start_slot + i_checked_path_backup_slots)



			b_new_best = false;
			if  (pc_effective_path  ==  NULL)  
				b_new_best = true;
			else
			{

				/*//prw remove_02.13
				if  (pcOrderedFenotype[ii]->i_demand_number == 962)
				{
					if  (pc_checked_path->iGetId() == 4201)
					{
						s_buf.Format("ORDER: %d dem: %d orig: %d checked: %d", ii, pcOrderedFenotype[ii]->i_demand_number, pcOrderedFenotype[ii]->pc_path->iGetId(), pc_checked_path->iGetId() );
						::Tools::vRepInFile("zz_constr.txt", s_buf);

						s_buf.Format("i_checked_path_start_slot: %d [%d] i_checked_path_backup_start_slot: %d [%d]", i_checked_path_start_slot, i_checked_path_slots, i_checked_path_backup_start_slot,  i_checked_path_backup_slots);
						::Tools::vRepInFile("zz_constr.txt", s_buf);
					}//if  (pc_checked_path->iGetId() == 4201)

					if  (pc_checked_path->iGetId() == 960)
					{
						s_buf.Format("ORDER: %d dem: %d orig: %d checked: %d", ii, pcOrderedFenotype[ii]->i_demand_number, pcOrderedFenotype[ii]->pc_path->iGetId(), pc_checked_path->iGetId() );
						::Tools::vRepInFile("zz_constr.txt", s_buf);

						s_buf.Format("i_checked_path_start_slot: %d [%d] i_checked_path_backup_start_slot: %d [%d]", i_checked_path_start_slot, i_checked_path_slots, i_checked_path_backup_start_slot,  i_checked_path_backup_slots);
						::Tools::vRepInFile("zz_constr.txt", s_buf);
					}//if  (pc_checked_path->iGetId() == 960)
					
				}//if  (pcOrderedFenotype[ii]->i_demand_number == 960)*/

				if  (i_checked_path_pessimistic_start_slot + i_checked_path_pessimistic_slots  <  i_effective_path_pessimistic_start_slot + i_effective_pessimistic_path_slots)  b_new_best = true;

				if  (
					(i_checked_path_pessimistic_start_slot + i_checked_path_pessimistic_slots  ==  i_effective_path_pessimistic_start_slot + i_effective_pessimistic_path_slots)&&
					(i_checked_path_pessimistic_slots  <  i_effective_pessimistic_path_slots)
					)
					b_new_best = true;



				

				if  (
					(i_checked_path_pessimistic_start_slot + i_checked_path_pessimistic_slots  ==  i_effective_path_pessimistic_start_slot + i_effective_pessimistic_path_slots)&&
					(i_checked_path_pessimistic_slots  ==  i_effective_pessimistic_path_slots)
					)
				{
					//prw remove
					//int i_test;
					//i_test = pc_checked_path->iGetId();
					//i_test = pc_effective_path->iGetId();
					//i_test = pcOrderedFenotype[ii]->pc_path->iGetId();					

					if  (
						iAbs(pc_checked_path->iGetId() - pcOrderedFenotype[ii]->pc_path->iGetId())
						<
						iAbs(pc_effective_path->iGetId() - pcOrderedFenotype[ii]->pc_path->iGetId())
						)
						b_new_best = true;

					if  (
						(
						iAbs(pc_checked_path->iGetId() - pcOrderedFenotype[ii]->pc_path->iGetId())
						==
						iAbs(pc_effective_path->iGetId() - pcOrderedFenotype[ii]->pc_path->iGetId())
						)&&
						(pc_checked_path->iGetId()  >  pc_effective_path->iGetId())
						)
						b_new_best = true;
					/*if  (
						pc_checked_path->iGetId() - pcOrderedFenotype[ii]->pc_path->iGetId()
						<
						pc_effective_path->iGetId() - pcOrderedFenotype[ii]->pc_path->iGetId()
						)
						b_new_best = true;

					if  (
						(
						pc_checked_path->iGetId() - pcOrderedFenotype[ii]->pc_path->iGetId()
						==
						pc_effective_path->iGetId() - pcOrderedFenotype[ii]->pc_path->iGetId()
						)&&
						(pc_checked_path->iGetId()  >  pc_effective_path->iGetId())
						)
						b_new_best = true;

					iAbs
					/*if  (
						::abs(pc_checked_path->iGetId() - pcOrderedFenotype[ii]->pc_path->iGetId())
						<
						::abs(pc_effective_path->iGetId() - pcOrderedFenotype[ii]->pc_path->iGetId())
						)
						b_new_best = true;

					if  (
						(
						::abs(pc_checked_path->iGetId() - pcOrderedFenotype[ii]->pc_path->iGetId())
						==
						::abs(pc_effective_path->iGetId() - pcOrderedFenotype[ii]->pc_path->iGetId())
						)&&
						(pc_checked_path->iGetId()  >  pc_effective_path->iGetId())
						)
						b_new_best = true;*/
				}//if  (
			}//else if  (pc_effective_path  ==  NULL)  



			if  (b_new_best == true)
			{
				pc_effective_path  =  pc_checked_path;

				i_effective_path_pessimistic_start_slot = i_checked_path_pessimistic_start_slot;
				i_effective_pessimistic_path_slots = i_checked_path_pessimistic_slots;

				i_effective_path_start_slot = i_checked_path_start_slot;
				i_effective_path_slots = i_checked_path_slots;			

				i_effective_path_backup_start_slot = i_checked_path_backup_start_slot;
				i_effective_backup_path_slots = i_checked_path_backup_slots;
			}//if  (b_new_best == true)
				
		}//for  (int  i_path = 0; i_path < pc_demands[pcOrderedFenotype[ii]->i_demand_number].pvGetPossiblePaths()->size(); i_path++)


		/*//prw remove_02.13
		if  (pcOrderedFenotype[ii]->pc_path->iGetId() != pc_effective_path->iGetId())
		{
			s_buf.Format("dem: %d orig: %d eff: %d", pcOrderedFenotype[ii]->i_demand_number, pcOrderedFenotype[ii]->pc_path->iGetId(), pc_effective_path->iGetId() );
			::Tools::vRepInFile("zz_constr.txt", s_buf);
		}//if  (pcOrderedFenotype[ii]->pc_path->iGetId() != pc_effective_path->iGetId())*/
		
				
		c_err = pc_fitness->eSetChanel(pcOrderedFenotype[ii]->i_demand_number, pc_effective_path, i_effective_path_start_slot, i_effective_path_slots);
		if  (c_err)
		{
			s_buf.Format("  //v_construct_solution()  demand: %d", pcOrderedFenotype[ii]->i_demand_number);
			c_err.csMessage += s_buf;
			c_err.vShowWindow();
		}//if  (c_err)assad


		if  (pc_effective_path->pcGetSlavePath() ==  NULL)
		{
			CString  s_buf;

			s_buf.Format("No slave path");
			c_err.vPutError(s_buf);
			c_err.vShowWindow();
		}//if  (pc_effective_path->pcGetSlavePath() ==  NULL)


		c_err = pc_fitness->eSetChanel(pcOrderedFenotype[ii]->i_demand_number, pc_effective_path->pcGetSlavePath(), i_effective_path_backup_start_slot, i_effective_backup_path_slots);
		if  (c_err)
		{
			s_buf.Format("  //v_construct_solution()  demand: %d", pcOrderedFenotype[ii]->i_demand_number);
			c_err.csMessage += s_buf;
			c_err.vShowWindow();
		}//if  (c_err)


	}//for  (int  ii = 0;  ii < i_feno_len; ii++)


	//::Tools::vShow("ok");
}//void  CMessyIndividual::v_construct_solution_with_fluent_optimization(CMessyGene  **pcOrderedFenotype, int  *piSolutionRepaired, CMessyGene  *pcCompetetiveTemplateToInfect)




void  CMessyIndividual::v_construct_solution(CMessyGene  **pcOrderedFenotype, int iSolutionConstructionMethod, int  *piSolutionRepaired, CMessyGene  *pcCompetetiveTemplateToInfect, CMessyGene  *pcGenotypeToSelfPrecisionUpgrade)
{
	//if  (i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION)
	if  (iSolutionConstructionMethod == SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION)
		v_construct_solution_with_fluent_optimization(pcOrderedFenotype, piSolutionRepaired, pcCompetetiveTemplateToInfect);

	//if  (i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION)
	if  ( (iSolutionConstructionMethod == SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION)||(iSolutionConstructionMethod == SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FIRST_FULL_LATER)||(iSolutionConstructionMethod == SOLUTION_CONSTRUCTION_METHOS_MIX) )
		v_construct_solution_with_fluent_optimization_simple(pcOrderedFenotype, piSolutionRepaired, pcCompetetiveTemplateToInfect, pcGenotypeToSelfPrecisionUpgrade);

	//if  (i_solution_construction_method == SOLUTION_CONSTRUCTION_METHOS_NORMAL)
	if  (iSolutionConstructionMethod == SOLUTION_CONSTRUCTION_METHOS_NORMAL)
		v_construct_solution_old(pcOrderedFenotype, piSolutionRepaired, pcCompetetiveTemplateToInfect);
}//void  CMessyIndividual::v_construct_solution(CMessyGene  **pcOrderedFenotype, int  *piSolutionRepaired, CMessyGene  *pcCompetetiveTemplateToInfect)



void  CMessyIndividual::v_construct_solution_old(CMessyGene  **pcOrderedFenotype, int  *piSolutionRepaired, CMessyGene  *pcCompetetiveTemplateToInfect)
{
	CError  c_err;
	CString  s_buf;

	c_err = pc_fitness->pcGetNet()->eResetNet();
	if (c_err)
	{
		::MessageBox(NULL, "error while reseting net", "error", MB_OK);
		return;
	}//if (c_err)

	if  ( (piSolutionRepaired !=  NULL)&&(*piSolutionRepaired  ==  10) )  return;


	CFiberPath  *pc_effective_path;

	int  i_feno_len = pc_fitness->pcGetNet()->iGetDemandNumber();	
	for  (int  ii = 0;  ii < i_feno_len; ii++)
	{
		pc_effective_path = pcOrderedFenotype[ii]->pc_path;

		if  (pc_fitness->pcGetNet()->pcGetDemands()[pcOrderedFenotype[ii]->iGetDemNumber()].iDemandType()  ==  DEMAND_TYPE_ANYCAST)
		{
			if  (pc_fitness->pcGetNet()->pcGetDemands()[pcOrderedFenotype[ii]->iGetDemNumber()].bDominatingTwin()  ==  false)
			{
				int  i_twin_offset = pc_fitness->pcGetNet()->pcGetDemands()[pcOrderedFenotype[ii]->iGetDemNumber()].pcGetTwin()->iGetId();

				if  (pc_fenotype[i_twin_offset]->iGetDemNumber() !=  i_twin_offset)
				{
					CError  c_err;
					CString  s_err;
					s_err.Format("fenotype twin offset = %d  demand twin offset = %d", i_twin_offset, pc_fenotype[i_twin_offset]->iGetDemNumber());
					c_err.vPutError(s_err);

					vShow(s_err);
				}//if  (pc_fenotype[i_twin_offset]->iGetDemNumber() !=  i_twin_offset)


				if  (
					(pc_fenotype[i_twin_offset]->pcGetPath()->iGetEndNode()  !=   pcOrderedFenotype[ii]->pcGetPath()->iGetStartNode())||
					(pc_fenotype[i_twin_offset]->pcGetPath()->iGetStartNode()  !=   pcOrderedFenotype[ii]->pcGetPath()->iGetEndNode())
					)
				{
					if  (bGlobalDebugConstructSol == true)
					{
						CString  s_buf;
						s_buf.Format("wrong nodes: (%d)%d->%d  (%d)%d->%d", pc_fenotype[i_twin_offset]->i_demand_number, pc_fenotype[i_twin_offset]->pc_path->iGetEndNode(), pc_fenotype[i_twin_offset]->pc_path->iGetStartNode(), pcOrderedFenotype[ii]->i_demand_number, pcOrderedFenotype[ii]->pc_path->iGetStartNode(), pcOrderedFenotype[ii]->pc_path->iGetEndNode());
						vShow(s_buf);
						bGlobalDebugConstructSol = false;
					}//if  (bGlobalDebugConstructSol == true)

					if  (piSolutionRepaired  !=  NULL)  *piSolutionRepaired = 1;

					pc_effective_path = pc_fitness->pcGetBestCleverPath
						(
						//reverse by mean - it is the opposite path
						pc_fenotype[i_twin_offset]->pcGetPath()->iGetEndNode(),
						pc_fenotype[i_twin_offset]->pcGetPath()->iGetStartNode()
						);

					
					if  (
						(pc_fenotype[i_twin_offset]->pcGetPath()->iGetEndNode()  !=   pc_effective_path->iGetStartNode())||
						(pc_fenotype[i_twin_offset]->pcGetPath()->iGetStartNode()  !=   pc_effective_path->iGetEndNode())
						)
					{
						CError  c_err;
						CString  s_err;
						s_err.Format("wrong nodes2: (%d)%d->%d  (%d)%d->%d", pc_fenotype[i_twin_offset]->i_demand_number, pc_fenotype[i_twin_offset]->pc_path->iGetEndNode(), pc_fenotype[i_twin_offset]->pc_path->iGetStartNode(), pcOrderedFenotype[ii]->i_demand_number, pc_effective_path->iGetStartNode(), pc_effective_path->iGetEndNode());
						c_err.vPutError(s_err);

						vShow(s_err);
					}//if  (
				}//if  (

			}//if  (pc_fitness->pcGetNet()->pcGetDemands()[pcOrderedFenotype[ii]->iGetDemNumber()].bDominatingTwin()  ==  false)
		}//if  (pc_fitness->pcGetNet()->pcGetDemands()[pcOrderedFenotype[ii]->iGetDemNumber()].bAnyCast()  ==  true)


		if  (pcCompetetiveTemplateToInfect  !=  NULL)
		{
			pcCompetetiveTemplateToInfect[pcOrderedFenotype[ii]->i_demand_number].d_order = pcOrderedFenotype[ii]->d_order;
			pcCompetetiveTemplateToInfect[pcOrderedFenotype[ii]->i_demand_number].i_demand_number = pcOrderedFenotype[ii]->i_demand_number;
			pcCompetetiveTemplateToInfect[pcOrderedFenotype[ii]->i_demand_number].pc_path = pc_effective_path;
		}//if  (pcCompetetiveTemplateToInfect  !=  NULL)
		
		c_err = pc_fitness->eFindAndSetChanel(pcOrderedFenotype[ii]->i_demand_number, pc_effective_path);
		if  (c_err)
		{
			s_buf.Format("  //v_construct_solution()  demand: %d", pcOrderedFenotype[ii]->i_demand_number);
			c_err.csMessage += s_buf;
			c_err.vShowWindow();
		}//if  (c_err)
	}//for  (int  ii = 0;  ii < i_feno_len; ii++)

	
}//void  CMessyIndividual::v_construct_solution()



double  CMessyIndividual::dGetPatternEntrophyFittnes(int  *piGeneFreqTable)
{
//	double  d_fit;
	
	/*d_fit  =  0;
	for  (int  ii = 0;  ii  <  (int) v_genotype.size(); ii++)
	{
		d_fit  +=  piGeneFreqTable[v_genotype.at(ii)->i_gene_pos];
	}//for  (int  ii = 0;  ii  <  (int) pc_buf->pvGetGenotype()->size(); ii++)

	d_fit  =  d_fit  /  v_genotype.size();*/

	return(0);
}//int  CMessyIndividual::iGetPatternEntrophyFittnes(int  *piGeneFreqTable)




//---------------------------class  CMessyPattern--------------------------------------
bool bPatternGreater (CMessyPattern *elem1, CMessyPattern *elem2)
{
	return elem1->iGetMultiple() > elem2->iGetMultiple();
}

CMessyPattern::CMessyPattern(CFiberSolution  *pcFitness)  : CMessyIndividual(pcFitness, BAYESIAN_EFECT_NONE, SOLUTION_CONSTRUCTION_METHOS_NORMAL)
{
	i_pattern_multiple  =  1;
	i_old  =  0;
};//CMessyPattern::CMessyPattern(int  iFenotypeLength)  : CMessyIndividual(pcFitness)


CMessyPattern::~CMessyPattern()
{

}//CMessyPattern::~CMessyPattern()



void  CMessyPattern::operator=(CMessyIndividual  &cMessyIndiv)
{
	CMessyIndividual  *pc_this_buf;

	pc_this_buf  =  (CMessyIndividual *)  this;

	*pc_this_buf  =  cMessyIndiv;
}//void  CMessyPattern::operator=(CMessyIndividual  &cMessyIndiv)




void  CMessyPattern::operator=(CMessyPattern  &cOther)
{
	CMessyIndividual  *pc_this_buf,   *pc_other_buf;

	pc_this_buf  =  (CMessyIndividual *)  this;
	pc_other_buf  =  (CMessyIndividual *)  &cOther;

	*pc_this_buf  =  *pc_other_buf;

	i_pattern_multiple  =  cOther.i_pattern_multiple;
	i_old  =  cOther.i_old;
}//void  CMessyPattern::operator=(CMessyIndividual  &cMessyIndiv)



CError  CMessyPattern::eReport(FILE  *pfReport,  bool  bTemplate  /*=  false*/)
{
	CError  c_err;
	fprintf(pfReport,  "%d\n",  iGetFenotypeLength());

	v_create_fenotype();

	int  i_feno_len = pc_fitness->pcGetNet()->iGetDemandNumber();

	for  (int  ii = 0; ii < i_feno_len; ii++)
	{
		if  (pc_fenotype[ii]  ==  NULL)
			fprintf(pfReport, "?  ");
		else
			fprintf(pfReport, "1  ");	
	}//for  (int  ii = 0; ii < i_feno_len; ii++)

	return(c_err);
}//CError  CMessyPattern::eReport(FILE  *pfReport,  bool  bTemplate  =  false)


CError  CMessyPattern::eReport
	(
	FILE  *pfReport,
	int  *piCompetetiveTemplate,  CFiberSolution  *pcFitness
	)
{
	CError  c_err;
	/*fprintf(pfReport,  "multiple:%d\nold:%d\n",  i_pattern_multiple,  i_old);

	c_err  =  ((CMessyIndividual *) this)->eReport(pfReport, piCompetetiveTemplate,  pcFitness);*/
	return(c_err);
}//CError  CMessyPattern::eReport




//---------------------------class  CMessyGene--------------------------------------

CMessyGene::CMessyGene()
{
	i_demand_number = 0;
	d_order = 0;
	pc_path = NULL;
}//CMessyGene::CMessyGene()

CMessyGene::CMessyGene(int  iDemandNumber,  double  dOrder, CFiberPath  *pcPath)
{
	i_demand_number = iDemandNumber;
	d_order = dOrder;
	pc_path = pcPath;
}//CMessyGene::CMessyGene(int  iGeneVal,  int  iGenePos)


CMessyGene::CMessyGene(CMessyGene  *pcOther)
{
	i_demand_number = pcOther->i_demand_number;
	d_order = pcOther->d_order;
	pc_path = pcOther->pc_path;
}//CMessyGene::CMessyGene(CMessyGene  *pcOther)


bool CMessyGene::operator==(CMessyGene  &cOther)
{
	if  (
		(i_demand_number == cOther.i_demand_number)&&
		(d_order == cOther.d_order)&&
		(pc_path->iGetId() == cOther.pc_path->iGetId())
		)
		return(true);

	return(false);
};//bool CMessyGene::operator==(CMessyGene  &cOther);



bool CMessyGene::operator!=(CMessyGene  &cOther)
{
	if  (
		(i_demand_number != cOther.i_demand_number)||
		(d_order != cOther.d_order)||
		(pc_path->iGetId() != cOther.pc_path->iGetId())
		)
		return(true);

	return(false);
};//bool CMessyGene::operator==(CMessyGene  &cOther);


void  CMessyGene::vReportGene(FILE  *pfDest)
{
	fprintf(pfDest, "dem num: %d\n", i_demand_number);
	fprintf(pfDest, "Path id: %d\n", pc_path->iGetId());
	fprintf(pfDest, "Order: %.8lf\n", d_order);
}//void  CMessyGene::vReportGene(FILE  *pfDest)


CMessyGene::~CMessyGene()
{

};//CMessyGene::~CMessyGene()

//------------------------------------------GRASP

//***********************************************************
//	CGrasp_start
//***********************************************************

const CString CGrasp::sLogDebugFile = "!grasp_debug.txt";

CGrasp::CGrasp(CFiberSolution *pc_fitness, gcroot<Random*> pcRandomGen, CRankedSelection *rankedSelection) 
: pcFitness(pc_fitness), pcRandomGen(pcRandomGen), debug(false), rankedSelection(rankedSelection)
{
	vClearReportFile();
	nrOfDemands = pc_fitness->pcGetNet()->iGetDemandNumber();
}

void CGrasp::vClearReportFile()
{
	::Tools::vRepInFile(sLogDebugFile, "", true);
}

void CGrasp::vFillSolutionWithMostSubcarriersFirst(CGraspSolution *solution)
{
	CFiberDemand *demands = pcFitness->pcGetNet()->pcGetDemands();

	// copy demands to new vector
	vector<CFiberDemand *> orderedDemands;
	orderedDemands.reserve(nrOfDemands);
	for (int d = 0; d < nrOfDemands; d++) 
	{
		orderedDemands.push_back(&demands[d]);
	}
	//create comparator by subcarriers
	struct {
		bool operator() (CFiberDemand* d1, CFiberDemand *d2)
		{
			return d1->dGetDemandCapacity() > d2->dGetDemandCapacity();
		}
	} comparatorBySubcarriers;
	// add pass it to the sort function
	sort(orderedDemands.begin(), orderedDemands.end(), comparatorBySubcarriers);
	
	// fill found ordering in current solution
	int orderingValue = 0;
	for (vector<CFiberDemand *>::iterator it = orderedDemands.begin(); it != orderedDemands.end(); it++) {
		int demandId = (*it)->iGetId();
		solution->vSetOrderForDemand(demandId, orderingValue);
		solution->vSetPathIdForDemand(demandId, 0);
		orderingValue++;
	}
	// recalculate vector how demand ids are ordered
	solution->vEvaluateOrderOfDemands();
}

void CGrasp::vRandomGreedyConstruction(CGraspSolution &solution, const int &rclSize)
{
	//// not needed as orderOfDemandIds vector stored in CGraspSolution is ordered
	// qsort(solution.pcGetSolution()->pc_ordered_template, nrOfDemands, sizeof(CMessyGene *),  i_comp_genes);
	// CMessyGene * oldGene = solution.pcGetSolution()->pc_ordered_template;

	// list has O(1) insert and removal time at the beginning and end
	// list has O(n/4) removal at given element, which is faster
	// than linear reallocation of vector after removing an element
	// elements are fetched only from the k-first elements of the list, depending on the rclSize
	list<int> oldDemandOrderIds;
	list<int>::iterator oldIt;
	list<int> newDemandOrderIds;

	for (vector<int>::iterator it = solution.pcGetOrderOfDemandIds()->begin(); it != solution.pcGetOrderOfDemandIds()->end(); it++)
	{
		oldDemandOrderIds.push_back(*it);
	}
	// check if rcl is not too big
	const int actualRCLSize = rclSize < nrOfDemands ? rclSize : nrOfDemands;
	for (int i = 0; i < nrOfDemands; i++)
	{
		// check if it we reach the end of the list and selection is made from smaller set
		int upperBound = oldDemandOrderIds.size() < actualRCLSize ?
			oldDemandOrderIds.size() : actualRCLSize ;
		int idx = rankedSelection->selectIndex(upperBound);
		
		// get and erase
		oldIt = oldDemandOrderIds.begin();
		advance(oldIt, idx);
		newDemandOrderIds.push_back(*oldIt);
		oldDemandOrderIds.erase(oldIt);
	}
	
	// refill orderOfDemandIds in CGraspSolution and correct order of demands
	solution.pcGetOrderOfDemandIds()->clear();
	int idx = 0;
	for (list<int>::iterator it = newDemandOrderIds.begin(); it != newDemandOrderIds.end(); it++)
	{
		solution.vSetOrderForDemand(*it, idx);
		solution.pcGetOrderOfDemandIds()->push_back(*it);
		idx++;
	}

	// force recalculation of fitness
	solution.vClearFitness();
}

// debug tools


void CGrasp::logToDebugFile(const CString & sBuf)
{
	if (debug) ::Tools::vRepInFile(sLogDebugFile, sBuf);
}

void CGrasp::logToDebugFile(const int &value)
{
	if (debug)
	{
		sBuf.Format("%d", value);
		::Tools::vRepInFile(sLogDebugFile, sBuf);
	}
}

//***********************************************************
//	CGrasp_end
//***********************************************************

//***********************************************************
//	CGraspSolution_start
//***********************************************************

CGraspSolution::CGraspSolution(gcroot<Random*> pcRandomGen, int iBayesianEffect, int iSolutionConstructionMethod, CFiberSolution *pcFitness)
: pcFitness(pcFitness), pcRandomGen(pcRandomGen)
{
	solution = new CmGA(pcRandomGen, iBayesianEffect, iSolutionConstructionMethod);
	solution->eSetFitness(pcFitness, true);
	solution->eConfigureTemplate();
	solution->eVirGenerateRandomTemplate();
	nrOfDemands = pcFitness->pcGetNet()->iGetDemandNumber();
	vEvaluateOrderOfDemands();
}

CGraspSolution::~CGraspSolution()
{
	delete solution;
}

void CGraspSolution::vCopySolutionTo(CGraspSolution *destination)
{
	solution->vCopyTo(destination->solution);
	destination->orderOfDemandIds = orderOfDemandIds;
	destination->vClearFitness();
}

void CGraspSolution::vEvaluateOrderOfDemands()
{
	orderOfDemandIds.clear();
	dGetFitness(); // to fill pc_ordered_template
	CMessyGene *orderedDemands = solution->pc_ordered_template;
	orderOfDemandIds.clear();
	for (int i = 0; i < nrOfDemands; i++)
	{
		orderOfDemandIds.push_back(orderedDemands[i].iGetDemNumber());
	}
}

inline void CGraspSolution::vSetOrderForDemand(const int &demandId, const double &order)
{
	solution->pc_competetive_template[demandId].d_order = order;
}

inline void CGraspSolution::vSetPathIdForDemand(const int &demandId, const int &pathId)
{
	solution->pc_competetive_template[demandId].pc_path = pcFitness->pcGetNet()->pcGetDemands()[demandId].pvGetPossiblePaths()->at(pathId);
}

CString CGraspSolution::sGetSolutionOrderAsString()
{
	CString sBuf;
	for (int demandId = 0; demandId < nrOfDemands; demandId++) {
		sBuf.AppendFormat("%1.2f ", solution->pc_competetive_template[demandId].dGetOrder());
	}
	return sBuf;
}

int CGraspSolution::iGetSolutionOrderHash()
{
	int seed = 0;
	for (int demandId = 0; demandId < nrOfDemands; demandId++)
	{
		double order = solution->pc_competetive_template[demandId].dGetOrder();
		// resize the double value
		if (order < 0) order *= -1;
		if (order > 0)
			while (order < 1e3 || order > 1e6) order *= (order < 1e3? 1e3 : 1e-3);
		seed ^= ((unsigned)order) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

void CGraspSolution::vRandomSwap()
{
	int first = pcRandomGen->Next(nrOfDemands);
	int second = pcRandomGen->Next(nrOfDemands - 1);
	if (first == second) second = nrOfDemands - 1;
	vSwapDemands(first, second);
}

void CGraspSolution::vSwapDemands(const int &idx1, const int &idx2)
{
	// todo: throw exception when idx exceeds size of vector???
	double tempOrder = solution->pc_competetive_template[idx1].d_order;
	solution->pc_competetive_template[idx1].d_order = solution->pc_competetive_template[idx2].d_order;
	solution->pc_competetive_template[idx2].d_order = tempOrder;
	// reflect changes in orderOfDemandIds
	int tempId = orderOfDemandIds.at(idx1);
	orderOfDemandIds.at(idx1) = orderOfDemandIds.at(idx2);
	orderOfDemandIds.at(idx2) = tempId;
	vClearFitness();
}

void CGraspSolution::vSwapDemands(const vector<int> &lIndices, const vector<int> &rIndices)
{
	vector<int>::const_iterator lIt = lIndices.begin();
	vector<int>::const_iterator rIt = rIndices.begin();

	while (lIt != lIndices.end() && rIt != rIndices.end())
	{
		vSwapDemands(*lIt, *rIt);
		lIt++;
		rIt++;
	}
}

void CGraspSolution::vSwapDemands(vector<int>::iterator &lIndicesIt, vector<int>::iterator &lIndicesEndIt, vector<int>::iterator &rIndicesIt, vector<int>::iterator &rIndicesEndIt)
{
	while(lIndicesIt != lIndicesEndIt && rIndicesIt != rIndicesEndIt)
	{
		vSwapDemands(*lIndicesIt, *rIndicesIt);
		lIndicesIt++;
		rIndicesIt++;
	}
}

//***********************************************************
//	CGraspSolution_end
//***********************************************************

//***********************************************************
//	PermutationRandomGenerator_start
//***********************************************************

//TODO: nie wiem gdzie wstawic inicjalizacje rzeczy statycznych w cpp
map<int, CPermutationRandomGenerator*> CPermutationRandomGenerator::cache;

CPermutationRandomGenerator::CPermutationRandomGenerator(gcroot<Random*> pcRandomGen, int size)
: size(size), pcRandomGen(pcRandomGen)
{
	consecutiveIntegers.reserve(size);
	for (int i = 0; i < size ; i++)
	{
		consecutiveIntegers.push_back(i);
	}
}

CPermutationRandomGenerator::CPermutationRandomGenerator(MessyGA::CPermutationRandomGenerator *other)
: size(other->size)
{
	consecutiveIntegers = other->consecutiveIntegers;
	gcroot<Random*> pcRandomGen =  other->pcRandomGen;
}

CPermutationRandomGenerator * CPermutationRandomGenerator::pcGetInstance(gcroot<Random*> pcRandomGen, const int &size)
{
	map<int, CPermutationRandomGenerator*>::iterator iterator = cache.find(size);
	CPermutationRandomGenerator * gen;
	if (iterator == cache.end())
	{
		gen = new CPermutationRandomGenerator(pcRandomGen, size);
		cache.insert(make_pair(size, gen));
	} else 
	{
		gen = iterator->second;
	}
	return gen;
}

void CPermutationRandomGenerator::vClearCache()
{
	for (map<int, CPermutationRandomGenerator*>::iterator it = cache.begin(); it != cache.end(); it++)
	{
		pair<int, CPermutationRandomGenerator*> pair = *it;
		delete it->second;
	}
	cache.clear();
}

vector<int>* CPermutationRandomGenerator::pvGenerateKDistinctIntegers(const int &size)
{
	// shuffle the vector
	for (int i = consecutiveIntegers.size(); i > 1; i--)
	{
		// swap element i-1 with random(0, i) where i exclusive
		int randomIndexSwap = pcRandomGen->Next(0, i);
		int swapValue = consecutiveIntegers.at(i - 1);
		consecutiveIntegers.at(i - 1) =  consecutiveIntegers.at(randomIndexSwap);
		consecutiveIntegers.at(randomIndexSwap) =  swapValue;
	}
	vector<int> *subview = new vector<int>(consecutiveIntegers.begin(), 
		consecutiveIntegers.begin() + size);
	return subview;
}

//***********************************************************
//	CRankedSelection_start
//***********************************************************

CRankedSelection* CRankedSelection::instance = NULL;

CRankedSelection::CRankedSelection()
{
	// intentionally private access
	sumOfNIntegers.push_back(1);
	extendSumOfNIntegers(20);
}

void CRankedSelection::deleteRankedSelection()
{
	delete instance;
	instance = NULL;
}

CRankedSelection* CRankedSelection::newInstance(gcroot<Random*> pcRandomGen)
{
	if (instance == NULL)
	{
		instance = new CRankedSelection();
		instance->pcRandomGen = pcRandomGen;
	}
	return instance;
}

void CRankedSelection::extendSumOfNIntegers(const int &newSize)
{
	int currentSize = sumOfNIntegers.size();
	int previousValue = sumOfNIntegers.at(currentSize - 1);
	int nextValue = 0;
	for (int i = currentSize + 1; i <= newSize; i++)
	{
		nextValue = previousValue + i;
		sumOfNIntegers.push_back(nextValue);
		previousValue = nextValue;
	}
}

void CRankedSelection::extendSumIfNeeded(const int &requiredIndex)
{
	if (requiredIndex > sumOfNIntegers.size())
	{
		if (requiredIndex > sumOfNIntegers.size() * 2)
		{
			extendSumOfNIntegers(requiredIndex);
		} else
		{
			extendSumOfNIntegers(sumOfNIntegers.size() * 2);
		}
	}
}

int CRankedSelection::selectIndex(const int &numberOfElements)
{
	extendSumIfNeeded(numberOfElements);
	int upperBound = sumOfNIntegers.at(numberOfElements - 1);
	int value = pcRandomGen->Next(upperBound);
	int inverseIdx = binarySearchLower(sumOfNIntegers, value);
	return numberOfElements - inverseIdx - 1;
}

template <typename T> T CRankedSelection::selectElement(vector<T> elements)
{
	int idx = selectIndex(elements.size());
	return elements.at(idx);
}
template int CRankedSelection::selectElement(vector<int> element);

int CRankedSelection::binarySearchLower(std::vector<int> &vec, const int &searchValue)
{
	return binarySearchLower(vec, 0, vec.size() - 1, searchValue);
}

int CRankedSelection::binarySearchLower(std::vector<int> &vec, const int &startIdx, const int &endIdx, const int &searchValue)
{
	if (startIdx == endIdx)
	{
		return searchValue < vec.at(startIdx) ? startIdx : -1;
	}
	int midIdx = startIdx + (endIdx - startIdx) / 2;
	if (searchValue < vec.at(midIdx))
	{
		return binarySearchLower(vec, startIdx, midIdx, searchValue);
	} else
	{
		return binarySearchLower(vec, midIdx + 1, endIdx, searchValue);
	}
}

//***********************************************************
//	CRankedSelection_end
//***********************************************************