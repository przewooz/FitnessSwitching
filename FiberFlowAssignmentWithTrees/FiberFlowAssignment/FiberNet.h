#pragma once


//tools
#include  "atlstr.h"  //CString
#include  "..\error\error.h"
#include  <iostream>
#include  <vector>
#include  <functional>
#include  <algorithm>

//for randomizer...
#using <mscorlib.dll>
using namespace System;



//time counter
#include  "..\util\timer.h"
//..and other tools...
#include  "..\util\tools.h"


using namespace std;
using namespace TimeCounters;


#define  SOLUTION_PRW_DEMAND_MARKER	"demand"
#define  SOLUTION_UNICAST_DEMAND_MARKER	"demand"
#define  SOLUTION_ANYCAST_DEMAND_MARKER	"anycastdemand"

#define  DEMAND_ANYCAST_EXT	"dea"
#define  DEMAND_UNICAST_EXT	"dem"

#define  DEMAND_ANYCAST_1_DIRECTION_UP	"up"
#define  DEMAND_ANYCAST_1_DIRECTION_DOWN	"dn"

#define  DEMAND_TYPE_UNICAST	1
#define  DEMAND_TYPE_ANYCAST	2
#define  DEMAND_TYPE_MULTICAST	3



namespace  FiberNets
{
	class  CFiberNode;//predefinition
	class  CFiberConnection;//predefinition
	class  CFiberConnectionOld;//predefinition
	class  CFiberConnectionCores;//predefinition
	class  CFiberDemand;//predefinition
	class  CFiberPath;//predefinition
	class  CFiberSolution;//predefinition
	class  CFiberSolutionChannel;//predefinition


	class  CFiberNet
	{
	friend class CFiberNode;
	friend class CFiberConnection;
	friend class CFiberConnectionOld;
	friend class CFiberConnectionCores;
	friend class CFiberDemand;
	friend class CFiberPath;
	friend class CFiberSolution;
	public:
		CFiberNet();
		~CFiberNet();


		CError  eSetCores(int  iCores);

		double  dCompCost();
		CError  eResetNet();
		bool  bCheckIfReplicaNode(CFiberNode  *pcNode);
		bool  bCheckIfReplicaNode(int  iNodeId);
		bool  bCompareNodes(int  iNodeId0, int iNodeId1);

		
		CError  eLoadTopology(CString  sNetFile);
		CError  eLoadReplicas(CString  sRepFile);
		CError  eLoadDemands(CString  sUnicastFile, CString  sAnycastFile, CString sMulticastFile, CString  sAnycastFile1Direction, int  iPathsToUse, int  iTreesToUse);
		
		CError  eLoadPaths(CString  sPathFile, CString  sPathBackupFile);
		CError  eLoadPathsSpec(CString  sUniSpecFile, CString  sUniSpecFileBackup, double  dTransferMultiplier);
		CError  eLoadPathsReg(CString  sUniRegFile);
		CError  eLoadModulation(CString  sModFile);

		CError  eLoadTrees(CString  sTreeFile);
		CError  eLoadTreesSpec(CString  sTreeSpecFile, double  dTransferMultiplier);
		CError  eLoadTreesReg(CString  sTreeRegFile);

		
		CError  eSaveTopology(CString  sNetFile);
		CError  eSaveDemands(CString  sDemFile);
		CError  eSavePaths(CString  sPathFile);
		CError  eSavePathsSummarized(CString  sPathFile);
		CError  eSaveTrees(CString  sTreeFile);
		CError  eSaveModulation(CString  sModFile);
		CError  eSaveReplicas(CString  sRepFile);

		CError  eSavePathsSpec(CString  sPathSpecFile);
		CError  eSavePathsReg(CString  sPathRegFile);
		CError  eSaveTreesSpec(CString  sTreeSpecFile);
		CError  eSaveTreesReg(CString  sTreeRegFile);

		

		int  iGetDemandNumber()  {return(i_demand_num);};
		int  iGetDemandNumberUni()  {return(i_uni_demands);}
		int  iGetDemandNumberAny()  {return(i_any_demands);}
		int  iGetDemandNumberAny1Direction()  {return(i_any_demands_1_direction);}
		int  iGetDemandNumberMulti()  {return(i_multi_demands);}
		int  iGetReplicesNum()  {return(i_rep_num);}
		int  iGetConnsNum()  {return(i_conns_num);}
		int  iGetNodesNum()  {return(i_nodes_num);}
				

