/* Process model C form file: channel.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char channel_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 5473DED7 5473DED7 1 lu-wspn lu 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                               ";
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
    double dis_X;
    double dis_Y;
    double dis_Z;
	double dis_total;
    PHASE_T pha_X;
    PHASE_T pha_Y;
    PHASE_T pha_Z;
} DISTANCE_PHASE_T;
/* end struct */


static double MPDU_receive_power_calculate(int, MPDU_T *);
static void PHY_medium_refresh(void);
static void MPDU_sinr_segment_refresh(void);
static void MPDU_sinr_calculate(MPDU_T *);
static DISTANCE_PHASE_T ** topology_init(FILE *fin);
static void propagation_attenuation_generate(DISTANCE_PHASE_T const **distance_phase_matrix, double **propagation_attenuation_matrix);
static void impedance_correlation_generate(DISTANCE_PHASE_T const **distance_phase_matrix, double **impedance_correlation_matrix);

int HE_num = 0, CPE_num = 0, NOISE_num = 0, X_num = 0, total_num = 0;

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
	DISTANCE_PHASE_T **	    		svpp_distance_phase_matrix                      ;
	double **	              		svpp_propagation_attenuation_matrix             ;
	double **	              		svpp_impedance_correlation_matrix               ;
	} channel_state;

#define svlist_channel          		op_sv_ptr->svlist_channel
#define svgstat_SINR            		op_sv_ptr->svgstat_SINR
#define svgstat_active_MPDU_number		op_sv_ptr->svgstat_active_MPDU_number
#define svpp_distance_phase_matrix		op_sv_ptr->svpp_distance_phase_matrix
#define svpp_propagation_attenuation_matrix		op_sv_ptr->svpp_propagation_attenuation_matrix
#define svpp_impedance_correlation_matrix		op_sv_ptr->svpp_impedance_correlation_matrix

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
static DISTANCE_PHASE_T **
topology_init(FILE *fin)
{
	/**	read file from fin, init topology and generate DISTANCE PHASE matrix.
	//  
	//	Args:
	//		FILE *fin: file with 6 columns, column type is int.
	//	Return:
	//  	DISTANCE_PHASE_T **: a matrix, length == total_num.    
	**/

	NODE_T *lvp_node;
	Prg_List **lvpp_rlist;    //reverse list
	int lvi_len_sub;
	
	FIN(topology_init());
	printf("enter topology_init\n");
	total_num = 0;
    
	/* read from file */
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
		lvp_node = (NODE_T *)op_prg_mem_realloc(lvp_node, (total_num+1)*sizeof(NODE_T));
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
	lvpp_rlist = (Prg_List **)op_prg_mem_alloc(total_num * sizeof(Prg_List *));
	for (int i=0; i<total_num; i++)
	{
		lvpp_rlist[i] = prg_list_create();
		prg_list_insert(lvpp_rlist[i], &(lvp_node[i].node_id), PRGC_LISTPOS_TAIL);
		int *lvp_tmp_id = (int *)prg_list_access(lvpp_rlist[i], PRGC_LISTPOS_TAIL);
		printf("rlist[%d]: %d", i, *lvp_tmp_id);
		while (*lvp_tmp_id != 0)
		{
			prg_list_insert(lvpp_rlist[i], &(lvp_node[*lvp_tmp_id].parent_id), PRGC_LISTPOS_TAIL);
			lvp_tmp_id = (int *)prg_list_access(lvpp_rlist[i], PRGC_LISTPOS_TAIL);
			printf("->%d", *lvp_tmp_id);
		}
		printf(" (len:%d)\n", prg_list_size(lvpp_rlist[i]));
	}
	
	svpp_distance_phase_matrix = (DISTANCE_PHASE_T **)op_prg_mem_alloc(total_num * sizeof(DISTANCE_PHASE_T **));
	// init must success before use!!!
	for (int i=0; i<HE_num+CPE_num+NOISE_num; i++)
	{
		svpp_distance_phase_matrix[i] = (DISTANCE_PHASE_T *)op_prg_mem_alloc(total_num * sizeof(DISTANCE_PHASE_T));
	}

	for (int i=0; i<HE_num+CPE_num+NOISE_num; i++)
	{
		
		for (int j=0; j<HE_num+CPE_num+NOISE_num; j++)
		{
			svpp_distance_phase_matrix[i][j].dis_X = 0;
			svpp_distance_phase_matrix[i][j].dis_Y = 0;
			svpp_distance_phase_matrix[i][j].dis_Z = 0;
			svpp_distance_phase_matrix[i][j].dis_total = 0;
			svpp_distance_phase_matrix[i][j].pha_X = PHASE_ABC;
			svpp_distance_phase_matrix[i][j].pha_Y = PHASE_ABC;
			svpp_distance_phase_matrix[i][j].pha_Z = PHASE_ABC;
			/* find nearest common node */
			if (i != j)
			{
				lvi_len_sub = prg_list_size(lvpp_rlist[i]) - prg_list_size(lvpp_rlist[j]);
				int h = 0, k = 0;
				if (lvi_len_sub > 0)
				{
					h = lvi_len_sub;
				}
				else 
				{
					k = -lvi_len_sub;
				}
				while (*((int *)prg_list_access(lvpp_rlist[i], h)) != *((int *)prg_list_access(lvpp_rlist[j], k)))
				{
					h += 1;
					k += 1;
				}
				/* generate distance phase matrix */
				printf("[%d]->[%d](h=%d, k=%d) ", h, k, i, j);
				for (int temp_h=0; temp_h<h; temp_h++)
				{
					if (lvp_node[*(int *)prg_list_access(lvpp_rlist[i], temp_h)].phase != PHASE_ABC)
					{
						svpp_distance_phase_matrix[i][j].dis_X += lvp_node[*(int *)prg_list_access(lvpp_rlist[i], temp_h)].distance;
						svpp_distance_phase_matrix[i][j].pha_X = lvp_node[*(int *)prg_list_access(lvpp_rlist[i], temp_h)].phase;
					}
					else
					{
						svpp_distance_phase_matrix[i][j].dis_Y += lvp_node[*(int *)prg_list_access(lvpp_rlist[i], temp_h)].distance;
					}
				}
				for (int temp_k=0; temp_k<k; temp_k++)
				{
					if (lvp_node[*(int *)prg_list_access(lvpp_rlist[j], temp_k)].phase != PHASE_ABC)
					{
						svpp_distance_phase_matrix[i][j].dis_Z += lvp_node[*(int *)prg_list_access(lvpp_rlist[j], temp_k)].distance;
						svpp_distance_phase_matrix[i][j].pha_Z = lvp_node[*(int *)prg_list_access(lvpp_rlist[j], temp_k)].phase;
					}
					else
					{
						svpp_distance_phase_matrix[i][j].dis_Y += lvp_node[*(int *)prg_list_access(lvpp_rlist[j], temp_k)].distance;
					}
				}
				svpp_distance_phase_matrix[i][j].dis_total = svpp_distance_phase_matrix[i][j].dis_X + svpp_distance_phase_matrix[i][j].dis_Y + svpp_distance_phase_matrix[i][j].dis_Z;
				printf("pha_X=%d, pha_Y=%d, pha_Z=%d, dis_X=%lf, dis_Y=%lf, dis_Z=%lf, dis_total=%lf\n", svpp_distance_phase_matrix[i][j].pha_X, svpp_distance_phase_matrix[i][j].pha_Y, svpp_distance_phase_matrix[i][j].pha_Z, svpp_distance_phase_matrix[i][j].dis_X, svpp_distance_phase_matrix[i][j].dis_Y, svpp_distance_phase_matrix[i][j].dis_Z, svpp_distance_phase_matrix[i][j].dis_total);
			}
		}
	}
	
	op_prg_mem_free(lvp_node);
	for (int i=0; i<HE_num+CPE_num+NOISE_num; i++)
	{
		op_prg_list_free(lvpp_rlist[i]);
	}
	op_prg_mem_free(lvpp_rlist);
	printf("leave topology_init...\n");
	FRET(svpp_distance_phase_matrix);
}


