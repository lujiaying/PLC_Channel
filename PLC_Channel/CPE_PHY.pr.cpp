/* Process model C++ form file: CPE_PHY.pr.cpp */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char CPE_PHY_pr_cpp [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 54C1C372 54C1C372 1 lu-wspn lu 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                               ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include "PLC_def.h"
#include "PLC_Channel.h"
#include "PLC_func.h"

#define SYS_INITED			((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_SYS_INIT))
#define PPDU_TIME_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_START))
#define PPDU_END			((op_intrpt_type() == OPC_INTRPT_REMOTE) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_END))			

#define PPDU_DELTA_TIME		1
#define PPDU_LIVE_TIME		5
#define PPDU_POWER_MEAN		398
#define PPDU_POWER_SD		1

Objid gvoid_PHY;

static void PPDU_attr_set(PPDU_T *);
static int self_index_find(Objid);

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
class CPE_PHY_state
	{
	private:
		/* Internal state tracking for FSM */
		FSM_SYS_STATE

	public:
		CPE_PHY_state (void);

		/* Destructor contains Termination Block */
		~CPE_PHY_state (void);

		/* State Variables */
		Distribution *	         		svp_exp_dist                                    ;
		Distribution *	         		svp_normal_dist                                 ;
		int	                    		svi_node_index                                  ;
		PHASE_T	                		sve_work_phase                                  ;

		/* FSM code */
		void CPE_PHY (OP_SIM_CONTEXT_ARG_OPT);
		/* Diagnostic Block */
		void _op_CPE_PHY_diag (OP_SIM_CONTEXT_ARG_OPT);

#if defined (VOSD_NEW_BAD_ALLOC)
		void * operator new (size_t) throw (VOSD_BAD_ALLOC);
#else
		void * operator new (size_t);
#endif
		void operator delete (void *);

		/* Memory management */
		static VosT_Obtype obtype;
	};

VosT_Obtype CPE_PHY_state::obtype = (VosT_Obtype)OPC_NIL;

#define svp_exp_dist            		op_sv_ptr->svp_exp_dist
#define svp_normal_dist         		op_sv_ptr->svp_normal_dist
#define svi_node_index          		op_sv_ptr->svi_node_index
#define sve_work_phase          		op_sv_ptr->sve_work_phase

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	CPE_PHY_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((CPE_PHY_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

/************************************************************/
/* Author: jiaying.lu                                       */
/* Last Update: 2014.12.23                                  */
/* Remarks:        											*/
/************************************************************/
static void
PPDU_attr_set(PPDU_T *ppdu)
{
	double lvd_prob_broadcast = 0.2;
	
	FIN(PPDU_attr_set());
	
	// set PPDU attr
	ppdu->type = 0;
	ppdu->start_time = op_sim_time();
	ppdu->end_time = op_sim_time() + op_dist_uniform(PPDU_LIVE_TIME);
	ppdu->transmitter_node_index = svi_node_index;
	// send broadcast or unicast
	if (op_dist_uniform(1.0) < lvd_prob_broadcast) {
		ppdu->receiver_node_index = PPDU_RECEIVER_INDEX_BROADCAST;
	}
	else {
		ppdu->receiver_node_index = PPDU_RECEIVER_INDEX_HE;
	}
	ppdu->power_linear = op_dist_outcome(svp_normal_dist);
	ppdu->send_phase = sve_work_phase;
	
	FOUT;
}

/* End of Function Block */

/* Undefine optional tracing in FIN/FOUT/FRET */
/* The FSM has its own tracing code and the other */
/* functions should not have any tracing.		  */
#undef FIN_TRACING
#define FIN_TRACING

#undef FOUTRET_TRACING
#define FOUTRET_TRACING

/* Undefine shortcuts to state variables because the */
/* following functions are part of the state class */
#undef svp_exp_dist
#undef svp_normal_dist
#undef svi_node_index
#undef sve_work_phase

/* Access from C kernel using C linkage */
extern "C"
{
	VosT_Obtype _op_CPE_PHY_init (int * init_block_ptr);
	VosT_Address _op_CPE_PHY_alloc (VosT_Obtype, int);
	void CPE_PHY (OP_SIM_CONTEXT_ARG_OPT)
		{
		((CPE_PHY_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->CPE_PHY (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_CPE_PHY_svar (void *, const char *, void **);

	void _op_CPE_PHY_diag (OP_SIM_CONTEXT_ARG_OPT)
		{
		((CPE_PHY_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->_op_CPE_PHY_diag (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_CPE_PHY_terminate (OP_SIM_CONTEXT_ARG_OPT)
		{
		/* The destructor is the Termination Block */
		delete (CPE_PHY_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr);
		}


} /* end of 'extern "C"' */




/* Process model interrupt handling procedure */


void
CPE_PHY_state::CPE_PHY (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (CPE_PHY_state::CPE_PHY ());
	try
		{


		FSM_ENTER ("CPE_PHY")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED (0, "init", state0_enter_exec, "CPE_PHY [init enter execs]")
				FSM_PROFILE_SECTION_IN ("CPE_PHY [init enter execs]", state0_enter_exec)
				{
				double lvd_ppdu_time;
				Objid lvoid_NODE_id = op_topo_parent(op_id_self());
				
				svp_exp_dist = op_dist_load("exponential", PPDU_DELTA_TIME, 0);
				svp_normal_dist = op_dist_load("normal", PPDU_POWER_MEAN, PPDU_POWER_SD);      //398mw = 26dBm
				
				
				/* find self index */
				svi_node_index = _global_self_index_find(lvoid_NODE_id);
				/* set PHY Objid */
				gvoid_node_oids[svi_node_index].phy_id = op_id_self();
				/* get work phase */
				op_ima_obj_attr_get(lvoid_NODE_id, "work_phase", &sve_work_phase);
				/* set power on flag */
				op_ima_obj_attr_set(lvoid_NODE_id, "power_on", NODE_POWER_ON_FLAG);
				printf("[PHY.init] NODE index:%d, work_phase:%d\n", svi_node_index, sve_work_phase);
				
				/* schedule first PPDU */
				lvd_ppdu_time = op_dist_outcome(svp_exp_dist);
				printf("[PHY.init] NODE index:%d, current time:%lf, first PPDU time:%lf\n", svi_node_index, op_sim_time(), op_sim_time()+lvd_ppdu_time);
				op_intrpt_schedule_self(op_sim_time()+lvd_ppdu_time, INTRPT_CHANNEL_PPDU_START);
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "CPE_PHY [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_0", "CPE_PHY [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "CPE_PHY [idle enter execs]")
				FSM_PROFILE_SECTION_IN ("CPE_PHY [idle enter execs]", state1_enter_exec)
				{
				printf("enter PHY.idle.\n");
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"CPE_PHY")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "CPE_PHY [idle exit execs]")
				FSM_PROFILE_SECTION_IN ("CPE_PHY [idle exit execs]", state1_exit_exec)
				{
				char lvc_err_msg[25];
				
				printf("Leave PHY.idle. ");
				
				if (PPDU_TIME_START)
				{
					printf("receive INTRPT: PPDU_START.\n");
				}
				else if (PPDU_END)
				{
					printf("receive REMOTE INTRPT: PPDU_END.\n");
				}
				else if (INTRPT_CHANNEL_INITED)
				{
					printf("receive MCAST INTRPT: CHANNEL_INITED. Stay in idle.\n");
				}
				else
				{
					sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());
					op_sim_end("Error: Unexpected INTRPT is received at \"PHY.idle.Exit\" state!", "Error source module: PHY", lvc_err_msg, "");
				}
				}
				FSM_PROFILE_SECTION_OUT (state1_exit_exec)


			/** state (idle) transition processing **/
			FSM_PROFILE_SECTION_IN ("CPE_PHY [idle trans conditions]", state1_trans_conds)
			FSM_INIT_COND (PPDU_TIME_START)
			FSM_TEST_COND (PPDU_END)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "PPDU_TIME_START", "", "idle", "send_PPDU_2Channel", "tr_1", "CPE_PHY [idle -> send_PPDU_2Channel : PPDU_TIME_START / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "PPDU_END", "", "idle", "receive_Channel_PPDU", "tr_3", "CPE_PHY [idle -> receive_Channel_PPDU : PPDU_END / ]")
				FSM_CASE_TRANSIT (2, 1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_5", "CPE_PHY [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (send_PPDU_2Channel) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "send_PPDU_2Channel", state2_enter_exec, "CPE_PHY [send_PPDU_2Channel enter execs]")
				FSM_PROFILE_SECTION_IN ("CPE_PHY [send_PPDU_2Channel enter execs]", state2_enter_exec)
				{
				Ici *lvp_ici;
				double lvd_next_ppdu_time;
				PPDU_T *lvp_ppdu;
				
				/* send PPDU to channel*/
				lvp_ici = op_ici_create("PPDU");
				lvp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));
				PPDU_attr_set(lvp_ppdu);
				printf("[PHY.send_PPDU_2Channel] transmitter:%d, receiver:%d, power_linear:%lf, start_time:%lf, end_time:%lf\n", lvp_ppdu->transmitter_node_index, lvp_ppdu->receiver_node_index, lvp_ppdu->power_linear, lvp_ppdu->start_time, lvp_ppdu->end_time);
				
				op_ici_attr_set_ptr(lvp_ici, "PPDU_ptr", lvp_ppdu);
				op_ici_install(lvp_ici);
				op_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_START, gvoid_channel);
				op_ici_install(OPC_NIL);
				
				/* schedule next PPDU */
				lvd_next_ppdu_time = op_dist_outcome(svp_exp_dist);
				printf("[PHY.send_PPDU_2Channel] current time:%lf, next PPDU time:%lf\n", op_sim_time(), op_sim_time()+lvd_next_ppdu_time);
				op_intrpt_schedule_self(op_sim_time()+lvd_next_ppdu_time, INTRPT_CHANNEL_PPDU_START);
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (send_PPDU_2Channel) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "send_PPDU_2Channel", "CPE_PHY [send_PPDU_2Channel exit execs]")


			/** state (send_PPDU_2Channel) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "send_PPDU_2Channel", "idle", "tr_2", "CPE_PHY [send_PPDU_2Channel -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (receive_Channel_PPDU) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "receive_Channel_PPDU", state3_enter_exec, "CPE_PHY [receive_Channel_PPDU enter execs]")
				FSM_PROFILE_SECTION_IN ("CPE_PHY [receive_Channel_PPDU enter execs]", state3_enter_exec)
				{
				Ici *lvp_ici;
				PPDU_T *lvp_PPDU;
				int lvi_actual_receiver_index;
				
				/* receive PPDU from Channel*/
				lvp_ici = op_intrpt_ici();
				op_ici_attr_get(lvp_ici, "PPDU_ptr", &lvp_PPDU);
				op_ici_destroy(lvp_ici);
				lvp_ici = OPC_NIL;
				
				printf("\n============================================\n");
				printf("|            receive_channel_PPDU         |\n");
				printf("transmitter:%d, receiver:%d, \n", lvp_PPDU->transmitter_node_index, lvp_PPDU->receiver_node_index);
				printf("power_linear:%lf, PPDU_sinr_dB:%lf\n", lvp_PPDU->power_linear, lvp_PPDU->actual_receiver_array[0].PPDU_sinr_dB);
				printf("start_time:%lf, end_time:%lf\n", lvp_PPDU->start_time, lvp_PPDU->end_time);
				printf("============================================\n");
				
				/* free memory */
				for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvp_PPDU->actual_receiver_number; lvi_actual_receiver_index++)
				{
					prg_list_free(lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].segment_sinr);
				}
				op_prg_mem_free(lvp_PPDU->actual_receiver_array);
				prg_mem_free(lvp_PPDU);
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (receive_Channel_PPDU) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "receive_Channel_PPDU", "CPE_PHY [receive_Channel_PPDU exit execs]")


			/** state (receive_Channel_PPDU) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "receive_Channel_PPDU", "idle", "tr_4", "CPE_PHY [receive_Channel_PPDU -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (wait_sys_inited) enter executives **/
			FSM_STATE_ENTER_UNFORCED (4, "wait_sys_inited", state4_enter_exec, "CPE_PHY [wait_sys_inited enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (9,"CPE_PHY")


			/** state (wait_sys_inited) exit executives **/
			FSM_STATE_EXIT_UNFORCED (4, "wait_sys_inited", "CPE_PHY [wait_sys_inited exit execs]")


			/** state (wait_sys_inited) transition processing **/
			FSM_PROFILE_SECTION_IN ("CPE_PHY [wait_sys_inited trans conditions]", state4_trans_conds)
			FSM_INIT_COND (SYS_INITED)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("wait_sys_inited")
			FSM_PROFILE_SECTION_OUT (state4_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 0, state0_enter_exec, ;, "SYS_INITED", "", "wait_sys_inited", "init", "tr_6", "CPE_PHY [wait_sys_inited -> init : SYS_INITED / ]")
				FSM_CASE_TRANSIT (1, 4, state4_enter_exec, ;, "default", "", "wait_sys_inited", "wait_sys_inited", "tr_7", "CPE_PHY [wait_sys_inited -> wait_sys_inited : default / ]")
				}
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (4,"CPE_PHY")
		}
	catch (...)
		{
		Vos_Error_Print (VOSC_ERROR_ABORT,
			(const char *)VOSC_NIL,
			"Unhandled C++ exception in process model (CPE_PHY)",
			(const char *)VOSC_NIL, (const char *)VOSC_NIL);
		}
	}




void
CPE_PHY_state::_op_CPE_PHY_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}

void
CPE_PHY_state::operator delete (void* ptr)
	{
	FIN (CPE_PHY_state::operator delete (ptr));
	Vos_Poolmem_Dealloc (ptr);
	FOUT
	}

CPE_PHY_state::~CPE_PHY_state (void)
	{

	FIN (CPE_PHY_state::~CPE_PHY_state ())


	/* No Termination Block */


	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

void *
CPE_PHY_state::operator new (size_t)
#if defined (VOSD_NEW_BAD_ALLOC)
		throw (VOSD_BAD_ALLOC)
#endif
	{
	void * new_ptr;

	FIN_MT (CPE_PHY_state::operator new ());

	new_ptr = Vos_Alloc_Object (CPE_PHY_state::obtype);
#if defined (VOSD_NEW_BAD_ALLOC)
	if (new_ptr == VOSC_NIL) throw VOSD_BAD_ALLOC();
#endif
	FRET (new_ptr)
	}

/* State constructor initializes FSM handling */
/* by setting the initial state to the first */
/* block of code to enter. */

CPE_PHY_state::CPE_PHY_state (void) :
		_op_current_block (8)
	{
#if defined (OPD_ALLOW_ODB)
		_op_current_state = "CPE_PHY [wait_sys_inited enter execs]";
#endif
	}

VosT_Obtype
_op_CPE_PHY_init (int * init_block_ptr)
	{
	FIN_MT (_op_CPE_PHY_init (init_block_ptr))

	CPE_PHY_state::obtype = Vos_Define_Object_Prstate ("proc state vars (CPE_PHY)",
		sizeof (CPE_PHY_state));
	*init_block_ptr = 8;

	FRET (CPE_PHY_state::obtype)
	}

VosT_Address
_op_CPE_PHY_alloc (VosT_Obtype, int)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	CPE_PHY_state * ptr;
	FIN_MT (_op_CPE_PHY_alloc ())

	/* New instance will have FSM handling initialized */
#if defined (VOSD_NEW_BAD_ALLOC)
	try {
		ptr = new CPE_PHY_state;
	} catch (const VOSD_BAD_ALLOC &) {
		ptr = VOSC_NIL;
	}
#else
	ptr = new CPE_PHY_state;
#endif
	FRET ((VosT_Address)ptr)
	}



void
_op_CPE_PHY_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	CPE_PHY_state		*prs_ptr;

	FIN_MT (_op_CPE_PHY_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (CPE_PHY_state *)gen_ptr;

	if (strcmp ("svp_exp_dist" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svp_exp_dist);
		FOUT
		}
	if (strcmp ("svp_normal_dist" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svp_normal_dist);
		FOUT
		}
	if (strcmp ("svi_node_index" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svi_node_index);
		FOUT
		}
	if (strcmp ("sve_work_phase" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->sve_work_phase);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