		CFiberNode  **pcGetReplices()  {return(pc_replices);}
		CFiberDemand *pcGetDemands()  {return(pc_demands);};
		CFiberPath  *pcPaths()  {return(pc_paths);};
		bool  bConstraintBroken()  {return(false);};

		CFiberConnection *pcGetConns()  {return(pc_conns);}

		vector<CFiberPath *>  *pvGetEmployedPathsForTabu()  {return(&v_paths_employed_for_tabu);}


		int  iGetPathNum()  {return(i_path_num);}
		
		vector<int>  **pvGetPathsBetweenNodes()  {return(pv_paths_between_nodes);};

	protected:

		int  i_cores;

		CFiberNode *pc_nodes;
		int  i_nodes_num;

		CFiberNode **pc_replices;
		int  i_rep_num;

		CFiberConnection *pc_conns;
		int  i_conns_num;
		
		CFiberConnection  ***pc_conn_mat;

		CFiberDemand  *pc_demands;
		int  i_demand_num;
		int  i_uni_demands;
		int  i_any_demands;
		int  i_multi_demands;
		int  i_any_demands_1_direction;

		CFiberPath  *pc_paths;
		int  i_path_num;

		vector<int>  **pv_paths_between_nodes;

		vector<CFiberPath *>  v_paths_employed_for_tabu;


		CFiberPath  *pc_trees;
		int  i_tree_num;

		//CFiberPath  **pc_tree_paths;

		CError  e_load_spec(CString  sSpecFile, CString  sUniSpecFileBackup, CFiberPath  *pcPaths, int  iPathNum, double  dTransferMultiplier);
		CError  e_load_reg(CString  sSpecFile, CFiberPath  *pcPaths, int  iPathNum);

		CError  e_save_spec(CString  sSpecFile, CFiberPath  *pcPaths, int  iPathNum);
		CError  e_save_reg(CString  sSpecFile, CFiberPath  *pcPaths, int  iPathNum);

	

		CError  e_load_uni_demands(FILE  *pfSource, int iMaxPathsToUseBetweenNodes);
		CError  e_load_any_demands(FILE  *pfSource);
		CError  e_load_multi_demands(FILE  *pfSource, int iMaxTreesToUseForNode);
		CError  e_load_any_demands_1_dir(FILE  *pfSource, int iMaxPathsToUseBetweenNodes);
	};//class  CFiberNet



	class  CFiberNode
	{
	friend class CFiberNet;
	friend class CFiberDemand;
	public:
		CFiberNode();
		~CFiberNode();

		void  vSetNodeNumber(int  iNodeNumber);

		void  vAddInConn(CFiberConnection  *pcConnection);
		void  vAddOutConn(CFiberConnection  *pcConnection);

		int  iGetNodeId()  {return(i_node_number);};


	private:
		CFiberConnection **pc_out_conns;
		CFiberConnection **pc_in_conns;

		int  i_node_number;

	};//class  CFiberNode



	/*class  CFiberConnection
	{
		friend class CFiberNet;
		friend class CFiberNode;
		friend class CFiberPath;

	public:
		CFiberConnection() {};
		virtual ~CFiberConnection() {};	

		virtual int  iGetMaxSlot() = 0;
		virtual void  vReset() = 0;

		virtual CError  eReserveSlots(int  iFirstSlot, int iSlotsNumber, bool  bErrorControl = true) = 0;
		virtual int  iGetLowestStartSlotForChannel(int  iFirstSlot, int iSlotsNumber) = 0;
		virtual int  iGetNumberOfFreeSlotsBelow(double  dMax) = 0;

		virtual  CError    eConfig(int  iConectionId, int  iFiberNodeStartOffset, int  iFiberNodeEndOffset, CFiberNet *pcParent, double  dConnectionCapacity) = 0;
		virtual void  vSaveStatePRW(FILE  *pfDest) = 0;
		virtual void  vSaveState(FILE  *pfDest, int iMaxSlotNumber) = 0;
		virtual bool  bSlotInUse(int iSlotNumber) = 0;

		
	protected:
		int  i_connection_id;
		int  i_fiber_node_start_offset;
		int  i_fiber_node_end_offset;
		CFiberNet *pc_parent;
		double  d_connection_capacity;
	
	};//class  CFiberConnection*/



