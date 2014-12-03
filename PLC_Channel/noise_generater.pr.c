/* Process model C form file: noise_generater.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char noise_generater_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 547E706D 547E706D 1 lu-wspn lu 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                               ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include "PLC_Channel.h"

#define CHANNEL_INITED		((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_CHANNEL_INITED))
#define PPDU_NEXT_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_START))
#define PPDU_END		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_END))

Objid gvoid_noise_generater;

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
	} noise_generater_state;


/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	noise_generater_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((noise_generater_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


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
	void noise_generater (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_noise_generater_init (int * init_block_ptr);
	void _op_noise_generater_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_noise_generater_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_noise_generater_alloc (VosT_Obtype, int);
	void _op_noise_generater_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
noise_generater (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (noise_generater ());

		{


		FSM_ENTER ("noise_generater")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (wait_channel_init) enter executives **/
			FSM_STATE_ENTER_UNFORCED (0, "wait_channel_init", state0_enter_exec, "noise_generater [wait_channel_init enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (1,"noise_generater")


			/** state (wait_channel_init) exit executives **/
			FSM_STATE_EXIT_UNFORCED (0, "wait_channel_init", "noise_generater [wait_channel_init exit execs]")


			/** state (wait_channel_init) transition processing **/
			FSM_PROFILE_SECTION_IN ("noise_generater [wait_channel_init trans conditions]", state0_trans_conds)
			FSM_INIT_COND (CHANNEL_INITED)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("wait_channel_init")
			FSM_PROFILE_SECTION_OUT (state0_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "CHANNEL_INITED", "", "wait_channel_init", "init", "tr_1", "noise_generater [wait_channel_init -> init : CHANNEL_INITED / ]")
				FSM_CASE_TRANSIT (1, 0, state0_enter_exec, ;, "default", "", "wait_channel_init", "wait_channel_init", "tr_3", "noise_generater [wait_channel_init -> wait_channel_init : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "noise_generater [idle enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"noise_generater")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "noise_generater [idle exit execs]")


			/** state (idle) transition processing **/
			FSM_PROFILE_SECTION_IN ("noise_generater [idle trans conditions]", state1_trans_conds)
			FSM_INIT_COND (PPDU_END)
			FSM_TEST_COND (PPDU_NEXT_START)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 3, state3_enter_exec, ;, "PPDU_END", "", "idle", "receive_PPDU", "tr_5", "noise_generater [idle -> receive_PPDU : PPDU_END / ]")
				FSM_CASE_TRANSIT (1, 4, state4_enter_exec, ;, "PPDU_NEXT_START", "", "idle", "send_PPDU", "tr_7", "noise_generater [idle -> send_PPDU : PPDU_NEXT_START / ]")
				FSM_CASE_TRANSIT (2, 1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_9", "noise_generater [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "init", state2_enter_exec, "noise_generater [init enter execs]")
				FSM_PROFILE_SECTION_IN ("noise_generater [init enter execs]", state2_enter_exec)
				{
				Distribution *lvp_exponential_dist;
				double lvd_send_time;
				
				gvoid_noise_generater = op_id_self();
				
				/* set FIRST PPDU send time */
				lvp_exponential_dist = op_dist_load("exponential", 1, 0);
				lvd_send_time = op_sim_time()+op_dist_outcome(lvp_exponential_dist);
				op_intrpt_schedule_self(lvd_send_time, INTRPT_CHANNEL_PPDU_START);
				printf("[noise_generater.init] first PPDU send time = %lf\n", lvd_send_time);
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "init", "noise_generater [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_2", "noise_generater [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (receive_PPDU) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "receive_PPDU", state3_enter_exec, "noise_generater [receive_PPDU enter execs]")

			/** state (receive_PPDU) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "receive_PPDU", "noise_generater [receive_PPDU exit execs]")


			/** state (receive_PPDU) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "receive_PPDU", "idle", "tr_6", "noise_generater [receive_PPDU -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (send_PPDU) enter executives **/
			FSM_STATE_ENTER_FORCED (4, "send_PPDU", state4_enter_exec, "noise_generater [send_PPDU enter execs]")
				FSM_PROFILE_SECTION_IN ("noise_generater [send_PPDU enter execs]", state4_enter_exec)
				{
				/* send PPDU to Channel */
				Ici *lvp_ici;
				PPDU_T *lvp_ppdu;
				Distribution *lvp_normal_dist;
				
				lvp_ici = op_ici_create("PULSE_NOISE_PPDU");
				lvp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));
				lvp_normal_dist = op_dist_load("normal", -22, 1);
				
				lvp_ppdu->type = 3;
				lvp_ppdu->power_linear = op_dist_outcome(lvp_normal_dist);
				lvp_ppdu->start_time = op_sim_time();
				lvp_ppdu->end_time = op_sim_time() + 0.01;
				op_ici_attr_set(lvp_ici, "PPDU_ptr", lvp_ppdu);
				op_ici_install(lvp_ici);
				op_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_START, gvoid_channel);
				op_ici_install(OPC_NIL);
				}
				FSM_PROFILE_SECTION_OUT (state4_enter_exec)

			/** state (send_PPDU) exit executives **/
			FSM_STATE_EXIT_FORCED (4, "send_PPDU", "noise_generater [send_PPDU exit execs]")


			/** state (send_PPDU) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "send_PPDU", "idle", "tr_8", "noise_generater [send_PPDU -> idle : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"noise_generater")
		}
	}




void
_op_noise_generater_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_noise_generater_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_noise_generater_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (op_sv_ptr);

	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_noise_generater_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_noise_generater_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (noise_generater)",
		sizeof (noise_generater_state));
	*init_block_ptr = 0;

	FRET (obtype)
	}

VosT_Address
_op_noise_generater_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	noise_generater_state * ptr;
	FIN_MT (_op_noise_generater_alloc (obtype))

	ptr = (noise_generater_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "noise_generater [wait_channel_init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_noise_generater_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{

	FIN_MT (_op_noise_generater_svar (gen_ptr, var_name, var_p_ptr))

	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

