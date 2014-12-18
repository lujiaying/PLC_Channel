/* Process model C form file: channel.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char channel_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 5492DA4C 5492DA4C 1 lu-wspn lu 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                               ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include "PLC_def.h"
//#include "PLC_data.h"
#include "PLC_func.h"
#include "PLC_Channel.h"

#define TIME_TO_UPDATE			((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_TIME_TO_UPDATE))
#define PPDU_START				((op_intrpt_type() == OPC_INTRPT_REMOTE) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_START))
#define PPDU_TIME_END			((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_END))			

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
    double dis_X;
    double dis_Y;
    double dis_Z;
	double dis_total;
    PHASE_T pha_X;
    PHASE_T pha_Y;
    PHASE_T pha_Z;
} DISTANCE_PHASE_T;
/* end struct */


static double PPDU_receive_power_calculate(int, PPDU_T *);
static double period_noise_power_get(void);
static double phase_coupling_coefficient_get(int, int);
static double pulse_noise_get(int);
//static void PHY_medium_refresh(void);
static void PPDU_sinr_segment_refresh(void);
static void PPDU_sinr_calculate(PPDU_T *);
static DISTANCE_PHASE_T ** topology_init(FILE *);
static void propagation_attenuation_generate(DISTANCE_PHASE_T const **, double **, int);
static void impedance_correlation_generate(DISTANCE_PHASE_T const **, double **, int);
static void phase_coupling_parameter_generate(DISTANCE_PHASE_T const **, double **, int);
static void impedance_vector_init(double *, int, double, double);
static void impedance_vector_update(double *, int, double, double);

int gvi_HE_num = 0, gvi_CPE_num = 0, gvi_NOISE_num = 0, gvi_X_num = 0, gvi_total_num = 0;
Objid gvoid_channel;

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
	Prg_List *	             		svlist_channel                                  ;
	Stathandle	             		svgstat_SINR                                    ;
	Stathandle	             		svgstat_active_MPDU_number                      ;
	DISTANCE_PHASE_T **	    		svpp_distance_phase_matrix                      ;
	double **	              		svpp_propagation_attenuation_matrix             ;
	double **	              		svpp_impedance_correlation_matrix               ;
	double **	              		svpp_phase_coupling_parameter_matrix            ;
	double *	               		svp_impedance_vector                            ;
	Prg_List *	             		svlist_noise_ppdu                               ;
	int	                    		svi_PPDU_index                                  ;
	} channel_state;

#define svlist_channel          		op_sv_ptr->svlist_channel
#define svgstat_SINR            		op_sv_ptr->svgstat_SINR
#define svgstat_active_MPDU_number		op_sv_ptr->svgstat_active_MPDU_number
#define svpp_distance_phase_matrix		op_sv_ptr->svpp_distance_phase_matrix
#define svpp_propagation_attenuation_matrix		op_sv_ptr->svpp_propagation_attenuation_matrix
#define svpp_impedance_correlation_matrix		op_sv_ptr->svpp_impedance_correlation_matrix
#define svpp_phase_coupling_parameter_matrix		op_sv_ptr->svpp_phase_coupling_parameter_matrix
#define svp_impedance_vector    		op_sv_ptr->svp_impedance_vector
#define svlist_noise_ppdu       		op_sv_ptr->svlist_noise_ppdu
#define svi_PPDU_index          		op_sv_ptr->svi_PPDU_index

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
/* Author: Li Yong	                                     	 */
/* Last Update: 2014.08.25                                  */
/* Remarks:                                            	  	 */
/************************************************************/
static double
PPDU_receive_power_calculate(int lvi_receiver_node_index, PPDU_T *lvp_PPDU)
{
	double lvd_pathloss_linear;
	double lvd_matchmiss_linear;
	double lvd_impedance_ratio;
	double lvd_phase_coupling_coefficient;
	
	FIN(PPDU_receive_power_calculate());
	
	/* Power * pathloss * mis */	
	lvd_pathloss_linear = svpp_propagation_attenuation_matrix[lvp_PPDU->transmitter_node_index][lvi_receiver_node_index];
	
	lvd_impedance_ratio = svp_impedance_vector[lvi_receiver_node_index] / 75.0;
	lvd_matchmiss_linear = 4 * lvd_impedance_ratio / pow(1+lvd_impedance_ratio, 2);
	lvd_phase_coupling_coefficient = phase_coupling_coefficient_get(lvi_receiver_node_index, lvp_PPDU->transmitter_node_index);
	
	printf("[PPDU_receive_power_calculate] ppdu_index:%d, receiver_node:%d, power:%lf mw,\n", lvp_PPDU->PPDU_index, lvi_receiver_node_index, lvp_PPDU->power_linear);  
	printf("pathloss:%lf, mis:%lf, phase_coupling:%lf\n", lvd_pathloss_linear, lvd_matchmiss_linear, lvd_phase_coupling_coefficient);
    
	FRET(lvp_PPDU->power_linear*(1-lvd_pathloss_linear)*lvd_matchmiss_linear*lvd_phase_coupling_coefficient);
}


/************************************************************/
/* Author: jiaying.lu                                       */
/* Last Update: 2014.12.10                                  */
/* Remarks:        														    */
/************************************************************/
static double
period_noise_power_get()
{
	double lvd_attenu_dB;
	double lvd_period_time;
	
	FIN(period_noise_power_get);
	
	lvd_period_time = (int)floor(op_sim_time()*1000+0.5) % 10;
	lvd_attenu_dB = 10 * log10(exp(-(lvd_period_time/0.75)));
	
	FRET(-35.0+lvd_attenu_dB);
}