	class  CFiberConnection
	{
	friend class CFiberNet;
	friend class CFiberNode;
	friend class CFiberPath;
	public:
		CFiberConnection();
		~CFiberConnection();	

		int  iGetMaxSlot();
		void  vReset();

		CError  eReserveSlots(int  iFirstSlot, int iSlotsNumber, bool  bErrorControl = true);
		int  iGetLowestStartSlotForChannel(int  iFirstSlot, int iSlotsNumber);
		int  iGetNumberOfFreeSlotsBelow(double  dMax);

		CError    eConfig(int  iConectionId, int  iFiberNodeStartOffset, int  iFiberNodeEndOffset, CFiberNet *pcParent, double  dConnectionCapacity, int iCores);
		void  vSaveStatePRW(FILE  *pfDest);
		void  vSaveState(FILE  *pfDest, int iMaxSlotNumber);
		bool  bSlotInUse(int iSlotNumber);

		
	protected:
		void  v_check_ranges_correctness();

		int  i_connection_id;
		int  i_fiber_node_start_offset;
		int  i_fiber_node_end_offset;
		CFiberNet *pc_parent;
		double  d_connection_capacity;

		int  i_cores;
		vector<int>  v_slots;//each int is one slot
		vector<int>  v_slots_full;//vector of pairs: (start-end)
	};//class  CFiberConnection



	class  CFiberConnectionOld
	{
	friend class CFiberNet;
	friend class CFiberNode;
	friend class CFiberPath;
	public:
		CFiberConnectionOld();
		~CFiberConnectionOld();	

		int  iGetMaxSlot();
		void  vReset();

		CError  eReserveSlots(int  iFirstSlot, int iSlotsNumber, bool  bErrorControl = true);
		int  iGetLowestStartSlotForChannel(int  iFirstSlot, int iSlotsNumber);
		int  iGetNumberOfFreeSlotsBelow(double  dMax);

		CError    eConfig(int  iConectionId, int  iFiberNodeStartOffset, int  iFiberNodeEndOffset, CFiberNet *pcParent, double  dConnectionCapacity);
		void  vSaveStatePRW(FILE  *pfDest);
		void  vSaveState(FILE  *pfDest, int iMaxSlotNumber);
		bool  bSlotInUse(int iSlotNumber);

		
	protected:

		int  i_connection_id;
		int  i_fiber_node_start_offset;
		int  i_fiber_node_end_offset;
		CFiberNet *pc_parent;
		double  d_connection_capacity;


		vector<int>  v_slots_in_use;//vector of pairs: (start-end)
	};//class  CFiberConnection



	class  CFiberDemand
	{
	friend class CFiberNet;
	friend class CFiberSolution;
	friend class CFiberSolutionChannel;
	public:
		CFiberDemand();
		~CFiberDemand();	

		vector  <CFiberPath  *>  *pvGetPossiblePaths()  {return(&v_possible_paths);};

		CError  eConfig
			(
			int  iDemandId, 
			int  iFiberNodeStartOffset, int  iFiberNodeEndOffset, double  dDemandCapacity, 
			CFiberNet *pcParent,
			int  iDemandType,
			CFiberDemand *pcAnycastTwinDemand, int  iMaxPathTreesAvailable = -1
			);
		void  vSave(FILE  *pfDest);

		void  vSummarizePathFitness();

		int  iDemandType()  {return(i_demand_type);}
		bool  bDominatingTwin();

		CFiberDemand  *pcGetTwin()  {return(pc_anycast_twin_dfemand);}

		int  iGetId()  {return(i_demand_id);}

		int  iGetStartNode()  {return(i_fiber_node_start_offset);}
		int  iGetEndNode()  {return(i_fiber_node_end_offset);}
		
		double dGetDemandCapacity() {return(d_demand_capacity);}
	protected:
		int  i_demand_id;
		int  i_fiber_node_start_offset;
		int  i_fiber_node_end_offset;
		CFiberNet *pc_parent;
		double d_path_fit_sum;

		vector  <CFiberPath  *>  v_possible_paths;

		CFiberDemand  *pc_anycast_twin_dfemand;
		int i_demand_type;
		double  d_demand_capacity;


		void  v_copy_offsets(vector  <int> *pvDest, vector<int>  *pvSource, int iMaxPathTreesAvailable);
	};//class  CFiberConnection




