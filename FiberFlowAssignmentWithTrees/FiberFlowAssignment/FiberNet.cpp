#include "stdafx.h"

#include "FiberNet.h"


using  namespace  FiberNets;
using namespace TimeCounters;
using namespace std;
using namespace Tools;


double  CFiberSolution::d_func_evaluations_full_better_than_simple;
double  CFiberSolution::d_best_solution;
double  CFiberSolution::d_time_till_best;
double  CFiberSolution::d_ffe_till_best;
CTimeCounter  CFiberSolution::c_time_counter;



//---------------------------class  CFiberNet--------------------------------------
CFiberNet::CFiberNet()
{
	pc_nodes = NULL;
	pc_conns = NULL;
	pc_conn_mat = NULL;
	pc_replices = NULL;
	pc_demands = NULL;
	pc_paths = NULL;
	pc_trees = NULL;

	
	i_nodes_num = 0;
	i_conns_num = 0;
	i_rep_num = 0;
	i_demand_num = 0;
	i_path_num = 0;
	i_tree_num = 0;
}//CFiberNet::CFiberNet()


CFiberNet::~CFiberNet()
{

	if  ( (i_nodes_num  >  0)&&(pc_conn_mat != NULL))
	{
		for  (int  ix = 0; ix < i_nodes_num; ix++)
		{
			delete  pc_conn_mat[ix];
			delete  pv_paths_between_nodes[ix];
		}//for  (int  ix = 0; ix < i_nodes_num; ix++)

		delete  pc_conn_mat;
		delete  pv_paths_between_nodes;
	}//if  ( (i_nodes_num  >  0)&&(pc_conn_mat != NULL))


	if  (pc_nodes != NULL)  delete  pc_nodes;
	if  (pc_conns != NULL)  delete  pc_conns;
	if  (pc_replices != NULL)  delete  pc_replices;
	if  (pc_demands != NULL)  delete  pc_demands;
	if  (pc_paths != NULL)  delete  pc_paths;
	
}//CFiberNet::~CFiberNet()




double  CFiberNet::dCompCost()
{
	double  d_result;

	d_result = 0;
	int  i_buf;
	for  (int  ii = 0; ii < i_conns_num; ii++)
	{
		pc_conns[ii].iGetMaxSlot();
		i_buf = pc_conns[ii].iGetMaxSlot();
		if  (i_buf  >  d_result)  d_result = i_buf;
	}//for  (int  ii = 0; ii < i_conns_num; ii++)

	return(d_result);
}//double  CFiberNet::dCompCost()



CError  CFiberNet::eSetCores(int  iCores)
{
	CError  c_err;

	i_cores = iCores;

	return(c_err);
}//CError  CFiberNet::eSetCores(int  iCores)




CError  CFiberNet::eResetNet()
{
	CError  c_err;

	for  (int  ii = 0; ii < i_conns_num; ii++)
		pc_conns[ii].vReset();

	return(c_err);
}//CError  CFiberNet::eResetNet()




CError  CFiberNet::eLoadTopology(CString  sNetFile)
{
	CError  c_err;

	FILE  *pf_source;

	pf_source = fopen(sNetFile, "r+");

	if  (pf_source  ==  NULL)
	{
		c_err.vPutError("No net file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	
	i_nodes_num = Tools::iReadLine(pf_source);
	i_conns_num = Tools::iReadLine(pf_source);

	if  (i_nodes_num  <  1)
	{
		c_err.vPutError("Nodes number must be greater than 0");
		return(c_err);
	}//if  (i_nodes_num  <  1)


	if  (i_conns_num  <  1)
	{
		c_err.vPutError("Connections number must be greater than 0");
		return(c_err);
	}//if  (i_nodes_num  <  1)


	pc_nodes  = new CFiberNode[i_nodes_num];
	//pc_conns = new CFiberConnectionCores[i_conns_num];
	//pc_conns = new CFiberConnectionOld[i_conns_num];
	pc_conns = new CFiberConnection[i_conns_num];




	pc_conn_mat = new  CFiberConnection** [i_nodes_num];
	pv_paths_between_nodes = new vector<int>* [i_nodes_num];

	for  (int  ix = 0; ix < i_nodes_num; ix++)
	{
		pc_conn_mat[ix] = new CFiberConnection* [i_nodes_num];
		pv_paths_between_nodes[ix] = new vector<int>[i_nodes_num];
	}//for  (int  ix = 0; ix < i_nodes_num; ix++)


	for  (int  ix = 0; ix < i_nodes_num; ix++)
	{
		for  (int  iy = 0; iy < i_nodes_num; iy++)
		{
			pc_conn_mat[ix][iy] = NULL;
		}//for  (int  iy = 0; iy < i_nodes_num; iy++)
	}//for  (int  ix = 0; ix < i_nodes_num; ix++)


	for  (int  ix = 0; ix < i_nodes_num; ix++)  pc_nodes[ix].vSetNodeNumber(ix);



	CString  s_line;
	double  d_value;
	int  i_connection_id;
	int  i_index;
	int  i_buf;

	i_connection_id = 0;
	for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)
	{
		s_line  =  Tools::sReadLine(pf_source);
		//::MessageBox(NULL, s_line, s_line, MB_OK);
		
		i_index = 0;
		for  (int i_node_end = 0; i_node_end < i_nodes_num; i_node_end++)
		{
			/*i_buf = i_node_end;
			i_node_end = i_node_start;
			i_node_start = i_buf;//*/


			d_value  =  Tools::dExtractFromString(s_line, i_index, &i_index);

			if  (d_value  >  0)
			{
				//c_err  =  ( ((CFiberConnectionCores *) pc_conns)[i_connection_id]).eConfig(i_connection_id, i_node_start, i_node_end, this, d_value, 1);
				//c_err  =  ( ((CFiberConnectionOld *) pc_conns)[i_connection_id]).eConfig(i_connection_id, i_node_start, i_node_end, this, d_value);
				c_err  =  pc_conns[i_connection_id].eConfig(i_connection_id, i_node_start, i_node_end, this, d_value, i_cores);
				pc_conn_mat[i_node_start][i_node_end] = &(pc_conns[i_connection_id]);
				pc_nodes[i_node_start].vAddOutConn(&pc_conns[i_connection_id]);
				pc_nodes[i_node_end].vAddInConn(&pc_conns[i_connection_id]);

				i_connection_id++;

				if  (c_err)
				{
					fclose(pf_source);
					return(c_err);
				}//if  (c_err)
			}//if  (d_value  >  0)


			/*i_buf = i_node_end;
			i_node_end = i_node_start;
			i_node_start = i_buf;//*/
		}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)
		
	}//for  (int ii = 0; ii < i_nodes_num; ii++)



	fclose(pf_source);
	return(c_err);
}//CError  CFiberNet::eLoadTopology(CString  sNetFile)



bool  CFiberNet::bCheckIfReplicaNode(CFiberNode  *pcNode)
{
	for  (int  ii = 0; ii < i_rep_num; ii++)
	{
		if  (pcNode->i_node_number  ==  pc_replices[ii]->i_node_number)  return(true);
	}//for  (int  ii = 0; ii < i_rep_num; ii++)

	return(false);
}//bool  CFiberNet::bCheckIfReplicaNode(CFiberNode  *pcNode)


bool  CFiberNet::bCheckIfReplicaNode(int  iNodeId)
{
	for  (int ii = 0; ii < i_rep_num; ii++)
	{
		if  (pc_replices[ii]->i_node_number  ==  iNodeId)  return(true);	
	}//for  (int ii = 0; ii < i_rep_num; ii++)

	return(false);
}//bool  CFiberNet::bCheckIfReplicaNode(int  iNodeId)




bool  CFiberNet::bCompareNodes(int  iNodeId0, int iNodeId1)
{
	if  (iNodeId0 == iNodeId1)  return(true);

	if  (iNodeId0  ==  -1)
	{
		return(bCheckIfReplicaNode(iNodeId1));
	}//if  (iNodeId0  ==  -1)

	if  (iNodeId1  ==  -1)
	{
		return(bCheckIfReplicaNode(iNodeId0));
	}//if  (iNodeId1  ==  -1)

	return(false);
}//bool  CFiberNet::bCompareNodes(int  iNodeId0, int iNodeId1)





CError  CFiberNet::eLoadDemands(CString  sUnicastFile, CString  sAnycastFile, CString sMulticastFile, CString  sAnycastFile1Direction, int  iPathsToUse, int  iTreesToUse)
{
	CError  c_err;
	CString  s_buf;
	FILE  *pf_unicast_source, *pf_anycast_source, *pf_multicast_source, *pf_anycast_source_1_direction;

	pf_unicast_source = NULL;//musi byc bo to jest potem sprawdzane
	pf_anycast_source = NULL;
	pf_multicast_source = NULL;
	pf_anycast_source_1_direction = NULL;



	if  (sUnicastFile  !=  "")
	{
		pf_unicast_source = fopen(sUnicastFile, "r+");
		if  (pf_unicast_source  ==  NULL)
		{
			s_buf.Format("No unicast dem file defined 1 (%s)", sUnicastFile);
			c_err.vPutError(s_buf);
			return(c_err);
		}//if  (pf_source  ==  NULL)
	}//if  (sUnicastFile  !=  "")


	/*pf_anycast_source = fopen(sAnycastFile, "r+");
	if  (pf_unicast_source  ==  NULL)
	{
		fclose(pf_unicast_source);
		c_err.vPutError("No anycast dem file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)*/


	i_multi_demands = 0;
	if  (sMulticastFile  !=  "")
	{
		pf_multicast_source = fopen(sMulticastFile, "r+");
		if  (pf_multicast_source  ==  NULL)
		{
			c_err.vPutError("No multicast dem file defined 2");
			return(c_err);
		}//if  (pf_multicast_source  ==  NULL)

		i_multi_demands = Tools::iReadLine(pf_multicast_source);
	}//if  (sMulticastFile  !=  "")


	i_any_demands_1_direction = 0;
	if  (sAnycastFile1Direction  !=  "")
	{
		pf_anycast_source_1_direction = fopen(sAnycastFile1Direction, "r+");
		if  (pf_anycast_source_1_direction  ==  NULL)
		{
			c_err.vPutError("No anycast 1 direction file defined 2");
			return(c_err);
		}//if  (pf_multicast_source  ==  NULL)

		i_any_demands_1_direction = Tools::iReadLine(pf_anycast_source_1_direction);
	}//if  (sMulticastFile  !=  "")



	
	if  (sUnicastFile  !=  "")
		i_uni_demands = Tools::iReadLine(pf_unicast_source);
	else
		i_uni_demands = 0;


	


	//i_any_demands = Tools::iReadLine(pf_anycast_source);
	//i_any_demands *= 2;
	i_any_demands = 0;
	i_demand_num = i_uni_demands + i_any_demands + i_multi_demands + i_any_demands_1_direction;


	//s_buf.Format("demands: %d  any1dir: %d", i_demand_num, i_any_demands_1_direction);
	//Tools::vShow(s_buf);
	

	pc_demands = new CFiberDemand[i_demand_num];


	//::Tools::vShow(0);

	if  (sUnicastFile  !=  "")  c_err = e_load_uni_demands(pf_unicast_source, iPathsToUse);
	if  (c_err)
	{
		if  (pf_unicast_source  !=  NULL)  fclose(pf_unicast_source);
		fclose(pf_anycast_source);
		fclose(pf_multicast_source);
		return(c_err);	
	}//if  (c_err)

	/*c_err = e_load_any_demands(pf_anycast_source);
	if  (c_err)
	{
		if  (pf_unicast_source  !=  NULL)  fclose(pf_unicast_source);
		fclose(pf_anycast_source);
		fclose(pf_multicast_source);
		return(c_err);
	}//if  (c_err)*/

	//::Tools::vShow(1);

	c_err = e_load_multi_demands(pf_multicast_source, iTreesToUse);
	if  (c_err)
	{
		if  (pf_unicast_source  !=  NULL)  fclose(pf_unicast_source);
		if  (pf_anycast_source != NULL) fclose(pf_anycast_source);
		if  (pf_multicast_source != NULL) fclose(pf_multicast_source);
		if  (pf_anycast_source_1_direction) fclose(pf_anycast_source_1_direction);
		return(c_err);	
	}//if  (c_err)

	//::Tools::vShow(2);

	c_err = e_load_any_demands_1_dir(pf_anycast_source_1_direction, iPathsToUse);
	if  (c_err)
	{
		if  (pf_unicast_source  !=  NULL)  fclose(pf_unicast_source);
		if  (pf_anycast_source != NULL) fclose(pf_anycast_source);
		if  (pf_multicast_source != NULL) fclose(pf_multicast_source);
		if  (pf_anycast_source_1_direction) fclose(pf_anycast_source_1_direction);
		return(c_err);
	}//if  (c_err)

	
	
	//::Tools::vShow(3);

	if  (pf_unicast_source  !=  NULL)  fclose(pf_unicast_source);
	//if  (pf_anycast_source != NULL) fclose(pf_anycast_source);
	if  (pf_multicast_source != NULL) fclose(pf_multicast_source);
	if  (pf_anycast_source_1_direction) fclose(pf_anycast_source_1_direction);



	//create employed paths for tabu
	int  i_size;
	for  (int  i_first_node = 0; i_first_node < i_nodes_num; i_first_node++)
	{
		for  (int  i_second_node = 0; i_second_node < i_nodes_num; i_second_node++)
		{
			if  (i_first_node !=  i_second_node)
			{
				for  (int  i_path = 0; (i_path < pv_paths_between_nodes[i_first_node][i_second_node].size())&&(i_path < iPathsToUse); i_path++ )
				{
					v_paths_employed_for_tabu.push_back(&(pc_paths[pv_paths_between_nodes[i_first_node][i_second_node].at(i_path)]));
				}//for  (int  i_path = 0; (i_path < pv_paths_between_nodes[i_first_node][i_second_node].size())&&(i_path < iPathsToUse); i_path++ )
			}//if  (i_first_node !=  i_second_node)
		}//for  (int  i_second_node = 0; i_second_node < pc_fitness->pcGetNet()->iGetNodesNum(); i_second_node++)	
	}//for  (int  i_first_node = 0; i_first_node < pc_fitness->pcGetNet()->iGetNodesNum(); i_first_node++)



	return(c_err);
}//CError  CFiberNet::eLoadDemands(CString  sUnicastFile, CString  sAnycastFile)



CError  CFiberNet::e_load_any_demands_1_dir(FILE  *pfSource, int iMaxPathsToUseBetweenNodes)
{
	CError  c_err;

	CString  s_buf;

	CString  s_line;
	int  i_client_node;
	double  d_stream;

	int  i_index;
	int  i_demand_offset;
	CString  s_demand_type;

	bool  b_demand_type_found;
	
	//::Tools::vShow(i_any_demands_1_direction);
	for  (int i_demand_line = 0; i_demand_line < i_any_demands_1_direction; i_demand_line++)
	{
		s_line  =  Tools::sReadLine(pfSource);

		i_index = 0;
		i_client_node = Tools::iExtractFromString(s_line, i_index, &i_index);

		d_stream = Tools::iExtractFromString(s_line, i_index, &i_index);
		s_demand_type = Tools::sExtractFromString(s_line, i_index, &i_index);

		
		if  (bCheckIfReplicaNode(&(pc_nodes[i_client_node]))  ==  true)
		{
			i_demand_num = i_demand_num - 1;
			i_any_demands_1_direction = i_any_demands_1_direction - 1;
			i_demand_line = i_demand_line - 1;

			//s_buf.Format("%d  %d", i_client_node, pc_nodes[i_client_node].i_node_number);
			//vShow(s_buf);
		}//if  (bCheckIfReplicaNode(pc_nodes[i_client_node])  ==  true)
		else
		{
			i_demand_offset = i_multi_demands + i_uni_demands + i_any_demands * 2 + i_demand_line;
			b_demand_type_found = false;


			if  (s_demand_type == DEMAND_ANYCAST_1_DIRECTION_DOWN)
			{
				pc_demands[i_demand_offset].eConfig
					(
					i_demand_offset,
					-1, i_client_node, d_stream, 
					this, DEMAND_TYPE_ANYCAST, NULL, iMaxPathsToUseBetweenNodes
					);

				b_demand_type_found = true;
			}//if  (s_demand_type == DEMAND_ANYCAST_1_DIRECTION_DOWN)


			if  (s_demand_type == DEMAND_ANYCAST_1_DIRECTION_UP)
			{
				pc_demands[i_demand_offset].eConfig
					(
					i_demand_offset,
					i_client_node, -1, d_stream, 
					this, DEMAND_TYPE_ANYCAST, NULL, iMaxPathsToUseBetweenNodes
					);

				b_demand_type_found = true;
			}//if  (s_demand_type == DEMAND_ANYCAST_1_DIRECTION_UP)


			
			//s_buf.Format("ind: %d  client: %d stream: %.2lf type: '%s'  DEMAND: %d->%d", i_demand_line, i_client_node, d_stream, s_demand_type, pc_demands[i_demand_offset].iGetStartNode(), pc_demands[i_demand_offset].iGetEndNode());
			//Tools::vRepInFile("zzz_reading.txt", s_buf);

			if  (b_demand_type_found == false)
			{
				s_buf.Format("Missing demand type '%s'", s_demand_type);
				c_err.vPutError(s_buf);
				return(c_err);
			}//if  (b_demand_type_found == false)


		}//else  if  (bCheckIfReplicaNode(pc_nodes[i_client_node])  ==  true)

	}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)


	return(c_err);
}//CError  CFiberNet::e_load_any_demands_1_dir(FILE  *pfSource)