/************************************************************/
/* Author: jiaying.lu                                       */
/* Last Update: 2014.12.11                                  */
/* Remarks:         				                      			  */
/************************************************************/
static double
phase_coupling_coefficient_get(int receiver_node_index, int transmitter_node_index)
{
	Distribution *lvp_dist;
	double lvd_phase_coupling_coefficient;

	FIN(phase_coupling_coefficient_get);
	lvp_dist = op_dist_load("normal", svpp_phase_coupling_parameter_matrix[transmitter_node_index][receiver_node_index], 1);
	lvd_phase_coupling_coefficient = op_dist_outcome(lvp_dist);
	op_dist_unload(lvp_dist);
	
	if (lvd_phase_coupling_coefficient < 0)
	{
		lvd_phase_coupling_coefficient = 0;
	}
	
	FRET(lvd_phase_coupling_coefficient);
}


/************************************************************/
/* Author: jiaying.lu                                       */
/* Last Update: 2014.12.11                                  */
/* Remarks:         				                      	*/
/************************************************************/
static double
pulse_noise_get(int receiver_node_index)
{
	int lvi_noise_index;
	double pulse_noise_power_linear;
	double lvd_pathloss_linear;
	PPDU_T *lvp_noise_PPDU;

	FIN(pulse_noise_get);
	
	pulse_noise_power_linear = 0;
	for (lvi_noise_index=0; lvi_noise_index<gvi_NOISE_num; lvi_noise_index++)
	{
		lvp_noise_PPDU =(PPDU_T *)prg_list_access(svlist_noise_ppdu, lvi_noise_index);
		lvd_pathloss_linear = svpp_propagation_attenuation_matrix[lvp_noise_PPDU->transmitter_node_index][receiver_node_index];
		pulse_noise_power_linear += lvp_noise_PPDU->power_linear * lvd_pathloss_linear * phase_coupling_coefficient_get(receiver_node_index, lvp_noise_PPDU->transmitter_node_index);
	}
	
	FRET(pulse_noise_power_linear);
}


/************************************************************/
/* Author: Li Yong	                                     	*/
/* Last Update: 2014.08.25                                  */
/* Remarks:                                             	*/
/************************************************************/
/**static void
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
}	**/


/************************************************************/
/* Author: Li Yong                                      	*/
/* Last Update: 2014.08.25                                  */
/* Remarks:                                             	*/
/************************************************************/
static void
PPDU_sinr_segment_refresh()
{
	int lvi_PPDU_number;
	int lvi_PPDU_index_1;
	int lvi_PPDU_index_2;
	PPDU_T *lvp_PPDU_1;
	PPDU_T *lvp_PPDU_2;
	double lvd_signal_power_linear;
	double lvd_interference_power_linear;
	double lvd_period_noise_power_dB, lvd_background_noise_power_dB;
	double lvd_sinr_linear;
	SEGMENT_SINR_T *lvp_segment_sinr;
	int lvi_actual_receiver_number;
	int lvi_actual_receiver_index;
	int lvi_actual_receiver_node_index;
	
	FIN(PPDU_sinr_segment_refresh());
	
	lvd_period_noise_power_dB = period_noise_power_get();
	lvd_background_noise_power_dB = -65.0;     // idle=-65dBm, busy=-85dBm
	
	lvi_PPDU_number = op_prg_list_size(svlist_channel);
	
	for (lvi_PPDU_index_1 = 0; lvi_PPDU_index_1 < lvi_PPDU_number; lvi_PPDU_index_1++)
	{
		lvp_PPDU_1 = (PPDU_T *)op_prg_list_access(svlist_channel, lvi_PPDU_index_1);
		
		lvi_actual_receiver_number = lvp_PPDU_1->actual_receiver_number;
		/* calculate for each actual receiver of each PPDU */
		for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvi_actual_receiver_number; lvi_actual_receiver_index++)
		{
			lvi_actual_receiver_node_index = lvp_PPDU_1->actual_receiver_array[lvi_actual_receiver_index].actual_receiver_node_index;
		
			lvd_signal_power_linear = 0.0;        
			lvd_interference_power_linear = 0.0;	
			/* loop for all PPDUs */
			for (lvi_PPDU_index_2 = 0; lvi_PPDU_index_2 < lvi_PPDU_number; lvi_PPDU_index_2++)
			{
				lvp_PPDU_2 = (PPDU_T *)op_prg_list_access(svlist_channel, lvi_PPDU_index_2);
				/* signal */
				if (lvi_PPDU_index_1 == lvi_PPDU_index_2)
				{
			    	if (lvd_signal_power_linear > 0.0)
					{
						op_sim_end("Error: Duplicate signal PPDU!", "Error source module: PLC_CHANNEL", "Error source function: PPDU_sinr_segment_refresh()", "");
					}	
					else
					{
						lvd_signal_power_linear = PPDU_receive_power_calculate(lvi_actual_receiver_node_index, lvp_PPDU_2);
					}	
				}	
				/* otherwise, interference */
				else
				{
					lvd_interference_power_linear += PPDU_receive_power_calculate(lvi_actual_receiver_node_index, lvp_PPDU_2);
				}
			}
			
			//printf("[PPDU_sinr_segment_refresh] signal_power:%lf, interference_power:%lf, pulse_noise_power:%lf, period_noise_power:%lf, background_noise_power:%lf\n", lvd_signal_power_linear, lvd_interference_power_linear, pulse_noise_get(lvi_actual_receiver_node_index), pow(10, lvd_period_noise_power_dB/10.0), pow(10, lvd_background_noise_power_dB/10.0));
			lvd_sinr_linear = lvd_signal_power_linear / (lvd_interference_power_linear + pulse_noise_get(lvi_actual_receiver_node_index) + pow(10, lvd_period_noise_power_dB/10.0) + pow(10, lvd_background_noise_power_dB/10.0));
			lvp_segment_sinr = (SEGMENT_SINR_T *)op_prg_mem_alloc(sizeof(SEGMENT_SINR_T));
			lvp_segment_sinr->segment_sinr_dB = 10.0 *log10(lvd_sinr_linear);
			lvp_segment_sinr->segment_start_time = op_sim_time();
			printf("!!!!!! [PPDU_sinr_segment_refresh] receiver_index:%d, lvd_sinr_linear:%lf, segmeng_sinr_dB:%lf\n", lvp_PPDU_1->actual_receiver_array[lvi_actual_receiver_index].actual_receiver_node_index, lvd_sinr_linear, lvp_segment_sinr->segment_sinr_dB);
			op_prg_list_insert(lvp_PPDU_1->actual_receiver_array[lvi_actual_receiver_index].segment_sinr, lvp_segment_sinr, OPC_LISTPOS_TAIL);
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
PPDU_sinr_calculate(PPDU_T *lvp_PPDU)
{
	int lvi_sinr_segment_number;
	int lvi_sinr_segment_index;
	SEGMENT_SINR_T *lvp_segment_sinr;
	double lvd_sinr_linear;
	double lvd_segment_end_time;
	int lvi_actual_receiver_number;
	int lvi_actual_receiver_index;
	List* lvlist_segment_sinr;
	
	FIN(MPDU_sinr_calculate());
	
	lvi_actual_receiver_number = lvp_PPDU->actual_receiver_number;
	
	/* calculate for each actual receiver of each MPDU */
	for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvi_actual_receiver_number; lvi_actual_receiver_index++)
	{
		lvlist_segment_sinr = lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].segment_sinr;
		lvi_sinr_segment_number = op_prg_list_size(lvlist_segment_sinr);
		lvd_segment_end_time = lvp_PPDU->end_time;
		lvd_sinr_linear = 0.0;
		
		for (lvi_sinr_segment_index = lvi_sinr_segment_number - 1; lvi_sinr_segment_index >= 0; lvi_sinr_segment_index--)
		{
			lvp_segment_sinr = (SEGMENT_SINR_T *)op_prg_list_access(lvlist_segment_sinr, lvi_sinr_segment_index);
			lvd_sinr_linear += lvp_segment_sinr->segment_sinr_dB * (lvd_segment_end_time - lvp_segment_sinr->segment_start_time);    // sum of time-weighted dB value
			lvd_segment_end_time = lvp_segment_sinr->segment_start_time;
		}
	
		lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].PPDU_sinr_dB = lvd_sinr_linear/(lvp_PPDU->end_time - lvp_PPDU->start_time);
	
		/* write MPDU SINR statistics */
		/**
		op_stat_write(svgstat_SINR, lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].PPDU_sinr_dB);
		if (lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].actual_receiver_node_index < gvi_HE_number)
		{	
			op_stat_write(gvo_HE_property[lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].actual_receiver_node_index].PHY_SINR_stathandle, lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].PPDU_sinr_dB);
		}	
		else
		{	
			op_stat_write(gvo_CPE_property[lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].actual_receiver_node_index - gvi_HE_number].PHY_SINR_stathandle, lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].PPDU_sinr_dB);
		}	**/
	}
	
	FOUT;
}

