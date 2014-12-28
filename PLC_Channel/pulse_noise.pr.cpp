/* Process model C++ form file: pulse_noise.pr.cpp */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char pulse_noise_pr_cpp [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 549FF6C9 549FF6C9 1 lu-wspn lu 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                               ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include "PLC_def.h"
#include "PLC_Channel.h"
#include "PLC_func.h"

#define SYS_INITED				((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_SYS_INIT))
#define NEXT_PULSE_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_NOISE_PULSE_START))
#define POWER_UPDATE			((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_NOISE_POWER_UPDATE))

#define NOISE_POWER_MEAN			0.006
#define NOISE_POWER_MIN				0.001
#define NOISE_PULSE_TIME_DELTA		1
#define NOISE_UPDATE_TIME_DELTA		0.001
#define PULSE_TIME_MIN				0.020
#define NOISE_POWER_UPDATE_PARA		0.00075

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
class pulse_noise_state
	{
	private:
		/* Internal state tracking for FSM */
		FSM_SYS_STATE

	public:
		pulse_noise_state (void);

		/* Destructor contains Termination Block */
		~pulse_noise_state (void);

		/* State Variables */
		PPDU_T *	               		svp_ppdu                                        ;
		double	                 		svd_pulse_start_time                            ;
		double	                 		svd_pulse_start_power                           ;
		int	                    		svi_node_index                                  ;
		PHASE_T	                		sve_work_phase                                  ;
		Distribution *	         		svp_normal_dist                                 ;
		Distribution *	         		svp_exp_dist                                    ;

		/* FSM code */
		void pulse_noise (OP_SIM_CONTEXT_ARG_OPT);
		/* Diagnostic Block */
		void _op_pulse_noise_diag (OP_SIM_CONTEXT_ARG_OPT);

#if defined (VOSD_NEW_BAD_ALLOC)
		void * operator new (size_t) throw (VOSD_BAD_ALLOC);
#else
		void * operator new (size_t);
#endif
		void operator delete (void *);

		/* Memory management */
		static VosT_Obtype obtype;
	};

VosT_Obtype pulse_noise_state::obtype = (VosT_Obtype)OPC_NIL;

#define svp_ppdu                		op_sv_ptr->svp_ppdu
#define svd_pulse_start_time    		op_sv_ptr->svd_pulse_start_time
#define svd_pulse_start_power   		op_sv_ptr->svd_pulse_start_power
#define svi_node_index          		op_sv_ptr->svi_node_index
#define sve_work_phase          		op_sv_ptr->sve_work_phase
#define svp_normal_dist         		op_sv_ptr->svp_normal_dist
#define svp_exp_dist            		op_sv_ptr->svp_exp_dist

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	pulse_noise_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((pulse_noise_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

/************************************************************/
/* Author: jiaying.lu                                       */
/* Last Update: 2014.12.28                                  */
/* Remarks:        											*/
/************************************************************/
static void
PPDU_attr_set(PPDU_T *ppdu)
{
	FIN(PPDU_attr_set());
	
	// set PPDU attr
	ppdu->type = PPDU_TYPE_NOISE;
	ppdu->start_time = op_sim_time();
	ppdu->end_time = -1;
	ppdu->transmitter_node_index = svi_node_index;
	ppdu->receiver_node_index = 0;
	ppdu->power_linear = op_dist_outcome(svp_normal_dist);
	if (ppdu->power_linear < NOISE_POWER_MIN)
	{
		ppdu->power_linear = NOISE_POWER_MIN;
	}
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
#undef svp_ppdu
#undef svd_pulse_start_time
#undef svd_pulse_start_power
#undef svi_node_index
#undef sve_work_phase
#undef svp_normal_dist
#undef svp_exp_dist

/* Access from C kernel using C linkage */
extern "C"
{
	VosT_Obtype _op_pulse_noise_init (int * init_block_ptr);
	VosT_Address _op_pulse_noise_alloc (VosT_Obtype, int);
	void pulse_noise (OP_SIM_CONTEXT_ARG_OPT)
		{
		((pulse_noise_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->pulse_noise (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_pulse_noise_svar (void *, const char *, void **);

	void _op_pulse_noise_diag (OP_SIM_CONTEXT_ARG_OPT)
		{
		((pulse_noise_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->_op_pulse_noise_diag (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_pulse_noise_terminate (OP_SIM_CONTEXT_ARG_OPT)
		{
		/* The destructor is the Termination Block */
		delete (pulse_noise_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr);
		}


} /* end of 'extern "C"' */




/* Process model interrupt handling procedure */


void
pulse_noise_state::pulse_noise (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (pulse_noise_state::pulse_noise ());
	try
		{


		FSM_ENTER ("pulse_noise")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED (0, "init", state0_enter_exec, "pulse_noise [init enter execs]")
				FSM_PROFILE_SECTION_IN ("pulse_noise [init enter execs]", state0_enter_exec)
				{
				Ici *lvp_ici;
				Distribution *lvp_exp_dist;
				double lvd_pulse_time;
				Objid lvoid_NODE_id = op_topo_parent(op_id_self());
				
				svp_normal_dist = op_dist_load("normal", NOISE_POWER_MEAN, 1);    // 0.006 mw = -22dBm
				svp_exp_dist = op_dist_load("exponential", NOISE_PULSE_TIME_DELTA, 0);
				
				
				/* find self index */
				svi_node_index = _global_self_index_find(lvoid_NODE_id);
				/* set PHY Objid */
				gvoid_node_oids[svi_node_index].phy_id = op_id_self();
				/* get work phase */
				op_ima_obj_attr_get(lvoid_NODE_id, "work_phase", &sve_work_phase);
				printf("[PHY.init] NODE index:%d, work_phase:%d\n", svi_node_index, sve_work_phase);
				
				/* send noise PPDU to Channel */
				lvp_ici = op_ici_create("PPDU");
				svp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));
				PPDU_attr_set(svp_ppdu);
				
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
			FSM_STATE_EXIT_FORCED (0, "init", "pulse_noise [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_0", "pulse_noise [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "pulse_noise [idle enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"pulse_noise")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "pulse_noise [idle exit execs]")
				FSM_PROFILE_SECTION_IN ("pulse_noise [idle exit execs]", state1_exit_exec)
				{
				char lvc_err_msg[25];
				
				printf("Leave pulse_noise_generater.idle. ");
				
				if (POWER_UPDATE)
				{
					//printf("receive INTRPT: POWER_UPDATE. Enter next state.\n");
					;
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
			FSM_PROFILE_SECTION_IN ("pulse_noise [idle trans conditions]", state1_trans_conds)
			FSM_INIT_COND (POWER_UPDATE)
			FSM_TEST_COND (NEXT_PULSE_START)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "POWER_UPDATE", "", "idle", "update_noise", "tr_5", "pulse_noise [idle -> update_noise : POWER_UPDATE / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "NEXT_PULSE_START", "", "idle", "set_next_pulse", "tr_7", "pulse_noise [idle -> set_next_pulse : NEXT_PULSE_START / ]")
				FSM_CASE_TRANSIT (2, 1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_9", "pulse_noise [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (update_noise) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "update_noise", state2_enter_exec, "pulse_noise [update_noise enter execs]")
				FSM_PROFILE_SECTION_IN ("pulse_noise [update_noise enter execs]", state2_enter_exec)
				{
				/* update noise power */
				svp_ppdu->power_linear = exp((svd_pulse_start_time-op_sim_time())/NOISE_POWER_UPDATE_PARA) * svd_pulse_start_power;
				//printf("[noise %d] current time:%lf, update pulse power: %lf\n", svi_node_index, op_sim_time(), svp_ppdu->power_linear);
				
				/* schedule next noise power update */
				// after 20ms, power drop to 0.
				if ((op_sim_time()-svd_pulse_start_time) <= PULSE_TIME_MIN)
				{
				    op_intrpt_schedule_self(op_sim_time()+NOISE_UPDATE_TIME_DELTA, INTRPT_NOISE_POWER_UPDATE);
				}
				else
				{
					printf("[noise %d] current time:%lf, and pulse start time:%lf. Already passed 20ms.\n", svi_node_index, op_sim_time(), svd_pulse_start_time);
				}
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (update_noise) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "update_noise", "pulse_noise [update_noise exit execs]")


			/** state (update_noise) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "update_noise", "idle", "tr_6", "pulse_noise [update_noise -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (set_next_pulse) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "set_next_pulse", state3_enter_exec, "pulse_noise [set_next_pulse enter execs]")
				FSM_PROFILE_SECTION_IN ("pulse_noise [set_next_pulse enter execs]", state3_enter_exec)
				{
				double lvd_pulse_time;
				
				/* set PPDU current pulse attr*/
				svp_ppdu->power_linear = op_dist_outcome(svp_normal_dist);
				svd_pulse_start_power = svp_ppdu->power_linear;
				printf("[node %d] start pulse power: %lf\n", svi_node_index, svp_ppdu->power_linear);
				
				
				/* schedule noise power update */
				svd_pulse_start_time = op_sim_time();
				printf("[node %d] current pulse start time: %lf, ", svi_node_index, svd_pulse_start_time);
				op_intrpt_schedule_self(svd_pulse_start_time, INTRPT_NOISE_POWER_UPDATE);
				
				
				/* schedule next pulse */
				lvd_pulse_time = op_dist_outcome(svp_exp_dist);
				if (lvd_pulse_time < PULSE_TIME_MIN)			//next pulse time should gt PULSE_TIME_MIN
				{
					lvd_pulse_time = PULSE_TIME_MIN;
				}
				printf(" next pulse time: %lf\n", op_sim_time()+lvd_pulse_time);
				op_intrpt_schedule_self(op_sim_time()+lvd_pulse_time, INTRPT_NOISE_PULSE_START);
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (set_next_pulse) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "set_next_pulse", "pulse_noise [set_next_pulse exit execs]")


			/** state (set_next_pulse) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "set_next_pulse", "idle", "tr_8", "pulse_noise [set_next_pulse -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (wait_sys_inited) enter executives **/
			FSM_STATE_ENTER_UNFORCED (4, "wait_sys_inited", state4_enter_exec, "pulse_noise [wait_sys_inited enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (9,"pulse_noise")


			/** state (wait_sys_inited) exit executives **/
			FSM_STATE_EXIT_UNFORCED (4, "wait_sys_inited", "pulse_noise [wait_sys_inited exit execs]")


			/** state (wait_sys_inited) transition processing **/
			FSM_PROFILE_SECTION_IN ("pulse_noise [wait_sys_inited trans conditions]", state4_trans_conds)
			FSM_INIT_COND ((1))
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("wait_sys_inited")
			FSM_PROFILE_SECTION_OUT (state4_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 0, state0_enter_exec, ;, "", "", "wait_sys_inited", "init", "tr_11", "pulse_noise [wait_sys_inited -> init :  / ]")
				FSM_CASE_TRANSIT (1, 4, state4_enter_exec, ;, "default", "", "wait_sys_inited", "wait_sys_inited", "tr_10", "pulse_noise [wait_sys_inited -> wait_sys_inited : default / ]")
				}
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (4,"pulse_noise")
		}
	catch (...)
		{
		Vos_Error_Print (VOSC_ERROR_ABORT,
			(const char *)VOSC_NIL,
			"Unhandled C++ exception in process model (pulse_noise)",
			(const char *)VOSC_NIL, (const char *)VOSC_NIL);
		}
	}




void
pulse_noise_state::_op_pulse_noise_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}

void
pulse_noise_state::operator delete (void* ptr)
	{
	FIN (pulse_noise_state::operator delete (ptr));
	Vos_Poolmem_Dealloc (ptr);
	FOUT
	}

pulse_noise_state::~pulse_noise_state (void)
	{

	FIN (pulse_noise_state::~pulse_noise_state ())


	/* No Termination Block */


	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

void *
pulse_noise_state::operator new (size_t)
#if defined (VOSD_NEW_BAD_ALLOC)
		throw (VOSD_BAD_ALLOC)
#endif
	{
	void * new_ptr;

	FIN_MT (pulse_noise_state::operator new ());

	new_ptr = Vos_Alloc_Object (pulse_noise_state::obtype);
#if defined (VOSD_NEW_BAD_ALLOC)
	if (new_ptr == VOSC_NIL) throw VOSD_BAD_ALLOC();
#endif
	FRET (new_ptr)
	}

/* State constructor initializes FSM handling */
/* by setting the initial state to the first */
/* block of code to enter. */

pulse_noise_state::pulse_noise_state (void) :
		_op_current_block (8)
	{
#if defined (OPD_ALLOW_ODB)
		_op_current_state = "pulse_noise [wait_sys_inited enter execs]";
#endif
	}

VosT_Obtype
_op_pulse_noise_init (int * init_block_ptr)
	{
	FIN_MT (_op_pulse_noise_init (init_block_ptr))

	pulse_noise_state::obtype = Vos_Define_Object_Prstate ("proc state vars (pulse_noise)",
		sizeof (pulse_noise_state));
	*init_block_ptr = 8;

	FRET (pulse_noise_state::obtype)
	}

VosT_Address
_op_pulse_noise_alloc (VosT_Obtype, int)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	pulse_noise_state * ptr;
	FIN_MT (_op_pulse_noise_alloc ())

	/* New instance will have FSM handling initialized */
#if defined (VOSD_NEW_BAD_ALLOC)
	try {
		ptr = new pulse_noise_state;
	} catch (const VOSD_BAD_ALLOC &) {
		ptr = VOSC_NIL;
	}
#else
	ptr = new pulse_noise_state;
#endif
	FRET ((VosT_Address)ptr)
	}



void
_op_pulse_noise_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	pulse_noise_state		*prs_ptr;

	FIN_MT (_op_pulse_noise_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (pulse_noise_state *)gen_ptr;

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
	if (strcmp ("svp_normal_dist" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svp_normal_dist);
		FOUT
		}
	if (strcmp ("svp_exp_dist" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svp_exp_dist);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

