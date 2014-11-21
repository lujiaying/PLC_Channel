/* Process model C form file: channel.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char channel_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 546F1417 546F1417 1 lu-wspn lu 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                               ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include "PLC_def.h"
#include "PLC_data.h"
#include "PLC_func.h"
//#include "PLC_Channel.h"

#define SYS_INIT				((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_SYS_INIT))
#define PHY_TXSTART				((op_intrpt_type() == OPC_INTRPT_REMOTE) && (op_intrpt_code() == INTRPT_PHY_TXSTART))
#define PHY_TXEND				((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_PHY_TXEND))

/* enum */
typedef enum NODE_TYPE_T
{
    TYPE_HE = 0,
    TYPE_CPE = 1,
    TYPE_NOISE = 2,
    TYPE_X = 3,
} NODE_TYPE_T;

typedef enum PHASE_T
{
    PHASE_ABC = 0,
    PHASE_A = 1,
    PHASE_B = 2,
    PHASE_C = 3,
} PHASE_T;
/* end enum */


/* struct */
typedef struct NODE_T
{
    int node_id;
    NODE_TYPE_T type;
    int parent_id;
    double distance;
    PHASE_T phase;
} NODE_T;


typedef struct DISTANCE_PHASE_T
{
    double dis_x;
    double dix_Y;
    double dis_Z;
    PHASE_T pha_X;
    PHASE_T pha_Y;
    PHASE_T pha_Z;
} DISTANCE_PHASE_T;
/* end struct */


static double MPDU_receive_power_calculate(int, MPDU_T *);
static void PHY_medium_refresh(void);
static void MPDU_sinr_segment_refresh(void);
static void MPDU_sinr_calculate(MPDU_T *);

int HE_num = 0, CPE_num = 0, NOISE_num = 0, X_num = 0;

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
	List *	                 		svlist_channel                                  ;
	Stathandle	             		svgstat_SINR                                    ;
	Stathandle	             		svgstat_active_MPDU_number                      ;
	} channel_state;

