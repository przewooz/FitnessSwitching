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


//system tools
#include  "..\DeceptiveTools\DecFunc.h"
#include  "error.h"

//time counter
#include  "..\util\timer.h"
//..and other tools...
#include  "..\util\tools.h"
#include  "..\util\LargeNum.h"
#include  "..\FiberFlowAssignment\FiberNet.h"


using namespace std;
using namespace DeceptiveTools;
using namespace TimeCounters;
using  namespace  FiberNets;


namespace Tabu
{
	#define  TABU_MOVE_TYPE_NO_MOVE	0
	#define  TABU_MOVE_TYPE_DEMAND	1
	#define  TABU_MOVE_TYPE_PATH	2
	#define  TABU_MOVE_TYPE_REPLICA	3


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

		void  vApply(CmGA  *pcCurSolution, CmGA  *pcCandidateSolution);


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


};//namespace Tabu


/*class TabuMove
    {
	public:
		int param1;
		int param2;
		string type;

        string GetMoveType()
        {
            return type;
        }

		void SetMoveType(string t)
		{
			type = t;
		}

        int param1Get()
        {
            return param1;
        }

		void param1Set(int p)
		{
			param1 = p;
		}

        int param2Get()
        {
            return param2;
        }

		void param2Set(int p)
		{
			param2 = p;
		}

		TabuMove()
		{
			param1=param2=0;
			type="";
		}

		TabuMove(int p1, int p2, string t)
		{
			param1=p1;
			param2=p2;
			type=t;
		}

	    bool operator==(const TabuMove& wzor)
		{
			return (param1==wzor.param1 && param2==wzor.param2 && type==wzor.type);
		}
    };*/