/************************************************************/
/* Author: jiaying.lu                                      	*/
/* Last Update: 2014.11.26                                  */
/* Remarks:                                             	*/
/************************************************************/
static DISTANCE_PHASE_T **
topology_init(FILE *fin)
{
	/**	read file from fin, init topology and generate DISTANCE PHASE matrix.
	//  
	//	 Args:
	//		 FILE *fin: file with 6 columns, column type is int.
	//  Return:
	//  	 DISTANCE_PHASE_T **: a matrix, length == gvi_total_num.    
	**/

	NODE_T *lvp_node;
	Prg_List **lvpp_rlist;    //reverse list
	int lvi_len_sub;
	int lvi_index_i, lvi_index_j;
	int lvi_index_common_i, lvi_index_common_j;
	int lvi_index_temp_i, lvi_index_temp_j;
	int *lvp_tmp_id;
	char lvc_msg[100];
	
	FIN(topology_init());
	printf("enter topology_init\n");
	gvi_total_num = 0;
    
	/* read from file */
	lvp_node = (NODE_T *)op_prg_mem_alloc((gvi_total_num+1)*sizeof(NODE_T));
	while (fscanf(fin, "%d %d %d %lf %d", &(lvp_node[gvi_total_num].node_id), &(lvp_node[gvi_total_num].type), &(lvp_node[gvi_total_num].parent_id), &(lvp_node[gvi_total_num].distance), &(lvp_node[gvi_total_num].phase)))
	{
		if (feof(fin))
		{
			printf("read file end!\n");
			break;
		}
		printf("%d: %d %d %d %lf %d\n", gvi_total_num, lvp_node[gvi_total_num].node_id, lvp_node[gvi_total_num].type, lvp_node[gvi_total_num].parent_id, lvp_node[gvi_total_num].distance, lvp_node[gvi_total_num].phase);
		/* calculate TYPE nums */
		if (lvp_node[gvi_total_num].type == TYPE_HE)
		{
			gvi_HE_num += 1;
		}
		else if (lvp_node[gvi_total_num].type == TYPE_CPE)
		{
			gvi_CPE_num += 1;
		}
		else if (lvp_node[gvi_total_num].type == TYPE_NOISE)
		{
	 		gvi_NOISE_num += 1;
		}
		else if (lvp_node[gvi_total_num].type == TYPE_X)
		{
			gvi_X_num += 1;
		}
		else
		{
	 		printf("error occurs in node[%d], it's type=%d\n", gvi_total_num, lvp_node[gvi_total_num].type);
		}
		
		gvi_total_num += 1;
		lvp_node = (NODE_T *)op_prg_mem_realloc(lvp_node, (gvi_total_num+1)*sizeof(NODE_T));
	}

	if (gvi_total_num != gvi_HE_num+gvi_CPE_num+gvi_NOISE_num+gvi_X_num)
	{

		sprintf(lvc_msg, "gvi_total_num: %d, sum: %d", gvi_total_num, gvi_HE_num+gvi_CPE_num+gvi_NOISE_num+gvi_X_num);
		op_sim_end("err: gvi_total_num != gvi_HE_num+gvi_CPE_num+gvi_NOISE_num+gvi_X_num", lvc_msg, "", "");
	}
	else
	{
		sprintf(lvc_msg, "gvi_total_num:%d, gvi_HE_num:%d, gvi_CPE_num:%d, gvi_NOISE_num:%d, gvi_X_num:%d", gvi_total_num, gvi_HE_num, gvi_CPE_num, gvi_NOISE_num, gvi_X_num);
		op_sim_message("read file success", lvc_msg);
	}
	
	/* generate distance_phase_matrix */
	//* find common father node *//
	///* generate reverse list: leaf->parent->...->root *///
	lvpp_rlist = (Prg_List **)op_prg_mem_alloc(gvi_total_num * sizeof(Prg_List *));
	for (lvi_index_i=0; lvi_index_i<gvi_total_num; lvi_index_i++)
	{
		lvpp_rlist[lvi_index_i] = prg_list_create();
		prg_list_insert(lvpp_rlist[lvi_index_i], &(lvp_node[lvi_index_i].node_id), PRGC_LISTPOS_TAIL);
		lvp_tmp_id = (int *)prg_list_access(lvpp_rlist[lvi_index_i], PRGC_LISTPOS_TAIL);
		printf("rlist[%d]: %d", lvi_index_i, *lvp_tmp_id);
		while (*lvp_tmp_id != 0)
		{
			prg_list_insert(lvpp_rlist[lvi_index_i], &(lvp_node[*lvp_tmp_id].parent_id), PRGC_LISTPOS_TAIL);
			lvp_tmp_id = (int *)prg_list_access(lvpp_rlist[lvi_index_i], PRGC_LISTPOS_TAIL);
			printf("->%d", *lvp_tmp_id);
		}
		printf(" (len:%d)\n", prg_list_size(lvpp_rlist[lvi_index_i]));
	}
	
	svpp_distance_phase_matrix = (DISTANCE_PHASE_T **)op_prg_mem_alloc(gvi_total_num * sizeof(DISTANCE_PHASE_T **));
	// init must success before use!!!
	for (lvi_index_i=0; lvi_index_i<gvi_HE_num+gvi_CPE_num+gvi_NOISE_num; lvi_index_i++)
	{
		svpp_distance_phase_matrix[lvi_index_i] = (DISTANCE_PHASE_T *)op_prg_mem_alloc(gvi_total_num * sizeof(DISTANCE_PHASE_T));
	}

	for (lvi_index_i=0; lvi_index_i<gvi_HE_num+gvi_CPE_num+gvi_NOISE_num; lvi_index_i++)
	{
		
		for (lvi_index_j=0; lvi_index_j<gvi_HE_num+gvi_CPE_num+gvi_NOISE_num; lvi_index_j++)
		{
			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_X = 0;
			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Y = 0;
			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Z = 0;
			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_total = 0;
			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_X = PHASE_ABC;
			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_Y = PHASE_ABC;
			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_Z = PHASE_ABC;
			/* find nearest common node */
			if (lvi_index_i != lvi_index_j)
			{
				lvi_index_common_i = 0;
				lvi_index_common_j = 0;
				lvi_len_sub = prg_list_size(lvpp_rlist[lvi_index_i+1]) - prg_list_size(lvpp_rlist[lvi_index_j+1]);
				if (lvi_len_sub > 0)
				{
					lvi_index_common_i = lvi_len_sub;
				}
				else 
				{
					lvi_index_common_j = -lvi_len_sub;
				}
				while (*((int *)prg_list_access(lvpp_rlist[lvi_index_i+1], lvi_index_common_i)) != *((int *)prg_list_access(lvpp_rlist[lvi_index_j+1], lvi_index_common_j)))
				{
					lvi_index_common_i += 1;
					lvi_index_common_j += 1;
				}
				/* generate distance phase matrix */
				printf("[%d]->[%d](lvi_index_common_i=%d, lvi_index_common_j=%d) ", lvi_index_i, lvi_index_j, lvi_index_common_i, lvi_index_common_j);
				for (lvi_index_temp_i=0; lvi_index_temp_i<lvi_index_common_i; lvi_index_temp_i++)
				{
					if (lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_i+1], lvi_index_temp_i)].phase != PHASE_ABC)
					{
						svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_X += lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_i+1], lvi_index_temp_i)].distance;
						svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_X = lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_i+1], lvi_index_temp_i)].phase;
					}
					else
					{
						svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Y += lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_i+1], lvi_index_temp_i)].distance;
					}
				}
				for (lvi_index_temp_j=0; lvi_index_temp_j<lvi_index_common_j; lvi_index_temp_j++)
				{
					if (lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_j+1], lvi_index_temp_j)].phase != PHASE_ABC)
					{
						svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Z += lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_j+1], lvi_index_temp_j)].distance;
						svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_Z = lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_j+1], lvi_index_temp_j)].phase;
					}
					else
					{
						svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Y += lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_j+1], lvi_index_temp_j)].distance;
					}
				}
				svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_total = svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_X + svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Y + svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Z;
				printf("pha_X=%d, pha_Y=%d, pha_Z=%d, dis_X=%lf, dis_Y=%lf, dis_Z=%lf, dis_total=%lf\n", svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_X, svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_Y, svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_Z, svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_X, svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Y, svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Z, svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_total);
			}
		}
	}
	
	op_prg_mem_free(lvp_node);
	for (lvi_index_i=0; lvi_index_i<gvi_HE_num+gvi_CPE_num+gvi_NOISE_num; lvi_index_i++)
	{
		op_prg_list_free(lvpp_rlist[lvi_index_i]);
	}
	op_prg_mem_free(lvpp_rlist);
	printf("leave topology_init...\n");
	FRET(svpp_distance_phase_matrix);
}