#define svlist_channel          		op_sv_ptr->svlist_channel
#define svgstat_SINR            		op_sv_ptr->svgstat_SINR
#define svgstat_active_MPDU_number		op_sv_ptr->svgstat_active_MPDU_number

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	channel_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((channel_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

/************************************************************/
/* Author: Li Yong	                                     	*/
/* Last Update: 2014.08.25                                  */
/* Remarks:                                             	*/
/************************************************************/
static double
MPDU_receive_power_calculate(int lvi_receiver_node_index, MPDU_T *lvp_MPDU)
{
	double lvd_pathloss_linear;
	
	FIN(MPDU_receive_power_calculate());
	
	lvd_pathloss_linear = pow(10.0, gvo_pathloss_matrix_dB[lvp_MPDU->transmitter_node_index][lvi_receiver_node_index]/10.0);
    
	FRET(lvp_MPDU->power_linear / lvd_pathloss_linear);
}


/************************************************************/
/* Author: Li Yong	                                     	*/
/* Last Update: 2014.08.25                                  */
/* Remarks:                                             	*/
/************************************************************/
static void
PHY_medium_refresh()	
{
	int lvi_HE_index;
	int lvi_CPE_index;
	int lvi_node_index;
	int lvi_MPDU_index;
	int lvi_MPDU_number;
	MPDU_T *lvp_MPDU;
	double lvd_sum_power_linear;
	double lvd_sum_power_dBm;
	
	FIN(PHY_medium_refresh());
	
	lvi_MPDU_number = op_prg_list_size(svlist_channel);
	
	for (lvi_HE_index = 0; lvi_HE_index < gvi_HE_number; lvi_HE_index++)
	{
		if (gvo_HE_property[lvi_HE_index].PHY_poweron_flag == OPC_TRUE)
		{	
			lvi_node_index = lvi_HE_index;
			lvd_sum_power_linear = 0.0;
		
			for (lvi_MPDU_index = 0; lvi_MPDU_index < lvi_MPDU_number; lvi_MPDU_index++)
			{
				lvp_MPDU = (MPDU_T *)op_prg_list_access(svlist_channel, lvi_MPDU_index);
				lvd_sum_power_linear += MPDU_receive_power_calculate(lvi_node_index, lvp_MPDU);
			}
		
			lvd_sum_power_linear += CONST_HE_THERMAL_NOISE_LINEAR;
			lvd_sum_power_dBm = 10.0 * log10(lvd_sum_power_linear) + 30.0;
			op_stat_write(gvo_HE_property[lvi_HE_index].PHY_medium_stathandle, lvd_sum_power_dBm);
		}	
	}	
	
	for (lvi_CPE_index = 0; lvi_CPE_index < gvi_CPE_number; lvi_CPE_index++)
	{
		if (gvo_CPE_property[lvi_CPE_index].PHY_poweron_flag == OPC_TRUE)
		{	
			lvi_node_index = lvi_CPE_index + gvi_HE_number;
			lvd_sum_power_linear = 0.0;
		
			for (lvi_MPDU_index = 0; lvi_MPDU_index < lvi_MPDU_number; lvi_MPDU_index++)
			{
				lvp_MPDU = (MPDU_T *)op_prg_list_access(svlist_channel, lvi_MPDU_index);
				lvd_sum_power_linear += MPDU_receive_power_calculate(lvi_node_index, lvp_MPDU);
			}

			lvd_sum_power_linear += CONST_CPE_THERMAL_NOISE_LINEAR;
			lvd_sum_power_dBm = 10.0 * log10(lvd_sum_power_linear) + 30.0;
			op_stat_write(gvo_CPE_property[lvi_CPE_index].PHY_medium_stathandle, lvd_sum_power_dBm);
		}
	}	
	
	FOUT;
}	


/************************************************************/
/* Author: Li Yong                                      	*/
/* Last Update: 2014.08.25                                  */
/* Remarks:                                             	*/
/************************************************************/
static void
MPDU_sinr_segment_refresh()
{
	int lvi_MPDU_number;
	int lvi_MPDU_index_1;
	int lvi_MPDU_index_2;
	MPDU_T *lvp_MPDU_1;
	MPDU_T *lvp_MPDU_2;
	double lvd_signal_power_linear;
	double lvd_interference_power_linear;
	double lvd_noise_power_linear;
	double lvd_sinr_linear;
	SEGMENT_SINR_T *lvp_segment_sinr;
	int lvi_actual_receiver_number;
	int lvi_actual_receiver_index;
	int lvi_actual_receiver_node_index;
	
	FIN(MPDU_sinr_segment_refresh());
	
	lvi_MPDU_number = op_prg_list_size(svlist_channel);
	
	for (lvi_MPDU_index_1 = 0; lvi_MPDU_index_1 < lvi_MPDU_number; lvi_MPDU_index_1++)
	{
		lvp_MPDU_1 = (MPDU_T *)op_prg_list_access(svlist_channel, lvi_MPDU_index_1);
		
		lvi_actual_receiver_number = lvp_MPDU_1->actual_receiver_number;
		/* calculate for each actual receiver of each MPDU */
		for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvi_actual_receiver_number; lvi_actual_receiver_index++)
		{
			lvi_actual_receiver_node_index = lvp_MPDU_1->actual_receiver[lvi_actual_receiver_index].actual_receiver_node_index;
		
			lvd_signal_power_linear = 0.0;
			lvd_interference_power_linear = 0.0;
			if (lvi_actual_receiver_node_index < gvi_HE_number)
			{
		    	lvd_noise_power_linear = CONST_HE_THERMAL_NOISE_LINEAR;
			}	
			else
			{
		    	lvd_noise_power_linear = CONST_CPE_THERMAL_NOISE_LINEAR;
			}	
			/* loop for all MPDUs */
			for (lvi_MPDU_index_2 = 0; lvi_MPDU_index_2 < lvi_MPDU_number; lvi_MPDU_index_2++)
			{
				lvp_MPDU_2 = (MPDU_T *)op_prg_list_access(svlist_channel, lvi_MPDU_index_2);
				/* signal */
				if (lvi_MPDU_index_1 == lvi_MPDU_index_2)
				{
			    	if (lvd_signal_power_linear > 0.0)
					{
						op_sim_end("Error: Duplicate signal MPDU!", "Error source module: PLC_CHANNEL", "Error source function: MPDU_sinr_segment_refresh()", "");
					}	
					else
					{
						lvd_signal_power_linear = MPDU_receive_power_calculate(lvi_actual_receiver_node_index, lvp_MPDU_2);
					}	
				}	
				/* otherwise, interference */
				else
				{
					lvd_interference_power_linear += MPDU_receive_power_calculate(lvi_actual_receiver_node_index, lvp_MPDU_2);
				}
			}
			
			lvd_sinr_linear = lvd_signal_power_linear / (lvd_interference_power_linear + lvd_noise_power_linear);
			lvp_segment_sinr = (SEGMENT_SINR_T *)op_prg_mem_alloc(sizeof(SEGMENT_SINR_T));
			lvp_segment_sinr->segment_sinr_dB = 10.0 * log10(lvd_sinr_linear);
			lvp_segment_sinr->segment_start_time = op_sim_time();
			op_prg_list_insert(lvp_MPDU_1->actual_receiver[lvi_actual_receiver_index].segment_sinr, lvp_segment_sinr, OPC_LISTPOS_TAIL);
		}
	}
	
	FOUT;
}


/************************************************************/
/* Author: Li Yong                                      	*/
/* Last Update: 2014.08.25                                  */
/* Remarks:                                             	*/
/************************************************************/
static void
MPDU_sinr_calculate(MPDU_T *lvp_MPDU)
{
	int lvi_sinr_segment_number;
	int lvi_sinr_segment_index;
	SEGMENT_SINR_T *lvp_segment_sinr;
	double lvd_sinr_dB;
	double lvd_segment_end_time;
	int lvi_actual_receiver_number;
	int lvi_actual_receiver_index;
	List* lvlist_segment_sinr;
	
	FIN(MPDU_sinr_calculate());
	
	lvi_actual_receiver_number = lvp_MPDU->actual_receiver_number;
	
	/* calculate for each actual receiver of each MPDU */
   for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvi_actual_receiver_number; lvi_actual_receiver_index++)
	{
		lvlist_segment_sinr = lvp_MPDU->actual_receiver[lvi_actual_receiver_index].segment_sinr;
		lvi_sinr_segment_number = op_prg_list_size(lvlist_segment_sinr);
		lvd_segment_end_time = lvp_MPDU->end_time;
		lvd_sinr_dB = 0.0;
		
		for (lvi_sinr_segment_index = lvi_sinr_segment_number - 1; lvi_sinr_segment_index >= 0; lvi_sinr_segment_index--)
		{
			lvp_segment_sinr = (SEGMENT_SINR_T *)op_prg_list_access(lvlist_segment_sinr, lvi_sinr_segment_index);
			lvd_sinr_dB += lvp_segment_sinr->segment_sinr_dB * (lvd_segment_end_time - lvp_segment_sinr->segment_start_time);    // sum of time-weighted dB value
			lvd_segment_end_time = lvp_segment_sinr->segment_start_time;
		}
	
		lvp_MPDU->actual_receiver[lvi_actual_receiver_index].MPDU_sinr_dB = lvd_sinr_dB/(lvp_MPDU->end_time - lvp_MPDU->start_time);
	
		/* write MPDU SINR statistics */
		op_stat_write(svgstat_SINR, lvp_MPDU->actual_receiver[lvi_actual_receiver_index].MPDU_sinr_dB);
		if (lvp_MPDU->actual_receiver[lvi_actual_receiver_index].actual_receiver_node_index < gvi_HE_number)
		{	
			op_stat_write(gvo_HE_property[lvp_MPDU->actual_receiver[lvi_actual_receiver_index].actual_receiver_node_index].PHY_SINR_stathandle, lvp_MPDU->actual_receiver[lvi_actual_receiver_index].MPDU_sinr_dB);
		}	
		else
		{	
			op_stat_write(gvo_CPE_property[lvp_MPDU->actual_receiver[lvi_actual_receiver_index].actual_receiver_node_index - gvi_HE_number].PHY_SINR_stathandle, lvp_MPDU->actual_receiver[lvi_actual_receiver_index].MPDU_sinr_dB);
		}	
	}
	
	FOUT;
}