	class  CFiberPath
	{
		friend class CFiberNet;
		friend class CFiberSolution;
	public:
		CFiberPath();
		~CFiberPath();	


		void  vSetActive(bool  bNewActive)  {b_active = bNewActive;}
		bool  bGetActive()  {return(b_active);}

		void  vSetTree(bool  bNewTree)  {b_tree = bNewTree;}
		bool  bGetTree()  {return(b_tree);}

		void  vConfigure(int  iPathId, CFiberNet *pcParent);
		CError  eLoad(CString  sPathLine, bool  bControlConnectionsOrder = true);
		CError  eLoadMod(CString  sModLine);

		void  vSetTransferMultiplier(double  dNewTransferMultiplier) {d_new_transfer_multiplier = dNewTransferMultiplier;};
		CError  eGetSlotsNumberForTransfer(CString  sLine);
		CError  eGetRegNumberForTransfer(CString  sLine);
		

		void  vSave(FILE  *pfDest);
		void  vSaveWithLinks(FILE  *pfDest);
		void  vSaveMod(FILE  *pfDest);
		void  vSaveSpec(FILE  *pfDest);
		void  vSaveReg(FILE  *pfDest);


				

		int  iGetId()  {return(i_path_id);};

		bool  bIsConnPartOfPath(CFiberConnection  *pcFiberConn);

		CError  eFindAndSetPathSlots(int  *piFirstSlot, int  iSlotsNumber);
		CError  eFindPath(int  *piFirstSlot, int  iSlotsNumber);
		CError  eSetPath(int  iFirstSlot, int  iSlotsNumber);

		CError  eSetPathSlots(int  iFirstSlot, int  iSlotsNumber);
		CError  eSetPathCapacity(int  iFirstSlot, double  dDemandCapacity);
		int  iGetSlotsNumber(double  dDemandCapacity);

		void  vSetPathFitness(double  dSolutionValue);
		double  dGetPathFitness()  {return(d_path_fitness);}

		int  iGetStartNode()  {return(i_fiber_node_start_offset);}
		int  iGetEndNode()  {return(i_fiber_node_end_offset);}

		CFiberPath  *pcGetSlavePath()  {return(pc_slave_path);}
		
	protected:
		int  i_get_first_connection_offset(vector<CFiberConnection *>  *pvConnections);
		int  i_get_linked_connection_offset(vector<CFiberConnection *>  *pvConnections, CFiberConnection *pcMotherConnection);



		bool  b_active;
		bool  b_tree;
		int  i_path_id;
		int  i_fiber_node_start_offset;
		int  i_fiber_node_end_offset;

		//modulation
		double  d_length;
		double  d_mp;
		double  d_delta_s;

		CFiberNet *pc_parent;

		vector<CFiberConnection *>  v_path_connections;

		double  d_new_transfer_multiplier;
		vector<int>  v_slot_number_for_transfer;//increases every 10 Gbps
		vector<int>  v_reg_number_for_transfer;//increases every 10 Gbps


		double  d_path_fitness;


		CFiberPath  *pc_slave_path;
	};//class  CFiberPath



	class CFitnessPair
	{
	public:
		CFitnessPair() {d_fitness_level = -1; i_counter = 0;}
		double  d_fitness_level;
		int  i_counter;	
	};//class CFitnessPair

	class  CFiberSolution
	{
	public:
		CFiberSolution(CFiberNet  *pcFibNet);
		CFiberSolution(CFiberSolution  *pcFibNet);
		~CFiberSolution();


		static void  vInitStatic();


		void  vSetPathFitness();
		CFiberPath  *pcGetCleverPath(int  iDemandNumber, gcroot<Random*> pcRandomGen);
		CFiberPath  *pcGetBestCleverPath(int  iStartNode, int iEndNode);
		CFiberPath  *pcGetRandomPath(int  iDemandNumber, gcroot<Random*> pcRandomGen);


		CError  eLoadSolutionWalk(CString  sSource);
		CError  eLoadSolutionWalk(FILE  *pfSource);

		CError  eLoadSolutionAibin(CString  sSource);
		CError  eLoadSolutionAibin(FILE  *pfSource);