/************************************************************/
/* Author: jiaying.lu                                       */
/* Last Update: 2014.11.24                                  */
/* Remarks:  propagation_attenuation_matrix[0] -> node[1]   */
/************************************************************/
static void
propagation_attenuation_generate(DISTANCE_PHASE_T const **distance_phase_matrix, double **propagation_attenuation_matrix, int attenu_num)
{
	double a0, a1, k, f;
	int lvi_index_i, lvi_index_j;
	
	FIN(propagation_attenuation_generate());
	printf("enter propagation_attenuation_generate, param [attenu_num=%d].\n", attenu_num);
	
	//formula: A(f,d) = e^(-(a0 + a1*f^k)*d)
	a0 = 0;
	a1 = 8.75 * pow(10, -10);
	k = 1;
	f = 5 * pow(10, 6);
	
	
	for (lvi_index_i=0; lvi_index_i<attenu_num; lvi_index_i++)
	{
		for (lvi_index_j=0; lvi_index_j<attenu_num; lvi_index_j++)
		{
			if (lvi_index_i == lvi_index_j)
			{
				propagation_attenuation_matrix[lvi_index_i][lvi_index_j] = 0;
			}
			else
			{
				propagation_attenuation_matrix[lvi_index_i][lvi_index_j] = exp(-(a0 + a1*pow(f, k))*distance_phase_matrix[lvi_index_i][lvi_index_j].dis_total);
			}
			printf("propagation_attenuation_matrix[%d][%d] =%0.4lf\n", lvi_index_i, lvi_index_j, propagation_attenuation_matrix[lvi_index_i][lvi_index_j]);
		}
	}
	
	printf("leave propagation_attenuation_generate...\n");
	FOUT;
}


