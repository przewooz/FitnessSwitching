#pragma once


//tools
#include  "atlstr.h"  //CString
#include  "..\error\error.h"
#include  <iostream>
#include  <vector>
#include  <map>
#include  <list>
#include  <functional>
#include  <algorithm>

//for randomizer...
#using <mscorlib.dll>
using namespace System;


//system tools
#include  "..\DeceptiveTools\DecFunc.h"
#include  "error.h"

//time counter
#include  "..\util\timer.h"
//..and other tools...
#include  "..\util\tools.h"
#include  "..\util\LargeNum.h"
#include  "..\FiberFlowAssignment\FiberNet.h"
//#include  "..\FiberFlowAssignment\TabuFunc.h"


using namespace std;
using namespace DeceptiveTools;
using namespace TimeCounters;
using  namespace  FiberNets;




	
#define  DEBUG_SAVE_MESSY  false



	

namespace MessyGA
{
	class  CMessyGene;//predefinition
	class  CMessyIndividual;//predefinition
	class  CMessyPattern;//predefinition
	class  CmGA;//predefinition



	#define  VGA_CT_CROSS_TYPE_NORMAL_SIZE_2	0
	#define  VGA_CT_CROSS_TYPE_NORMAL_SIZE_2_AND_DIFFRENCE_SIZE_3	1

	#define  VGA_PARAM_MAX_TIME			"max time"
	#define  VGA_PARAM_SWITCH_TIME			"switch time"
	#define  VGA_PARAM_PROBING_TIME			"probing time"
	#define  VGA_PARAM_JUXTA_GENERATIONS	"juxta generations"
	#define  VGA_PARAM_POP_SIZE			"population size"
	#define  VGA_PARAM_INFECTED_POP_SIZE	"infected pop size"
	#define  VGA_PARAM_PROB_CROSS		"crossing"
	#define  VGA_PARAM_STATIC_CROSS		"static crossing"
	#define  VGA_PARAM_CROSS_TYPE		"crossing type//0-tournament size 2; 1-tournament size 2 + chance for tournament size 3 with difference-fitness"
	#define  VGA_PARAM_CHANCE_FOR_3_PARENT_DIFFR_TOURNAMENT		"chance for size 3 diffrence fitness tournament//important ONLY for tournament type 1"
	#define  VGA_PARAM_GLUE_INFECTIONS	"glue infections"
	#define  VGA_PARAM_TEMPL_FITNESS_CHECK	"template fitness check"
	#define  VGA_PARAM_PATTERN_POOL_SIZE	"pattern pool size"
	#define  VGA_PARAM_SAME_PATTERN_CHECK	"the same pattern check"
	#define  VGA_PARAM_PREFERRED_PATTERN_LENGTH	"preferred pattern length"
	#define  VGA_PARAM_MINIMAL_PATTERN_LEN	"minimal pattern length"
	#define  VGA_PARAM_LEN_OR_ENTROPHY	"length or entrophy at pattern number check"

	#define  VGA_PARAM_USE_TEMPL_AT_VIRUS_INIT		"use templates at virus init"
	#define  VGA_PARAM_VIR_INIT_DIFFERENT_TO_TEMPLATE		"init viruses genes as opposite to competetive template"
	
	#define  VGA_PARAM_VIR_GENERATIONS		"virus generations"
	#define  VGA_PARAM_VIR_POP			"virus population"
	#define  VGA_PARAM_VIR_POP_RED		"virus population reduction"
	#define  VGA_PARAM_VIR_PROB_CUT		"virus prob cut"
	#define  VGA_PARAM_VIR_PROB_SPLICE	"virus prob splice"
	#define  VGA_PARAM_VIR_PROB_MUT		"virus prob mutation"
	#define  VGA_PARAM_VIR_REM_GENE		"virus prob rem gene"
	#define  VGA_PARAM_VIR_ADD_GENE		"virus prob add gene"


	#define  VGA_PARAM_VIRGIN_ROUNDS		"virginity rounds"
	
	#define  VGA_PARAM_TREE_CONST_METHOD		"tree construction (0-old, 1-new)"
	#define  VGA_PARAM_PARENT_LEVEL_MUT_RATIO		"parent/level mutation ratio"
	#define  VGA_PARAM_CT_ADD_BY_TURN			"number of ct added by each turn"
	#define  VGA_PARAM_CT_TAKE_BACK_ENABLED		"enable take backs"
	#define  VGA_PARAM_DIVIDE_VIRUS_AMONG_CT	"divide virus population among ct"
	#define  VGA_PARAM_ACTUALIZE_FENO_GENO_AFTER_TREE_CONST	"actualize fenotype and genotype after trees construction"
	#define  VGA_PARAM_CT_ADD_REMOVE_STRATEGY	"ct add remove strategy (0-classic; 1-new)"
	#define  VGA_PARAM_BAYESIAN_EFFECT	"Bayesian effect (0-none; 1-brutal; 2-clever)"
	#define  VGA_PARAM_SOLUTION_CONSTRUCTION_METHOD	"Solution constructon (0-normal; 1-fluent optimization)"
	#define  VGA_PARAM_SIMULATED_ANNEALING_MAX_ITER_NUMBER	"max iteration number"