		CError  eLoadSolutionAFA(CString  sSource);
		CError  eLoadSolutionAFA(FILE  *pfSource);
		CError  eLoadSolution(CString  sSource);
		CError  eLoadSolution(FILE  *pfSource);
		CError  eLoadSolutionPRW(CString  sSource);
		CError  eLoadSolutionPRW(FILE  *pfSource);

		CError  eSaveSolution(CString  sSolDest);
		CError  eSaveSolution(FILE  *pfDest);


		double  dCompCost(bool  *pbConstraintBroken, bool  bNoPenalty = false);
		CFiberNet  *pcGetNet() {return(pc_fib_net);};
		CError  eFindAndSetChanel(int  iChanelOffset, CFiberPath  *pcPath);
		CError  eFindChanel(int  iChanelOffset, CFiberPath  *pcPath, int *piStartSlot, int  *piSlotsNumber);
		CError  eSetChanel(int  iChanelOffset, CFiberPath  *pcPath, int iStartSlot, int  iSlotsNumber);

		CError  eApplySolution();

		void  vCopyChannelsState(CFiberSolutionChannel  *pcChannelsDest);

		CFiberSolutionChannel  *pcGetChannels() {return(pc_channels);}

		static  double  dGetFuncEval()  {return(d_func_evaluations);};
		static  double  dGetCleverPathsEval()  {return(d_clever_paths_evaluations);};
		static  double  dGetCleverPathsInits()  {return(d_clever_paths_initializations);};
		static  double  dGetConstructionTime()  {return(d_construction_time);};
		static  void  vAddConstructionTime(double  dAdd)  {d_construction_time += dAdd;};

		static  double  dGetTimeTillBest()  {return(d_time_till_best);};
		static  double  dGetFfeTillBest()  {return(d_ffe_till_best);};
		static  double  dGetBest()  {return(d_best_solution);};

		static  double  dGetFullBetterThanSimple()  {return(d_func_evaluations_full_better_than_simple);}
		static  void  vIncFullBetterThanSimple() {d_func_evaluations_full_better_than_simple += 1;};
		
		static  void  vZeroEval()  {d_func_evaluations = 0; d_clever_paths_evaluations = 0; d_clever_paths_initializations = 0; d_construction_time = 0;};

		CString  sFitnessDiversity();
		void  vFitnessDiversitySave(CString  sDest);
		void  vFitnessDiversityReset();

		
	private:
		static  double  d_func_evaluations_full_better_than_simple;
		static  double  d_func_evaluations;
		static  double  d_clever_paths_evaluations;
		static  double  d_clever_paths_initializations;
		static  double  d_construction_time;
		double  d_last_raw_sol_value;

		static double  d_best_solution;
		static double  d_time_till_best;
		static double  d_ffe_till_best;
		static CTimeCounter  c_time_counter;


		vector<CFitnessPair> v_fitness_levels;

		CFiberNet  *pc_fib_net;
		CFiberSolutionChannel  *pc_channels;


		CError  e_load_anycast(FILE  *pfSource);
		CError  e_load_unicast(FILE  *pfSource);

		CFiberPath  *pc_get_clever_path(vector<CFiberPath*>  *pvPossiblePaths, double dPathFitSumm, gcroot<Random*> pcRandomGen);
	
	};//class  CFiberSolution



	class  CFiberSolutionChannel
	{
	public:
		CFiberSolutionChannel();
		~CFiberSolutionChannel();


		CError  eSetChannel();
		CError  eFindAndSetChanel(CFiberPath  *pcChannelPath);
		CError  eFindChanel(CFiberPath  *pcChannelPath, int *piStartSlot, int  *piSlotsNumber);
		CError  eSetChanel(CFiberPath  *pcChannelPath, int iStartSlot, int  iSlotsNumber);

		void  vConfig(CFiberSolution  *pcSolution, CFiberDemand  *pcDemand);

		void  vSave(FILE  *pfDest);

		void  vCopy(CFiberSolutionChannel  *pcOther);
		bool operator==(CFiberSolutionChannel  &cOther);
		bool operator!=(CFiberSolutionChannel  &cOther);


		CFiberDemand  *pcGetDemand()  {return(pc_demand);};

		CFiberPath  *pcPath;
		int  iFirstSlotNumber;

	private:
		CFiberSolution  *pc_solution;
		CFiberDemand  *pc_demand;
	
	};//class  CFiberSolution






	
}//namespace  FiberNets


