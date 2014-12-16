/* Process model C form file: global_init.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char global_init_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 548F91CA 548F91CA 1 lu-wspn lu 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                               ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include "PLC_def.h"
#include "PLC_Channel.h"

#define CHANNEL_INITED		((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_CHANNEL_INITED))

Objid *gvp_CPE_id, *gvp_HE_id;

/* End of Header Block */

#if !defined (VOSD_NO_FIN)
#undef	BIN
#undef	BOUT
#define	BIN		FIN_LOCAL_FIELD(_op_last_line_passed) = __LINE__ - _op_block_origin;
#define	BOUT	BIN
#define	BINIT	FIN_LOCAL_FIELD(_op_last_line_passed) = 0; _op_block_origin = __LINE__;
#else
#define	BINIT
#endif /* #if !defined (VOSD_NO_FIN) */



/* State variable definitions */
typedef struct
	{
	/* Internal state tracking for FSM */
	FSM_SYS_STATE
	} global_init_state;


/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	global_init_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((global_init_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* No Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ };
#endif

/* Undefine optional tracing in FIN/FOUT/FRET */
/* The FSM has its own tracing code and the other */
/* functions should not have any tracing.		  */
#undef FIN_TRACING
#define FIN_TRACING

#undef FOUTRET_TRACING
#define FOUTRET_TRACING

#if defined (__cplusplus)
extern "C" {
#endif
	void global_init (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_global_init_init (int * init_block_ptr);
	void _op_global_init_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_global_init_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_global_init_alloc (VosT_Obtype, int);
	void _op_global_init_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
global_init (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (global_init ());

		{


		FSM_ENTER ("global_init")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (channel_init) enter executives **/
			FSM_STATE_ENTER_UNFORCED (0, "channel_init", state0_enter_exec, "global_init [channel_init enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (1,"global_init")


			/** state (channel_init) exit executives **/
			FSM_STATE_EXIT_UNFORCED (0, "channel_init", "global_init [channel_init exit execs]")


			/** state (channel_init) transition processing **/
			FSM_PROFILE_SECTION_IN ("global_init [channel_init trans conditions]", state0_trans_conds)
			FSM_INIT_COND (CHANNEL_INITED)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("channel_init")
			FSM_PROFILE_SECTION_OUT (state0_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 1, state1_enter_exec, ;, "CHANNEL_INITED", "", "channel_init", "HE_init", "tr_0", "global_init [channel_init -> HE_init : CHANNEL_INITED / ]")
				FSM_CASE_TRANSIT (1, 0, state0_enter_exec, ;, "default", "", "channel_init", "channel_init", "tr_7", "global_init [channel_init -> channel_init : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (HE_init) enter executives **/
			FSM_STATE_ENTER_FORCED (1, "HE_init", state1_enter_exec, "global_init [HE_init enter execs]")
				FSM_PROFILE_SECTION_IN ("global_init [HE_init enter execs]", state1_enter_exec)
				{
				int lvi_HE_index;
				char lvch_HE_name[12];
				char lvch_string[100];
				Objid lvoid_HE_id, lvoid_subnet;
				
				gvp_HE_id = (Objid *)op_prg_mem_alloc(gvi_HE_num * sizeof(Objid));
				lvoid_subnet = op_topo_parent(op_topo_parent(op_id_self()));
				
				/* HE_init */
				for (lvi_HE_index = 0; lvi_HE_index < gvi_HE_num; lvi_HE_index++)
				{
					/* Objid node_id */
					memset(lvch_HE_name, 0, 12);
					sprintf(lvch_HE_name, "HE_%d", lvi_HE_index);
					lvoid_HE_id = op_id_from_name(lvoid_subnet, OPC_OBJTYPE_NDFIX, lvch_HE_name);
					if (lvoid_HE_id == OPC_OBJID_INVALID)
					{
						memset(lvch_string, 0, 100);
						sprintf(lvch_string, "Error: HE_%d can't be found!", lvi_HE_index);
						op_sim_end(lvch_string, "Error source module: global_init", "Error source state: HE_init.enter", "");
					}
					else
					{
						gvp_HE_id[lvi_HE_index] = lvoid_HE_id;
					}
				}	
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** state (HE_init) exit executives **/
			FSM_STATE_EXIT_FORCED (1, "HE_init", "global_init [HE_init exit execs]")


			/** state (HE_init) transition processing **/
			FSM_TRANSIT_FORCE (2, state2_enter_exec, ;, "default", "", "HE_init", "CPE_init", "tr_1", "global_init [HE_init -> CPE_init : default / ]")
				/*---------------------------------------------------------*/



			/** state (CPE_init) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "CPE_init", state2_enter_exec, "global_init [CPE_init enter execs]")
				FSM_PROFILE_SECTION_IN ("global_init [CPE_init enter execs]", state2_enter_exec)
				{
				int lvi_CPE_index;
				char lvch_CPE_name[12];
				char lvch_string[100];
				Objid lvoid_CPE_id, lvoid_subnet;
					
				gvp_CPE_id = (Objid *)op_prg_mem_alloc(gvi_CPE_num * sizeof(Objid));
				lvoid_subnet = op_topo_parent(op_topo_parent(op_id_self()));
				
				/* CPE init */
				for (lvi_CPE_index = 0; lvi_CPE_index < gvi_CPE_num; lvi_CPE_index++)
				{
					/* Objid node_id */
					memset(lvch_CPE_name, 0, 12);
					sprintf(lvch_CPE_name, "CPE_%d", lvi_CPE_index);
					lvoid_CPE_id = op_id_from_name(lvoid_subnet, OPC_OBJTYPE_NDMOB, lvch_CPE_name);
					if (lvoid_CPE_id == OPC_OBJID_INVALID)
					{
						memset(lvch_string, 0, 100);
						sprintf(lvch_string, "Error: CPE %d can't be found!", lvi_CPE_index);
						op_sim_end(lvch_string, "Error source module: PLC_GLOBAL_INIT", "Error source function: CPE_init()", "");
					}
					else
					{
						gvp_CPE_id[lvi_CPE_index] = lvoid_CPE_id;
					}		
				}	
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (CPE_init) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "CPE_init", "global_init [CPE_init exit execs]")


			/** state (CPE_init) transition processing **/
			FSM_TRANSIT_FORCE (3, state3_enter_exec, ;, "default", "", "CPE_init", "send_sys_init", "tr_2", "global_init [CPE_init -> send_sys_init : default / ]")
				/*---------------------------------------------------------*/



			/** state (send_sys_init) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "send_sys_init", state3_enter_exec, "global_init [send_sys_init enter execs]")
				FSM_PROFILE_SECTION_IN ("global_init [send_sys_init enter execs]", state3_enter_exec)
				{
				char lvch_string[100];
				
				memset(lvch_string, 0, 100);
				sprintf(lvch_string, "%d HEs and %d CPEs are initialized.", gvi_HE_num, gvi_CPE_num);
				op_sim_message("Simulation initialization completes.", lvch_string);
				op_intrpt_schedule_mcast_global(op_sim_time(), INTRPT_SYS_INIT);
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (send_sys_init) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "send_sys_init", "global_init [send_sys_init exit execs]")


			/** state (send_sys_init) transition processing **/
			FSM_TRANSIT_FORCE (4, state4_enter_exec, ;, "default", "", "send_sys_init", "idle", "tr_6", "global_init [send_sys_init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (4, "idle", state4_enter_exec, "global_init [idle enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (9,"global_init")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (4, "idle", "global_init [idle exit execs]")


			/** state (idle) transition processing **/
			FSM_TRANSIT_FORCE (4, state4_enter_exec, ;, "default", "", "idle", "idle", "tr_5", "global_init [idle -> idle : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"global_init")
		}
	}




void
_op_global_init_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_global_init_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_global_init_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (op_sv_ptr);

	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_global_init_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_global_init_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (global_init)",
		sizeof (global_init_state));
	*init_block_ptr = 0;

	FRET (obtype)
	}

VosT_Address
_op_global_init_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	global_init_state * ptr;
	FIN_MT (_op_global_init_alloc (obtype))

	ptr = (global_init_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "global_init [channel_init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_global_init_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{

	FIN_MT (_op_global_init_svar (gen_ptr, var_name, var_p_ptr))

	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