	#define  VGA_PARAM_FMGA_POP_RED_RATE		"fmga primordial pop red rate"
	#define  VGA_PARAM_FMGA_POP_RED_PERIOD		"fmga primordial pop red period"
	#define  VGA_PARAM_FMGA_INDIV_TO_CHECK		"fmga primordial individuals to check"
	#define  VGA_PARAM_FMGA_THRESHOLD_MULTIPLE		"fmga primordial threshold multiple"

	#define  VGA_PARAM_TABU_PRO_REP	"ProRep"
	#define  VGA_PARAM_TABU_PRO_PATH	"ProPath"
	#define  VGA_PARAM_TABU_LIST_LIMIT_PERC	"TabuListLimitPercent"
	#define  VGA_PARAM_TABU_NO_IMPROV_LIMIT_PERC	"NoImprovementLimitPercent"
	#define  VGA_PARAM_TABU_WORSENING_FACTOR	"WorseningFactor"


	#define  VGA_PARAM_GRASP_lRestrictedCandidateListSize	"lRestrictedCandidateListSize"
	#define  VGA_PARAM_GRASP_dRestrictedCandidateListSizeFactor	"dRestrictedCandidateListSizeFactor"
	#define  VGA_PARAM_GRASP_iValueBasedCandidateListSize	"iValueBasedCandidateListSize"
	#define  VGA_PARAM_GRASP_lAcceptableDistanceFromOptimal	"lAcceptableDistanceFromOptimal"
	#define  VGA_PARAM_GRASP_lLocalSearchMaxIterations	"lLocalSearchMaxIterations"
	#define  VGA_PARAM_GRASP_dLocalSearchCoolingRateFactor	"dLocalSearchCoolingRateFactor"
	#define  VGA_PARAM_GRASP_dLocalSearchNrOfDemandsToSwapFactor	"dLocalSearchNrOfDemandsToSwapFactor"
	#define  VGA_PARAM_GRASP_lLocalSearchNrOfDemandsToSwap	"lLocalSearchNrOfDemandsToSwap"
	#define  VGA_PARAM_GRASP_dLocalSearchInitialTemperature	"dLocalSearchInitialTemperature"
	#define  VGA_PARAM_GRASP_dLocalSearchInitialDistanceFromOptimal	"dLocalSearchInitialDistanceFromOptimal"
	#define  VGA_PARAM_GRASP_dLocalSearchInitialProbabilityOfAcceptingWorseSolution	"dLocalSearchInitialProbabilityOfAcceptingWorseSolution"

	

	#define  SGA_PARAM_POP_SIZE		"simple GA popsize"
	#define  SGA_PARAM_PROB_CROSS	"simple GA cross"
	#define  SGA_PARAM_PROB_MUT		"simple GA mut"

	#define  BAYESIAN_EFECT_NONE		0
	#define  BAYESIAN_EFECT_BRUTAL		1
	#define  BAYESIAN_EFECT_CLEVER		2

	#define  SOLUTION_CONSTRUCTION_METHOS_NORMAL	0
	#define  SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION	1
	#define  SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FLUENT_OPTIMIZATION	2
	#define  SOLUTION_CONSTRUCTION_METHOS_FLUENT_OPTIMIZATION_BOTH	3
	#define  SOLUTION_CONSTRUCTION_METHOS_SIMPLE_FIRST_FULL_LATER	4
	#define  SOLUTION_CONSTRUCTION_METHOS_MIX	5
	#define  SOLUTION_CONSTRUCTION_METHOS_ONLINE_SWITCHING	6



	#define  TABU_MOVE_TYPE_NO_MOVE	0
	#define  TABU_MOVE_TYPE_DEMAND	1
	#define  TABU_MOVE_TYPE_PATH	2
	#define  TABU_MOVE_TYPE_REPLICA	3


	class  CMessyGene;//predefinition
	class  CMessyIndividual;//predefinition
	class  CMessyPattern;//predefinition
	class  CmGA;//predefinition


	class CTabuMove
	{
	public:

		CTabuMove()  {i_move_type = TABU_MOVE_TYPE_NO_MOVE;}

		CTabuMove(int  iDemandForPathChange, CFiberPath  *pcPathToChange)  
		{
			i_move_type = TABU_MOVE_TYPE_PATH; 
			i_demand_for_path_change = iDemandForPathChange; 
			pc_path_to_change = pcPathToChange;
		}//CTabuMove(int  iDemandForPathChange, CFiberPath  *pcPathToChange)  