/************************************************************/
/* Author: jiaying.lu                                      	*/
/* Last Update: 2014.11.16                                  */
/* Remarks:                                             	*/
/************************************************************/
static int
topology_init(FILE *fin)
{
	NODE_T *lvp_node;
	extern int HE_num, CPE_num, NOISE_num, X_num;
	int total_num;
	Prg_List *lvp_rlist;
	
	FIN(topology_init());
	printf("enter topology_init\n");
    
	/* read from file */
	total_num = 0;
	lvp_node = (NODE_T *)op_prg_mem_alloc((total_num+1)*sizeof(NODE_T));
	while (fscanf(fin, "%d %d %d %lf %d", &(lvp_node[total_num].node_id), &(lvp_node[total_num].type), &(lvp_node[total_num].parent_id), &(lvp_node[total_num].distance), &(lvp_node[total_num].phase)))
	{
		if (feof(fin))
		{
			printf("read file end!\n");
			break;
		}
		printf("%d: %d %d %d %lf %d\n", total_num, lvp_node[total_num].node_id, lvp_node[total_num].type, lvp_node[total_num].parent_id, lvp_node[total_num].distance, lvp_node[total_num].phase);
		/* calculate TYPE nums */
		if (lvp_node[total_num].type == TYPE_HE)
		{
			HE_num += 1;
		}
		else if (lvp_node[total_num].type == TYPE_CPE)
		{
			CPE_num += 1;
		}
		else if (lvp_node[total_num].type == TYPE_NOISE)
		{
	 		NOISE_num += 1;
		}
		else if (lvp_node[total_num].type == TYPE_X)
		{
			X_num += 1;
		}
		else
		{
	 		printf("error occurs in node[%d], it's type=%d\n", total_num, lvp_node[total_num].type);
		}
		
		total_num += 1;
		lvp_node = (NODE_T *)realloc(lvp_node, (total_num+1)*sizeof(NODE_T));
	}

	if (total_num != HE_num+CPE_num+NOISE_num+X_num)
	{
		char err_msg[20];
		sprintf(err_msg, "total_num: %d, sum: %d", total_num, HE_num+CPE_num+NOISE_num+X_num);
		op_sim_end("err: total_num != HE_num+CPE_num+NOISE_num+X_num", err_msg, "", "");
	}
	
	/* generate distance_phase_matrix */
	//* find common father node *//
	///* generate reverse list: leaf->parent->...->root *///
	lvp_rlist = (Prg_List *)op_prg_mem_alloc(total_num * sizeof(Prg_List));
	for (int i=0; i<total_num; i++)
	{
		lvp_rlist[i] = prg_list_create();
		prg_list_insert(lvp_rlist[i], &(lvp_node[i].node_id), PRGC_LISTPOS_TAIL);
		int *lvp_tmp_id = (int *)prg_list_access(lvp_rlist[i], PRGC_LISTPOS_TAIL);
		printf("rlist[i]: %d->", *lvp_tmp_id);
		while ( *lvp_tmp_id != 0)
		{
			prg_list_insert(lvp_rlist[i], &(lvp_node[*lvp_tmp_id].parent_id), PRGC_LISTPOS_TAIL);
			lvp_tmp_id = (int *)prg_list_access(lvp_rlist[i], PRGC_LISTPOS_TAIL);
			printf("%d->", *lvp_tmp_id);
		}
		printf("\n");
	}
	
	printf("leave topology_init\n");
	op_prg_mem_free(lvp_node);
	for (int i=0; i<total_num; i++)
	{
		op_prg_list_free(lvp_rlist[i]);
	}
	op_prg_mem_free(lvp_rlist);
	FRET(total_num);
}