/************************************************************/
/* Author: jiaying.lu                                       */
/* Last Update: 2014.11.25                                  */
/* Remarks:                                             	 */
/************************************************************/
static void
impedance_correlation_generate(const DISTANCE_PHASE_T **distance_phase_matrix, double **impedance_correlation_matrix, int impedance_correlation_num)
{
	int a;
	int lvi_index_i, lvi_index_j;
	double lvd_row_sum;
	
	FIN(impedance_correlation_generate());
	printf("enter impedance_correlation_generate, param [impedance_correlation_num=%d].\n", impedance_correlation_num);
	
	//formula: c = e^(-d/a)
	a = 15;
	
	for (lvi_index_i=0; lvi_index_i<impedance_correlation_num; lvi_index_i++)
	{
		lvd_row_sum = 0;
		for (lvi_index_j=0; lvi_index_j<impedance_correlation_num; lvi_index_j++)
		{
			if (lvi_index_i == lvi_index_j)
			{
				impedance_correlation_matrix[lvi_index_i][lvi_index_j] = 0;
			}
			else
			{
				impedance_correlation_matrix[lvi_index_i][lvi_index_j] = exp(-distance_phase_matrix[lvi_index_i][lvi_index_j].dis_total / a);
			}
			lvd_row_sum += impedance_correlation_matrix[lvi_index_i][lvi_index_j];
		}
		
		// row normalization
		for (lvi_index_j=0; lvi_index_j<impedance_correlation_num; lvi_index_j++)
		{
			impedance_correlation_matrix[lvi_index_i][lvi_index_j] /= lvd_row_sum;
			printf("impedance_correlation_matrix[%d][%d] =%0.4lf\n", lvi_index_i, lvi_index_j, impedance_correlation_matrix[lvi_index_i][lvi_index_j]);
		}
	}
	
	printf("leave impedance_correlation_generate...\n");
	FOUT;
}


/************************************************************/
/* Author: jiaying.lu                                       */
/* Last Update: 2014.11.27                                  */
/* Remarks:                                             	 */
/************************************************************/
static void
phase_coupling_parameter_generate(const DISTANCE_PHASE_T **distance_phase_matrix, double **phase_coupling_parameter_matrix, int phase_coupling_num)
{
	double k;
	int lvi_index_i, lvi_index_j;
	
	FIN(phase_coupling_parameter_generate);
	printf("enter phase_coupling_parameter_generate\n");
	
	//formula: c = k*dis_Y / (dis_x + dis_Y + dis_Z)
	k = 0.5;	
	
	for (lvi_index_i=0; lvi_index_i<phase_coupling_num; lvi_index_i++)
	{
		for (lvi_index_j=0; lvi_index_j<phase_coupling_num; lvi_index_j++)
		{
			if (lvi_index_i == lvi_index_j)
			{
				phase_coupling_parameter_matrix[lvi_index_i][lvi_index_j] = 1;
			}
			else
			{
				phase_coupling_parameter_matrix[lvi_index_i][lvi_index_j] = k*distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Y / distance_phase_matrix[lvi_index_i][lvi_index_j].dis_total;
			}
			printf("phase_coupling_parameter_matrix[%d][%d]=%lf\n", lvi_index_i, lvi_index_j, phase_coupling_parameter_matrix[lvi_index_i][lvi_index_j]);
		}
	}
	
	printf("leave phase_coupling_parameter_generate...\n");
	FOUT;
}