		CTabuMove(int  iMoveType, int  iPar0, int iPar1)  
		{
			
			if  (iMoveType == TABU_MOVE_TYPE_REPLICA)
			{
				i_move_type = TABU_MOVE_TYPE_REPLICA; 
				i_demand_for_replica_change = iPar0; 
				i_new_replica = iPar1;
			}//if  (iMoveType == TABU_MOVE_TYPE_REPLICA)
		}//CTabuMove(int  iMoveType, int  iPar0, int iPar1)  


		bool operator==(const CTabuMove& wzor)
		{
			if  (i_move_type != wzor.i_move_type)  return(false);

			if  (i_move_type == TABU_MOVE_TYPE_PATH)
			{
				if  (i_demand_for_path_change != wzor.i_move_type)  return(false); 
				if  (pc_path_to_change != wzor.pc_path_to_change)  return(false); 
				return(true);			
			}//if  (i_move_type == TABU_MOVE_TYPE_PATH)


			if  (i_move_type == TABU_MOVE_TYPE_REPLICA)
			{
				if  (i_demand_for_replica_change != wzor.i_demand_for_replica_change)  return(false); 
				if  (i_new_replica != wzor.i_new_replica)  return(false); 
				return(true);			
			}//if  (i_move_type == TABU_MOVE_TYPE_PATH)


			if  (i_move_type == TABU_MOVE_TYPE_DEMAND)
			{
				if  (i_demand_0 != wzor.i_demand_0)  return(false); 
				if  (i_demand_1 != wzor.i_demand_1)  return(false); 
				return(true);			
			}//if  (i_move_type == TABU_MOVE_TYPE_PATH)

			return(false);
		}//bool operator==(const TabuMove& wzor)

		void  vApply(CmGA  *pcCurSolution, CmGA  *pcCandidateSolution, CFiberSolution  *pcFitness, gcroot<Random*> pcRandomGen);


	//private:
		int  i_move_type;

		//TABU_MOVE_TYPE_DEMAND
		int  i_demand_0;
		int  i_demand_1;

		//TABU_MOVE_TYPE_PATH
		int  i_demand_for_path_change;
		CFiberPath  *pc_path_to_change;

		//tabu move replica
		int  i_demand_for_replica_change;
		int  i_new_replica;
	
	};//class CTabuMove



	
	
	class  CVirusGA
	{
	friend  class  CmGA;// for ::eVirGeneratePopulation(int  iVirusInitLength,  CVirusGA  *pcVGA);
	public:
		CVirusGA();
		~CVirusGA();


		CError  eRunGRASP
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
			);

		CError  eRunTabuSearch
			(
			double  dMaxTime, double  dSimpleTime, 
			CString  sReportFile,  
			CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, 
			double dProRep, double dProPath,
			double dTabuListLimitPercent, double dNoImprovementLimitPercent, double dWorseningFactor,
			System::Windows::Forms::ListBox*  listInfo
			);
		CError  eRunSimulatedAnnealing(double  dMaxTime, double  dProbingTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, System::Windows::Forms::ListBox*  listInfo);
		CError  eRunRandomSearch(double  dMaxTime, double  dProbingTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, System::Windows::Forms::ListBox*  listInfo);
		CError  eRunSimpleGA(double  dMaxTime, double  dProbingTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, System::Windows::Forms::ListBox*  listInfo);
		void  vSgaSaveParameters(FILE  *pfReport);

		CError  eRun(double  dMaxTime, double  dSimpleTime, double  dProbingTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, double  *pdOptimalValue, System::Windows::Forms::ListBox*  listInfo);
		CError  eSgaLoadSettings(FILE  *pfSettings);
		CError  eRandomSearchLoadSettings(FILE  *pfSettings);
		CError  eTabuSearchLoadSettings(FILE  *pfSettings);
		CError  eSimulatedAnnealingLoadSettings(FILE  *pfSettings);
		CError  eLoadSettings(FILE  *pfSettings);
		CError  eSetFitness(CFiberSolution  *pcFitnessFunc);
		
		void  vSaveParameters(FILE  *pfReport);
	private:

		//simple GA
		CmGA*  pc_sga_get_best_individual(vector  <CmGA  *>  *pvPopulation);
		CmGA  *pc_sga_get_parent_roulette(double  dFitnessSum);
		void  v_create_simple_ga_pop(vector  <CmGA  *>  *pvPopulationToMake);

		int  i_sga_pop_size;
		double  d_sga_cross_prob;
		double  d_sga_mut_prob;


		//TABU
		CTabuMove  cGenerateMoveIntelligent
			(
			vector<CTabuMove> *pvTabuList,	vector<CTabuMove> *pvUsedMoves, CmGA *pcSolution, double dProRep, double dProPath
			);
		CTabuMove  cGenerateMoveRandom(vector<CTabuMove> *pvTabuList,	vector<CTabuMove> *pvUsedMoves, CmGA *pcSolution);