/************************************************************/
/* Author: jiaying.lu                                      	*/
/* Last Update: 2014.11.24                                  */
/* Remarks:                                             	*/
/************************************************************/
static void
propagation_attenuation_generate(DISTANCE_PHASE_T const **distance_phase_matrix, double **propagation_attenuation_matrix)
{
	double a0, a1, k, f;
	int const attenu_num = HE_num + CPE_num + NOISE_num;
	
	FIN(propagation_attenuation_generate());
	printf("enter propagation_attenuation_generate, param [attenu_num=%d].\n", attenu_num);
	
	//formula: A(f,d) = e^(-(a0 + a1*f^k)*d)
	a0 = 0;
	a1 = 8.75 * pow(10, -10);
	k = 1;
	f = 5 * pow(10, 6);
	
	for (int i=0; i<attenu_num; i++)
	{
		for (int j=0; j<attenu_num; j++)
		{
			if (i == j)
			{
				propagation_attenuation_matrix[i][j] = 0;
			}
			else
			{
				propagation_attenuation_matrix[i][j] = exp(-(a0 + a1*pow(f, k))*distance_phase_matrix[i][j].dis_total);
			}
			printf("propagation_attenuation_matrix[%d][%d] =%0.4lf\n", i, j, propagation_attenuation_matrix[i][j]);
		}
	}
	
	printf("leave propagation_attenuation_generate...\n");
	FOUT;
}


/************************************************************/
/* Author: jiaying.lu                                      	*/
/* Last Update: 2014.11.25                                  */
/* Remarks:                                             	*/
/************************************************************/
static void
impedance_correlation_generate(DISTANCE_PHASE_T const **distance_phase_matrix, double **impedance_correlation_matrix)
{
	FIN(impedance_correlation_generate());
	
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


		FSM_ENTER_NO_VARS ("channel")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (0, "init", "channel [init enter execs]")
				FSM_PROFILE_SECTION_IN ("channel [init enter execs]", state0_enter_exec)
				{
				FILE *lvp_fin;
				
				/* topology init. */
				// global varieble HE_num, CPE_num, NOISE_num, X_num and total_num init.
				if ((lvp_fin=fopen("./network.txt", "r")) == NULL)
				{
					op_sim_end("err: Can not open file", "", "", "");
				}
				else
				{
					printf("lvp_fin != NULL\n");
					svpp_distance_phase_matrix = topology_init(lvp_fin);
					printf("total_num == %d\n", total_num);
				}
				fclose(lvp_fin);
				
				int const num_without_X = HE_num + CPE_num + NOISE_num;
				
				/* generate propagation_attenuation_matrix */
				// propagation_attenuation_matrix malloc memory.
				svpp_propagation_attenuation_matrix = (double **)op_prg_mem_alloc(num_without_X * sizeof(double *));
				for (int i=0; i<num_without_X; i++)
				{
					svpp_propagation_attenuation_matrix[i] = (double *)op_prg_mem_alloc(num_without_X * sizeof(double));
				}
				propagation_attenuation_generate(svpp_distance_phase_matrix, svpp_propagation_attenuation_matrix);
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
#undef svpp_distance_phase_matrix
#undef svpp_propagation_attenuation_matrix
#undef svpp_impedance_correlation_matrix

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
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