/************************************************************/
/* Author: jiaying.lu                                       */
/* Last Update: 2014.11.27                                  */
/* Remarks:                                             	 */
/************************************************************/
static void
impedance_vector_init(double *impedance_vector, int impedance_num, double mean, double std_deviation)
{
	int lvi_index_i;
	Distribution *lvp_normal_dist = op_dist_load("normal", mean, std_deviation*std_deviation);
	
	FIN(impedance_vector_init());
	//printf("enter impedance_vector_init\n");
	
	for (lvi_index_i=0; lvi_index_i<impedance_num; lvi_index_i++)
	{
		impedance_vector[lvi_index_i] = op_dist_outcome(lvp_normal_dist);
		//printf("impedance_vector[%d] = %lf\n", lvi_index_i, impedance_vector[lvi_index_i]);
	}
	
	op_dist_unload(lvp_normal_dist);
	//printf("leave impedance_vector_init...\n");
	FOUT;
}


/************************************************************/
/* Author: jiaying.lu                                       */
/* Last Update: 2014.11.27                                  */
/* Remarks:                                                 */
/************************************************************/
static void
impedance_vector_update(double *impedance_vector, int impedance_num, double mean, double std_deviation)
{
	int lvi_index_i;
	double a, b;
	Distribution *lvp_normal_dist = op_dist_load("normal", 0, std_deviation*std_deviation);
	
	FIN(impedance_vector_update());
	//printf("enter impedance_vector_update\n");
	
	// formula: y(t) = a*x(t-1) + sqrt(1-a^2)*x(t)
	a = 0.75;
	b = sqrt(1 - a*a);
	
	for (lvi_index_i=0; lvi_index_i<impedance_num; lvi_index_i++)
	{
		impedance_vector[lvi_index_i] = mean + a*(impedance_vector[lvi_index_i]-mean) + b*op_dist_outcome(lvp_normal_dist);
		//printf("impedance_vector[%d] = %lf\n", lvi_index_i, impedance_vector[lvi_index_i]);
	}
	
	//printf("leave impedance_vector_update...\n");
	FOUT;
}


/************************************************************/
/* Author: jiaying.lu                                       */
/* Last Update: 2014.11.27                                  */
/* Remarks: Used when calculate SINR                        */
/*          Free impedance_space_vector after calculate.    */
/************************************************************/
static void
impedance_space_relate_generate(double const *impedance_vector, const double **impedance_correlation_matrix, double *impedance_space_vector, int impedance_num)
{
	FIN(impedance_space_relate);
	
	//consider matrix R^(1/2)
	
	FOUT;
}