CError  CFiberNet::e_load_any_demands(FILE  *pfSource)
{
	CError  c_err;

	CString  s_buf;

	CString  s_line;
	int  i_client_node;
	double  d_downstream, d_upstream;

	int  i_index;
	int  i_demand_offset = 0;
	int  i_anycast_lines_to_read = i_any_demands / 2;
	for  (int i_demand_line = 0; i_demand_line < i_anycast_lines_to_read; i_demand_line++)
	{
		s_line  =  Tools::sReadLine(pfSource);

		i_index = 0;
		i_client_node = Tools::iExtractFromString(s_line, i_index, &i_index);

		d_downstream = Tools::iExtractFromString(s_line, i_index, &i_index);
		d_upstream  =  Tools::dExtractFromString(s_line, i_index, &i_index);

		if  (bCheckIfReplicaNode(&(pc_nodes[i_client_node]))  ==  true)
		{
			i_demand_num = i_demand_num - 2;
			i_any_demands = i_any_demands - 2;

			//s_buf.Format("%d  %d", i_client_node, pc_nodes[i_client_node].i_node_number);
			//vShow(s_buf);
		}//if  (bCheckIfReplicaNode(pc_nodes[i_client_node])  ==  true)
		else
		{
			pc_demands[i_uni_demands + i_demand_offset * 2].eConfig
				(
				i_uni_demands + i_demand_offset * 2, 
				-1, i_client_node, d_downstream, 
				this, DEMAND_TYPE_ANYCAST, &(pc_demands[i_uni_demands + i_demand_offset * 2 + 1])
				);


			pc_demands[i_uni_demands + i_demand_offset * 2 + 1].eConfig
				(
				i_uni_demands + i_demand_offset * 2 + 1, 
				i_client_node, -1, d_upstream, 
				this, DEMAND_TYPE_ANYCAST, &(pc_demands[i_uni_demands + i_demand_offset * 2])
				);

			i_demand_offset++;
		}//else  if  (bCheckIfReplicaNode(pc_nodes[i_client_node])  ==  true)

	}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)


	return(c_err);
}//CError  CFiberNet::eLoadAnyDemands(CString  sDemFile)



CError  CFiberNet::e_load_uni_demands(FILE  *pfSource, int iMaxPathsToUseBetweenNodes)
{
	CError  c_err;


	CString  s_line;
	int  i_node_start, i_node_end;
	double  d_capacity;

	int  i_index;
	for  (int i_demand_offset = 0; i_demand_offset < i_uni_demands; i_demand_offset++)
	{
		s_line  =  Tools::sReadLine(pfSource);
		//Tools::vShow(s_line);

		i_index = 0;
		i_node_start = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_node_end = Tools::iExtractFromString(s_line, i_index, &i_index);

		d_capacity  =  Tools::dExtractFromString(s_line, i_index, &i_index);

		//Tools::vShow(i_node_start);
		//Tools::vShow(i_node_end);
		//Tools::vShow(d_capacity);

		c_err  = pc_demands[i_demand_offset].eConfig
			(
			i_demand_offset, 
			i_node_start, i_node_end, d_capacity, this,
			DEMAND_TYPE_UNICAST, NULL, iMaxPathsToUseBetweenNodes
			);
		if  (c_err)
		{
			fclose(pfSource);
			return(c_err);
		}//if  (c_err)
	}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)


	return(c_err);
}//CError  CFiberNet::e_load_unicast(CString  sDemFile)




CError  CFiberNet::e_load_multi_demands(FILE  *pfSource, int iMaxTreesToUseForNode)
{
	CError  c_err;


	CString  s_line;
	double  d_capacity;
	int  i_children_number;
	int  i_node_start;
	
	CString  s_report_buf;


	int  i_demand_offset;
	int  i_index;
	for  (int i_multi_demand = 0; i_multi_demand < i_multi_demands; i_multi_demand++)
	{
		s_line  =  Tools::sReadLine(pfSource);

		//::vShow(s_line);

		i_index = 0;

		d_capacity  =  Tools::dExtractFromString(s_line, i_index, &i_index);
		i_children_number = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_node_start = Tools::iExtractFromString(s_line, i_index, &i_index);

		//::vShow(d_capacity);
		//::vShow(i_children_number);
		//::vShow(i_node_start);

		

		i_demand_offset = i_multi_demand + i_uni_demands + i_any_demands * 2;

		s_report_buf.Format("%d  %d  %s\n", i_multi_demand, i_demand_offset, s_line);
		::Tools::vRepInFile("!!test.txt", s_report_buf);

		c_err  = pc_demands[i_demand_offset].eConfig
			(
			i_demand_offset, 
			i_node_start, -1, d_capacity, this,
			DEMAND_TYPE_MULTICAST, NULL,
			iMaxTreesToUseForNode
			);
		if  (c_err)
		{
			fclose(pfSource);
			return(c_err);
		}//if  (c_err)*/
	}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)

	return(c_err);
}//CError  CFiberNet::e_load_multi_demands(FILE  *pfSource)




CError  CFiberNet::eLoadReplicas(CString  sRepFile)
{
	CError  c_err;

	if  (pc_replices != NULL)  delete  pc_replices;
	pc_replices = NULL;


	FILE  *pf_source;
	pf_source = fopen(sRepFile, "r+");

	if  (pf_source  ==  NULL)
	{
		//NO ERROR HERE!!!
		//c_err.vPutError("No replices file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	i_rep_num = Tools::iReadLine(pf_source);
	pc_replices = new CFiberNode*[i_rep_num];


	CString  s_line;
	int  i_rep_node_offset;
	s_line  =  Tools::sReadLine(pf_source);
	int  i_last_offset = 0;

	for  (int ii = 0; ii < i_rep_num; ii++)
	{
		
		i_rep_node_offset = Tools::iExtractFromString(s_line, i_last_offset, &i_last_offset);

		if  (
			(i_rep_node_offset  >= 0)&&(i_rep_node_offset < i_nodes_num)
			)
		{
			pc_replices[ii] = &(pc_nodes[i_rep_node_offset]);
		}//if  (
		else
		{
			CString  s_buf;
			s_buf.Format("Wrong replica offset (%d)", i_rep_node_offset);
			c_err.vPutError(s_buf);
			fclose(pf_source);
			return(c_err);
		}//else  
	}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)


	fclose(pf_source);	

	return(c_err);
}//CError  CFiberNet::eLoadReplices(CString  sRepFile)