/* End of Function Block */

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
	void channel (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_channel_init (int * init_block_ptr);
	void _op_channel_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_channel_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_channel_alloc (VosT_Obtype, int);
	void _op_channel_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
channel (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (channel ());

		{


		FSM_ENTER_NO_VARS ("channel")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (0, "init", "channel [init enter execs]")
				FSM_PROFILE_SECTION_IN ("channel [init enter execs]", state0_enter_exec)
				{
				//List *lptr;
				//int *temp_ptr;
				//char debug_str[50];
				int total_num;
				 
				FILE *lvp_fin;
				
				if ((lvp_fin=fopen("./network.txt", "r")) == NULL)
				{
					op_sim_end("err: Can not open file", "", "", "");
				}
				else
				{
					printf("lvp_fin != NULL\n");
					total_num = topology_init(lvp_fin);
					printf("total_num == %d\n", total_num);
				}
				
				fclose(lvp_fin);
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "channel [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_0", "channel [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "channel [idle enter execs]")
				FSM_PROFILE_SECTION_IN ("channel [idle enter execs]", state1_enter_exec)
				{
				op_sim_end("success~", "end!", "", "");
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"channel")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "channel [idle exit execs]")


			/** state (idle) transition processing **/
			FSM_TRANSIT_MISSING ("idle")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"channel")
		}
	}




void
_op_channel_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_channel_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_channel_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (op_sv_ptr);

	FOUT
	}


/* Undefine shortcuts to state variables to avoid */
/* syntax error in direct access to fields of */
/* local variable prs_ptr in _op_channel_svar function. */
#undef svlist_channel
#undef svgstat_SINR
#undef svgstat_active_MPDU_number

#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_channel_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_channel_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (channel)",
		sizeof (channel_state));
	*init_block_ptr = 0;

	FRET (obtype)
	}

VosT_Address
_op_channel_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	channel_state * ptr;
	FIN_MT (_op_channel_alloc (obtype))

	ptr = (channel_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "channel [init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_channel_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	channel_state		*prs_ptr;

	FIN_MT (_op_channel_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (channel_state *)gen_ptr;

	if (strcmp ("svlist_channel" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svlist_channel);
		FOUT
		}
	if (strcmp ("svgstat_SINR" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svgstat_SINR);
		FOUT
		}
	if (strcmp ("svgstat_active_MPDU_number" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svgstat_active_MPDU_number);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