/************************************************************/
/* Author: jiaying.lu                                       */
/* Last Update: 2014.11.27                                  */
/* Remarks: Used when calculate SINR                        */
/*          Free phase_coupling_coefficient after calculate.*/
/************************************************************/
static void
phase_coupling_coefficient_generate(const double **phase_coupling_parameter_matrix, double **phase_coupling_coefficient_matrix, int phase_coupling_num, double std_deviation)
{
	FIN(phase_coupling_coefficient_generate);
	
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


		FSM_ENTER ("channel")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (0, "init", "channel [init enter execs]")
				FSM_PROFILE_SECTION_IN ("channel [init enter execs]", state0_enter_exec)
				{
				FILE *lvp_fin;
				int lvi_index_i;
				double lvd_mean, lvd_std_deviation;
				
				/* init state varible*/
				svi_PPDU_index = 0;
				printf("enter CHANNEL.init...\n");
				/** //show current dir
				printf("file=%s,func=%s,line=%d\n",__FILE__,__FUNCTION__,__LINE__);
				system("dir");
				**/
				
				
				/* step 1: topology init. */
				// global varieble HE_num, CPE_num, NOISE_num, X_num and total_num init.
				if ((lvp_fin=fopen("./network.txt", "r")) == NULL)
				{
					op_sim_end("err: Can not open file", "", "", "");
				}
				else
				{
					printf("lvp_fin != NULL\n");
					svpp_distance_phase_matrix = topology_init(lvp_fin);
					printf("total_num == %d\n", gvi_total_num);
				}
				fclose(lvp_fin);
				
				
				/* step 2: generate propagation_attenuation_matrix */
				// malloc memory for propagation_attenuation_matrix.
				svpp_propagation_attenuation_matrix = (double **)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num+gvi_NOISE_num) * sizeof(double *));
				for (lvi_index_i=0; lvi_index_i<(gvi_HE_num+gvi_CPE_num+gvi_NOISE_num); lvi_index_i++)
				{
					svpp_propagation_attenuation_matrix[lvi_index_i] = (double *)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num+gvi_NOISE_num) * sizeof(double));
				}
				propagation_attenuation_generate(svpp_distance_phase_matrix, svpp_propagation_attenuation_matrix, gvi_HE_num+gvi_CPE_num+gvi_NOISE_num);
				
				
				/* step 3: generate impedance_correlation_matrix */
				// malloc memory for impedance_correlation_matrix.
				svpp_impedance_correlation_matrix = (double **)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num) * sizeof(double *));
				for (lvi_index_i=0; lvi_index_i<(gvi_HE_num+gvi_CPE_num); lvi_index_i++)
				{
					svpp_impedance_correlation_matrix[lvi_index_i] = (double *)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num) * sizeof(double));
				}
				impedance_correlation_generate(svpp_distance_phase_matrix, svpp_impedance_correlation_matrix, gvi_HE_num+gvi_CPE_num);
				
				
				/* step 4: generate phase_coupling_parameter_matrix */
				// malloc memory for phase_coupling_parameter_matrix.
				svpp_phase_coupling_parameter_matrix = (double **)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num+gvi_NOISE_num) * sizeof(double *));
				for (lvi_index_i=0; lvi_index_i<(gvi_HE_num+gvi_CPE_num+gvi_NOISE_num); lvi_index_i++)
				{
					svpp_phase_coupling_parameter_matrix[lvi_index_i] = (double *)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num+gvi_NOISE_num) * sizeof(double));
				}
				phase_coupling_parameter_generate(svpp_distance_phase_matrix, svpp_phase_coupling_parameter_matrix, gvi_HE_num+gvi_CPE_num+gvi_NOISE_num);
				
				
				/* step 5: generate impedance_vector */
				// malloc memory for impedance_vector.
				svp_impedance_vector = (double *)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num) * sizeof(double));
				lvd_mean = 90;
				lvd_std_deviation = 4.5;
				impedance_vector_init(svp_impedance_vector, gvi_HE_num+gvi_CPE_num, lvd_mean, lvd_std_deviation);
				
				
				/* init channel PPDU list */
				svlist_channel = prg_list_create();
				svlist_noise_ppdu = prg_list_create();
				
				/* set self intrpt, update impedance_vector*/
				printf("init end. And current sim time: %ld\n", op_sim_time());
				op_intrpt_schedule_self(op_sim_time()+10, INTRPT_CHANNEL_TIME_TO_UPDATE);
				
				
				/* init global objid */
				gvoid_channel = op_id_self();
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "channel [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (5, state5_enter_exec, ;, "default", "", "init", "send_channel_init", "tr_16", "channel [init -> send_channel_init : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "channel [idle enter execs]")
				FSM_PROFILE_SECTION_IN ("channel [idle enter execs]", state1_enter_exec)
				{
				printf("enter CHANNEL.idle..\n");
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"channel")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "channel [idle exit execs]")
				FSM_PROFILE_SECTION_IN ("channel [idle exit execs]", state1_exit_exec)
				{
				char lvc_err_msg[25];
				
				printf("Leave channel.idle ");
				
				if (TIME_TO_UPDATE)
				{
					printf("receive SELF INTRPT: TIME_TO_UPDATE.\n");
				}
				else if (PPDU_START)
				{
					printf("receive REMOTE INTRPT: PPDU_START.\n");
				}
				else if (PPDU_TIME_END)
				{
					printf("receive SELF INTRPT: PPDU_END.\n");
				}
				else if (INTRPT_CHANNEL_INITED)
				{
					printf("receive MCAST INTRPT: CHANNEL_INITED. Stay in idle.\n");
				}
				else
				{
					sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());
					op_sim_end("Error: Unexpected INTRPT is received at \"channel.idle.Exit\" state!", "Error source module: CHANNEL", lvc_err_msg, "");
				}
				}
				FSM_PROFILE_SECTION_OUT (state1_exit_exec)


			/** state (idle) transition processing **/
			FSM_PROFILE_SECTION_IN ("channel [idle trans conditions]", state1_trans_conds)
			FSM_INIT_COND (TIME_TO_UPDATE)
			FSM_TEST_COND (PPDU_START)
			FSM_TEST_COND (PPDU_TIME_END)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "TIME_TO_UPDATE", "", "idle", "update", "tr_6", "channel [idle -> update : TIME_TO_UPDATE / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "PPDU_START", "", "idle", "receive_PPDU", "tr_8", "channel [idle -> receive_PPDU : PPDU_START / ]")
				FSM_CASE_TRANSIT (2, 4, state4_enter_exec, ;, "PPDU_TIME_END", "", "idle", "send_PPDU", "tr_10", "channel [idle -> send_PPDU : PPDU_TIME_END / ]")
				FSM_CASE_TRANSIT (3, 1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_12", "channel [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (update) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "update", state2_enter_exec, "channel [update enter execs]")
				FSM_PROFILE_SECTION_IN ("channel [update enter execs]", state2_enter_exec)
				{
				double lvd_mean, lvd_std_deviation;
				
				/* update impedance_vector */
				lvd_mean = 90;
				lvd_std_deviation = 4.5;
				impedance_vector_update(svp_impedance_vector, gvi_HE_num+gvi_CPE_num, lvd_mean, lvd_std_deviation);
				op_intrpt_schedule_self(op_sim_time()+10, INTRPT_CHANNEL_TIME_TO_UPDATE);
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (update) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "update", "channel [update exit execs]")


			/** state (update) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "update", "idle", "tr_7", "channel [update -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (receive_PPDU) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "receive_PPDU", state3_enter_exec, "channel [receive_PPDU enter execs]")
				FSM_PROFILE_SECTION_IN ("channel [receive_PPDU enter execs]", state3_enter_exec)
				{
				Ici *lvp_ici;
				PPDU_T *lvp_PPDU;
				int lvi_actual_receiver_index;
				
				/* receive PPDU */
				lvp_ici = op_intrpt_ici();
				op_ici_attr_get(lvp_ici, "PPDU_ptr", &lvp_PPDU);
				op_ici_destroy(lvp_ici);
				lvp_ici = OPC_NIL;
				printf("[channel.receive_PPDU] type=%d, start_time=%lf, end_time=%lf\n", lvp_PPDU->type, lvp_PPDU->start_time, lvp_PPDU->end_time);
				
				if (lvp_PPDU->type < 3)
				{
					/* insert PPDU to channel list */
					prg_list_insert(svlist_channel, lvp_PPDU, PRGC_LISTPOS_TAIL);
					
					/* update PPDU attribute */
					lvp_PPDU->PPDU_index = svi_PPDU_index++;
					lvp_PPDU->actual_receiver_number = 1;    // for easy, consider num = 1
					lvp_PPDU->actual_receiver_array = (PPDU_SINR_T *)op_prg_mem_alloc(lvp_PPDU->actual_receiver_number * sizeof(PPDU_SINR_T));
					for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvp_PPDU->actual_receiver_number; lvi_actual_receiver_index++)
					{
						lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].actual_receiver_node_index = lvp_PPDU->receiver_node_index;  //TODO
						lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].segment_sinr = prg_list_create();
					}
					
					
					/* set self intrpt at end time*/
					lvp_ici = op_ici_create("PPDU");
					op_ici_attr_set(lvp_ici, "PPDU_index", &(lvp_PPDU->PPDU_index));
					op_ici_install(lvp_ici);
					op_intrpt_schedule_self(lvp_PPDU->end_time, INTRPT_CHANNEL_PPDU_END);
					op_ici_install(OPC_NIL);
					
					/* PPDU segment refresh*/
					PPDU_sinr_segment_refresh();
				}
				else
				{
					/* insert PPDU to channel list */
					printf("get a noise PPDU from noise_%d\n", lvp_PPDU->PPDU_index);
					prg_list_insert(svlist_noise_ppdu, lvp_PPDU, PRGC_LISTPOS_TAIL);	
				}
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (receive_PPDU) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "receive_PPDU", "channel [receive_PPDU exit execs]")


			/** state (receive_PPDU) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "receive_PPDU", "idle", "tr_9", "channel [receive_PPDU -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (send_PPDU) enter executives **/
			FSM_STATE_ENTER_FORCED (4, "send_PPDU", state4_enter_exec, "channel [send_PPDU enter execs]")
				FSM_PROFILE_SECTION_IN ("channel [send_PPDU enter execs]", state4_enter_exec)
				{
				Ici *lvp_ici;
				int *lvp_PPDU_index;
				int lvi_PPDU_list_index, lvi_PPDU_list_num;
				PPDU_T *lvp_PPDU;
				int lvi_actual_receiver_index;
				
				
				/* find PPDU related to the current interrupt */
				lvp_ici = op_intrpt_ici();
				op_ici_attr_get(lvp_ici, "PPDU_index", &lvp_PPDU_index);
				op_ici_destroy(lvp_ici);
				lvp_ici = OPC_NIL;
				
				lvi_PPDU_list_num = prg_list_size(svlist_channel);
				for (lvi_PPDU_list_index=0; lvi_PPDU_list_index<lvi_PPDU_list_num; lvi_PPDU_list_index++)
				{
					lvp_PPDU =(PPDU_T *)prg_list_access(svlist_channel, lvi_PPDU_list_index);
					if (lvp_PPDU->PPDU_index == *lvp_PPDU_index)
					{
						break;
					}
				}
				
				
				/* remove PPDU from list */
				lvp_PPDU = (PPDU_T *)prg_list_remove(svlist_channel, lvi_PPDU_list_index);
				
				
				/* calculate SINR*/
				PPDU_sinr_calculate(lvp_PPDU);
				
				/* PPDU segment refresh*/
				PPDU_sinr_segment_refresh();
				
				
				/* free memory */
				for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvp_PPDU->actual_receiver_number; lvi_actual_receiver_index++)
				{
					prg_list_free(lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].segment_sinr);
				}
				
				
				/* send PPDU back to PHY*/
				lvp_ici = op_ici_create("PPDU");
				op_ici_attr_set(lvp_ici, "PPDU_ptr", lvp_PPDU);
				op_ici_install(lvp_ici);
				op_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_END, gvp_node_objid[lvp_PPDU->transmitter_node_index].PHY);
				op_ici_install(OPC_NIL);
				}
				FSM_PROFILE_SECTION_OUT (state4_enter_exec)

			/** state (send_PPDU) exit executives **/
			FSM_STATE_EXIT_FORCED (4, "send_PPDU", "channel [send_PPDU exit execs]")


			/** state (send_PPDU) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "send_PPDU", "idle", "tr_11", "channel [send_PPDU -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (send_channel_init) enter executives **/
			FSM_STATE_ENTER_FORCED (5, "send_channel_init", state5_enter_exec, "channel [send_channel_init enter execs]")
				FSM_PROFILE_SECTION_IN ("channel [send_channel_init enter execs]", state5_enter_exec)
				{
				/* send channel inited intrpt */
				op_intrpt_schedule_mcast_global(op_sim_time(), INTRPT_CHANNEL_INITED);
				op_sim_message("Channel init success!", "channel send MCAST intrpt: INTRPT_CHANNEL_INITED");
				}
				FSM_PROFILE_SECTION_OUT (state5_enter_exec)

			/** state (send_channel_init) exit executives **/
			FSM_STATE_EXIT_FORCED (5, "send_channel_init", "channel [send_channel_init exit execs]")


			/** state (send_channel_init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "send_channel_init", "idle", "tr_17", "channel [send_channel_init -> idle : default / ]")
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
#undef svpp_distance_phase_matrix
#undef svpp_propagation_attenuation_matrix
#undef svpp_impedance_correlation_matrix
#undef svpp_phase_coupling_parameter_matrix
#undef svp_impedance_vector
#undef svlist_noise_ppdu
#undef svi_PPDU_index

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
	if (strcmp ("svpp_distance_phase_matrix" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svpp_distance_phase_matrix);
		FOUT
		}
	if (strcmp ("svpp_propagation_attenuation_matrix" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svpp_propagation_attenuation_matrix);
		FOUT
		}
	if (strcmp ("svpp_impedance_correlation_matrix" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svpp_impedance_correlation_matrix);
		FOUT
		}
	if (strcmp ("svpp_phase_coupling_parameter_matrix" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svpp_phase_coupling_parameter_matrix);
		FOUT
		}
	if (strcmp ("svp_impedance_vector" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svp_impedance_vector);
		FOUT
		}
	if (strcmp ("svlist_noise_ppdu" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svlist_noise_ppdu);
		FOUT
		}
	if (strcmp ("svi_PPDU_index" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->svi_PPDU_index);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