CError  CFiberNet::eLoadTrees(CString  sTreeFile)
{
	CError  c_err;

	if  (pc_paths != NULL)  
	{
		delete  pc_trees;
		pc_trees = NULL;
	}//if  (pc_paths != NULL)  
	i_tree_num = 0;


	if  (sTreeFile == "")  return(c_err);


	FILE  *pf_source;
	pf_source = fopen(sTreeFile, "r+");

	if  (pf_source  ==  NULL)
	{
		c_err.vPutError("No tree file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	//first count lines;
	CString  s_line;
	int  i_lines;

	i_lines = 0;
	while (feof(pf_source)  ==  false)
	{
		s_line  =  Tools::sReadLine(pf_source);
		s_line.Replace(" ", "");
		if  (s_line.GetLength()  >  0)  i_lines++;
	}//while (foef(pf_source)  ==  false)
	fclose(pf_source);

	pf_source = fopen(sTreeFile, "r+");

	i_tree_num = i_lines;
	pc_trees = new CFiberPath[i_tree_num];

	

	for  (int i_tree_offset = 0; i_tree_offset < i_tree_num; i_tree_offset++)
	{
		s_line  =  Tools::sReadLine(pf_source);

		pc_trees[i_tree_offset].vConfigure(i_tree_offset, this);
		c_err = pc_trees[i_tree_offset].eLoad(s_line, false);
		pc_trees[i_tree_offset].vSetTree(true);
		if  (c_err)
		{
			fclose(pf_source);
			return(c_err);
		}//if  (c_err)

		/*pv_paths_between_nodes
			[pc_paths[i_path_offset].i_fiber_node_start_offset]
			[pc_paths[i_path_offset].i_fiber_node_end_offset]
			.push_back(i_path_offset);*/
	}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)


	fclose(pf_source);

	return(c_err);
}//CError  CFiberNet::eLoadTrees(CString  sTreeFile)


CError  CFiberNet::eLoadPaths(CString  sPathFile, CString  sPathBackupFile)
{
	CError  c_err;
	CString  s_buf;

	if  (pc_paths != NULL)  delete  pc_paths;


	FILE  *pf_source;
	pf_source = fopen(sPathFile, "r+");

	if  (pf_source  ==  NULL)
	{
		c_err.vPutError("No path file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	i_path_num = Tools::iReadLine(pf_source);
	pc_paths = new CFiberPath[i_path_num];


	FILE  *pf_source_backup;
	int  i_path_backup_num;

	pf_source_backup = NULL;
	if  (sPathBackupFile  != "")
	{
		pf_source_backup = fopen(sPathBackupFile, "r+");

		if  (pf_source_backup  ==  NULL)
		{
			c_err.vPutError("No path file defined");
			fclose(pf_source);
			return(c_err);
		}//if  (pf_source  ==  NULL)

		i_path_backup_num = Tools::iReadLine(pf_source_backup);
	}//if  (sPathBackupFile  != "")
	

	if  (pf_source_backup  != NULL)
	{
		if  (i_path_num !=  i_path_backup_num)
		{
			s_buf.Format("Paths number %d != backup paths number %d", i_path_num, i_path_backup_num);
			c_err.vPutError(s_buf);
			fclose(pf_source_backup);
			fclose(pf_source);
			return(c_err);
		}//if  (i_path_num !=  i_path_backup_num)
	}//if  (pf_source_backup  != NULL)

	


	CString  s_line, s_line_backup;
	CFiberPath  *pc_backup_path;

	for  (int i_path_offset = 0; i_path_offset < i_path_num; i_path_offset++)
	{
		s_line  =  Tools::sReadLine(pf_source);
		if  (pf_source_backup != NULL) s_line_backup =  Tools::sReadLine(pf_source_backup);

		pc_paths[i_path_offset].vConfigure(i_path_offset, this);
		c_err = pc_paths[i_path_offset].eLoad(s_line);
		if  (c_err)
		{
			fclose(pf_source);
			return(c_err);
		}//if  (c_err)


		if  (pf_source_backup != NULL) 
		{
			//s_line_backup =  Tools::sReadLine(pf_source_backup);

			pc_backup_path = new CFiberPath();
			pc_backup_path->vConfigure(i_path_offset, this);
			c_err = pc_backup_path->eLoad(s_line_backup);

			if  (c_err)
			{
				//s_buf.Format("error 2 (%d)", i_path_offset);
				//vShow(s_buf);
				fclose(pf_source_backup);
				return(c_err);
			}//if  (c_err)

			pc_paths[i_path_offset].pc_slave_path = pc_backup_path;
		}//if  (pf_source_backup != NULL) 

		pv_paths_between_nodes
			[pc_paths[i_path_offset].i_fiber_node_start_offset]
			[pc_paths[i_path_offset].i_fiber_node_end_offset]
			.push_back(i_path_offset);
	}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)


	fclose(pf_source);
	if  (pf_source_backup  != NULL)  fclose(pf_source_backup);




	return(c_err);
}//CError  CFiberNet::eLoadPaths(CString  sPathFile)


CError  CFiberNet::e_load_spec(CString  sSpecFile, CString sUniSpecFileBackup, CFiberPath  *pcPaths, int  iPathNum, double  dTransferMultiplier)
{
	CError  c_err;

	if  (sSpecFile == "")  return(c_err);


	FILE  *pf_source;
	pf_source = fopen(sSpecFile, "r+");	

	if  (pf_source  ==  NULL)
	{
		c_err.vPutError("No uni spec file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)


	FILE  *pf_source_backup;
	pf_source_backup = NULL;
	if  (sUniSpecFileBackup  !=  "")
	{
		pf_source_backup = fopen(sUniSpecFileBackup, "r+");	

		if  (pf_source_backup  ==  NULL)
		{
			fclose(pf_source);
			c_err.vPutError("No uni spec backup file defined");
			return(c_err);
		}//if  (pf_source  ==  NULL)	
	}//if  (sUniSpecFileBackup  !=  "")

	
	CString  s_line;

	for  (int i_path_offset = 0; i_path_offset < iPathNum; i_path_offset++)
	{
		s_line  =  Tools::sReadLine(pf_source);

		c_err = pcPaths[i_path_offset].eGetSlotsNumberForTransfer(s_line);
		pcPaths[i_path_offset].vSetTransferMultiplier(dTransferMultiplier);
		if  (c_err)
		{
			fclose(pf_source);
			if  (pf_source_backup != NULL)  fclose(pf_source_backup);
			return(c_err);
		}//if  (c_err)


		if  (pf_source_backup != NULL) 
		{
			s_line  =  Tools::sReadLine(pf_source_backup);
			c_err = pcPaths[i_path_offset].pc_slave_path->eGetSlotsNumberForTransfer(s_line);
			pcPaths[i_path_offset].pc_slave_path->vSetTransferMultiplier(dTransferMultiplier);
			if  (c_err)
			{
				fclose(pf_source);
				if  (pf_source_backup != NULL)  fclose(pf_source_backup);
				return(c_err);
			}//if  (c_err)
		
		}//if  (pf_source_backup != NULL) 

	}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)


	if  (pf_source_backup != NULL)  fclose(pf_source_backup);
	fclose(pf_source);
	return(c_err);
}//CError  CFiberNet::e_load_spec(CString  sSpecFile, CFiberPath  *pcPaths, int  *piPathNum)


CError  CFiberNet::eLoadPathsSpec(CString  sUniSpecFile, CString  sUniSpecFileBackup, double  dTransferMultiplier)
{
	CError  c_err;

	c_err = e_load_spec(sUniSpecFile, sUniSpecFileBackup, pc_paths, i_path_num, dTransferMultiplier);

	return(c_err);
}//CError  CFiberNet::eLoadPathsSpec(CString  sUniSpecFile)


CError  CFiberNet::eLoadTreesSpec(CString  sTreeSpecFile, double  dTransferMultiplier)
{
	CError  c_err;

	c_err = e_load_spec(sTreeSpecFile, "", pc_trees, i_tree_num, dTransferMultiplier);

	return(c_err);
}//CError  CFiberNet::eLoadPathsSpec(CString  sUniSpecFile)


		

CError  CFiberNet::e_load_reg(CString  sSpecFile, CFiberPath  *pcPaths, int  iPathNum)
{
	CError  c_err;

	if  (sSpecFile == "")  return(c_err);//no error returned in fact


	FILE  *pf_source;
	pf_source = fopen(sSpecFile, "r+");

	if  (pf_source  ==  NULL)
	{
		c_err.vPutError("No uni reg file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	
	CString  s_line;

	for  (int i_path_offset = 0; i_path_offset < iPathNum; i_path_offset++)
	{
		s_line  =  Tools::sReadLine(pf_source);

		c_err = pcPaths[i_path_offset].eGetRegNumberForTransfer(s_line);
		if  (c_err)
		{
			fclose(pf_source);
			return(c_err);
		}//if  (c_err)

	}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)


	fclose(pf_source);
	return(c_err);
}//CError  CFiberNet::e_load_reg(CString  sSpecFile, CFiberPath  *pcPaths, int  iPathNum)



CError  CFiberNet::eLoadPathsReg(CString  sUniRegFile)
{
	CError  c_err;

	c_err = e_load_reg(sUniRegFile, pc_paths, i_path_num);

	return(c_err);
}//CError  CFiberNet::eLoadPathsReg(CString  sUniRegFile)



CError  CFiberNet::eLoadTreesReg(CString  sTreeRegFile)
{
	CError  c_err;

	c_err = e_load_reg(sTreeRegFile, pc_trees, i_tree_num);

	return(c_err);
}//CError  CFiberNet::eLoadTreesReg(CString  sTreeRegFile)




CError  CFiberNet::eLoadModulation(CString  sModFile)
{
	CError  c_err;

	CString  s_buf;

	FILE  *pf_source;
	pf_source = fopen(sModFile, "r+");

	if  (pf_source  ==  NULL)
	{
		s_buf.Format("No mod file defined (%s)", sModFile);
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  (pf_source  ==  NULL)

	int  i_mod_num;
	i_mod_num = Tools::iReadLine(pf_source);

	if  (i_mod_num  !=  i_path_num)
	{
		fclose(pf_source);
		CString  s_buf;
		s_buf.Format("different path and modulation number: %d != %d", i_path_num,  i_mod_num);
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  (i_mod_num  !=  i_path_num)


	CString  s_line;

	for  (int i_path_offset = 0; i_path_offset < i_path_num; i_path_offset++)
	{
		s_line  =  Tools::sReadLine(pf_source);

		pc_paths[i_path_offset].eLoadMod(s_line);
		if  (c_err)
		{
			fclose(pf_source);
			return(c_err);
		}//if  (c_err)
	}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)
	fclose(pf_source);

	return(c_err);
}//CError  CFiberNet::eLoadModulation(CString  sModFile)




CError  CFiberNet::eSaveModulation(CString  sModFile)
{
	CError  c_err;

	FILE  *pf_dest;
	pf_dest = fopen(sModFile, "w+");

	if  (pf_dest  ==  NULL)
	{
		c_err.vPutError("No net file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)


	fprintf(pf_dest, "%d\n", i_path_num);

	

	for  (int i_path_offset = 0; i_path_offset < i_path_num; i_path_offset++)
	{
		pc_paths[i_path_offset].vSaveMod(pf_dest);
	}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)
	fclose(pf_dest);

	return(c_err);
}//CError  CFiberNet::eSaveModulation(CString  sModFile)




CError  CFiberNet::eSaveTopology(CString  sNetFile)
{
	CError  c_err;
	FILE  *pf_dest;

	pf_dest = fopen(sNetFile, "w+");

	if  (pf_dest  ==  NULL)
	{
		c_err.vPutError("No net file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	
	fprintf(pf_dest, "%d\n", i_nodes_num);
	fprintf(pf_dest, "%d\n", i_conns_num);


	for  (int  ix = 0; ix < i_nodes_num; ix++)
	{
		for  (int  iy = 0; iy < i_nodes_num; iy++)
		{
			if  (pc_conn_mat[ix][iy]  ==  NULL)
				fprintf(pf_dest, "0\t", i_nodes_num);
			else
				fprintf(pf_dest, "%d\t", (int) pc_conn_mat[ix][iy]->d_connection_capacity);
		}//for  (int  iy = 0; iy < i_nodes_num; iy++)
		fprintf(pf_dest, "\n");
	}//for  (int  ix = 0; ix < i_nodes_num; ix++)



	fclose(pf_dest);
	return(c_err);

}//void  CFiberNet::vSaveTopology(CString  sNetFile)



CError  CFiberNet::eSaveReplicas(CString  sRepFile)
{
	CError  c_err;
	FILE  *pf_dest;

	pf_dest = fopen(sRepFile, "w+");

	if  (pf_dest  ==  NULL)
	{
		c_err.vPutError("No rep file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	
	fprintf(pf_dest, "number: %d\n", i_rep_num);

	for  (int  ii = 0; ii < i_rep_num; ii++)
	{
		fprintf(pf_dest, "%d\n", pc_replices[ii]->i_node_number);
	}//for  (int  ix = 0; ix < i_nodes_num; ix++)

	fprintf(pf_dest, "\n");



	fclose(pf_dest);
	return(c_err);
}//CError  CFiberNet::eSaveReplicas(CString  sRepFile)




CError  CFiberNet::eSaveDemands(CString  sDemFile)
{
	CError  c_err;
	FILE  *pf_dest;

	pf_dest = fopen(sDemFile, "w+");

	if  (pf_dest  ==  NULL)
	{
		c_err.vPutError("No dem file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	
	fprintf(pf_dest, "ALL: %d\n", i_demand_num);
	fprintf(pf_dest, "Unicast: %d\n", i_uni_demands);
	fprintf(pf_dest, "Anycast: %d\n", i_any_demands);
	fprintf(pf_dest, "Multicast: %d\n", i_multi_demands);
	fprintf(pf_dest, "Anycast1dir: %d\n", i_any_demands_1_direction);

	fprintf(pf_dest, "\n");

	for  (int  ii = 0; ii < i_demand_num; ii++)
	{
		pc_demands[ii].vSave(pf_dest);
		fprintf(pf_dest, "\n");
	}//for  (int  ix = 0; ix < i_nodes_num; ix++)



	fclose(pf_dest);
	return(c_err);
}//CError  CFiberNet::eSaveDemands(CString  sDemFile)



CError  CFiberNet::eSaveTrees(CString  sTreeFile)
{
	CError  c_err;
	FILE  *pf_dest;

	pf_dest = fopen(sTreeFile, "w+");

	if  (pf_dest  ==  NULL)
	{
		c_err.vPutError("No tree file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	
	fprintf(pf_dest, "%d\n", this->i_tree_num);


	for  (int  ii = 0; ii < i_tree_num; ii++)
	{
		pc_trees[ii].vSave(pf_dest);
		fprintf(pf_dest, "\n");
	}//for  (int  ix = 0; ix < i_nodes_num; ix++)



	fprintf(pf_dest, "\n");
	fprintf(pf_dest, "\n");
	/*for  (int  ii = 0; ii < pc_demands[5].pvGetPossiblePaths()->size(); ii++)
	{
		pc_demands[5].pvGetPossiblePaths()->at(ii)->vSaveWithLinks(pf_dest);
		fprintf(pf_dest, "\n");	
	}//for  (int  ii = 0; ii < pc_demands[5].pvGetPossiblePaths(); ii++)*/


	/*for  (int  ii = 0; ii < i_tree_num; ii++)
	{
		if  (pc_paths[ii].v_path_connections.size()  ==  5)
		{
			fprintf(pf_dest, "%d    root: %d\n",ii,  pc_trees[ii].i_fiber_node_start_offset);
			pc_trees[ii].vSaveWithLinks(pf_dest);
			fprintf(pf_dest, "\n");
			fprintf(pf_dest, "\n");
		}//if  (pc_paths[ii].v_path_connections.size()  ==  5)

	}//for  (int  ii = 0; ii < i_path_num; ii++)*/


	fclose(pf_dest);
	return(c_err);
}//CError  CFiberNet::eSaveTrees(CString  sTreeFile)




CError  CFiberNet::eSavePathsSummarized(CString  sPathFile)
{
	CError  c_err;
	FILE  *pf_dest;

	pf_dest = fopen(sPathFile, "w+");

	if  (pf_dest  ==  NULL)
	{
		c_err.vPutError("No path file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	
	fprintf(pf_dest, "%d\n", i_path_num);


	for  (int  ii = 0; ii < i_path_num; ii++)
	{
		//if  ( (pc_paths[ii].i_fiber_node_start_offset == 18)&&(pc_paths[ii].i_fiber_node_end_offset == 25) )
		{
			fprintf(pf_dest, "%d    %d->%d\n",ii,  pc_paths[ii].i_fiber_node_start_offset, pc_paths[ii].i_fiber_node_end_offset);
			fprintf(pf_dest, "BACKUP:%d    %d->%d\n",ii,  pc_paths[ii].pcGetSlavePath()->i_fiber_node_start_offset, pc_paths[ii].pcGetSlavePath()->i_fiber_node_end_offset);
			//pc_paths[ii].vSave(pf_dest);
			//fprintf(pf_dest, "\n");
		}//if  ( (pc_paths[ii].i_fiber_node_start_offset == 18)&&(pc_paths[ii].i_fiber_node_end_offset == 25) )
	}//for  (int  ix = 0; ix < i_nodes_num; ix++)



	fprintf(pf_dest, "\n");
	fprintf(pf_dest, "\n");
	/*for  (int  ii = 0; ii < pc_demands[5].pvGetPossiblePaths()->size(); ii++)
	{
		pc_demands[5].pvGetPossiblePaths()->at(ii)->vSaveWithLinks(pf_dest);
		fprintf(pf_dest, "\n");	
	}//for  (int  ii = 0; ii < pc_demands[5].pvGetPossiblePaths(); ii++)*/


	/*for  (int  ii = 0; ii < i_path_num; ii++)
	{
		if  (pc_paths[ii].v_path_connections.size()  ==  5)
		{
			fprintf(pf_dest, "%d    %d->%d\n",ii,  pc_paths[ii].i_fiber_node_start_offset, pc_paths[ii].i_fiber_node_end_offset);
			pc_paths[ii].vSaveWithLinks(pf_dest);
			fprintf(pf_dest, "\n");
			fprintf(pf_dest, "\n");
		}//if  (pc_paths[ii].v_path_connections.size()  ==  5)

	}//for  (int  ii = 0; ii < i_path_num; ii++)*/


	fclose(pf_dest);
	return(c_err);
}//CError  CFiberNet::eSavePaths(CString  sPathFile)


CError  CFiberNet::eSavePaths(CString  sPathFile)
{
	CError  c_err;
	FILE  *pf_dest;

	pf_dest = fopen(sPathFile, "w+");

	if  (pf_dest  ==  NULL)
	{
		c_err.vPutError("No path file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	
	fprintf(pf_dest, "%d\n", i_path_num);


	for  (int  ii = 0; ii < i_path_num; ii++)
	{
		//if  ( (pc_paths[ii].i_fiber_node_start_offset == 18)&&(pc_paths[ii].i_fiber_node_end_offset == 25) )
		{
			//fprintf(pf_dest, "%d    %d->%d\n",ii,  pc_paths[ii].i_fiber_node_start_offset, pc_paths[ii].i_fiber_node_end_offset);
			pc_paths[ii].vSave(pf_dest);
			fprintf(pf_dest, "\n");
			fprintf(pf_dest, "BACKUP:");
			pc_paths[ii].pcGetSlavePath()->vSave(pf_dest);
			fprintf(pf_dest, "\n");
		}//if  ( (pc_paths[ii].i_fiber_node_start_offset == 18)&&(pc_paths[ii].i_fiber_node_end_offset == 25) )
	}//for  (int  ix = 0; ix < i_nodes_num; ix++)



	fprintf(pf_dest, "\n");
	fprintf(pf_dest, "\n");
	/*for  (int  ii = 0; ii < pc_demands[5].pvGetPossiblePaths()->size(); ii++)
	{
		pc_demands[5].pvGetPossiblePaths()->at(ii)->vSaveWithLinks(pf_dest);
		fprintf(pf_dest, "\n");	
	}//for  (int  ii = 0; ii < pc_demands[5].pvGetPossiblePaths(); ii++)*/


	/*for  (int  ii = 0; ii < i_path_num; ii++)
	{
		if  (pc_paths[ii].v_path_connections.size()  ==  5)
		{
			fprintf(pf_dest, "%d    %d->%d\n",ii,  pc_paths[ii].i_fiber_node_start_offset, pc_paths[ii].i_fiber_node_end_offset);
			pc_paths[ii].vSaveWithLinks(pf_dest);
			fprintf(pf_dest, "\n");
			fprintf(pf_dest, "\n");
		}//if  (pc_paths[ii].v_path_connections.size()  ==  5)

	}//for  (int  ii = 0; ii < i_path_num; ii++)*/


	fclose(pf_dest);
	return(c_err);
}//CError  CFiberNet::eSavePaths(CString  sPathFile)




CError  CFiberNet::e_save_spec(CString  sSpecFile, CFiberPath  *pcPaths, int  iPathNum)
{
	CError  c_err;
	FILE  *pf_dest;

	pf_dest = fopen(sSpecFile, "w+");

	if  (pf_dest  ==  NULL)
	{
		c_err.vPutError("No path file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	


	for  (int  ii = 0; ii < iPathNum; ii++)
	{
		pcPaths[ii].vSaveSpec(pf_dest);
		fprintf(pf_dest, "\n");
	}//for  (int  ix = 0; ix < i_nodes_num; ix++)

	fprintf(pf_dest, "\n");


	fclose(pf_dest);
	return(c_err);
}//CError  CFiberNet::e_save_spec(CString  sSpecFile, CFiberPath  *pcPaths, int  iPathNum)




CError  CFiberNet::eSavePathsSpec(CString  sPathSpecFile)
{
	CError  c_err;
	
	c_err = e_save_spec(sPathSpecFile, pc_paths, i_path_num);

	return(c_err);
}//CError  CFiberNet::eSavePathsSpec(CString  sPathFile)


CError  CFiberNet::eSaveTreesSpec(CString  sTreeSpecFile)
{
	CError  c_err;
	
	c_err = e_save_spec(sTreeSpecFile, pc_trees, i_tree_num);

	return(c_err);
}//CError  CFiberNet::eSaveTreesSpec(CString  sTreeSpecFile)



CError  CFiberNet::e_save_reg(CString  sSpecFile, CFiberPath  *pcPaths, int  iPathNum)
{
	CError  c_err;
	FILE  *pf_dest;

	pf_dest = fopen(sSpecFile, "w+");

	if  (pf_dest  ==  NULL)
	{
		c_err.vPutError("No reg file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)

	


	for  (int  ii = 0; ii < iPathNum; ii++)
	{
		pcPaths[ii].vSaveReg(pf_dest);
		fprintf(pf_dest, "\n");
	}//for  (int  ix = 0; ix < i_nodes_num; ix++)

	fprintf(pf_dest, "\n");


	fclose(pf_dest);
	return(c_err);
}//CError  CFiberNet::e_save_reg(CString  sSpecFile, CFiberPath  *pcPaths, int  iPathNum)




CError  CFiberNet::eSaveTreesReg(CString  sTreeRegFile)
{
	CError  c_err;
	
	c_err = e_save_reg(sTreeRegFile, pc_paths, i_path_num);

	return(c_err);
}//CError  CFiberNet::eSaveTreesReg(CString  sTreeRegFile)




CError  CFiberNet::eSavePathsReg(CString  sPathRegFile)
{
	CError  c_err;
	
	c_err = e_save_reg(sPathRegFile, pc_paths, i_path_num);

	return(c_err);	
}//CError  CFiberNet::eSavePathsReg(CString  sPathRegFile)


//---------------------------class  CFiberNode--------------------------------------
CFiberNode::CFiberNode()
{
	pc_in_conns = NULL;
	pc_out_conns = NULL;
	i_node_number = 0;
}//CFiberNode::CFiberNode()


CFiberNode::~CFiberNode()
{
	if  (pc_in_conns != NULL)  delete  pc_in_conns;
	if  (pc_out_conns != NULL)  delete  pc_out_conns;
}//CFiberNode::~CFiberNode()


void  CFiberNode::vSetNodeNumber(int  iNodeNumber)
{
	if  (pc_in_conns != NULL)  delete  pc_in_conns;
	if  (pc_out_conns != NULL)  delete  pc_out_conns;

	i_node_number = iNodeNumber;

	pc_in_conns = new CFiberConnection*[i_node_number];
	pc_out_conns = new CFiberConnection*[i_node_number];

	for  (int  ii = 0; ii < i_node_number; ii++)  pc_in_conns[ii] = NULL;
	for  (int  ii = 0; ii < i_node_number; ii++)  pc_out_conns[ii] = NULL;
}//void  CFiberNode::vSetNodeNumber(int  iNodeNumber)



void  CFiberNode::vAddInConn(CFiberConnection  *pcConnection)
{
	if  (pcConnection->i_fiber_node_start_offset  <  i_node_number)
	{
		pc_in_conns[pcConnection->i_fiber_node_start_offset] = pcConnection;
	}//if  (pcConnection->i_fiber_node_start_offset  <  i_node_number)
}//void  CFiberNode::vAddInConn(CFiberConnection  *pcConnection)



void  CFiberNode::vAddOutConn(CFiberConnection  *pcConnection)
{
	if  (pcConnection->i_fiber_node_end_offset  <  i_node_number)
	{
		pc_out_conns[pcConnection->i_fiber_node_end_offset] = pcConnection;
	}//if  (pcConnection->i_fiber_node_start_offset  <  i_node_number)
}//void  CFiberNode::vAddOutConn(CFiberConnection  *pcConnection)






//---------------------------class  CFiberConnectionOld--------------------------------------
CFiberConnectionOld::CFiberConnectionOld()
{
	i_connection_id = -1;
	i_fiber_node_start_offset = -1;
	i_fiber_node_end_offset = -1;
	pc_parent = NULL;
	d_connection_capacity = 0;
}//CFiberConnection::CFiberConnection(int  iFiberNodeStartOffset, int  iFiberNodeEndOffset, CFiberNet *pcParent, double  dConnectionCapacity)



CFiberConnectionOld::~CFiberConnectionOld()
{

}//CFiberConnectionOld::~CFiberConnectionOld()



CError  CFiberConnectionOld::eReserveSlots(int  iFirstSlot, int iSlotsNumber, bool  bErrorControl /*= true*/)
{
	CError  c_err;
	CString  s_buf;

	
	if  (bErrorControl  ==  true)
	{
		int  i_test;
		i_test = iGetLowestStartSlotForChannel(iFirstSlot, iSlotsNumber);
		if  (i_test  != iFirstSlot)
		{
			s_buf.Format("unable to settle channel in node connection %d at start slot: %d width: %d", i_connection_id, iFirstSlot, iSlotsNumber);			
			c_err.vPutError(s_buf);

			return(c_err);
		}//if  (i_test  != iFirstSlot)
	}//if  (bErrorControl  ==  true)

	bool  b_glue_with_after, b_glue_with_before;
	
	for  (int  ii = 0; ii < v_slots_in_use.size()/2; ii++)
	{
		if  (v_slots_in_use.at(ii*2 + 1)  > iFirstSlot)
		{
			
			b_glue_with_after = false;
			b_glue_with_before = false;

			if  (iFirstSlot + iSlotsNumber - 1  ==  v_slots_in_use.at(ii*2) - 1)  b_glue_with_after = true;
			if  (ii > 0)
			{
				if  (iFirstSlot ==  v_slots_in_use.at((ii-1)*2 + 1) + 1)  b_glue_with_before = true;
			}//if  (ii > 0)


			if  ( (b_glue_with_before  ==  true)&&(b_glue_with_after ==  true) )
			{
				v_slots_in_use.erase(v_slots_in_use.begin() + ii*2 - 1);
				v_slots_in_use.erase(v_slots_in_use.begin() + ii*2 - 1);
				return(c_err);
			}//if  ( (b_glue_with_before  ==  true)&&(b_glue_with_after ==  true) )


			if  ( (b_glue_with_before  ==  true)&&(b_glue_with_after ==  false) )
			{
				v_slots_in_use.at((ii-1)*2 + 1) = iFirstSlot + iSlotsNumber - 1;
				return(c_err);
			}//if  ( (b_glue_with_before  ==  true)&&(b_glue_with_after ==  false) )


			if  ( (b_glue_with_before  ==  false)&&(b_glue_with_after ==  true) )
			{
				v_slots_in_use.at(ii*2) = iFirstSlot;
				return(c_err);
			}//if  ( (b_glue_with_before  ==  false)&&(b_glue_with_after ==  true) )


			if  ( (b_glue_with_before  ==  false)&&(b_glue_with_after ==  false) )
			{
				v_slots_in_use.insert(v_slots_in_use.begin() + ii*2,0);
				v_slots_in_use.insert(v_slots_in_use.begin() + ii*2,0);

				v_slots_in_use.at(ii * 2) = iFirstSlot;
				v_slots_in_use.at(ii * 2 + 1) = iFirstSlot + iSlotsNumber - 1;

				return(c_err);
			}//if  ( (b_glue_with_before  ==  false)&&(b_glue_with_after ==  false) )

		}//if  (v_slots_in_use.at(ii*2 + 1)  >= i_first_slot_allowed)
	}//for  (int  ii = 0; ii < v_slots_in_use.size()/2; ii++)

	b_glue_with_before = false;
	if  (v_slots_in_use.size() > 0)
	{
		if  (iFirstSlot ==  v_slots_in_use.at(v_slots_in_use.size() - 1) + 1)  b_glue_with_before = true;
	}//if  (v_slots_in_use.size() > 0)


	if  (b_glue_with_before  ==  true)
	{
		v_slots_in_use.at(v_slots_in_use.size() - 1) = iFirstSlot + iSlotsNumber - 1;
		return(c_err);
	}//if  (b_glue_with_before  ==  true)
	else
	{
		v_slots_in_use.push_back(iFirstSlot);
		v_slots_in_use.push_back(iFirstSlot + iSlotsNumber - 1);
		return(c_err);
	}//else  if  (b_glue_with_before  ==  true)

	return(c_err);
}//CError  CFiberConnectionOld::eReserveSlots(int  iFirstSlot, int iSlotsNumber)



int  CFiberConnectionOld::iGetMaxSlot()
{
	if  (v_slots_in_use.size()  ==  0)  return(0);
	return(v_slots_in_use.at(v_slots_in_use.size() - 1));
}//int  CFiberConnectionOld::iGetMaxSlot()




bool  CFiberConnectionOld::bSlotInUse(int iSlotNumber)
{
	for  (int  ii = 0; ii < v_slots_in_use.size()/2; ii++)
	{
		if  (
			(v_slots_in_use.at(ii * 2) <= iSlotNumber)&&
			iSlotNumber <= (v_slots_in_use.at(ii * 2 + 1))
			)
			return(true);
	}//for  (int  ii = 0; ii < v_slots_in_use.size()/2; ii++)

	return(false);
}//bool  CFiberConnectionOld::bSlotInUse(int iSlotNumber)



void  CFiberConnectionOld::vSaveState(FILE  *pfDest, int iMaxSlotNumber)
{
	fprintf(pfDest, "edge %.3d    ", i_connection_id);

	for  (int  ii = 0; ii < iMaxSlotNumber; ii++)
	{
		if  (bSlotInUse(ii)  ==  true)
			fprintf(pfDest, "1   ");
		else
			fprintf(pfDest, "0   ");
	}//for  (int  ii = 0; ii < iMaxSlotNumber; ii++)
}//void  CFiberConnectionOld::vSaveState(FILE  *pfDest, int iMaxSlotNumber)



void  CFiberConnectionOld::vSaveStatePRW(FILE  *pfDest)
{
	fprintf(pfDest, "edge %.3d    ", i_connection_id);

	for  (int  ii = 0; ii < v_slots_in_use.size()/2; ii++)
	{
		fprintf(pfDest, " (%d->%d) ", v_slots_in_use.at(ii*2), v_slots_in_use.at(ii*2 + 1));
	}//for  (int  ii = 0; ii < iMaxSlotNumber; ii++)
}//void  CFiberConnectionOld::vSaveStatePRW(FILE  *pfDest)




int  CFiberConnectionOld::iGetNumberOfFreeSlotsBelow(double  dMax)
{
	int  i_result = (int) dMax;
	int  i_buf;

	for  (int  ii = 0; ii < v_slots_in_use.size()/2; ii++)
	{
		i_buf = v_slots_in_use.at(ii*2 + 1) - v_slots_in_use.at(ii*2);
		i_result = i_result - i_buf;
	}//for  (int  ii = 0; ii < v_slots_in_use.size(); ii++)

	return(i_result);
}//int  CFiberConnectionOld::iGetNumberOfFreeSlotsBelow(double  dMax)




int  CFiberConnectionOld::iGetLowestStartSlotForChannel(int  iFirstSlot, int iSlotsNumber)
{
	int  i_first_slot_allowed = iFirstSlot;


	for  (int  ii = 0; ii < v_slots_in_use.size()/2; ii++)
	{
		if  (v_slots_in_use.at(ii*2 + 1)  >= i_first_slot_allowed)
		{
			if  (v_slots_in_use.at(ii*2) <= i_first_slot_allowed + iSlotsNumber - 1)
			{
				i_first_slot_allowed = v_slots_in_use.at(ii*2 + 1) + 1;
			}//if  (v_slots_in_use.at(ii*2) <= i_first_slot_allowed + iSlotsNumber)
			else
			{
				return(i_first_slot_allowed);
			}//else  if  (v_slots_in_use.at(ii*2) <= i_first_slot_allowed + iSlotsNumber)

		}//if  (v_slots_in_use.at(ii*2 + 1)  >= i_first_slot_allowed)
	}//for  (int  ii = 0; ii < v_slots_in_use.size(); ii++)

	return(i_first_slot_allowed);
}//int  CFiberConnectionOld::iGetLowestStartSlotForChannel(int  iFirstSlot, int iSlotsNumber)


void  CFiberConnectionOld::vReset()
{
	v_slots_in_use.clear();
}//void  CFiberConnectionOld::vReset()asd





CError  CFiberConnectionOld::eConfig(int  iConectionId, int  iFiberNodeStartOffset, int  iFiberNodeEndOffset, CFiberNet *pcParent, double  dConnectionCapacity)
{
	CError c_err;

	//Tools::vShow("aaaa");

	CString  s_buf;

	i_connection_id = iConectionId;
	i_fiber_node_start_offset = iFiberNodeStartOffset;
	i_fiber_node_end_offset = iFiberNodeEndOffset;
	pc_parent = pcParent;
	d_connection_capacity = dConnectionCapacity;

	if  (
		(i_fiber_node_start_offset  <  0)||(i_fiber_node_start_offset  >=  pc_parent->i_nodes_num)||
		(i_fiber_node_end_offset  <  0)||(i_fiber_node_end_offset  >=  pc_parent->i_nodes_num)
		)
	{
		s_buf.Format("%d connection - wrong start/end node", i_connection_id);	
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  (	


	return(c_err);
}//void  CFiberConnectionOld::vConfig(int  iConectionId, int  iFiberNodeStartOffset, int  iFiberNodeEndOffset, CFiberNet *pcParent, double  dConnectionCapacity)





//---------------------------class  CFiberConnectionCores--------------------------------------
CFiberConnection::CFiberConnection()
{
	i_connection_id = -1;
	i_fiber_node_start_offset = -1;
	i_fiber_node_end_offset = -1;
	pc_parent = NULL;
	d_connection_capacity = 0;
	i_cores = 0;
}//CFiberConnection::CFiberConnection(int  iFiberNodeStartOffset, int  iFiberNodeEndOffset, CFiberNet *pcParent, double  dConnectionCapacity)



CFiberConnection::~CFiberConnection()
{

}//CFiberConnection::~CFiberConnection()


int  CFiberConnection::iGetMaxSlot()
{
	for  (int  ii = this->v_slots.size()-1; ii >= 0; ii--)
	{
		if  (v_slots.at(ii) > 0)  return(ii);
	}//for  (int  ii = this->v_slots.size()-1; ii >= 0; ii++)

	return(0);
}//int  CFiberConnectionCores::iGetMaxSlot()



void  CFiberConnection::vReset()
{
	for  (int  ii = 0; ii < v_slots.size(); ii++)
		v_slots.at(ii) = 0;

	v_slots_full.clear();
}//void  CFiberConnectionCores::vReset()




CError  CFiberConnection::eReserveSlots(int  iFirstSlot, int iSlotsNumber, bool  bErrorControl /*= true*/)
{
	CError  c_err;
	CString  s_buf;

	
	if  (bErrorControl  ==  true)
	{
		int  i_test;
		i_test = iGetLowestStartSlotForChannel(iFirstSlot, iSlotsNumber);
		if  (i_test  != iFirstSlot)
		{
			s_buf.Format("unable to settle channel in node connection %d at start slot: %d width: %d", i_connection_id, iFirstSlot, iSlotsNumber);			
			c_err.vPutError(s_buf);

			return(c_err);
		}//if  (i_test  != iFirstSlot)
	}//if  (bErrorControl  ==  true)


	//add slots if necessary
	while  (v_slots.size()  <  iFirstSlot + iSlotsNumber)	v_slots.push_back(0);


	int  i_first_range_before;
	i_first_range_before = -1;

	bool  b_found = false;
	for  (int  ii = 0; ii < (v_slots_full.size()/2)&&(b_found == false); ii++)
	{
		if  (v_slots_full.at(ii*2) > iFirstSlot)
		{
			b_found = true;
		}//if  (v_slots_full.at(ii*2) > iFirstSlot)
		else
			i_first_range_before = ii;
	}//for  (int  ii = 0; ii < (v_slots_full.size()/2)&&(b_found == false); ii++)


	int  i_full_slot_decision;//0-add new; -1 -glue to before;
	int  i_full_slot_pos;//-1-push;  0 and more - insert

	for  (int  i_slot = iFirstSlot; i_slot < iFirstSlot + iSlotsNumber; i_slot++)
	{
		v_slots.at(i_slot)++;

		//full slot making
		if  (v_slots.at(i_slot)  >=  i_cores)
		{
			if  (i_first_range_before < 0)
			{
				i_full_slot_decision = 0;
				i_full_slot_pos = 0;

				if  (v_slots_full.size() == 0)
				{
					i_full_slot_decision = 0;
					i_full_slot_pos = -1;
				}//if  (v_slots_full.size() == 0)
				
			}//if  (i_first_range_before < 0)
			else
			{
				if  (v_slots_full.at(i_first_range_before * 2 + 1)  +  1 == i_slot)
				{
					i_full_slot_decision = -1;
					i_full_slot_pos = i_first_range_before;
				}//if  (v_slots_full.at(i_first_range_before * 2 + 1)  +  1 == i_slot)
				else
				{
					i_full_slot_decision = 0;
					if  (i_first_range_before + 1 == v_slots_full.size()/2)
						i_full_slot_pos = -1;
					else
						i_full_slot_pos = i_first_range_before + 1;				
				}//else  if  (v_slots_full.at(i_first_range_before * 2 + 1)  +  1 == i_slot)
			}//else  if  (i_first_range_before < 0)


			if  ( (i_full_slot_decision == 0)&&(i_full_slot_pos == -1) )
			{
				v_slots_full.push_back(i_slot);
				v_slots_full.push_back(i_slot);
				i_first_range_before = v_slots_full.size() / 2 - 1;

				/*if  (v_slots_full.at(0) == 10)
				{
					Tools::vShow("aaa");
				}*/
			}//if  ( (i_full_slot_decision == 0)&&(i_full_slot_pos == -1) )

			if  ( (i_full_slot_decision == 0)&&(i_full_slot_pos >= 0) )
			{
				v_slots_full.insert(v_slots_full.begin() + i_full_slot_pos * 2, i_slot);
				v_slots_full.insert(v_slots_full.begin() + i_full_slot_pos * 2, i_slot);
				i_first_range_before = i_full_slot_pos;

				/*if  (v_slots_full.at(0) == 10)
				{
					Tools::vShow("aaa");
				}*/

				
			}//if  ( (i_full_slot_decision == 0)&&(i_full_slot_pos == -1) )


			if  (i_full_slot_decision == -1)
			{
				v_slots_full.at(i_first_range_before * 2 + 1) = i_slot;

				//check glueing with the next...
				if  ((i_first_range_before+1) * 2 + 1  <  v_slots_full.size())
				{
					if  (v_slots_full.at((i_first_range_before+1) * 2) == i_slot + 1)
					{
						v_slots_full.at(i_first_range_before * 2 + 1) = v_slots_full.at((i_first_range_before+1) * 2 + 1);

						//now remove next range
						v_slots_full.erase(v_slots_full.begin() + (i_first_range_before+1)*2);
						v_slots_full.erase(v_slots_full.begin() + (i_first_range_before+1)*2);					
					}//if  (v_slots_full.at((i_first_range_before+1) * 2) == i_slot + 1)
				}//if  ((i_first_range_before+1) * 2 + 1  <  v_slots_full.size())

			}//if  ( (i_full_slot_decision == 0)&&(i_full_slot_pos == -1) )

		
		}//if  (v_slots.at(i_slot)  >=  i_cores)
	}//for  (int  i_slot = iFirstSlot; i_slot < iFirstSlot + iSlotsNumber; i_slot++)


	//v_check_ranges_correctness();


	return(c_err);
}//CError  CFiberConnectionCores::eReserveSlots(int  iFirstSlot, int iSlotsNumber)



void  CFiberConnection::v_check_ranges_correctness()
{
	for  (int i_slot = 0; i_slot < v_slots.size(); i_slot++)
	{
		if  (v_slots.at(i_slot)  <  i_cores)
		{
			for  (int i_range = 0; i_range < v_slots_full.size()/2; i_range++)
			{
				if  ( (v_slots_full.at(i_range*2) <= i_slot) && (i_slot <= v_slots_full.at(i_range*2)) )
				{
					Tools::vShow("wrong 1");
				}//if  ( (v_slots_full.at(i_range*2) <= i_slot) && (i_slot <= v_slots_full.at(i_range*2)) )
			
			}//for  (int i_range = 0; i_range < v_slots_full.size(); i_range++)
		}//if  (v_slots.at(ii)  <  i_cores)
		else
		{
			bool  b_found = false;
			for  (int i_range = 0; (i_range < v_slots_full.size()/2)&&(b_found == false); i_range++)
			{
				if  ( (v_slots_full.at(i_range*2) <= i_slot) && (i_slot <= v_slots_full.at(i_range*2 + 1)) )
				{
					b_found = true;
				}//if  ( (v_slots_full.at(i_range*2) <= i_slot) && (i_slot <= v_slots_full.at(i_range*2)) )			
			}//for  (int i_range = 0; i_range < v_slots_full.size(); i_range++)	

			if  (b_found == false)
			{
				Tools::vShow("wrong 2");
			}//if  (b_found == false)
		}//else  if  (v_slots.at(ii)  <  i_cores)
	}//for  (int ii = 0; ii < v_slots.size(); ii++)

}//void  CFiberConnectionCores::v_check_ranges_correctness()




int  CFiberConnection::iGetLowestStartSlotForChannel(int  iFirstSlot, int iSlotsNumber)
{
	int  i_lowest_slot = iFirstSlot;


	int  i_first_slot_allowed = iFirstSlot;


	bool  b_found = false;//REMOVE
	for  (int  ii = 0; (ii < v_slots_full.size()/2)&&(b_found == false); ii++)
	{
		if  (v_slots_full.at(ii*2 + 1)  >= i_first_slot_allowed)
		{
			if  (v_slots_full.at(ii*2) <= i_first_slot_allowed + iSlotsNumber - 1)
			{
				i_first_slot_allowed = v_slots_full.at(ii*2 + 1) + 1;
			}//if  (v_slots_in_use.at(ii*2) <= i_first_slot_allowed + iSlotsNumber)
			else
			{
				b_found = true;
				return(i_first_slot_allowed);
			}//else  if  (v_slots_in_use.at(ii*2) <= i_first_slot_allowed + iSlotsNumber)

		}//if  (v_slots_in_use.at(ii*2 + 1)  >= i_first_slot_allowed)
	}//for  (int  ii = 0; ii < v_slots_in_use.size(); ii++)

	return(i_first_slot_allowed);


	//for  (int  i_slot = iFirstSlot; (i_slot < i_lowest_slot + iSlotsNumber)&&(i_slot  <  v_slots.size()); i_slot++)
	//{
	//	if  (v_slots.at(i_slot)  >=  i_cores)  i_lowest_slot = i_slot + 1;
	//}//for  (int  i_slot = iFirstSlot; i_slot < iFirstSlot + iSlotsNumber; i_slot++)


	//if  (i_first_slot_allowed != i_lowest_slot)
	//{
	//	Tools::vShow("wrong 58");
	//}//if  (i_first_slot_allowed != i_lowest_slot)


	//return(i_lowest_slot);
}//int  CFiberConnectionCores::iGetLowestStartSlotForChannel(int  iFirstSlot, int iSlotsNumber)



int  CFiberConnection::iGetNumberOfFreeSlotsBelow(double  dMax)
{
	//int  i_result = (int) dMax;
	//int  i_buf;

	//for  (int  i_slot = 0; i_slot < v_slots.size(); i_slot++)
	//{
	//	if  (v_slots.at(i_slot)  >=  i_cores)  i_result--;
	//}//for  (int  ii = 0; ii < v_slots_in_use.size(); ii++)



	int  i_result = (int) dMax;
	int  i_buf;

	for  (int  ii = 0; ii < v_slots_full.size()/2; ii++)
	{
		i_buf = v_slots_full.at(ii*2 + 1) - v_slots_full.at(ii*2) + 1;
		i_result = i_result - i_buf;
	}//for  (int  ii = 0; ii < v_slots_in_use.size(); ii++)

	return(i_result);
}//int  CFiberConnectionCores::iGetNumberOfFreeSlotsBelow(double  dMax)



CError  CFiberConnection::eConfig(int  iConectionId, int  iFiberNodeStartOffset, int  iFiberNodeEndOffset, CFiberNet *pcParent, double  dConnectionCapacity, int iCores)
{
	CError c_err;

	CString  s_buf;

	i_connection_id = iConectionId;
	i_fiber_node_start_offset = iFiberNodeStartOffset;
	i_fiber_node_end_offset = iFiberNodeEndOffset;
	pc_parent = pcParent;
	d_connection_capacity = dConnectionCapacity;
	i_cores = iCores;

	if  (
		(i_fiber_node_start_offset  <  0)||(i_fiber_node_start_offset  >=  pc_parent->i_nodes_num)||
		(i_fiber_node_end_offset  <  0)||(i_fiber_node_end_offset  >=  pc_parent->i_nodes_num)
		)
	{
		s_buf.Format("%d connection - wrong start/end node", i_connection_id);	
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  (	


	return(c_err);
}//void  CFiberConnection::vConfig(int  iConectionId, int  iFiberNodeStartOffset, int  iFiberNodeEndOffset, CFiberNet *pcParent, double  dConnectionCapacity)asd



bool  CFiberConnection::bSlotInUse(int iSlotNumber)
{
	if  (v_slots.size() >  iSlotNumber)
	{
		if (v_slots.at(iSlotNumber)  >=  i_cores)  return(true);
	}//if  (v_slots.size() >  iSlotNumber)


	return(false);
}//bool  CFiberConnectionCores::bSlotInUse(int iSlotNumber)



void  CFiberConnection::vSaveStatePRW(FILE  *pfDest)
{
	fprintf(pfDest, "edge %.3d    ", i_connection_id);

	int  i_current_cores_taken;
	i_current_cores_taken = -1;

	int  i_period_start, i_period_end;

	for  (int  ii = 0; ii < v_slots.size(); ii++)
	{
		if  (v_slots.at(ii) !=  i_current_cores_taken)
		{
			i_period_end = ii-1;
			
			if  (i_period_end >= 0)
				fprintf(pfDest, " (%d[%d]->%d) ", i_period_start, i_current_cores_taken, i_period_end);

			i_period_start = ii;
			i_current_cores_taken = v_slots.at(ii);
		}//if  (v_slots.at(ii) !=  i_current_cores_taken)		
	}//for  (int  ii = 0; ii < iMaxSlotNumber; ii++)
}//void  CFiberConnectionCores::vSaveStatePRW(FILE  *pfDest)



void  CFiberConnection::vSaveState(FILE  *pfDest, int iMaxSlotNumber)
{
	fprintf(pfDest, "edge %.3d    ", i_connection_id);

	for  (int  ii = 0; ii < iMaxSlotNumber; ii++)
	{
		if  (ii  <  v_slots.size())
			fprintf(pfDest, "%d   ",v_slots.at(ii));
		else
			fprintf(pfDest, "0   ");

	}//for  (int  ii = 0; ii < iMaxSlotNumber; ii++)
}//void  CFiberConnectionCores::vSaveState(FILE  *pfDest, int iMaxSlotNumber)

//---------------------------class  CFiberDemand--------------------------------------
CFiberDemand::CFiberDemand()
{
	i_demand_id = -1;
	i_fiber_node_start_offset = -1;
	i_fiber_node_end_offset = -1;
	pc_parent = NULL;
	d_demand_capacity = 0;
	i_demand_type = DEMAND_TYPE_UNICAST;
	pc_anycast_twin_dfemand = NULL;
	d_path_fit_sum = 0;
}//CFiberDemand::CFiberDemand()
		


CFiberDemand::~CFiberDemand()
{

}//CFiberDemand::~CFiberDemand()



void  CFiberDemand::v_copy_offsets(vector  <int> *pvDest, vector<int>  *pvSource, int iMaxPathTreesAvailable)
{
	for  (int  ii  = 0; (ii < pvSource->size())&&( (ii < iMaxPathTreesAvailable)||(iMaxPathTreesAvailable == -1) ); ii++)
		pvDest->push_back(pvSource->at(ii));
}//void  CFiberDemand::v_copy_offsets(vector  <int> *pvDest, vector<int>  *pvSource, iMaxPathTreesAvailable)


CError  CFiberDemand::eConfig
	(
	int  iDemandId, 
	int  iFiberNodeStartOffset, int  iFiberNodeEndOffset, double  dDemandCapacity, 
	CFiberNet *pcParent,
	int  iDemandType,
	CFiberDemand *pcAnycastTwinDemand,
	int  iMaxPathTreesAvailable
	)
{
	CError  c_err;

	CString  s_buf;

	i_demand_id = iDemandId;
	pc_parent = pcParent;
	d_demand_capacity = dDemandCapacity;
	pc_anycast_twin_dfemand = pcAnycastTwinDemand;
	i_demand_type = iDemandType;

	i_fiber_node_start_offset = iFiberNodeStartOffset;
	i_fiber_node_end_offset = iFiberNodeEndOffset;

	if  (i_demand_type  ==  DEMAND_TYPE_UNICAST)
	{
		if  (
			(i_fiber_node_start_offset  <  0)||(i_fiber_node_start_offset  >=  pc_parent->i_nodes_num)||
			(i_fiber_node_end_offset  <  0)||(i_fiber_node_end_offset  >=  pc_parent->i_nodes_num)
			)
		{
			s_buf.Format("%d demand - wrong start/end node", i_demand_id);	
			c_err.vPutError(s_buf);
			return(c_err);
		}//if  (	
	}//if  (i_demand_type  ==  DEMAND_TYPE_UNICAST)
	

	if  (i_demand_type  ==  DEMAND_TYPE_ANYCAST)
	{
		if  (
			(i_fiber_node_start_offset  <  -1)||(i_fiber_node_start_offset  >=  pc_parent->i_nodes_num)||
			(i_fiber_node_end_offset  <  -1)||(i_fiber_node_end_offset  >=  pc_parent->i_nodes_num)
			)
		{
			s_buf.Format("%d demand - wrong start/end node", i_demand_id);	
			c_err.vPutError(s_buf);
			return(c_err);
		}//if  (		
	}//if  (i_demand_type  ==  DEMAND_TYPE_ANYCAST)



	if  (i_demand_type  ==  DEMAND_TYPE_MULTICAST)
	{
		if  (
			(i_fiber_node_start_offset  <  0)||(i_fiber_node_start_offset  >=  pc_parent->i_nodes_num)
			)
		{
			s_buf.Format("%d demand - wrong tree root node", i_demand_id);	
			c_err.vPutError(s_buf);
			return(c_err);
		}//if  (		
	}//if  (i_demand_type  ==  DEMAND_TYPE_ANYCAST)





	v_possible_paths.clear();
	vector <int>  *pv_poss_paths_offsets;
	vector <int>  v_anycast_paths_buf;
	pv_poss_paths_offsets = NULL;

	if  (i_demand_type  ==  DEMAND_TYPE_UNICAST)
	{
		pv_poss_paths_offsets = &(pc_parent->pv_paths_between_nodes[i_fiber_node_start_offset][i_fiber_node_end_offset]);
	}//if  (i_demand_type  ==  DEMAND_TYPE_UNICAST)


	if  (i_demand_type  ==  DEMAND_TYPE_MULTICAST)
	{
		CFiberPath  *pc_poss_tree;

		//first find demand offset
		bool  b_found = false;
		for  (int  i_dem_offset = 0; (i_dem_offset < pc_parent->i_multi_demands)&&(b_found == false); i_dem_offset++)
		{
			if  (pc_parent->pc_demands[pc_parent->i_uni_demands + i_dem_offset].i_demand_id  ==  i_demand_id)
			{
				for  (int i_tree = 0; i_tree < 1000; i_tree++)
				{
					pc_poss_tree = &(pc_parent->pc_trees[i_dem_offset * 1000 + i_tree]);

					//if  (i_fiber_node_start_offset  ==  pc_poss_tree->iGetStartNode())
					{
						if  (
							(iMaxPathTreesAvailable  ==  -1)||
							(iMaxPathTreesAvailable  >  v_possible_paths.size())
							)
						{
							pc_poss_tree->vSetActive(true);
							v_possible_paths.push_back(pc_poss_tree);
						}//if  (
						
					}//if  (i_fiber_node_start_offset  ==  pc_poss_tree->iGetStartNode())
				
				}//for  (int i_tree = 0; i_tree < iMaxPathTreesAvailable; i_tree++)
			}//if  (pc_parent->pc_demands[pc_parent->i_uni_demands + i_dem_offset]  ==  this)
		}//for  (int  i_dem_offset = 0; (i_dem_offset < pc_parent->i_multi_demands)&&(b_found == false); i_dem_offset++)

		//for multicast we initialize exactly here and do nothing else
		/*for  (int  ii = 0; ii < pc_parent->i_tree_num; ii++)
		{
			pc_poss_tree = &(pc_parent->pc_trees[ii]);

			if  (i_fiber_node_start_offset  ==  pc_poss_tree->iGetStartNode())
			{
				if  (
					(iMaxPathTreesAvailable  ==  -1)||
					(iMaxPathTreesAvailable  >  v_possible_paths.size())
					)
				{
					pc_poss_tree->vSetActive(true);
					v_possible_paths.push_back(pc_poss_tree);
				}//if  (
				
			}//if  (i_fiber_node_start_offset  ==  pc_poss_tree->iGetStartNode())		
		}//for  (int  ii = 0; ii < pc_parent->i_tree_num; ii++)*/
	}//if  (i_demand_type  ==  DEMAND_TYPE_MULTICAST)


	if  (i_demand_type  ==  DEMAND_TYPE_ANYCAST)
	{
		if  (i_fiber_node_start_offset  ==  -1)
		{
			//s_buf.Format("replica number: %d", pc_parent->i_rep_num);
			//Tools::vShow(s_buf);

			for  (int  ii = 0; ii < pc_parent->i_rep_num; ii++)
			{
				//s_buf.Format("path: %d->%d", pc_parent->pc_replices[ii]->i_node_number, i_fiber_node_end_offset);
				//Tools::vShow(s_buf);
				v_copy_offsets(&v_anycast_paths_buf, &(pc_parent->pv_paths_between_nodes[pc_parent->pc_replices[ii]->i_node_number][i_fiber_node_end_offset]), iMaxPathTreesAvailable);
			}//for  (int  ii = 0; ii < pc_parent->i_rep_num; ii++)
		}//if  (i_fiber_node_start_offset  ==  -1)


		if  (i_fiber_node_end_offset  ==  -1)
		{
			for  (int  ii = 0; ii < pc_parent->i_rep_num; ii++)
			{
				v_copy_offsets(&v_anycast_paths_buf, &(pc_parent->pv_paths_between_nodes[i_fiber_node_start_offset][pc_parent->pc_replices[ii]->i_node_number]), iMaxPathTreesAvailable);
			}//for  (int  ii = 0; ii < pc_parent->i_rep_num; ii++)
		}//if  (i_fiber_node_end_offset  ==  -1)
		
		pv_poss_paths_offsets = &v_anycast_paths_buf;	

		//s_buf.Format("path: (%d->%d) anycast path buf size: %d", i_fiber_node_start_offset, i_fiber_node_end_offset, v_anycast_paths_buf.size());
		//Tools::vShow(s_buf);asd




	}//if  (i_demand_type  ==  DEMAND_TYPE_ANYCAST)




	if  (pv_poss_paths_offsets  !=  NULL)
	{
		CFiberPath  *pc_poss_path;
		for  (int  ii = 0; ii < pv_poss_paths_offsets->size(); ii++)
		{
			if  (
				(iMaxPathTreesAvailable  ==  -1)||
				(i_demand_type  ==  DEMAND_TYPE_ANYCAST)||  // for ANYCAST  it is already filtered in v_copy_offsets
				(iMaxPathTreesAvailable  >  v_possible_paths.size())
				)
			{
				pc_poss_path = &(pc_parent->pc_paths[pv_poss_paths_offsets->at(ii)]);
				pc_poss_path->vSetActive(true);
				v_possible_paths.push_back(pc_poss_path);
			}//if  (
		}//for  (int  ii = 0; ii < pv_poss_paths_offsets->size(); ii++)
	}//if  (pv_poss_paths_offsets  !=  NULL)

	
	

	return(c_err);
}//CError  CFiberDemand::eConfig(int  iDemandId, int  iFiberNodeStartOffset, int  iFiberNodeEndOffset, double  dDemandCapacity, CFiberNet *pcParent)


bool  CFiberDemand::bDominatingTwin()
{
	if  (pc_anycast_twin_dfemand  ==  NULL)  return(true);

	if  (d_demand_capacity == pc_anycast_twin_dfemand->d_demand_capacity)
	{
		if  (i_demand_id  >  pc_anycast_twin_dfemand->i_demand_id)
			return(true);
		else
			return(false);
	}//if  (d_demand_capacity == pc_anycast_twin_dfemand->d_demand_capacity)

	if  (d_demand_capacity > pc_anycast_twin_dfemand->d_demand_capacity)
		return(true);
	else
		return(false);

	return(false);
}//bool  CFiberDemand::bDominatingTwin()



void  CFiberDemand::vSummarizePathFitness()
{
	d_path_fit_sum = 0;

	for  (int  ii = 0; ii < v_possible_paths.size(); ii++)
	{
		d_path_fit_sum +=  v_possible_paths.at(ii)->dGetPathFitness();
	}//for  (int  ii = 0; ii < v_possible_paths.size(); ii++)
}//void  CFiberDemand::vSummarizePathFitness()



void  CFiberDemand::vSave(FILE  *pfDest)
{
	if  (i_demand_type ==  DEMAND_TYPE_UNICAST)  fprintf(pfDest, "unicastdemand ");
	if  (i_demand_type ==  DEMAND_TYPE_ANYCAST)  fprintf(pfDest, "anycastdemand ");
	if  (i_demand_type ==  DEMAND_TYPE_MULTICAST)  fprintf(pfDest, "multicastdemand ");



	fprintf(pfDest, "%d\t", i_fiber_node_start_offset);
	fprintf(pfDest, "%d\t", i_fiber_node_end_offset);
	fprintf(pfDest, "%d\t", (int) d_demand_capacity);
}//void  CFiberDemand::vSave(FILE  *pfDest)



//---------------------------class  CFiberPath--------------------------------------
CFiberPath::CFiberPath()
{
	i_path_id = -1;
	i_fiber_node_start_offset = -1;
	i_fiber_node_end_offset = -1;

	b_active = false;
	b_tree = false;

	d_length = 0;
	d_mp = 1;
	d_delta_s = 6.25;

	d_new_transfer_multiplier = 0;

	pc_parent = NULL;
	pc_slave_path = NULL;
}//CFiberPath::CFiberPath()



CFiberPath::~CFiberPath()
{
	if  (pc_slave_path !=  NULL)  delete pc_slave_path;
}//CFiberPath::~CFiberPath()




void  CFiberPath::vConfigure(int  iPathId, CFiberNet *pcParent)
{
	i_path_id = iPathId;
	pc_parent = pcParent;
}//void  CFiberPath::vConfigure(int  iPathId, CFiberNet *pcParent)



bool  CFiberPath::bIsConnPartOfPath(CFiberConnection  *pcFiberConn)
{
	for  (int  ii = 0; ii < v_path_connections.size(); ii++)
	{
		if  (v_path_connections.at(ii)  ==  pcFiberConn)  return(true);
	}//for  (int  ii = 0; ii < v_path_connections.size(); ii++)

	return(false);
}//bool  bIsConnPartOfPath(CFiberConnection  *pcFiberConn)



void  CFiberPath::vSaveWithLinks(FILE  *pfDest)
{
	for  (int  ii = 0; ii < v_path_connections.size(); ii++)
	{
		fprintf(pfDest, "%d ", v_path_connections.at(ii)->i_connection_id);
	}//for  (int  ii = 0; ii < v_path_connections.size(); ii++)
}//void  CFiberPath::vSaveWithLinks(FILE  *pfDest)


void  CFiberPath::vSave(FILE  *pfDest)
{
	for  (int  ii = 0; ii < pc_parent->i_conns_num; ii++)
	{
		if  (ii  >  0)  fprintf(pfDest, " ");

		if  (bIsConnPartOfPath(&(pc_parent->pc_conns[ii]))  ==  true)
			fprintf(pfDest, "1");
		else
			fprintf(pfDest, "0");
	}//for  (int  ii = 0; ii < pc_parent->i_conns_num; ii++)
}//void  CFiberPath::vSave(FILE  *pfDest)



void  CFiberPath::vSaveMod(FILE  *pfDest)
{
	fprintf(pfDest, "%d\t%d\n", (int) d_length, (int) d_mp);
}//void  CFiberPath::vSaveMod(FILE  *pfDest)



void  CFiberPath::vSaveSpec(FILE  *pfDest)
{
	for  (int  ii = 0; ii < v_slot_number_for_transfer.size(); ii++)
	{
		fprintf(pfDest, "%d\t", v_slot_number_for_transfer.at(ii));
	}//for  (int  ii = 0; ii < v_slot_number_for_transfer.size(); ii++)

}//void  CFiberPath::vSaveSpec(FILE  *pfDest)


void  CFiberPath::vSaveReg(FILE  *pfDest)
{
	for  (int  ii = 0; ii < v_reg_number_for_transfer.size(); ii++)
	{
		fprintf(pfDest, "%d\t", v_reg_number_for_transfer.at(ii));
	}//for  (int  ii = 0; ii < v_slot_number_for_transfer.size(); ii++)
}//void  CFiberPath::vSaveReg(FILE  *pfDest)



CError  CFiberPath::eLoadMod(CString  sModLine)
{
	CError  c_err;

	int  i_index;
	i_index = 0;
	d_length = Tools::dExtractFromString(sModLine, i_index, &i_index);
	d_mp = Tools::dExtractFromString(sModLine, i_index, &i_index);

	return(c_err);
}//CError  CFiberPath::eLoadMod(CString  sModLine)


int  CFiberPath::iGetSlotsNumber(double  dDemandCapacity)
{
	if  (b_tree  ==  true)
	{
		if  (v_slot_number_for_transfer.size()  !=  1)
		{
			CString  s_buf;
			s_buf.Format("Number of slots in the spec file for tree different than 1 (%d)", v_slot_number_for_transfer.size());

			CError  c_err;
			c_err.vPutError(s_buf);

			return(0);
		}//if  (v_slot_number_for_transfer.size()  !=  1)

		return(v_slot_number_for_transfer.at(0));
	}//if  (b_tree  ==  true)

	double  d_slot_number_offset;
	d_slot_number_offset = dDemandCapacity / d_new_transfer_multiplier;

	int  i_slot_number_offset;
	i_slot_number_offset = (int) ::Math::Round(d_slot_number_offset);
	i_slot_number_offset--;//we start from zero
	if  (i_slot_number_offset < 0)  i_slot_number_offset = 0;

	if  (i_slot_number_offset  >=  v_slot_number_for_transfer.size())
	{
		CString  s_buf;
		s_buf.Format
			(
			"Number of slots in the spec file for tree too small (%d positions). Required for %.2lf transfer: %d slots",
			v_slot_number_for_transfer.size(), dDemandCapacity,
			i_slot_number_offset
			);

		CError  c_err;
		c_err.vPutError(s_buf);

		return(0);
	}//if  (v_slot_number_for_transfer.size()  !=  1)

	return(v_slot_number_for_transfer.at(i_slot_number_offset));
	


	//old slot computation
	double  d_slot_number;

	d_slot_number = dDemandCapacity / (d_mp * 4.0 * d_delta_s);
	int  i_slot_number;
	i_slot_number = (int) d_slot_number;
	if  (i_slot_number  <  d_slot_number)  i_slot_number++;
	i_slot_number *= 2;

	return(i_slot_number);
}//int  CFiberPath::iGetSlotsNumber(double  dDemandCapacity)





CError  CFiberPath::eSetPathSlots(int  iFirstSlot, int  iSlotsNumber)
{
	CError  c_err;

	for  (int  i_conn = 0; i_conn < v_path_connections.size(); i_conn++)
	{
		c_err  = v_path_connections.at(i_conn)->eReserveSlots(iFirstSlot, iSlotsNumber);
		if  (c_err)  return(c_err);
	}//for  (int  i_conn = 0; i_conn < v_path_connections.size(); i_conn++)

	return(c_err);
}//CError  CFiberPath::eSetPathModules(int  iModulesNumber)




CError  CFiberPath::eFindPath(int  *piFirstSlot, int  iSlotsNumber)
{
	CError  c_err;

	int  i_current_start_slot = 0;
	int  i_proposed_start_slot;


	for  (int  i_conn = 0; i_conn < v_path_connections.size(); i_conn++)
	{
		i_proposed_start_slot  = v_path_connections.at(i_conn)->iGetLowestStartSlotForChannel(i_current_start_slot, iSlotsNumber);
		if  (i_proposed_start_slot  !=  i_current_start_slot)
		{
			i_conn = -1;
			i_current_start_slot = i_proposed_start_slot;
		}//if  (i_proposed_start_slot  !=  i_current_start_slot)
	}//for  (int  i_conn = 0; i_conn < v_path_connections.size(); i_conn++)

	*piFirstSlot = i_current_start_slot;

	return(c_err);
}//CError  CFiberPath::eFindPath(int  *piFirstSlot, int  iSlotsNumber)





CError  CFiberPath::eSetPath(int  iFirstSlot, int  iSlotsNumber)
{
	CError  c_err;

	for  (int  i_conn = 0; i_conn < v_path_connections.size(); i_conn++)
	{
		c_err  = v_path_connections.at(i_conn)->eReserveSlots(iFirstSlot, iSlotsNumber);
		if  (c_err)  
		{
			c_err.vPutError("v_path_connections.at(i_conn)->eSetPath");
			return(c_err);
		}//if  (c_err)  
	}//for  (int  i_conn = 0; i_conn < v_path_connections.size(); i_conn++)

	return(c_err);
}//CError  CFiberPath::eSetPath(int  iFirstSlot, int  iSlotsNumber)



CError  CFiberPath::eFindAndSetPathSlots(int  *piFirstSlot, int  iSlotsNumber)
{
	CError  c_err;

	int  i_current_start_slot = 0;
	int  i_proposed_start_slot;


	for  (int  i_conn = 0; i_conn < v_path_connections.size(); i_conn++)
	{
		i_proposed_start_slot  = v_path_connections.at(i_conn)->iGetLowestStartSlotForChannel(i_current_start_slot, iSlotsNumber);
		if  (i_proposed_start_slot  !=  i_current_start_slot)
		{
			i_conn = -1;
			i_current_start_slot = i_proposed_start_slot;
		}//if  (i_proposed_start_slot  !=  i_current_start_slot)
	}//for  (int  i_conn = 0; i_conn < v_path_connections.size(); i_conn++)

	*piFirstSlot = i_current_start_slot;

	for  (int  i_conn = 0; i_conn < v_path_connections.size(); i_conn++)
	{
		c_err  = v_path_connections.at(i_conn)->eReserveSlots(i_current_start_slot, iSlotsNumber);
		if  (c_err)  
		{
			c_err.vPutError("v_path_connections.at(i_conn)->eReserveSlots");
			return(c_err);
		}//if  (c_err)  
	}//for  (int  i_conn = 0; i_conn < v_path_connections.size(); i_conn++)

	return(c_err);
}//CError  CFiberPath::eFindAndSetPathSlots(int  *piFirstSlot, int  iSlotsNumber)



CError  CFiberPath::eSetPathCapacity(int  iFirstSlot, double  dDemandCapacity)
{
	CError  c_err;

	int  i_modules = iGetSlotsNumber(dDemandCapacity);
	c_err  = eSetPathSlots(iFirstSlot, i_modules);

	return(c_err);
}//CError  CFiberPath::eSetPathCapacity(double  dDemandCapacity)



void  CFiberPath::vSetPathFitness(double  dSolutionValue)
{
	//if the path is not active we dont comute its fitness
	if  (b_active  ==  false)
	{
		d_path_fitness = 0;
		return;
	}//if  (b_active  ==  false)

	int  i_min_slots;
	int  i_buf;

	i_min_slots = 0;
	for  (int  i_conn = 0; i_conn < v_path_connections.size(); i_conn++)
	{
		i_buf  =  v_path_connections.at(i_conn)->iGetNumberOfFreeSlotsBelow(dSolutionValue);
		if  (i_conn == 0)  i_min_slots = i_buf;
		if  (i_buf  <  i_min_slots)  i_min_slots = i_buf;		
	}//for  (int  i_conn = 0; i_conn < v_path_connections.size(); i_conn++)

	d_path_fitness = i_min_slots;
	if  (d_path_fitness  <  1)  d_path_fitness  =  1;
}//void  CFiberPath::vSetPathWeight(double  dSolutionValue)




CError  CFiberPath::eLoad(CString  sPathLine, bool  bControlConnectionsOrder)
{
	CError  c_err;
	CString  s_buf;

	int  i_index;
	i_fiber_node_start_offset = -1;
	i_fiber_node_start_offset = -1;
	v_path_connections.clear();
	i_index = 0;
	int  i_conn_in_path;
	vector<CFiberConnection *>  v_path_connections_buf;

	
	for  (int i_conn_offset = 0; i_conn_offset < pc_parent->i_conns_num; i_conn_offset++)
	{
		
		i_conn_in_path = Tools::iExtractFromString(sPathLine, i_index, &i_index);
		if  ( (i_conn_offset  <  0)||(pc_parent->i_conns_num <= i_conn_offset) )
		{
			s_buf.Format("Connection %d offset in path %d outside borders", i_conn_offset, i_path_id);
			c_err.vPutError(s_buf);
			return(c_err);
		}//if  ( (i_conn_offset  <  0)||(pc_parent->i_conns_num <= i_conn_offset) )

		if  (i_conn_in_path == 1)  v_path_connections_buf.push_back(&(pc_parent->pc_conns[i_conn_offset]));
		
	}//for  (int i_node_start = 0; i_node_start < i_nodes_num; i_node_start++)



	int  i_conn_offset;
	CFiberConnection  *pc_conn_buf;

	//Tools::vRepInFile("zzzz_test_connections.txt", sPathLine);

	i_conn_offset = i_get_first_connection_offset(&v_path_connections_buf);
	if  (i_conn_offset  < 0)
	{
		CString  s_err;
		s_err.Format("First connection not found. Path: %d", i_path_id);

		/*Tools::vRepInFile("zzzz_test_connections.txt", sPathLine, true);

		//Tools::vRepInFile("zzzz_test_connections.txt", "ERROR");
		//Tools::vRepInFile("zzzz_test_connections.txt", sPathLine);

		::vShow("aaa");

		for  (int  ii = 0; ii < v_path_connections_buf.size(); ii++)
		{
			s_buf.Format("Start: %d  end:%d", v_path_connections_buf.at(ii)->i_fiber_node_start_offset, v_path_connections_buf.at(ii)->i_fiber_node_end_offset);
			Tools::vRepInFile("zzzz_test_connections.txt", s_buf);
		}//for  (int  ii = 0; ii < v_path_connections_buf.size(); ii++)*/

		c_err.vPutError(s_err);
		return(c_err);
	}//if  (i_conn_offset  < 0)

	pc_conn_buf = v_path_connections_buf.at(i_conn_offset);
	v_path_connections_buf.erase(v_path_connections_buf.begin() + i_conn_offset);	
	v_path_connections.push_back(pc_conn_buf);

	if  (bControlConnectionsOrder  ==  true)
	{
		while  ( (v_path_connections_buf.size()  >  0)&&(i_conn_offset >= 0) )
		{
			i_conn_offset = i_get_linked_connection_offset(&v_path_connections_buf, pc_conn_buf);
			if  (i_conn_offset  < 0)
			{
				CString  s_err;
				s_err.Format("Next connection not found. Path: %d", i_path_id);
				c_err.vPutError(s_err);
				return(c_err);
			}//if  (i_conn_offset  < 0)

			pc_conn_buf = v_path_connections_buf.at(i_conn_offset);
			v_path_connections_buf.erase(v_path_connections_buf.begin() + i_conn_offset);	
			v_path_connections.push_back(pc_conn_buf);
		}//while  ( (v_path_connections_buf.size()  >  0)&&(i_conn_offset > 0) )

		i_fiber_node_start_offset = v_path_connections.at(0)->i_fiber_node_start_offset;
		i_fiber_node_end_offset = v_path_connections.at(v_path_connections.size() - 1)->i_fiber_node_end_offset;
	}//if  (bControlConnectionsOrder  ==  true)
	else
	{
		while  (v_path_connections_buf.size()  >  0)
		{
			v_path_connections.push_back(v_path_connections_buf.at(0));
			v_path_connections_buf.erase(v_path_connections_buf.begin());			
		}//while  (v_path_connections_buf.size()  >  0)

		i_fiber_node_start_offset = v_path_connections.at(0)->i_fiber_node_start_offset;
		i_fiber_node_end_offset = -1;
	}//else  if  (bControlConnectionsOrder  ==  true)

	


	return(c_err);
}//CError  CFiberPath::eLoad(CString  sPathLine)



CError  CFiberPath::eGetSlotsNumberForTransfer(CString  sLine)
{
	CError  c_err;

	v_slot_number_for_transfer.clear();
	int  i_index = 0;
	int  i_buf;
	while  (i_index  <  sLine.GetLength())
	{
		i_buf = Tools::iExtractFromString(sLine, i_index, &i_index);
		//i_buf++;
		v_slot_number_for_transfer.push_back(i_buf);
	}//while  (i_index  <  sLine.GetLength())

	
	return(c_err);
}//CError  CFiberPath::eGetSlotsNumberForTransfer(CString  sLine)



CError  CFiberPath::eGetRegNumberForTransfer(CString  sLine)
{
	CError  c_err;

	v_reg_number_for_transfer.clear();
	int  i_index = 0;
	int  i_buf;
	while  (i_index  <  sLine.GetLength())
	{
		i_buf = Tools::iExtractFromString(sLine, i_index, &i_index);
		v_reg_number_for_transfer.push_back(i_buf);
	}//while  (i_index  <  sLine.GetLength())

	
	return(c_err);
}//CError  CFiberPath::eGetRegNumberForTransfer(CString  sLine)




int  CFiberPath::i_get_first_connection_offset(vector<CFiberConnection *>  *pvConnections)
{
	bool  b_prev_conn_found;


	for  (int  i_checked = 0; i_checked < pvConnections->size(); i_checked++)
	{
		b_prev_conn_found = false;
		
		for  (int  ii = 0; (ii < pvConnections->size())&&(b_prev_conn_found == false); ii++)
		{
			if  (
				pvConnections->at(i_checked)->i_fiber_node_start_offset
				==
				pvConnections->at(ii)->i_fiber_node_end_offset
				)
				b_prev_conn_found = true;
		}//for  (int  ii = 0; (ii < pvConnections->size())&&(b_prev_conn_found == true); ii++)
	
		if  (b_prev_conn_found == false)  return(i_checked);
	}//for  (int  i_checked = 0; i_checked < pvConnections->size(); i_checked++)


	return(-1);
}//int  CFiberPath::i_get_first_connection_offset(vector<CFiberConnection *>  vConnections)



int  CFiberPath::i_get_linked_connection_offset(vector<CFiberConnection *>  *pvConnections, CFiberConnection *pcMotherConnection)
{
	//Tools::vRepInFile("zzzzz_conns.txt","",true);
	for  (int  ii = 0; ii < pvConnections->size(); ii++)
	{
		if  (
			pcMotherConnection->i_fiber_node_end_offset
			==
			pvConnections->at(ii)->i_fiber_node_start_offset
			)
			return(ii);

		//CString  s_buf;
		//s_buf.Format("mother conn %d   next conn: %d  size:%d", pcMotherConnection->i_fiber_node_end_offset, pvConnections->at(ii)->i_fiber_node_start_offset, pvConnections->size());
		//Tools::vRepInFile("zzzzz_conns.txt",s_buf);
	}//for  (int  ii = 0; (ii < pvConnections->size())&&(b_prev_conn_found == true); ii++)

	return(-1);
}//int  CFiberPath::i_get_linked_connection_offset(vector<CFiberConnection *>  vConnections, CFiberConnection *pcMotherConnection)





//---------------------------class  CFiberSolution--------------------------------------
CFiberSolution::CFiberSolution(CFiberNet  *pcFibNet)
{
	pc_channels = NULL;
	pc_fib_net = pcFibNet;

	if  (pc_fib_net->i_demand_num  <= 0)  return;
	pc_channels = new CFiberSolutionChannel[pc_fib_net->i_demand_num];

	for  (int  ii = 0; ii < pc_fib_net->i_demand_num; ii++)
		pc_channels[ii].vConfig(this, &(pc_fib_net->pc_demands[ii]));

	d_last_raw_sol_value = 1;
}//CFiberSolution::CFiberSolution(CFiberNet  *pcFibNet)



CFiberSolution::CFiberSolution(CFiberSolution  *pcFibSol)
{
	pc_channels = NULL;
	pc_fib_net = pcFibSol->pc_fib_net;

	if  (pc_fib_net->i_demand_num  <= 0)  return;
	pc_channels = new CFiberSolutionChannel[pc_fib_net->i_demand_num];

	for  (int  ii = 0; ii < pc_fib_net->i_demand_num; ii++)
	{
		pc_channels[ii].vConfig(this, &(pc_fib_net->pc_demands[ii]));
		pc_channels[ii].iFirstSlotNumber = pcFibSol->pc_channels[ii].iFirstSlotNumber;
		pc_channels[ii].pcPath = pcFibSol->pc_channels[ii].pcPath;
	}//for  (int  ii = 0; ii < pc_fib_net->i_demand_num; ii++)

}//CFiberSolution::CFiberSolution(CFiberSolution  *pcFibNet)



CFiberSolution::~CFiberSolution()
{
	if  (pc_channels  !=  NULL)  delete  pc_channels;
}//CFiberSolution::~CFiberSolution()


void  CFiberSolution::vInitStatic()
{
	d_func_evaluations_full_better_than_simple = 0;
	d_best_solution = -1;
	d_time_till_best = -1;
	d_ffe_till_best = -1;
	c_time_counter.vSetStartNow();
}//void  CFiberSolution::vInitStatic()



void  CFiberSolution::vCopyChannelsState(CFiberSolutionChannel  *pcChannelsDest)
{
	for  (int ii = 0; ii < pc_fib_net->iGetDemandNumber(); ii++)
	{
		pc_channels[ii].vCopy(&(pcChannelsDest[ii]));
	}//for  (int ii = 0; ii < pc_fib_net->iGetDemandNumber(); ii++)

}//void  CFiberSolution::vCopyChannelsState(CFiberSolutionChannel  *pcChannelsDest)




CError  CFiberSolution::eFindChanel(int  iChanelOffset, CFiberPath  *pcPath, int *piStartSlot, int  *piSlotsNumber)
{
	CError  c_err;

	if  ( (iChanelOffset  <  0)||(iChanelOffset  >=  pc_fib_net->iGetDemandNumber()) )
	{
		CString  s_buf;

		s_buf.Format("CFiberSolution::eFindAndSetChanel -> wrong chanel offset: %d allowed: %d", iChanelOffset, pc_fib_net->iGetDemandNumber());
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  ( (iChanelOffset  <  0)||(iChanelOffset  >=  pc_fib_net->iGetDemandNumber()) )

	c_err = pc_channels[iChanelOffset].eFindChanel(pcPath, piStartSlot, piSlotsNumber);

	return(c_err);
}//CError  CFiberSolution::eFindAndSetChanel(int  iChanelOffset, CFiberPath  *pcPath)




CError  CFiberSolution::eSetChanel(int  iChanelOffset, CFiberPath  *pcPath, int iStartSlot, int  iSlotsNumber)
{
	CError  c_err;

	if  ( (iChanelOffset  <  0)||(iChanelOffset  >=  pc_fib_net->iGetDemandNumber()) )
	{
		CString  s_buf;

		s_buf.Format("CFiberSolution::eFindAndSetChanel -> wrong chanel offset: %d allowed: %d", iChanelOffset, pc_fib_net->iGetDemandNumber());
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  ( (iChanelOffset  <  0)||(iChanelOffset  >=  pc_fib_net->iGetDemandNumber()) )

	c_err = pc_channels[iChanelOffset].eSetChanel(pcPath, iStartSlot, iSlotsNumber);

	return(c_err);
}//CError  CFiberSolution::eFindAndSetChanel(int  iChanelOffset, CFiberPath  *pcPath)





CError  CFiberSolution::eFindAndSetChanel(int  iChanelOffset, CFiberPath  *pcPath)
{
	CError  c_err;

	if  ( (iChanelOffset  <  0)||(iChanelOffset  >=  pc_fib_net->iGetDemandNumber()) )
	{
		CString  s_buf;

		s_buf.Format("CFiberSolution::eFindAndSetChanel -> wrong chanel offset: %d allowed: %d", iChanelOffset, pc_fib_net->iGetDemandNumber());
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  ( (iChanelOffset  <  0)||(iChanelOffset  >=  pc_fib_net->iGetDemandNumber()) )

	c_err = pc_channels[iChanelOffset].eFindAndSetChanel(pcPath);

	return(c_err);
}//CError  CFiberSolution::eFindAndSetChanel(int  iChanelOffset, CFiberPath  *pcPath)



void  CFiberSolution::vSetPathFitness()
{
	d_clever_paths_initializations++;

	CFiberPath  *pc_all_paths;

	pc_all_paths = pc_fib_net->pc_paths;
	for  (int  ii = 0; ii < pc_fib_net->i_path_num; ii++)
	{
		pc_all_paths[ii].vSetPathFitness(d_last_raw_sol_value);
	}//for  (int  ii = 0; ii < pc_fib_net->i_path_num; ii++)


	int  i_dem_number = pc_fib_net->iGetDemandNumber();
	for  (int  ii = 0; ii < i_dem_number; ii++)
	{
		pc_fib_net->pcGetDemands()[ii].vSummarizePathFitness();
	}//for  (int  ii = 0; ii < i_dem_number; ii++)

}//void  CFiberSolution::vSetPathFitness()



CFiberPath  *CFiberSolution::pcGetRandomPath(int  iDemandNumber, gcroot<Random*> pcRandomGen)
{
	vector  <CFiberPath  *>  *pv_possible_paths;
	pv_possible_paths = (pc_fib_net->pcGetDemands()[iDemandNumber]).pvGetPossiblePaths();

	if  (pv_possible_paths->size()  <=  0)  return(NULL);

	int i_chosen_path; 
	i_chosen_path = pcRandomGen->Next(pv_possible_paths->size());
	if  (i_chosen_path  >=  pv_possible_paths->size())  i_chosen_path  =  pv_possible_paths->size() - 1;

	return(pv_possible_paths->at(i_chosen_path));
}//CFiberPath  *CFiberSolution::pcGetRandomPath(int  iDemandNumber, gcroot<Random*> pcRandomGen)


CFiberPath  *CFiberSolution::pcGetCleverPath(int  iDemandNumber, gcroot<Random*> pcRandomGen)
{
	d_clever_paths_evaluations++;


	double  d_fit_sum;

	vector<CFiberPath*>  *pv_possible_paths = NULL;
	pv_possible_paths = (pc_fib_net->pcGetDemands()[iDemandNumber]).pvGetPossiblePaths();
	d_fit_sum = (pc_fib_net->pcGetDemands()[iDemandNumber]).d_path_fit_sum;

	if  (d_fit_sum < 1)  return(pcGetRandomPath(iDemandNumber, pcRandomGen));

	double  d_random;
	d_random = pcRandomGen->NextDouble();
	d_random *= d_fit_sum;

	return(pc_get_clever_path(pv_possible_paths, d_fit_sum, pcRandomGen));
}//CFiberPath  *CFiberSolution::pcGetCleverPath(int  iDemandNumber, gcroot<Random*> pcRandomGen)




CFiberPath  *CFiberSolution::pcGetBestCleverPath(int  iStartNode, int iEndNode)
{
	double  d_current_fit;
	double  d_best_fit;
	int  i_best_fit_offset;

	vector<int>  *pv_possible_paths_offsets = NULL;
	pv_possible_paths_offsets = &(pc_fib_net->pv_paths_between_nodes[iStartNode][iEndNode]);

	CFiberPath  *pc_all_paths = NULL;
	pc_all_paths = pc_fib_net->pc_paths;

	d_best_fit = pc_all_paths[pv_possible_paths_offsets->at(0)].dGetPathFitness();
	i_best_fit_offset = pv_possible_paths_offsets->at(0);
	for  (int  ii = 1; ii < pv_possible_paths_offsets->size(); ii++)
	{
		d_current_fit = pc_all_paths[pv_possible_paths_offsets->at(ii)].dGetPathFitness();
		if  (d_best_fit  <  d_current_fit)
		{
			d_best_fit = d_current_fit;
			i_best_fit_offset = pv_possible_paths_offsets->at(ii);
		}//if  (d_best_fit  <  d_fit_sum)
	}//for  (int  ii = 1; ii < pv_possible_paths->size(); ii++)
		
	return(&(pc_all_paths[i_best_fit_offset]));
}//CFiberPath  *CFiberSolution::pcGetBestCleverPath(int  iStartNode, int iEndNode)





CFiberPath  *CFiberSolution::pc_get_clever_path(vector<CFiberPath*>  *pvPossiblePaths, double dPathFitSumm, gcroot<Random*> pcRandomGen)
{
	double  d_random;
	d_random = pcRandomGen->NextDouble();
	d_random *= dPathFitSumm;

	double  d_one_after_one_sum;
	d_one_after_one_sum = 0;
	for  (int  ii = 0; ii < pvPossiblePaths->size(); ii++)
	{
		d_one_after_one_sum += pvPossiblePaths->at(ii)->dGetPathFitness();
		if  (d_random < d_one_after_one_sum)  return(pvPossiblePaths->at(ii));
	}//for  (int  ii = 0; ii < pv_possible_paths->size(); ii++)

	return(pvPossiblePaths->at(pvPossiblePaths->size() - 1));
}//CFiberPath  *CFiberSolution::pc_get_clever_path(vector<CFiberPath*>  pcPossiblePaths, double dPathFitSumm, gcroot<Random*> pcRandomGen)




CError  CFiberSolution::eApplySolution()
{
	CError  c_err;

	pc_fib_net->eResetNet();

	for  (int  ii = 0; ii < pc_fib_net->i_demand_num; ii++)
	{
		c_err = pc_channels[ii].eSetChannel();
		if  (c_err)  return(c_err);
	}//for  (int  ii = 0; ii < pc_fib_net->i_demand_num; ii++)

	return(c_err);
}//CError  CFiberSolution::eApplySolution()



CError  CFiberSolution::eLoadSolution(CString  sSource)
{
	CError  c_err;

	FILE  *pf_source;
	pf_source = fopen(sSource, "r+");

	if  (pf_source  ==  NULL)
	{
		c_err.vPutError("No solf file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)


	c_err = eLoadSolution(pf_source);
	fclose(pf_source);

	if  (c_err)  return(c_err);
	c_err = eApplySolution();

	return(c_err);
}//CError  CFiberSolution::eLoadSolution(CString  sSource)



CError  CFiberSolution::eLoadSolutionWalk(CString  sSource)
{
	CError  c_err;

	FILE  *pf_source;
	pf_source = fopen(sSource, "r+");

	if  (pf_source  ==  NULL)
	{
		CString  s_buf;

		s_buf.Format("No solf file defined (%s)", sSource);
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  (pf_source  ==  NULL)


	c_err = eLoadSolutionWalk(pf_source);
	fclose(pf_source);


	if  (c_err)  return(c_err);
	::vRepInFile("ai_solution.txt", "", true);
	c_err = eApplySolution();
	eSaveSolution("zz_filled_what_i_can.txt");


	return(c_err);
}//CError  CFiberSolution::eLoadSolutionWalk(CString  sSource)




CError  CFiberSolution::eLoadSolutionAibin(CString  sSource)
{
	CError  c_err;

	FILE  *pf_source;
	pf_source = fopen(sSource, "r+");

	if  (pf_source  ==  NULL)
	{
		CString  s_buf;

		s_buf.Format("No solf file defined (%s)", sSource);
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  (pf_source  ==  NULL)


	c_err = eLoadSolutionAibin(pf_source);
	fclose(pf_source);


	if  (c_err)  return(c_err);
	::vRepInFile("ai_solution.txt", "", true);
	c_err = eApplySolution();
	eSaveSolution("zz_filled_what_i_can.txt");


	return(c_err);
}//CError  CFiberSolution::eLoadSolutionAibin(CString  sSource)

		


CError  CFiberSolution::eLoadSolutionAFA(CString  sSource)
{
	CError  c_err;

	FILE  *pf_source;
	pf_source = fopen(sSource, "r+");

	if  (pf_source  ==  NULL)
	{
		CString  s_buf;

		s_buf.Format("No solf file defined (%s)", sSource);
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  (pf_source  ==  NULL)


	c_err = eLoadSolutionAFA(pf_source);
	fclose(pf_source);

	if  (c_err)  return(c_err);
	c_err = eApplySolution();

	return(c_err);
}//CError  CFiberSolution::eLoadSolution(CString  sSource)





CError  CFiberSolution::eLoadSolutionPRW(CString  sSource)
{
	CError  c_err;

	FILE  *pf_source;
	pf_source = fopen(sSource, "r+");

	if  (pf_source  ==  NULL)
	{
		c_err.vPutError("No prw solf file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)


	c_err = eLoadSolutionPRW(pf_source);
	
	fclose(pf_source);
	return(c_err);
}//CError  CFiberSolution::eLoadSolutionPRW(CString  sSource)


CError  CFiberSolution::eLoadSolutionPRW(FILE  *pfSource)
{
	CError  c_err;

	CString  s_buf;
    CString  s_line  = "";
	
	int  i_demand_offset = 0;

	int  i_marker;
	int  i_index;

	int  i_start_node, i_end_node;
	int  i_path_number;
	int  i_start_slot_number;

	CString  s_marker = SOLUTION_PRW_DEMAND_MARKER;


	while  ( (i_demand_offset  <  pc_fib_net->i_demand_num)&&(!feof(pfSource)) )
	{
		s_line  =  Tools::sReadLine(pfSource);

		i_marker = s_line.Find(SOLUTION_PRW_DEMAND_MARKER);

		if  (i_marker  ==  0)
		{
			i_index = s_marker.GetLength();

			i_start_node = Tools::iExtractFromString(s_line, i_index, &i_index);//the first number is unimportant
			i_start_node = Tools::iExtractFromString(s_line, i_index, &i_index);
			i_end_node = Tools::iExtractFromString(s_line, i_index, &i_index);
			i_start_slot_number = Tools::iExtractFromString(s_line, i_index, &i_index);

			i_path_number = Tools::iExtractFromString(s_line, i_index, &i_index);//this is width in slots
			i_path_number = Tools::iExtractFromString(s_line, i_index, &i_index);

			if  ( (i_path_number < 0)||(i_path_number >= pc_fib_net->i_path_num) )
			{
				s_buf.Format("Wrong path offset (%d) at demand %d", i_path_number, i_demand_offset);
				c_err.vPutError(s_buf);
				return(c_err);
			}//if  ( (i_path_number < 0)||(i_path_number >= pc_fib_net->i_path_num) )

			pc_channels[i_demand_offset].pcPath = &(pc_fib_net->pc_paths[i_path_number]);
			pc_channels[i_demand_offset].iFirstSlotNumber = i_start_slot_number;

			i_demand_offset++;
		}//if  (i_marker  ==  0)

	}//while  ( (i_demands_read  <  pc_fib_net->i_demand_num)&&(feof(pfSource)) )


	
	return(c_err);
}//CError  CFiberSolution::eLoadSolution(FILE  *pfSource)



CError  CFiberSolution::eLoadSolutionWalk(FILE  *pfSource)
{
	CError  c_err;

	CString  s_buf, s_slot_info, s_buf2;
	CString  s_line, s_setting_name;

	int  i_index;
	int  i_offset, i_path, i_first_slot, i_slot_number, i_slot_number_mine;
	int  i_counter = 0;


	s_buf.Format("Dem number: %d", pc_fib_net->iGetDemandNumber());
	::vRepInFile("test.txt", s_buf, true);
	s_buf.Format("uni number: %d", pc_fib_net->i_uni_demands);
	::vRepInFile("test.txt", s_buf);
	s_buf.Format("multi number: %d", pc_fib_net->i_multi_demands);
	::vRepInFile("test.txt", s_buf);

	while  (!feof(pfSource)  &&  (i_counter < pc_fib_net->i_demand_num))
	{
		i_counter++;

		i_index = 0;
		s_line  =  Tools::sReadLine(pfSource, &s_setting_name);
		i_offset = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_path = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_slot_number = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_first_slot = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_first_slot--;
		i_slot_number_mine = 
			pc_fib_net->pcGetDemands()[i_offset].pvGetPossiblePaths()->at(i_path)->iGetSlotsNumber
					(
					pc_fib_net->pcGetDemands()[i_offset].d_demand_capacity
					);

		s_slot_info = "";
		for  (int  ii = 0; ii < pc_fib_net->pcGetDemands()[i_offset].pvGetPossiblePaths()->at(i_path)->v_slot_number_for_transfer.size(); ii++)
		{
			s_buf2.Format("%d ", pc_fib_net->pcGetDemands()[i_offset].pvGetPossiblePaths()->at(i_path)->v_slot_number_for_transfer.at(ii));
			s_slot_info += s_buf2;
		}//for  (int  ii = 0; ii < pc_fib_net->pcGetDemands()[i_offset].pvGetPossiblePaths()->at(i_path)->v_slot_number_for_transfer.size(); ii++)

		s_buf.Format("%d %d %d (pos paths: %d) slice_num: %d (mine:%d)", 
			i_offset, i_path, i_first_slot, 
			pc_fib_net->pcGetDemands()[i_offset].pvGetPossiblePaths()->size(), i_slot_number, i_slot_number_mine
			);
		s_buf += s_slot_info;

		if  (i_slot_number !=  i_slot_number_mine)
			s_buf = "diffr ->" + s_buf;

		::vRepInFile("test.txt", s_buf);

		pc_channels[i_offset].pcPath = pc_fib_net->pcGetDemands()[i_offset].pvGetPossiblePaths()->at(i_path);
		pc_channels[i_offset].iFirstSlotNumber = i_first_slot;
	
	}//while  (!feof(pfSource)  &&  (i_counter < pc_fib_net->i_demand_num))

	return(c_err);
}//CError  CFiberSolution::eLoadSolutionWalk(FILE  *pfSource)


CError  CFiberSolution::eLoadSolutionAibin(FILE  *pfSource)
{
	CError  c_err;

	CString  s_buf, s_slot_info, s_buf2;
	CString  s_line, s_setting_name;

	int  i_index;
	int  i_offset, i_path, i_first_slot, i_slot_number, i_slot_number_mine;
	int  i_counter = 0;


	s_buf.Format("Dem number: %d", pc_fib_net->iGetDemandNumber());
	::vRepInFile("test.txt", s_buf, true);
	s_buf.Format("uni number: %d", pc_fib_net->i_uni_demands);
	::vRepInFile("test.txt", s_buf);
	s_buf.Format("multi number: %d", pc_fib_net->i_multi_demands);
	::vRepInFile("test.txt", s_buf);

	while  (!feof(pfSource)  &&  (i_counter < pc_fib_net->i_demand_num))
	{
		i_counter++;

		i_index = 0;
		s_line  =  Tools::sReadLine(pfSource, &s_setting_name);
		i_offset = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_path = Tools::iExtractFromString(s_line, i_index, &i_index);
		//i_slot_number = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_first_slot = Tools::iExtractFromString(s_line, i_index, &i_index);
		//i_first_slot--;
		i_slot_number_mine = 
			pc_fib_net->pcGetDemands()[i_offset].pvGetPossiblePaths()->at(i_path)->iGetSlotsNumber
					(
					pc_fib_net->pcGetDemands()[i_offset].d_demand_capacity
					);

		s_slot_info = "";
		for  (int  ii = 0; ii < pc_fib_net->pcGetDemands()[i_offset].pvGetPossiblePaths()->at(i_path)->v_slot_number_for_transfer.size(); ii++)
		{
			s_buf2.Format("%d ", pc_fib_net->pcGetDemands()[i_offset].pvGetPossiblePaths()->at(i_path)->v_slot_number_for_transfer.at(ii));
			s_slot_info += s_buf2;
		}//for  (int  ii = 0; ii < pc_fib_net->pcGetDemands()[i_offset].pvGetPossiblePaths()->at(i_path)->v_slot_number_for_transfer.size(); ii++)

		s_buf.Format("%d %d %d (pos paths: %d) (mine slot num:%d)", 
			i_offset, i_path, i_first_slot, 
			pc_fib_net->pcGetDemands()[i_offset].pvGetPossiblePaths()->size(), i_slot_number_mine
			);
		s_buf += s_slot_info;

		//if  (i_slot_number !=  i_slot_number_mine)
		//	s_buf = "diffr ->" + s_buf;

		::vRepInFile("test.txt", s_buf);

		pc_channels[i_offset].pcPath = pc_fib_net->pcGetDemands()[i_offset].pvGetPossiblePaths()->at(i_path);
		pc_channels[i_offset].iFirstSlotNumber = i_first_slot;
	
	}//while  (!feof(pfSource)  &&  (i_counter < pc_fib_net->i_demand_num))

	return(c_err);
}//CError  CFiberSolution::eLoadSolutionAibin(FILE  *pfSource)




CError  CFiberSolution::eLoadSolutionAFA(FILE  *pfSource)
{
	CError  c_err;

	CString  s_line, s_setting_name;
	CString  s_buf;

	int  i_index;
	int  i_number, i_order, i_route_global, i_route_local, i_first_slot, i_modulation, i_replica, i_slots;
	int  i_counter = 0;

	int  i_replica_shift = 0;

	while  (!feof(pfSource)  &&  (i_counter < pc_fib_net->i_demand_num))
	{
		i_index = 0;
		s_line  =  Tools::sReadLine(pfSource, &s_setting_name);

		i_number = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_order = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_route_global = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_route_local = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_first_slot = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_modulation = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_replica = Tools::iExtractFromString(s_line, i_index, &i_index);
		i_slots = Tools::iExtractFromString(s_line, i_index, &i_index);


		pc_channels[i_number].pcPath = &(pc_fib_net->pc_paths[i_route_global]);
		pc_channels[i_number].iFirstSlotNumber = i_first_slot;

		i_counter++;

		if  (
			(pc_fib_net->bCompareNodes(pc_channels[i_number].pcGetDemand()->iGetStartNode(), pc_channels[i_number].pcPath->iGetStartNode())  == false)
			||
			(pc_fib_net->bCompareNodes(pc_channels[i_number].pcGetDemand()->iGetEndNode(),  pc_channels[i_number].pcPath->iGetEndNode())  == false)
			)
		{
			s_buf.Format
				(
				"[%d %d]   (%d %d) == (%d %d)", 
				i_number, i_counter,
				pc_channels[i_number].pcGetDemand()->iGetStartNode(), pc_channels[i_number].pcGetDemand()->iGetEndNode(),
				pc_channels[i_number].pcPath->iGetStartNode(), pc_channels[i_number].pcPath->iGetEndNode()
				);

			::MessageBox(NULL, s_buf, s_line, MB_OK);

			s_buf.Format("%d  %d  %d  %d  %d  %d  %d  %d", i_number, i_order, i_route_global, i_route_local, i_first_slot, i_modulation, i_replica, i_slots);
			::MessageBox(NULL, s_buf, s_line, MB_OK);
		}//if  ((
		
	}//while  (!feof(pfSource))
	
	return(c_err);
}//CError  CFiberSolution::eLoadSolutionAFA(FILE  *pfSource)




CError  CFiberSolution::eLoadSolution(FILE  *pfSource)
{
	CError  c_err;

	CString  s_buf;
    CString  s_line  = "";
	
	int  i_demand_offset = 0;

	int  i_ind_uni, i_ind_any;
	int  i_index;

	int  i_path_number;
	int  i_start_slot_number;

	CString  s_uni_marker = SOLUTION_UNICAST_DEMAND_MARKER;
	CString  s_any_marker = SOLUTION_ANYCAST_DEMAND_MARKER;

	while  ( (i_demand_offset  <  pc_fib_net->i_demand_num)&&(!feof(pfSource)) )
	{
		s_line  =  Tools::sReadLine(pfSource);

		i_ind_uni = s_line.Find(SOLUTION_UNICAST_DEMAND_MARKER);
		i_ind_any = s_line.Find(SOLUTION_ANYCAST_DEMAND_MARKER);

		if  ( (i_ind_uni  ==  0)||(i_ind_any  ==  0) )
		{
			if  (i_ind_uni  ==  0)
			{
				i_index = s_uni_marker.GetLength();

				i_path_number = Tools::iExtractFromString(s_line, i_index, &i_index);//the first number is unimportant
				i_path_number = Tools::iExtractFromString(s_line, i_index, &i_index);
				i_start_slot_number = Tools::iExtractFromString(s_line, i_index, &i_index);


				if  (pc_fib_net->pc_demands[i_demand_offset].pvGetPossiblePaths()->size()  <= i_path_number)
				{
					CString  s_buf;

					s_buf.Format
						(
						"Demand: %d(%d -> %d) possible paths: %d demanded path number: %d", 
						i_demand_offset, pc_fib_net->pc_demands[i_demand_offset].i_fiber_node_start_offset, pc_fib_net->pc_demands[i_demand_offset].i_fiber_node_end_offset,
						pc_fib_net->pc_demands[i_demand_offset].pvGetPossiblePaths()->size(), i_path_number
						);
					c_err.vPutError(s_buf);
					return(c_err);
				}//if  (pv_possible_path_offsets->size()  >= i_path_number)

				pc_channels[i_demand_offset].pcPath = pc_fib_net->pc_demands[i_demand_offset].pvGetPossiblePaths()->at(i_path_number);
				pc_channels[i_demand_offset].iFirstSlotNumber = i_start_slot_number;


			}//if  (i_ind_uni  ==  0)
			else
			{
				i_index = s_any_marker.GetLength();

				i_path_number = Tools::iExtractFromString(s_line, i_index, &i_index);//the first number is unimportant
				i_path_number = Tools::iExtractFromString(s_line, i_index, &i_index);
				i_path_number = Tools::iExtractFromString(s_line, i_index, &i_index);
				i_path_number = Tools::iExtractFromString(s_line, i_index, &i_index);
				i_start_slot_number = Tools::iExtractFromString(s_line, i_index, &i_index);


				if  (pc_fib_net->pc_demands[i_demand_offset].pvGetPossiblePaths()->size()  <= i_path_number)
				{
					CString  s_buf;

					s_buf.Format
						(
						"Demand: %d(%d -> %d) possible paths: %d demanded path number: %d", 
						i_demand_offset, pc_fib_net->pc_demands[i_demand_offset].i_fiber_node_start_offset, pc_fib_net->pc_demands[i_demand_offset].i_fiber_node_end_offset,
						pc_fib_net->pc_demands[i_demand_offset].pvGetPossiblePaths()->size(), i_path_number
						);
					c_err.vPutError(s_buf);
					return(c_err);
				}//if  (pv_possible_path_offsets->size()  >= i_path_number)

				pc_channels[i_demand_offset].pcPath = pc_fib_net->pc_demands[i_demand_offset].pvGetPossiblePaths()->at(i_path_number);
				pc_channels[i_demand_offset].iFirstSlotNumber = i_start_slot_number;
			
			}//else  if  (i_ind_uni  ==  0)

			i_demand_offset++;
		}//if  ( (i_ind_uni  ==  0)||(i_ind_any  ==  0) )
	}//while  ( (i_demands_read  <  pc_fib_net->i_demand_num)&&(feof(pfSource)) )


	
	return(c_err);
}//CError  CFiberSolution::eLoadSolution(FILE  *pfSource)



CError  CFiberSolution::eSaveSolution(FILE  *pfDest)
{
	CError  c_err;


	for  (int  i_dem = 0; i_dem < pc_fib_net->i_demand_num; i_dem++)
	{
		pc_channels[i_dem].vSave(pfDest);
		fprintf(pfDest, "\n");
	}//for  (int  ix = 0; ix < i_nodes_num; ix++)


	//c_err = eApplySolution();
	//if  (c_err)  return(c_err);

	fprintf(pfDest, "\n");
	fprintf(pfDest, "\n");

	for  (int  i_con = 0; i_con < pc_fib_net->i_conns_num; i_con++)
	{
		pc_fib_net->pc_conns[i_con].vSaveStatePRW(pfDest);
		fprintf(pfDest, "\n");
	}//for  (int  i_con = 0; i_con < pc_fib_net->i_conns_num; i_con++)


	bool  b_con_broken;
	fprintf(pfDest, "\n\n RESULT VALUE: %.2lf",  pc_fib_net->dCompCost() /*dCompCost(&b_con_broken)*/);

	return(c_err);
}//CError  CFiberSolution::eSaveSolution(FILE  *pfDest)


CError  CFiberSolution::eSaveSolution(CString  sSolDest)
{
	CError  c_err;
	FILE  *pf_dest;

	pf_dest = fopen(sSolDest, "w+");

	if  (pf_dest  ==  NULL)
	{
		c_err.vPutError("No sol file defined");
		return(c_err);
	}//if  (pf_source  ==  NULL)


	c_err  =  eSaveSolution(pf_dest);


	fclose(pf_dest);
	return(c_err);
}//CError  CFiberSolution::eSaveSolution(CString  sSource)






double  CFiberSolution::dCompCost (bool  *pbConstraintBroken, bool  bNoPenalty /*= false*/)
{
	double  d_result = 0;
	d_func_evaluations++;

	/*CError  c_err;
	c_err = eApplySolution();
	if  (c_err)  
	{
		c_err.csMessage += "//CFiberSolution::dCompCost";
		c_err.vShowWindow();
		d_result = -1;
		return(d_result);
	}//if  (c_err)  */

	

	*pbConstraintBroken  =  pc_fib_net->bConstraintBroken();
	d_result = pc_fib_net->dCompCost();
	//vShow("result");
	//vShow(d_result);

	if  ( (bNoPenalty  ==  false)&&(*pbConstraintBroken  ==  true) )
	{
		double  d_penalty = 0;
		d_result += d_penalty;
	}//if  ( (bNoPenalty  ==  false)&&(*pbConstraintBroken  ==  true) )

	d_last_raw_sol_value = d_result;

	//::Tools::vShow("1");

	if  (WATCH_FITNESS_LEVELS)
	{
		//::Tools::vShow("2");

		bool b_found = false;
		for  (int ii = 0; (ii < v_fitness_levels.size())&&(b_found == false); ii++)
		{
			if  (v_fitness_levels.at(ii).d_fitness_level == d_last_raw_sol_value)  
			{
				v_fitness_levels.at(ii).i_counter++;
				b_found = true;
			}//if  (v_fitness_levels.at(ii) == d_last_raw_sol_value)  
		}//for  (int ii = 0; (ii < v_fitness_levels.size())&&(b_found == false); ii++)

		if  (b_found == false)
		{
			CFitnessPair  c_fitness_info;
			c_fitness_info.i_counter = 1;
			c_fitness_info.d_fitness_level = d_last_raw_sol_value;
			v_fitness_levels.push_back(c_fitness_info);		
		}//if  (b_found == false)

		//CString  s_buf;
		//s_buf.Format("Size: %d", v_fitness_levels.size());
		//::Tools::vShow(s_buf);

	}//if  (WATCH_FITNESS_LEVELS)


	if  ( (d_best_solution  >  d_result)||(d_best_solution == -1) )
	{
		//::Tools::vShow(d_result);
		d_best_solution = d_result;
		c_time_counter.bGetTimePassed(&d_time_till_best);
		d_ffe_till_best = d_func_evaluations;		
	}//if  ( (d_best_solution  <  d_result)||(d_best_solution == -1) )
		

	//vShow("result2");
	//vShow(d_result);

	return(d_result);
}//double  CFiberSolution::dCompCost()


CString  CFiberSolution::sFitnessDiversity()
{
	CString  s_res;

	if  (v_fitness_levels.size()  >  0)
		s_res.Format("FFElev: \t %d \t %.4lf \t %.4lf", v_fitness_levels.size(), v_fitness_levels.at(0).d_fitness_level, v_fitness_levels.at(v_fitness_levels.size()-1).d_fitness_level);
	else
		s_res.Format("Summary: 0 levels min: -1 max: -1");

	return(s_res);
}//CString  CFiberSolution::sFitnessDiversity()



void  CFiberSolution::vFitnessDiversitySave(CString  sDest)
{
	struct {
		bool operator() (CFitnessPair d1, CFitnessPair d2)
		{
			return d1.d_fitness_level > d2.d_fitness_level;
		}
	} fitLevelCOmpare;
	//sort(orderedDemands.begin(), orderedDemands.end(), comparatorBySubcarriers);
	sort(v_fitness_levels.begin(), v_fitness_levels.end(), fitLevelCOmpare);

	CString  s_line;
	//s_line.Format("SIZE: %d", v_fitness_levels.size());
	//::Tools::vShow(s_line);

	s_line.Format("Summary: %d levels min: %.4lf max: %.4lf", v_fitness_levels.size(), v_fitness_levels.at(0).d_fitness_level, v_fitness_levels.at(v_fitness_levels.size()-1).d_fitness_level);

	::Tools::vRepInFile(sDest, s_line);
	/*for  (int  ii = 0; ii < v_fitness_levels.size(); ii++)
	{
		s_line.Format("Lev: %.4lf  \t count: %d", v_fitness_levels.at(ii).d_fitness_level, v_fitness_levels.at(ii).i_counter);
		::Tools::vRepInFile(sDest, s_line);	
	}//for  (int  ii = 0; ii < v_fitness_levels.size(); ii++)*/

	::Tools::vShow("OK");
}//void  CFiberSolution::vFitnessDiversitySave(CString  sDest)


void  CFiberSolution::vFitnessDiversityReset()
{
	v_fitness_levels.clear();
}//void  CFiberSolution::vFitnessDiversityReset()




//---------------------------class  CFiberSolutionChannel--------------------------------------
CFiberSolutionChannel::CFiberSolutionChannel()
{
	pc_solution = NULL;

	pcPath = NULL;
	iFirstSlotNumber  = -1;
}//CFiberSolution::CFiberSolution(CFiberNet  *pcFibNet)




CFiberSolutionChannel::~CFiberSolutionChannel()
{

}//CFiberSolutionChannel::~CFiberSolutionChannel()



CError  CFiberSolutionChannel::eSetChannel()
{
	CError  c_err;

	CString  s_buf;

	if  (pcPath  ==  NULL)
	{
		s_buf.Format("No path for the demand %d", pc_demand->i_demand_id);
		c_err.vPutError(s_buf);
		return(c_err);
	}//if  (pcPath  ==  NULL)


	if  (iFirstSlotNumber < 0)
	{
		s_buf.Format("No slot defined for the demand %d", pc_demand->i_demand_id);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (iSlotNumber < 0)



	int  i_slots_number;
	i_slots_number = pcPath->iGetSlotsNumber(pc_demand->d_demand_capacity);
	c_err  =  pcPath->eSetPathSlots(iFirstSlotNumber, i_slots_number);

	if  (c_err)
	{
		s_buf.Format("  //demand %d  first slot: %d + %d", pc_demand->i_demand_id,  iFirstSlotNumber, i_slots_number);
		c_err.csMessage += s_buf;

		::vRepInFile("ai_solution.txt", c_err.csMessage);
	}//if  (c_err)



	return(c_err);
}//CError  CFiberSolutionChannel::eSetChannel()



CError  CFiberSolutionChannel::eFindChanel(CFiberPath  *pcChannelPath, int *piStartSlot, int  *piSlotsNumber)
{
	CError  c_err;

	*piSlotsNumber = pcChannelPath->iGetSlotsNumber(pc_demand->d_demand_capacity);	
	c_err  =  pcChannelPath->eFindPath(piStartSlot, *piSlotsNumber);

	return(c_err);
}//CError  CFiberSolutionChannel::eFindChanel(CFiberPath  *pcChannelPath)


CError  CFiberSolutionChannel::eSetChanel(CFiberPath  *pcChannelPath, int iStartSlot, int  iSlotsNumber)
{
	CError  c_err;

	iFirstSlotNumber = iStartSlot;
	pcPath =  pcChannelPath;
	c_err  =  pcChannelPath->eSetPath(iStartSlot, iSlotsNumber);

	return(c_err);
}//CError  CFiberSolutionChannel::eFindChanel(CFiberPath  *pcChannelPath)






CError  CFiberSolutionChannel::eFindAndSetChanel(CFiberPath  *pcChannelPath)
{
	CError  c_err;

	pcPath = pcChannelPath;
	
	int  i_slots_number;
	i_slots_number = pcPath->iGetSlotsNumber(pc_demand->d_demand_capacity);	
	c_err  =  pcPath->eFindAndSetPathSlots(&iFirstSlotNumber, i_slots_number);

	return(c_err);
}//CError  CFiberSolutionChannel::eFindAndSetChanel




void  CFiberSolutionChannel::vConfig(CFiberSolution  *pcSolution, CFiberDemand  *pcDemand)
{
	pc_solution = pcSolution;
	pc_demand = pcDemand;
}//void  CFiberSolutionChannel::vConfig(CFiberSolution  *pcSolution)


void  CFiberSolutionChannel::vCopy(CFiberSolutionChannel  *pcOther)
{
	pcOther->iFirstSlotNumber = iFirstSlotNumber;
	pcOther->pc_demand = pc_demand;
	pcOther->pc_solution = pc_solution;
	pcOther->pcPath = pcPath;
}//void  CFiberSolutionChannel::vCopy(CFiberSolutionChannel  *pcOther)


bool CFiberSolutionChannel::operator==(CFiberSolutionChannel  &cOther)
{
	if  (
		(iFirstSlotNumber == cOther.iFirstSlotNumber)&&
		(pc_demand == cOther.pc_demand)&&
		(pcPath == cOther.pcPath)
		)
		return(true);

	return(false);
}//bool CFiberSolutionChannel::operator==(CFiberSolutionChannel  &cOther)



bool CFiberSolutionChannel::operator!=(CFiberSolutionChannel  &cOther)
{
	if  (
		(iFirstSlotNumber != cOther.iFirstSlotNumber)||
		(pc_demand != cOther.pc_demand)||
		(pcPath != cOther.pcPath)
		)
		return(true);

	return(false);
}//bool CFiberSolutionChannel::operator!=(CFiberSolutionChannel  &cOther)



void  CFiberSolutionChannel::vSave(FILE  *pfDest)
{
	CString  s_buf;

	if  (pc_demand->i_demand_type  ==  DEMAND_TYPE_UNICAST)
		s_buf.Format("uni demand  %d  %d  %d  %d  %d %d", pc_demand->i_demand_id, pc_demand->i_fiber_node_start_offset, pc_demand->i_fiber_node_end_offset, iFirstSlotNumber, pcPath->iGetSlotsNumber(pc_demand->d_demand_capacity), pcPath->iGetId());

	if  (pc_demand->i_demand_type  ==  DEMAND_TYPE_MULTICAST)
		s_buf.Format("multi demand  %d  %d  %d  %d %d", pc_demand->i_demand_id, pc_demand->i_fiber_node_start_offset, iFirstSlotNumber, pcPath->iGetSlotsNumber(pc_demand->d_demand_capacity), pcPath->iGetId());
	
	if  (pc_demand->i_demand_type  ==  DEMAND_TYPE_ANYCAST)
		s_buf.Format("anycast demand  %d  %d(%d)  %d(%d)  %d  %d %d", pc_demand->i_demand_id, pc_demand->i_fiber_node_start_offset, pcPath->iGetStartNode(),  pc_demand->i_fiber_node_end_offset, pcPath->iGetEndNode(), iFirstSlotNumber, pcPath->iGetSlotsNumber(pc_demand->d_demand_capacity), pcPath->iGetId());
	fprintf(pfDest, s_buf);
}//void  CFiberSolutionChannel::vSave(FILE  *pfDest)








