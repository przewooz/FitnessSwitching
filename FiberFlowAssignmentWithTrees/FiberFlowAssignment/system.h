#pragma once


//tools
#include  "atlstr.h"  //CString
#include  "..\error\error.h"
#include  <vector>


//system tools
#include  "..\DeceptiveTools\DecFunc.h"
#include  "..\MessGAs\MessyGA.h"
#include  "FiberNet.h"

using namespace std;
using namespace DeceptiveTools;
using  namespace  MessyGA;
using  namespace  FiberNets;


#define  DEBUG_SAVE					false

#define  SYSTEM_PARAM_FILE_NAME		"file name"
#define  SYSTEM_PARAM_SUMMARIZE		"summarize"

#define  SYSTEM_PARAM_NET_FILE		"net"
#define  SYSTEM_PARAM_REP_FILE		"replic"
#define  SYSTEM_PARAM_TRANSFER_MULTIPLIER		"transfer multiplier"
#define  SYSTEM_PARAM_DEMAND_ANYCAST_1_DIR_FILE	"anycast1dir"
#define  SYSTEM_PARAM_DEMAND_MULTICAST_FILE	"multicast"
#define  SYSTEM_PARAM_DEMAND_ANYCAST_FILE	"anycast"
#define  SYSTEM_PARAM_DEMAND_UNICAST_FILE	"unicast"
#define  SYSTEM_PARAM_PATHS_FILE	"paths"
#define  SYSTEM_PARAM_PATHS_BACKUP_FILE	"paths_backup"
#define  SYSTEM_PARAM_TREES_FILE	"trees"
#define  SYSTEM_PARAM_MOD_FILE		"modulation"
#define  SYSTEM_PARAM_SOLF_FILE		"solution"
#define  SYSTEM_PARAM_OPTIMAL		"optimal solution"
#define  SYSTEM_PARAM_FIT_FUNC		"fit func"

#define  SYSTEM_PARAM_SPEC_FOR_UNI_FILE	"unispec"
#define  SYSTEM_PARAM_SPEC_FOR_UNI_FILE_BACKUP	"unispec_backup"
#define  SYSTEM_PARAM_REG_FOR_UNI_FILE	"unireg"

#define  SYSTEM_PARAM_SPEC_FOR_TREE_FILE	"treespec"
#define  SYSTEM_PARAM_REG_FOR_TREE_FILE	"treereg"

#define  SYSTEM_PARAM_PATH_NUMBER	"paths to use"
#define  SYSTEM_PARAM_TREE_NUMBER	"trees to use"

#define  SYSTEM_PARAM_CORE_NUMBER	"cores"


#define  SYSTEM_PARAM_FIT_FUNC_HIGHEST_SLOT		"HighestSlot"


#define  SYSTEM_PARAM_ALGORITHM		"algorithm"
#define  SYSTEM_PARAM_ALGORITHM_MUPPETS	"muppets"
#define  SYSTEM_PARAM_ALGORITHM_TABU	"tabu"
#define  SYSTEM_PARAM_ALGORITHM_GRASP	"grasp"
#define  SYSTEM_PARAM_ALGORITHM_FMGA		"fmga"
#define  SYSTEM_PARAM_ALGORITHM_SIMPLE_GA	"simple_ga"
#define  SYSTEM_PARAM_ALGORITHM_RANDOM_SEARCH	"random_search"
#define  SYSTEM_PARAM_ALGORITHM_SIMMULATED_ANNEALING	"sa"

#define  SYSTEM_PARAM_FMGA_BB		"BB lengths to check up"
#define  SYSTEM_PARAM_FMGA_CT_RANDOM_GEN	"CT random gen - 0=only zeros; 1= random generation"


class  CSystem
{
public:
	CError  eRun(CString  sMainDir, CString  sSettings, System::Windows::Forms::ListBox*  listInfo);
	CError  eRunSerie(CString  sFileList, System::Windows::Forms::ListBox*  listInfo);

	CString  sInParameters;
private:

	void  v_log(CString  sLog);
	void  v_create_bb_vector(vector  <int>  *pvBBOrder,  CString  sBBOrder);

};//class  CSystem

