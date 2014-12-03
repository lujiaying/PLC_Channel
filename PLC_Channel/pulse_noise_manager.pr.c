/* Process model C form file: pulse_noise_manager.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char pulse_noise_manager_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 547F124A 547F124A 1 lu-wspn lu 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                               ";
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
	} pulse_noise_manager_state;


/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	pulse_noise_manager_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((pulse_noise_manager_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


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
	void pulse_noise_manager (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_pulse_noise_manager_init (int * init_block_ptr);
	void _op_pulse_noise_manager_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_pulse_noise_manager_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_pulse_noise_manager_alloc (VosT_Obtype, int);
	void _op_pulse_noise_manager_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
pulse_noise_manager (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (pulse_noise_manager ());

		{


		FSM_ENTER ("pulse_noise_manager")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (wait_channel_init) enter executives **/
			FSM_STATE_ENTER_UNFORCED (0, "wait_channel_init", state0_enter_exec, "pulse_noise_manager [wait_channel_init enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (1,"pulse_noise_manager")


			/** state (wait_channel_init) exit executives **/
			FSM_STATE_EXIT_UNFORCED (0, "wait_channel_init", "pulse_noise_manager [wait_channel_init exit execs]")


			/** state (wait_channel_init) transition processing **/
			FSM_PROFILE_SECTION_IN ("pulse_noise_manager [wait_channel_init trans conditions]", state0_trans_conds)
			FSM_INIT_COND (CHANNEL_INITED)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("wait_channel_init")
			FSM_PROFILE_SECTION_OUT (state0_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "CHANNEL_INITED", "", "wait_channel_init", "init", "tr_1", "pulse_noise_manager [wait_channel_init -> init : CHANNEL_INITED / ]")
				FSM_CASE_TRANSIT (1, 0, state0_enter_exec, ;, "default", "", "wait_channel_init", "wait_channel_init", "tr_3", "pulse_noise_manager [wait_channel_init -> wait_channel_init : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "pulse_noise_manager [idle enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"pulse_noise_manager")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "pulse_noise_manager [idle exit execs]")


			/** state (idle) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_10", "pulse_noise_manager [idle -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "init", state2_enter_exec, "pulse_noise_manager [init enter execs]")
				FSM_PROFILE_SECTION_IN ("pulse_noise_manager [init enter execs]", state2_enter_exec)
				{
				int lvi_noise_index;
				Prohandle lvpro_child_process;
					
				gvoid_noise_generater = op_id_self();
				
				printf("start generate pulse_noise_generater process\n");
				for (lvi_noise_index=0; lvi_noise_index<gvi_NOISE_num; lvi_noise_index++)
				{
					lvpro_child_process = op_pro_create("pulse_noise_generater", OPC_NIL);
					op_pro_invoke(lvpro_child_process, &lvi_noise_index);
				}
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "init", "pulse_noise_manager [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_2", "pulse_noise_manager [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"pulse_noise_manager")
		}
	}




void
_op_pulse_noise_manager_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_pulse_noise_manager_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_pulse_noise_manager_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (op_sv_ptr);

	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_pulse_noise_manager_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_pulse_noise_manager_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (pulse_noise_manager)",
		sizeof (pulse_noise_manager_state));
	*init_block_ptr = 0;

	FRET (obtype)
	}

VosT_Address
_op_pulse_noise_manager_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	pulse_noise_manager_state * ptr;
	FIN_MT (_op_pulse_noise_manager_alloc (obtype))

	ptr = (pulse_noise_manager_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "pulse_noise_manager [wait_channel_init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_pulse_noise_manager_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{

	FIN_MT (_op_pulse_noise_manager_svar (gen_ptr, var_name, var_p_ptr))

	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

