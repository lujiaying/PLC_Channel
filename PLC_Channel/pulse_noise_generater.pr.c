/* Process model C form file: pulse_noise_generater.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char pulse_noise_generater_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 549CCEF8 549CCEF8 1 lu-wspn lu 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                               ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include "PLC_def.h"
#include "PLC_Channel.h"

#define NEXT_PULSE_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_NOISE_PULSE_START))
#define POWER_UPDATE			((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_NOISE_POWER_UPDATE))

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
	/* State Variables */
	int	                    		svi_noise_index                                 ;
	PPDU_T *	               		svp_ppdu                                        ;
	double	                 		svd_pulse_start_time                            ;
	double	                 		svd_pulse_start_power                           ;
	} pulse_noise_generater_state;

#define svi_noise_index         		op_sv_ptr->svi_noise_index
#define svp_ppdu                		op_sv_ptr->svp_ppdu
#define svd_pulse_start_time    		op_sv_ptr->svd_pulse_start_time
#define svd_pulse_start_power   		op_sv_ptr->svd_pulse_start_power

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	pulse_noise_generater_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((pulse_noise_generater_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


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
	void pulse_noise_generater (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_pulse_noise_generater_init (int * init_block_ptr);
	void _op_pulse_noise_generater_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_pulse_noise_generater_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_pulse_noise_generater_alloc (VosT_Obtype, int);
	void _op_pulse_noise_generater_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
pulse_noise_generater (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (pulse_noise_generater ());

		{


		FSM_ENTER ("pulse_noise_generater")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (0, "init", "pulse_noise_generater [init enter execs]")
				FSM_PROFILE_SECTION_IN ("pulse_noise_generater [init enter execs]", state0_enter_exec)
				{
				Ici *lvp_ici;
				Distribution *lvp_exp_dist;
				double lvd_pulse_time;
				
				/* init */
				svi_noise_index = *(int *)op_pro_argmem_access();
				printf("[pulse_noise_generater.init] generate pulse_noise_generater process success, index is %d.  ", svi_noise_index);
				
				
				/* send noise PPDU to Channel */
				lvp_ici = op_ici_create("PPDU");
				svp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));
				svp_ppdu->type = 3;
				svp_ppdu->start_time = op_sim_time();
				svp_ppdu->end_time = -1;
				svp_ppdu->PPDU_index = svi_noise_index;
				svp_ppdu->transmitter_node_index = 1 + gvi_HE_num + gvi_CPE_num + svi_noise_index;
				
				op_ici_attr_set(lvp_ici, "PPDU_ptr", svp_ppdu);
				op_ici_install(lvp_ici);
				op_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_START, gvoid_channel);
				op_ici_install(OPC_NIL);
				
				
				/* schedule first pulse */
				lvp_exp_dist = op_dist_load("exponential", 1, 0);
				lvd_pulse_time = op_dist_outcome(lvp_exp_dist);
				printf("current time:%lf, first pulse time: %lf\n", op_sim_time(), op_sim_time()+lvd_pulse_time);
				op_intrpt_schedule_self(op_sim_time()+lvd_pulse_time, INTRPT_NOISE_PULSE_START);
				op_dist_unload(lvp_exp_dist);
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "pulse_noise_generater [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_0", "pulse_noise_generater [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "pulse_noise_generater [idle enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"pulse_noise_generater")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "pulse_noise_generater [idle exit execs]")
				FSM_PROFILE_SECTION_IN ("pulse_noise_generater [idle exit execs]", state1_exit_exec)
				{
				char lvc_err_msg[25];
				
				printf("Leave pulse_noise_generater.idle. ");
				
				if (POWER_UPDATE)
				{
					printf("receive INTRPT: POWER_UPDATE. Enter next state.\n");
				}
				else if (NEXT_PULSE_START)
				{
					printf("receive INTRPT: NEXT_PULSE_START. Enter next state.\n");
				}
				else
				{
					sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());
					op_sim_end("Error: Unexpected INTRPT is received at \"pulse_noise_generater.idle.Exit\" state!", "Error source module: CHANNEL", lvc_err_msg, "");
				}
				}
				FSM_PROFILE_SECTION_OUT (state1_exit_exec)


			/** state (idle) transition processing **/
			FSM_PROFILE_SECTION_IN ("pulse_noise_generater [idle trans conditions]", state1_trans_conds)
			FSM_INIT_COND (POWER_UPDATE)
			FSM_TEST_COND (NEXT_PULSE_START)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "POWER_UPDATE", "", "idle", "update_noise", "tr_5", "pulse_noise_generater [idle -> update_noise : POWER_UPDATE / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "NEXT_PULSE_START", "", "idle", "set_next_pulse", "tr_7", "pulse_noise_generater [idle -> set_next_pulse : NEXT_PULSE_START / ]")
				FSM_CASE_TRANSIT (2, 1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_9", "pulse_noise_generater [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (update_noise) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "update_noise", state2_enter_exec, "pulse_noise_generater [update_noise enter execs]")
				FSM_PROFILE_SECTION_IN ("pulse_noise_generater [update_noise enter execs]", state2_enter_exec)
				{
				double ne_exponential_para;
				
				/* update noise power */
				ne_exponential_para = 0.00075;
				svp_ppdu->power_linear = exp((svd_pulse_start_time-op_sim_time())/ne_exponential_para) * svd_pulse_start_power;
				//printf("[noise %d] current time:%lf, update pulse power: %lf\n", svi_noise_index, op_sim_time(), svp_ppdu->power_linear);
				
				/* schedule next noise power update */
				// after 20ms, power drop to 0.
				if ((op_sim_time()-svd_pulse_start_time) <= 0.020)
				{
				    op_intrpt_schedule_self(op_sim_time()+0.001, INTRPT_NOISE_POWER_UPDATE);
				}
				else
				{
					printf("[noise %d] current time:%lf, and pulse start time:%lf. Already passed 20ms.\n", svi_noise_index, op_sim_time(), svd_pulse_start_time);
				}
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (update_noise) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "update_noise", "pulse_noise_generater [update_noise exit execs]")


			/** state (update_noise) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "update_noise", "idle", "tr_6", "pulse_noise_generater [update_noise -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (set_next_pulse) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "set_next_pulse", state3_enter_exec, "pulse_noise_generater [set_next_pulse enter execs]")
				FSM_PROFILE_SECTION_IN ("pulse_noise_generater [set_next_pulse enter execs]", state3_enter_exec)
				{
				Distribution *lvp_dist;
				double lvd_pulse_time;
				
				
				/* set PPDU current pulse attr*/
				lvp_dist = op_dist_load("normal", 158.4893, 1);    // 158.4893 mw = 22dBm
				svp_ppdu->power_linear = op_dist_outcome(lvp_dist);
				svd_pulse_start_power = svp_ppdu->power_linear;
				op_dist_unload(lvp_dist);
				printf("[noise %d] start pulse power: %lf\n", svi_noise_index, svp_ppdu->power_linear);
				
				
				/* schedule noise power update */
				svd_pulse_start_time = op_sim_time();
				printf("[noise %d] current pulse start time: %lf, ", svi_noise_index, svd_pulse_start_time);
				op_intrpt_schedule_self(svd_pulse_start_time, INTRPT_NOISE_POWER_UPDATE);
				
				
				/* schedule next pulse */
				lvp_dist = op_dist_load("exponential", 1, 0);
				lvd_pulse_time = op_dist_outcome(lvp_dist);
				printf(" next pulse time: %lf\n", op_sim_time()+lvd_pulse_time);
				op_intrpt_schedule_self(op_sim_time()+lvd_pulse_time, INTRPT_NOISE_PULSE_START);
				op_dist_unload(lvp_dist);
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (set_next_pulse) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "set_next_pulse", "pulse_noise_generater [set_next_pulse exit execs]")


			/** state (set_next_pulse) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "set_next_pulse", "idle", "tr_8", "pulse_noise_generater [set_next_pulse -> idle : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"pulse_noise_generater")
		}
	}




void
_op_pulse_noise_generater_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_pulse_noise_generater_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_pulse_noise_generater_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (op_sv_ptr);

	FOUT
	}


/* Undefine shortcuts to state variables to avoid */
/* syntax error in direct access to fields of */
/* local variable prs_ptr in _op_pulse_noise_generater_svar function. */
#undef svi_noise_index
#undef svp_ppdu
#undef svd_pulse_start_time
#undef svd_pulse_start_power

#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_pulse_noise_generater_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_pulse_noise_generater_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (pulse_noise_generater)",
		sizeof (pulse_noise_generater_state));
	*init_block_ptr = 0;

	FRET (obtype)
	}

VosT_Address
_op_pulse_noise_generater_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	pulse_noise_generater_state * ptr;
	FIN_MT (_op_pulse_noise_generater_alloc (obtype))

	ptr = (pulse_noise_generater_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "pulse_noise_generater [init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_pulse_noise_generater_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	pulse_noise_generater_state		*prs_ptr;

	FIN_MT (_op_pulse_noise_generater_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (pulse_noise_generater_state *)gen_ptr;

	if (strcmp ("svi_noise_index" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svi_noise_index);
		FOUT
		}
	if (strcmp ("svp_ppdu" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svp_ppdu);
		FOUT
		}
	if (strcmp ("svd_pulse_start_time" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svd_pulse_start_time);
		FOUT
		}
	if (strcmp ("svd_pulse_start_power" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svd_pulse_start_power);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