		int  iGetFirstOffsetInEmployedPathsForTabu(int iStartNode, int  iEndNode);
		bool IsTabu(CTabuMove *pcMove, vector<CTabuMove> *pvTabuList, vector<CTabuMove> *pvUsedMoves, CmGA *pcSolution);

		//PL:GRASP:definitions
		//TODO: here place some GRASP functions

		//MuPPetS

		void  v_pattern_number_control_new();
		void  v_get_new_patterns();
		void  v_get_new_pattern_proposition_into_pattern_pool(CmGA  *pcCT);
		void  v_pattern_debug_control(int  iNum, bool bBuf);
		void  v_add_new_ct(System::Windows::Forms::ListBox*  listInfo, double  *pdAverage, double *pdBestFitness);
		void  v_remove_the_same_ct();
		void  v_remove_all_ct_except_best();
		bool  b_the_same_templates_check(CMessyIndividual  *pcTemplCandidate);

		CError  e_load_init_solution(CString sReferenceSolution);

		CMessyPattern*  pc_get_random_pattern();
		CmGA*  pc_get_parent_tournament(bool  bBetterTakesAll  =  false);
		CmGA*  pc_get_parent_tournament_normal(int  iTournamentSize,  bool  bBetterTakesAll  = false);
		CmGA*  pc_get_parent_tournament_normal(int  iIndiv1Offset,  int  iIndiv2Offset,  bool  bBetterTakesAll  =  false);
		CmGA*  pc_get_parent_tournament_normal_for_pattern_different(CMessyPattern  *pcPattern,  CmGA  *pcParentTaker);

		void  v_switch_coding_of_cm_GA(CString  sName, CmGA *pcSolution, System::Windows::Forms::ListBox*  listInfo);

		gcroot<Random*> pc_random_gen;
		CFiberSolution  *pc_fitness;

		//data
		int  i_pattern_max_number;
		int  i_the_same_template_check;
		int  i_minimal_template_length;

		bool  b_use_templates_at_virus_pop_init;//when true the viruses are inited using one of templates
		int  i_templ_length;
		
		int  i_virus_gen;
		int  i_virus_pop_size;
		double  d_vir_prob_cut;
		double  d_vir_prob_splice;
		double  d_vir_prob_mut;
		double  d_prob_mut_rem_gene;
		double  d_prob_mut_add_gene;

		int  i_virginity_rounds;
		int  i_bayesian_effect;
		int  i_solution_construction_method;

		int  i_sa_max_iteration;


		//steering data
		int  i_ct_add_by_turn;  //number of ct added each time
		int  i_ct_take_back_enable;//0-not enabled,  1-take back if fitness dropped after virus population run
		int  i_divide_virus_population_among_ct;  //0- do not divide viruses among ct(always run all cts with the same virus number)  1-divide virus population
		int  i_actualize_feno_and_genotype_after_tree_construction;
		int  i_ct_add_remove_strategy;//0-old strategy   1-new (leave only 1 best ct after best ct fit value improves)


        vector  <CmGA  *>  *pv_population;

		//crossing templates...
		vector  <CMessyPattern  *>  *pv_pattern_pool;

		int  i_seed;
	
	};//class  CVirusGA

	

	class  CmGA
	{
	friend class CVirusGA;
	friend class CTabuMove;
	friend class CGrasp;
	friend class CGraspSolution;
	public:
		CmGA(gcroot<Random*> pcRandomGen, int  iBayesianEffect, int iSolutionConstructionMethod);
		~CmGA();

		CError  eConfigureTemplate();
		CError  eSetTemplate(CMessyGene  *pcCompetetiveTemplate,  int  iBitLength);

		CError  eMuppetsVirRun
			(
			vector  <CMessyPattern  *>  *pvInfectionHistory,  CVirusGA  *pcVGA,  int  *piInitDone, 
			int iRunVirusesOnlyAfterCTImprove, int iPatternExtractionBasedOnLocalOpt,
			System::Windows::Forms::ListBox*  listInfo = NULL
			);

		CError  eConfigure
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
			);


		CError  eVirGeneratePopulation(int  iVirusInitLength);
		CError  eVirGeneratePopulation(CVirusGA  *pcVGA, int  *piPopSize, bool bCleverPaths);
		CError  eVirGenerateRandomTemplate();
		CError  eVirGenerateRandomSwap();
		CError  eVirConfigure
			(
			int  iVirusGen,  int  iVirusPopSize,
			double  dVirProbCut,  double  dVirProbSplice,  double  dVirProbMut,
			double  dVirAddGene, double  dVirRemGene
			);

		void  vCopyTo(CmGA*  pcDest);
		CmGA*  pcSgaClone();
		void  vSgaCrossSinglePoint
			(
			CmGA  *pcFather,
			CmGA  **pcChild1, CmGA  **pcChild2,
			double  dCrossProbability
			);

		void  vSgaMutate();

		void  vCross
			(
			CmGA  *pcOther,
			CmGA  *pcChild1, CmGA  *pcChild2,
			CMessyIndividual  *pcTemplate
			);
		void  vCrossStatic
			(
			CmGA  *pcOther,
			CmGA  *pcChild,
			CMessyIndividual  *pcTemplate
			);

		CMessyIndividual  *pcGetLastInfection()  {return(pc_best_found);};
