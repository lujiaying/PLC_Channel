/* Process model C form file: HE_PHY.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char HE_PHY_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 5492437C 5492437C 1 lu-wspn lu 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                               ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include "PLC_def.h"
#include "PLC_Channel.h"


#define SYS_INIT			((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_SYS_INIT))
#define PPDU_TIME_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_START))
#define PPDU_END			((op_intrpt_type() == OPC_INTRPT_REMOTE) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_END))			

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
	int	                    		svi_HE_index                                    ;
	} HE_PHY_state;

#define svi_HE_index            		op_sv_ptr->svi_HE_index

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	HE_PHY_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((HE_PHY_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


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
	void HE_PHY (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_HE_PHY_init (int * init_block_ptr);
	void _op_HE_PHY_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_HE_PHY_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_HE_PHY_alloc (VosT_Obtype, int);
	void _op_HE_PHY_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
HE_PHY (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (HE_PHY ());

		{


		FSM_ENTER ("HE_PHY")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED (0, "init", state0_enter_exec, "HE_PHY [init enter execs]")
				FSM_PROFILE_SECTION_IN ("HE_PHY [init enter execs]", state0_enter_exec)
				{
				Distribution *lvp_exp_dist;
				double lvd_ppdu_time;
				
				/* find self node index */
				for (svi_HE_index=0; svi_HE_index<gvi_HE_num; svi_HE_index++)
				{
					if (gvp_node_objid[svi_HE_index].PHY == op_id_self())
					{
						break;
					}
				}
				printf("gvp_node_objid[svi_HE_index].PHY:%d, op_id_self:%d\n", gvp_node_objid[0].PHY, op_id_self());
				if (svi_HE_index == gvi_HE_num)
				{
					op_sim_end("HE index exceed gvi_HE_num",  "Error source module: HE_PHY", "Error source state: init.enter", "");
				}
				
				/* schedule first PPDU */
				lvp_exp_dist = op_dist_load("exponential", 1, 0);
				lvd_ppdu_time = op_dist_outcome(lvp_exp_dist);
				printf("[PHY.init] current time:%lf, first PPDU time:%lf\n", op_sim_time(), op_sim_time()+lvd_ppdu_time);
				op_intrpt_schedule_self(op_sim_time()+lvd_ppdu_time, INTRPT_CHANNEL_PPDU_START);
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "HE_PHY [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_0", "HE_PHY [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "HE_PHY [idle enter execs]")
				FSM_PROFILE_SECTION_IN ("HE_PHY [idle enter execs]", state1_enter_exec)
				{
				printf("enter PHY.idle.\n");
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"HE_PHY")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "HE_PHY [idle exit execs]")
				FSM_PROFILE_SECTION_IN ("HE_PHY [idle exit execs]", state1_exit_exec)
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
			FSM_PROFILE_SECTION_IN ("HE_PHY [idle trans conditions]", state1_trans_conds)
			FSM_INIT_COND (PPDU_TIME_START)
			FSM_TEST_COND (PPDU_END)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "PPDU_TIME_START", "", "idle", "send_PPDU_2Channel", "tr_1", "HE_PHY [idle -> send_PPDU_2Channel : PPDU_TIME_START / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "PPDU_END", "", "idle", "receive_Channel_PPDU", "tr_3", "HE_PHY [idle -> receive_Channel_PPDU : PPDU_END / ]")
				FSM_CASE_TRANSIT (2, 1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_5", "HE_PHY [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (send_PPDU_2Channel) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "send_PPDU_2Channel", state2_enter_exec, "HE_PHY [send_PPDU_2Channel enter execs]")
				FSM_PROFILE_SECTION_IN ("HE_PHY [send_PPDU_2Channel enter execs]", state2_enter_exec)
				{
				Ici *lvp_ici;
				Distribution *lvp_dist;
				double lvd_next_ppdu_time;
				PPDU_T *lvp_ppdu;
				
				/* send PPDU to channel*/
				lvp_ici = op_ici_create("PPDU");
				lvp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));
				lvp_ppdu->type = 0;
				lvp_ppdu->start_time = op_sim_time();
				lvp_ppdu->end_time = op_sim_time() + op_dist_uniform(5);
				lvp_ppdu->transmitter_node_index = svi_HE_index;
				lvp_ppdu->receiver_node_index = gvi_HE_num + 1 + (int)(op_dist_uniform(gvi_CPE_num));
				lvp_dist = op_dist_load("normal", 3.16, 1);    //mean power 35dBm = 3.16w
				lvp_ppdu->power_linear = op_dist_outcome(lvp_dist);
				op_dist_unload(lvp_dist);
				printf("[PHY.send_PPDU_2Channel] transmitter:%d, receiver:%d, power_linear:%lf, start_time:%lf, end_time:%lf\n", lvp_ppdu->transmitter_node_index, lvp_ppdu->receiver_node_index, lvp_ppdu->power_linear, lvp_ppdu->start_time, lvp_ppdu->end_time);
				
				op_ici_attr_set(lvp_ici, "PPDU_ptr", lvp_ppdu);
				op_ici_install(lvp_ici);
				op_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_START, gvoid_channel);
				op_ici_install(OPC_NIL);
				
				/* schedule next PPDU */
				lvp_dist = op_dist_load("exponential", 1, 0);
				lvd_next_ppdu_time = op_dist_outcome(lvp_dist);
				op_dist_unload(lvp_dist);
				printf("[PHY.send_PPDU_2Channel] current time:%lf, next PPDU time:%lf\n", op_sim_time(), op_sim_time()+lvd_next_ppdu_time);
				op_intrpt_schedule_self(op_sim_time()+lvd_next_ppdu_time, INTRPT_CHANNEL_PPDU_START);
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (send_PPDU_2Channel) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "send_PPDU_2Channel", "HE_PHY [send_PPDU_2Channel exit execs]")


			/** state (send_PPDU_2Channel) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "send_PPDU_2Channel", "idle", "tr_2", "HE_PHY [send_PPDU_2Channel -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (receive_Channel_PPDU) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "receive_Channel_PPDU", state3_enter_exec, "HE_PHY [receive_Channel_PPDU enter execs]")
				FSM_PROFILE_SECTION_IN ("HE_PHY [receive_Channel_PPDU enter execs]", state3_enter_exec)
				{
				Ici *lvp_ici;
				PPDU_T *lvp_PPDU;
				
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
				op_prg_mem_free(lvp_PPDU->actual_receiver_array);
				prg_mem_free(lvp_PPDU);
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (receive_Channel_PPDU) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "receive_Channel_PPDU", "HE_PHY [receive_Channel_PPDU exit execs]")


			/** state (receive_Channel_PPDU) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "receive_Channel_PPDU", "idle", "tr_4", "HE_PHY [receive_Channel_PPDU -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (wait_sys_init) enter executives **/
			FSM_STATE_ENTER_UNFORCED (4, "wait_sys_init", state4_enter_exec, "HE_PHY [wait_sys_init enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (9,"HE_PHY")


			/** state (wait_sys_init) exit executives **/
			FSM_STATE_EXIT_UNFORCED (4, "wait_sys_init", "HE_PHY [wait_sys_init exit execs]")
				FSM_PROFILE_SECTION_IN ("HE_PHY [wait_sys_init exit execs]", state4_exit_exec)
				{
				char lvc_err_msg[25];
				
				printf("Leave HE_PHY.wait_sys_init ");
				
				if (SYS_INIT)
				{
					printf("receive MCAST INTRPT: SYS_INIT.\n");
				}
				else
				{
					sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());
					op_sim_end("Error: Unexpected INTRPT is received at \"HE_PHY.wait_sys_init.Exit\" state!", "Error source module: HE_PHY", lvc_err_msg, "");
				}
				}
				FSM_PROFILE_SECTION_OUT (state4_exit_exec)


			/** state (wait_sys_init) transition processing **/
			FSM_PROFILE_SECTION_IN ("HE_PHY [wait_sys_init trans conditions]", state4_trans_conds)
			FSM_INIT_COND (SYS_INIT)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("wait_sys_init")
			FSM_PROFILE_SECTION_OUT (state4_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 0, state0_enter_exec, ;, "SYS_INIT", "", "wait_sys_init", "init", "tr_6", "HE_PHY [wait_sys_init -> init : SYS_INIT / ]")
				FSM_CASE_TRANSIT (1, 4, state4_enter_exec, ;, "default", "", "wait_sys_init", "wait_sys_init", "tr_7", "HE_PHY [wait_sys_init -> wait_sys_init : default / ]")
				}
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (4,"HE_PHY")
		}
	}




void
_op_HE_PHY_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_HE_PHY_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_HE_PHY_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (op_sv_ptr);

	FOUT
	}


/* Undefine shortcuts to state variables to avoid */
/* syntax error in direct access to fields of */
/* local variable prs_ptr in _op_HE_PHY_svar function. */
#undef svi_HE_index

#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_HE_PHY_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_HE_PHY_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (HE_PHY)",
		sizeof (HE_PHY_state));
	*init_block_ptr = 8;

	FRET (obtype)
	}

VosT_Address
_op_HE_PHY_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	HE_PHY_state * ptr;
	FIN_MT (_op_HE_PHY_alloc (obtype))

	ptr = (HE_PHY_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "HE_PHY [wait_sys_init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_HE_PHY_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	HE_PHY_state		*prs_ptr;

	FIN_MT (_op_HE_PHY_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (HE_PHY_state *)gen_ptr;

	if (strcmp ("svi_HE_index" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svi_HE_index);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