//		vector  <CMessyIndividual  *>  *pvGetInfections()  {return(&v_best_found);};
		CMessyIndividual  *pcGetInfectionHistory()  {return(pc_infection_history);};

		int  iGetMaximumSlot();
		double  dGetCompTemplateFitness();
		double  dGetCompTemplateFitnessBeforeCrossing()  {return(d_templ_fittness_before_crossing);};

		bool  bIsTheSame(CMessyPattern  *pcPattern, CmGA  *pcOther);
		bool  bIsTheSame(CmGA  *pcOther);
		int  iGetNumberOfDiffrGenes(CMessyPattern  *pcPattern,  CmGA  *pcOther);
		int  iGetNumberOfDiffrGenes(CmGA  *pcOther);

		
		CError  eSetFitness(CFiberSolution  *pcFitnessFunc, bool  bSGAMode = false);


        CError  eRun(CString  sReportFile,  vector  <CMessyPattern  *>  *pvBestFound, vector  <int> *pvBBLengths);
		CError  eRunFastMessyGa
			(
			vector  <int> *pvBBLengths, int  iRandomTemplateStart,
			CString  sReportFile,  CString  sSummarizedReport,  System::Windows::Forms::ListBox*  listInfo
			);
		CError  eReportTransparent(FILE  *pfReport);
		CError  eReport(FILE  *pfReport);
		CError  eReportShort(FILE  *pfReport);
		CError  eReportIndividuals(FILE  *pfReport);
		int  iCountGenes();
		//int  iGetUnitation();


		CMessyIndividual  *pcGetPatternProposition()  {return(pc_pattern_proposition);}
		void  vRemovePatternProposition();

		CError  eVirRun(double  dMaxTime, CString  sReportFile,  CString  sSummarizedReport,  CString sReferenceSolution, System::Windows::Forms::ListBox*  listInfo);

		CError  eLoadSettings(FILE  *pfSettings);

		bool  bGetVirgin()  {return(b_virgin_init);};

		void  vSetInfectionHistory(CMessyIndividual  *pcInfectionHistory);

		CError  eMakeGentoypeFromSolution(CFiberSolution *pcSolution);
		CError  eMakePreciseGentoypeFromFluent();

		CError  eSwitchToFull(double  *pdAfterSwitch,  double *pdRestoring);
		CError  eSwitchToSimple();

	private:

		CError  e_run_iteration_fast_mga
				(
				FILE  *pfReport,  int  iIterationNum,  vector  <CMessyPattern  *>  *pvBestFound, 
				double  dReductionRate,  int  iReductionPeriod,
				System::Windows::Forms::ListBox*  listInfo
				);
		CError  e_run_iteration(FILE  *pfReport,  int  iIterationNum, vector  <CMessyPattern  *>  *pvBestFound);
		CError  e_primordial_phase_fast_mga(int  *piGenNum,  int  iBBLength,  double  dReductionRate,  int  iReductionPeriod,  System::Windows::Forms::ListBox*  listInfo);
		CError  e_primordial_phase(int  *piGenNum,  int  iIterationNum);
		CError  e_juxtapositional_phase(int  *piGenNum,  vector  <CMessyPattern  *>  *pvInfectionHistory, int  iPatternExtractionBasedOnLocOpt = 0);
		void  v_take_back_ct_genotype_changes();

		void	v_fill_generator
			(
			vector  <CMessyGene  *>  *pvGenerator,  
			int  iIterationNum,  
			int  iCurrentBit,  int  iFilledBits
			);

		bool  b_repair_necessary_();

		void  v_generate_genotype_fmga(vector <CMessyGene  *>  *pvGenotype,  int iVirusInitLength);
		double  d_compute_threshold(int  iCurVirLength);

		void  v_shuffle_population();
		void  v_find_best_indiv();
		void  v_infect_competetive_template(CMessyIndividual  *pcBestFound);
		void  v_try_pattern_and_template_insert(CVirusGA  *pcVGA,  int  *piInitDone, int iCrossOnlyIfImproves);

		CMessyIndividual  *pc_get_parent_tournament(int  iIndiv1Offset,  int  iIndiv2Offset,  bool  bBetterTakesAll  =  false);
		CMessyIndividual  *pc_get_parent_tournament(int  iTournamentSize,  bool  bBetterTakesAll  =  false);
		

		//random number generator
		gcroot<Random*> pc_random_gen;

		//data
		CMessyGene  *pc_competetive_template;
		CMessyGene  *pc_ordered_template;
		int  i_templ_length;

		int  i_bayesian_effect;
		int  i_solution_construction_method;

		double  d_prob_cut,  d_prob_splice;
		double  d_prob_mut;
		double  d_prob_mut_rem_gene;
		double  d_prob_mut_add_gene;


		double  d_fmga_primordial_red_rate;
		int		i_fmga_primordial_red_period;
		int		i_fmga_primordial_number_of_indiv_to_check;//the number of individuals checked if they are similar enough before tournament
		double  d_threshold_multiplier;

		int  i_generations;

		int  i_prim_gen_without_pop_red;//population is not reduced
		int  i_prim_gen_reduce_pop_every_2nd;//population reduced every other generation
		int  i_prim_gen_sized_but_not_reduced;//population is sized at iPopSize, but primordial phase is still going on


		int  i_pop_size;
		double  d_pop_reduction_primordial;//factor for primordial phase
		double  d_pop_reduction_juxtapositional;//factor for juxtal but only in VmGA phase

		double  d_templ_fitness;
		double  d_templ_fittness_before_crossing;//used for remembering the fittness before crossing 2 CmGA individuals; SET OUTSIDE, BY CVirusGA
		CMessyIndividual  *pc_best_found;
		CMessyIndividual  *pc_infection_history;

		CFiberSolutionChannel  *pc_ct_channels;
		CFiberSolutionChannel  *pc_best_virus_channels;
		CMessyIndividual  *pc_pattern_proposition;

		//used for remembering the state before the pattern * template insertion
		double  d_templ_fittness_before_insert;
		CMessyGene  *pc_competetive_template_before_insert;

		//used for remembering the state before switching full to simple
		double  d_templ_fittness_full_before_switch_to_simple;
		CMessyGene  *pc_competetive_template_before_switch_from_full_to_simple;
		

		bool  b_virgin_init;//for the first time viruses are inited without using any other information (like other cts or patterns)
		
		vector  <CMessyIndividual  *>  *pv_population;
		CFiberSolution *pc_fitness;
		
	};//class  CmGA



	class  CMessyIndividual
	{
	friend class CVirusGA;
	friend class CmGA;
	public:
		CMessyIndividual(CFiberSolution  *pcFitness, int  iBayesianEffect, int iSolutionConstructionMethod);
		~CMessyIndividual();

		void  vCopyOrderedFenotype(CMessyGene  *pcDest);
		double  dComputeFitness(CMessyGene  *pcCompetetiveTemplate, int  *piSolutionRepaired = NULL, bool bSelfUpgradeToPreciseEncoding = false);
		void  vInfect(CMessyGene  *pcCompetetiveTemplate);
		
		void  vSetGenotype(vector  <CMessyGene  *>  *pvNewGenotype);
		void  vAddGenotype(vector  <CMessyGene  *>  *pvNewGenotype);
		void  vFlushGentoype();
		void  vFlushFenotype();
		void  vRemoveUnimportantGenes(CMessyGene  *pcCompetetiveTemplate);


		void  vCheckTemplate_(CMessyGene  *pcCompetetiveTemplate);
		CError  eReport(FILE  *pfReport,  CMessyGene  *pcCompetetiveTemplate);
		
		CError  eReportFenotype
			(
			FILE  *pfReport,
			CMessyGene  *pcCompetetiveTemplate
			);

		void  operator=(CMessyIndividual  &cOther);

		CError  eCut
			(
			double  dCutProb,  
			gcroot<Random*> pcRandomGen,  
			CMessyIndividual  *pcPart1,  CMessyIndividual  *pcPart2,
			int  *piReturnedPartsNumber
			);

		void  vSplice(CMessyIndividual  *pcSplicedIndividual);
		CError  eSplice //this individual will be a splice result...
			(
			double  dSpliceProb,  
			gcroot<Random*> pcRandomGen,  
			CMessyIndividual  *pcSplicedIndividual,
			bool  *pbSpliced
			);

		void  vMutateRemGene(double  dProbMutRemGene,  gcroot<Random*> pcRandomGen);
		void  vMutate(double  dProbMut,  gcroot<Random*> pcRandomGen);
		void  vMutateAddGene(double  dProbMutAddGene,  gcroot<Random*> pcRandomGen,  int  iTemplLength);

		void  vRemoveRandomGene(gcroot<Random*> pcRandomGen);


		vector  <CMessyGene  *>  *pvGetGenotype()  {return(&v_genotype);};
		CMessyGene  *pcGetFenotype();

		int  iGetFenotypeLength();

		double  dGetPatternEntrophyFittnes(int  *piGeneFreqTable);
		void  vSetFitness(CFiberSolution  *pcFitness);
		
		bool  bCompareFenotypes(CMessyIndividual  *pcOther);

	protected:

		double  d_compute_fitness(CMessyGene  *pcCompetetiveTemplate, int  *piSolutionRepaired = NULL, bool bSelfUpgradeToPreciseEncoding = false);
		void  v_construct_solution(CMessyGene  **pcOrderedFenotype, int iSolutionConstructionMethod, int  *piSolutionRepaired = NULL, CMessyGene  *pcCompetetiveTemplateToInfect = NULL, CMessyGene  *pcGenotypeToSelfPrecisionUpgrade = NULL);
		void  v_construct_solution_old(CMessyGene  **pcOrderedFenotype, int  *piSolutionRepaired = NULL, CMessyGene  *pcCompetetiveTemplateToInfect = NULL);
		void  v_construct_solution_with_fluent_optimization(CMessyGene  **pcOrderedFenotype, int  *piSolutionRepaired = NULL, CMessyGene  *pcCompetetiveTemplateToInfect = NULL);
		void  v_construct_solution_with_fluent_optimization_simple(CMessyGene  **pcOrderedFenotype, int  *piSolutionRepaired, CMessyGene  *pcCompetetiveTemplateToInfect = NULL, CMessyGene  *pcGenotypeToSelfPrecisionUpgrade = NULL);
		
		void  v_construct_solution_order_queue(CMessyGene  *pcCompetetiveTemplate);

		void  v_create_fenotype();

		
		CFiberSolution  *pc_fitness;

		vector  <CMessyGene  *>  v_genotype;
		CMessyGene  **pc_fenotype;
		CMessyGene  **pc_ordered_fenotype;
		
		int  i_global_feno_len;

		bool  b_fenotype_actual;

		double  d_fitness;

		int  i_bayesian_effect;
		int i_solution_construction_method;

	};//class  CMessyIndividual


	
	class  CMessyPattern  :  public  CMessyIndividual
	{
	public:
		CMessyPattern(CFiberSolution  *pcFitness);
		~CMessyPattern();

		void  operator=(CMessyIndividual  &cMessyIndiv);
		void  operator=(CMessyPattern  &cOther);

		bool operator<(CMessyPattern  &cOther)  {return(i_pattern_multiple  <  cOther.i_pattern_multiple);};
		bool operator>(CMessyPattern  &cOther)  {return(i_pattern_multiple  >  cOther.i_pattern_multiple);};
		bool operator==(CMessyPattern  &cOther)  {return(i_pattern_multiple  ==  cOther.i_pattern_multiple);};

		int  iGetMultiple()  {return(i_pattern_multiple);};
		void  vIncMultiple()  {i_pattern_multiple++;};

		bool  bGetNew()  {if (i_old  ==  0) return(true);  return(false);};
		void  vSetOld()  {i_old++;};

		CError  eReport(FILE  *pfReport,  bool  bTemplate  =  false);
		CError  eReport
			(
			FILE  *pfReport,
			int  *piCompetetiveTemplate,  CFiberSolution  *pcFitness
			);

	private:

		int  i_pattern_multiple;
		int  i_old;
	
	};//class  CMessyPattern  :  public  CMessyIndividual



	class  CMessyGene
	{
	friend class  CmGA;
	friend class  CMessyIndividual;
	friend class  CTabuMove;
	friend class  CGrasp;
	friend class CGraspSolution;
	public:
		CMessyGene();
		CMessyGene(int  iDemandNumber,  double  dOrder, CFiberPath  *pcPath);
		CMessyGene(CMessyGene  *pcOther);
		~CMessyGene();

		double dGetOrder()  {return(d_order);}
		CFiberPath  *pcGetPath()  {return(pc_path);}
		int  iGetDemNumber()  {return(i_demand_number);}

		bool operator==(CMessyGene  &cOther);
		bool operator!=(CMessyGene  &cOther);
		
		void  vReportGene(FILE  *pfDest);

	private:

		int  i_demand_number;
		double  d_order;
		CFiberPath  *pc_path;
	};//class  CMessyGene

	class CRankedSelection;

	/**
	 * Provides tools required to run grasp algorithm
	 */
	class CGrasp
	{
	friend class CmGA;
	
	public:
		CGrasp(CFiberSolution *pc_fitness, gcroot<Random*> pcRandomGen, CRankedSelection *rankedSelection);
		/**
		 * fill solution with MSF demands order
		 */
		void vFillSolutionWithMostSubcarriersFirst(CGraspSolution * solution);
		/**
		 * reconstructs provided solution with fixed size RGC
		 */
		void vRandomGreedyConstruction(CGraspSolution &solution, const int &rclSize);

		inline void logToDebugFile(const CString &sBuf);
		inline void logToDebugFile(const int &value);
		inline void vSetDebug(bool debug) {this->debug = debug;};
		inline bool iGetDebug() {return debug;};
		inline int iGetNrOfDemands() {return nrOfDemands;};
	private:
		CFiberSolution *pcFitness;
		gcroot<Random*> pcRandomGen;
		CRankedSelection * rankedSelection;
		int nrOfDemands;

		 // logging tools
		static const CString sLogDebugFile; // where to store debug info
		CString sBuf; // buffer for logging
		bool debug;
		void vClearReportFile(); // clear the debug file
	};//class CGrasp

	/**
	 * Represents single Grasp solution (demands order).
	 * Sth between facade and adapter, however iface is missing.
     * 
	 */
	class CGraspSolution
	{
	public:
		CGraspSolution(gcroot<Random*> pcRandomGen, int  iBayesianEffect, int iSolutionConstructionMethod, CFiberSolution *pcFitness);
		~CGraspSolution();

		inline void vSetOrderForDemand(const int &demandId, const double &order);
		inline void vSetPathIdForDemand(const int &demandId, const int &pathId);

		void vRandomSwap();
		void vSwapDemands(const vector<int> &lIndices, const vector<int> &rIndices);
		void vSwapDemands(vector<int>::iterator &lIndicesIt, vector<int>::iterator &lIndicesEndIt, vector<int>::iterator &rIndicesIt, vector<int>::iterator &rIndicesEndIt);
		void vSwapDemands(const int &idx1, const int &idx2);
		/**
		 * call to calculate {@link orderOfDemandIds};
		 * After random swaps based on indices or vector, this operation is not required
		 */
		void vEvaluateOrderOfDemands(); 
		inline void vCopySolutionTo(CGraspSolution *destination); // copy one solution to the other one
		
		inline double dGetFitness() {return 1 / solution->dGetCompTemplateFitness();};
		inline void vClearFitness() {solution->d_templ_fitness = -1;};
		inline CmGA * pcGetSolution() {return solution;};
		inline vector<int> * pcGetOrderOfDemandIds() {return &orderOfDemandIds;};

		// debug tools, do not use it as source of hash tables
		int iGetSolutionOrderHash();
		CString sGetSolutionOrderAsString();

		CFiberSolution  *pcGetFiberSol()  {return(pcFitness);}
	private:
		CmGA *solution;
		CFiberSolution *pcFitness;
		gcroot<Random*> pcRandomGen;
		int nrOfDemands;

		vector<int> orderOfDemandIds;
	};//class CGraspSolution

	/**
	 * Selects k distinct indices from list
	 */
	class CPermutationRandomGenerator
	{
	public:
		/**
		 * static factory
		 */
		static CPermutationRandomGenerator * pcGetInstance(gcroot<Random*> pcRandomGen, const int &upperBound);
		/**
		 * calls destructor on every item in cache
		 */
		static void vClearCache();

		/**
		 * returns pointer to new vector with kSize random distinct integers within range [0, size), works as subview in java
		 */
		vector<int>* pvGenerateKDistinctIntegers(const int & kSize);
	private:
		/**
		 * constructors intentionally private, use static factory
		 */
		CPermutationRandomGenerator(CPermutationRandomGenerator *other);
		CPermutationRandomGenerator(gcroot<Random*> pcRandomGen, int size);

		static map<int, CPermutationRandomGenerator *> cache;
		gcroot<Random*> pcRandomGen;
		const int size;
		// vector which is permuted to create k-distinct integers numbers from size [0, size)
		vector<int> consecutiveIntegers;
	};//class CPermutationRandomGenerator

	/**
	 * Calculates indices according to rank selection,
	 * i.e., index 0 has the highest probability.
	 * e.g., if there are 5 elements, element with indices:
	 * 0 has probability 5/15
	 * 1 has probability 4/15
	 * 2 has probability 3/15 etc.
     * Singleton pattern.
	 */
	class CRankedSelection
	{
	public:
		static CRankedSelection* newInstance(gcroot<Random*> pcRandomGen);
		static void deleteRankedSelection();

		int selectIndex(const int &numberOfElements);
		template <typename T> T selectElement(vector<T> elements);
	private:
		static CRankedSelection * instance;
		gcroot<Random*> pcRandomGen;
		vector<int> sumOfNIntegers;

		CRankedSelection();
		void extendSumIfNeeded(const int &requiredIndex);
		void extendSumOfNIntegers(const int &newSize);
		int binarySearchLower(vector<int> &vec, const int &searchValue);
		int binarySearchLower(vector<int> &vec, const int &startIdx,
			const int &endIdx, const int &searchValue);
	};//class CRankedSelection;

};//namespace MessyGA









