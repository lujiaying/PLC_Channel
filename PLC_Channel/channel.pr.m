MIL_3_Tfile_Hdr_ 145A 140A modeler 9 546EFE1B 547C6185 E0 lu-wspn lu 0 0 none none 0 0 none 6557C460 8968 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                    ��g�      @   D   H      o7  �  �!  �%  �)  �\  �`  �d  o+           	   begsim intrpt         
   ����   
   doc file            	nd_module      endsim intrpt         
    ����   
   failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����             List *	\svlist_channel;       Stathandle	\svgstat_SINR;       'Stathandle	\svgstat_active_MPDU_number;       0DISTANCE_PHASE_T **	\svpp_distance_phase_matrix;       /double **	\svpp_propagation_attenuation_matrix;       -double **	\svpp_impedance_correlation_matrix;       0double **	\svpp_phase_coupling_parameter_matrix;       double *	\svp_impedance_vector;           B   #include "PLC_def.h"   #include "PLC_data.h"   #include "PLC_func.h"   #include "PLC_Channel.h"       e#define SYS_INIT				((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_SYS_INIT))   w#define TIME_TO_UPDATE			((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_TIME_TO_UPDATE))   r#define PPDU_START				((op_intrpt_type() == OPC_INTRPT_REMOTE) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_START))   o#define PPDU_END				((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_END))			       
/* enum */   typedef enum NODE_TYPE_T   {       TYPE_HE = 0,       TYPE_CPE = 1,       TYPE_NOISE = 2,       TYPE_X = 3,   } NODE_TYPE_T;       typedef enum PHASE_T   {       PHASE_ABC = 0,       PHASE_A = 1,       PHASE_B = 2,       PHASE_C = 3,   
} PHASE_T;   /* end enum */           /* struct */   typedef struct NODE_T   {       int node_id;       NODE_TYPE_T type;       int parent_id;       double distance;       PHASE_T phase;   	} NODE_T;           typedef struct DISTANCE_PHASE_T   {       double dis_X;       double dis_Y;       double dis_Z;   	double dis_total;       PHASE_T pha_X;       PHASE_T pha_Y;       PHASE_T pha_Z;   } DISTANCE_PHASE_T;   /* end struct */           :static double MPDU_receive_power_calculate(int, MPDU_T *);   %static void PHY_medium_refresh(void);   ,static void MPDU_sinr_segment_refresh(void);   *static void MPDU_sinr_calculate(MPDU_T *);   4static DISTANCE_PHASE_T ** topology_init(FILE *fin);   �static void propagation_attenuation_generate(DISTANCE_PHASE_T const **distance_phase_matrix, double **propagation_attenuation_matrix, int attenu_num);   �static void impedance_correlation_generate(DISTANCE_PHASE_T const **distance_phase_matrix, double **impedance_correlation_matrix, int impedance_correlation_num);   �static void phase_coupling_parameter_generate(DISTANCE_PHASE_T const **distance_phase_matrix, double **phase_coupling_parameter_matrix, int phase_coupling_num);   rstatic void impedance_vector_init(double *impedance_vector, int impedance_num, double mean, double std_deviation);   sstatic void impedance_vector_update(double *impedance_vector, int impedance_num, double mean, double std_deviatio);       Yint gvi_HE_num = 0, gvi_CPE_num = 0, gvi_NOISE_num = 0, gvi_X_num = 0, gvi_total_num = 0;   Objid gvoid_channel_id;  Y   >/************************************************************/   ;/* Author: Li Yong	                                     	*/   >/* Last Update: 2014.08.25                                  */   ;/* Remarks:                                             	*/   >/************************************************************/   static double   KMPDU_receive_power_calculate(int lvi_receiver_node_index, MPDU_T *lvp_MPDU)   {   	double lvd_pathloss_linear;   	   %	FIN(MPDU_receive_power_calculate());   	   y	lvd_pathloss_linear = pow(10.0, gvo_pathloss_matrix_dB[lvp_MPDU->transmitter_node_index][lvi_receiver_node_index]/10.0);          4	FRET(lvp_MPDU->power_linear / lvd_pathloss_linear);   }           >/************************************************************/   ;/* Author: Li Yong	                                     	*/   >/* Last Update: 2014.08.25                                  */   ;/* Remarks:                                             	*/   >/************************************************************/   static void   PHY_medium_refresh()	   {   	int lvi_HE_index;   	int lvi_CPE_index;   	int lvi_node_index;   	int lvi_MPDU_index;   	int lvi_MPDU_number;   	MPDU_T *lvp_MPDU;   	double lvd_sum_power_linear;   	double lvd_sum_power_dBm;   	   	FIN(PHY_medium_refresh());   	   4	lvi_MPDU_number = op_prg_list_size(svlist_channel);   	   E	for (lvi_HE_index = 0; lvi_HE_index < gvi_HE_number; lvi_HE_index++)   	{   A		if (gvo_HE_property[lvi_HE_index].PHY_poweron_flag == OPC_TRUE)   		{	   !			lvi_node_index = lvi_HE_index;   			lvd_sum_power_linear = 0.0;   		   O			for (lvi_MPDU_index = 0; lvi_MPDU_index < lvi_MPDU_number; lvi_MPDU_index++)   			{   L				lvp_MPDU = (MPDU_T *)op_prg_list_access(svlist_channel, lvi_MPDU_index);   S				lvd_sum_power_linear += MPDU_receive_power_calculate(lvi_node_index, lvp_MPDU);   			}   		   9			lvd_sum_power_linear += CONST_HE_THERMAL_NOISE_LINEAR;   A			lvd_sum_power_dBm = 10.0 * log10(lvd_sum_power_linear) + 30.0;   Y			op_stat_write(gvo_HE_property[lvi_HE_index].PHY_medium_stathandle, lvd_sum_power_dBm);   		}	   	}	   	   I	for (lvi_CPE_index = 0; lvi_CPE_index < gvi_CPE_number; lvi_CPE_index++)   	{   C		if (gvo_CPE_property[lvi_CPE_index].PHY_poweron_flag == OPC_TRUE)   		{	   2			lvi_node_index = lvi_CPE_index + gvi_HE_number;   			lvd_sum_power_linear = 0.0;   		   O			for (lvi_MPDU_index = 0; lvi_MPDU_index < lvi_MPDU_number; lvi_MPDU_index++)   			{   L				lvp_MPDU = (MPDU_T *)op_prg_list_access(svlist_channel, lvi_MPDU_index);   S				lvd_sum_power_linear += MPDU_receive_power_calculate(lvi_node_index, lvp_MPDU);   			}       :			lvd_sum_power_linear += CONST_CPE_THERMAL_NOISE_LINEAR;   A			lvd_sum_power_dBm = 10.0 * log10(lvd_sum_power_linear) + 30.0;   [			op_stat_write(gvo_CPE_property[lvi_CPE_index].PHY_medium_stathandle, lvd_sum_power_dBm);   		}   	}	   	   	FOUT;   }	           >/************************************************************/   ;/* Author: Li Yong                                      	*/   >/* Last Update: 2014.08.25                                  */   ;/* Remarks:                                             	*/   >/************************************************************/   static void   MPDU_sinr_segment_refresh()   {   	int lvi_MPDU_number;   	int lvi_MPDU_index_1;   	int lvi_MPDU_index_2;   	MPDU_T *lvp_MPDU_1;   	MPDU_T *lvp_MPDU_2;    	double lvd_signal_power_linear;   &	double lvd_interference_power_linear;   	double lvd_noise_power_linear;   	double lvd_sinr_linear;   "	SEGMENT_SINR_T *lvp_segment_sinr;    	int lvi_actual_receiver_number;   	int lvi_actual_receiver_index;   $	int lvi_actual_receiver_node_index;   	   "	FIN(MPDU_sinr_segment_refresh());   	   4	lvi_MPDU_number = op_prg_list_size(svlist_channel);   	   S	for (lvi_MPDU_index_1 = 0; lvi_MPDU_index_1 < lvi_MPDU_number; lvi_MPDU_index_1++)   	{   N		lvp_MPDU_1 = (MPDU_T *)op_prg_list_access(svlist_channel, lvi_MPDU_index_1);   		   B		lvi_actual_receiver_number = lvp_MPDU_1->actual_receiver_number;   7		/* calculate for each actual receiver of each MPDU */   z		for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvi_actual_receiver_number; lvi_actual_receiver_index++)   		{   v			lvi_actual_receiver_node_index = lvp_MPDU_1->actual_receiver[lvi_actual_receiver_index].actual_receiver_node_index;   		   !			lvd_signal_power_linear = 0.0;   '			lvd_interference_power_linear = 0.0;   6			if (lvi_actual_receiver_node_index < gvi_HE_number)   			{   >		    	lvd_noise_power_linear = CONST_HE_THERMAL_NOISE_LINEAR;   			}	   			else   			{   ?		    	lvd_noise_power_linear = CONST_CPE_THERMAL_NOISE_LINEAR;   			}	   			/* loop for all MPDUs */   U			for (lvi_MPDU_index_2 = 0; lvi_MPDU_index_2 < lvi_MPDU_number; lvi_MPDU_index_2++)   			{   P				lvp_MPDU_2 = (MPDU_T *)op_prg_list_access(svlist_channel, lvi_MPDU_index_2);   				/* signal */   -				if (lvi_MPDU_index_1 == lvi_MPDU_index_2)   				{   *			    	if (lvd_signal_power_linear > 0.0)   					{   �						op_sim_end("Error: Duplicate signal MPDU!", "Error source module: PLC_CHANNEL", "Error source function: MPDU_sinr_segment_refresh()", "");   					}	   						else   					{   i						lvd_signal_power_linear = MPDU_receive_power_calculate(lvi_actual_receiver_node_index, lvp_MPDU_2);   					}	   				}	   !				/* otherwise, interference */   				else   				{   o					lvd_interference_power_linear += MPDU_receive_power_calculate(lvi_actual_receiver_node_index, lvp_MPDU_2);   				}   			}   			   h			lvd_sinr_linear = lvd_signal_power_linear / (lvd_interference_power_linear + lvd_noise_power_linear);   Q			lvp_segment_sinr = (SEGMENT_SINR_T *)op_prg_mem_alloc(sizeof(SEGMENT_SINR_T));   E			lvp_segment_sinr->segment_sinr_dB = 10.0 * log10(lvd_sinr_linear);   8			lvp_segment_sinr->segment_start_time = op_sim_time();   			op_prg_list_insert(lvp_MPDU_1->actual_receiver[lvi_actual_receiver_index].segment_sinr, lvp_segment_sinr, OPC_LISTPOS_TAIL);   		}   	}   	   	FOUT;   }           >/************************************************************/   ;/* Author: Li Yong                                      	*/   >/* Last Update: 2014.08.25                                  */   ;/* Remarks:                                             	*/   >/************************************************************/   static void   %MPDU_sinr_calculate(MPDU_T *lvp_MPDU)   {   	int lvi_sinr_segment_number;   	int lvi_sinr_segment_index;   "	SEGMENT_SINR_T *lvp_segment_sinr;   	double lvd_sinr_dB;   	double lvd_segment_end_time;    	int lvi_actual_receiver_number;   	int lvi_actual_receiver_index;   	List* lvlist_segment_sinr;   	   	FIN(MPDU_sinr_calculate());   	   ?	lvi_actual_receiver_number = lvp_MPDU->actual_receiver_number;   	   6	/* calculate for each actual receiver of each MPDU */   {   for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvi_actual_receiver_number; lvi_actual_receiver_index++)   	{   Z		lvlist_segment_sinr = lvp_MPDU->actual_receiver[lvi_actual_receiver_index].segment_sinr;   B		lvi_sinr_segment_number = op_prg_list_size(lvlist_segment_sinr);   ,		lvd_segment_end_time = lvp_MPDU->end_time;   		lvd_sinr_dB = 0.0;   		   s		for (lvi_sinr_segment_index = lvi_sinr_segment_number - 1; lvi_sinr_segment_index >= 0; lvi_sinr_segment_index--)   		{   h			lvp_segment_sinr = (SEGMENT_SINR_T *)op_prg_list_access(lvlist_segment_sinr, lvi_sinr_segment_index);   �			lvd_sinr_dB += lvp_segment_sinr->segment_sinr_dB * (lvd_segment_end_time - lvp_segment_sinr->segment_start_time);    // sum of time-weighted dB value   ?			lvd_segment_end_time = lvp_segment_sinr->segment_start_time;   		}   	   ~		lvp_MPDU->actual_receiver[lvi_actual_receiver_index].MPDU_sinr_dB = lvd_sinr_dB/(lvp_MPDU->end_time - lvp_MPDU->start_time);   	   "		/* write MPDU SINR statistics */   a		op_stat_write(svgstat_SINR, lvp_MPDU->actual_receiver[lvi_actual_receiver_index].MPDU_sinr_dB);   f		if (lvp_MPDU->actual_receiver[lvi_actual_receiver_index].actual_receiver_node_index < gvi_HE_number)   		{	   �			op_stat_write(gvo_HE_property[lvp_MPDU->actual_receiver[lvi_actual_receiver_index].actual_receiver_node_index].PHY_SINR_stathandle, lvp_MPDU->actual_receiver[lvi_actual_receiver_index].MPDU_sinr_dB);   		}	   		else   		{	   �			op_stat_write(gvo_CPE_property[lvp_MPDU->actual_receiver[lvi_actual_receiver_index].actual_receiver_node_index - gvi_HE_number].PHY_SINR_stathandle, lvp_MPDU->actual_receiver[lvi_actual_receiver_index].MPDU_sinr_dB);   		}	   	}   	   	FOUT;   }       >/************************************************************/   >/* Author: jiaying.lu                                      	*/   >/* Last Update: 2014.11.26                                  */   ;/* Remarks:                                             	*/   >/************************************************************/   static DISTANCE_PHASE_T **   topology_init(FILE *fin)   {   J	/**	read file from fin, init topology and generate DISTANCE PHASE matrix.   	//     
	//	 Args:   9	//		 FILE *fin: file with 6 columns, column type is int.   	//  Return:   B	//  	 DISTANCE_PHASE_T **: a matrix, length == gvi_total_num.       	**/       	NODE_T *lvp_node;   )	Prg_List **lvpp_rlist;    //reverse list   	int lvi_len_sub;   	int lvi_index_i, lvi_index_j;   ,	int lvi_index_common_i, lvi_index_common_j;   (	int lvi_index_temp_i, lvi_index_temp_j;   	int *lvp_tmp_id;   	char lvc_msg[100];   	   	FIN(topology_init());   !	printf("enter topology_init\n");   	gvi_total_num = 0;          	/* read from file */   I	lvp_node = (NODE_T *)op_prg_mem_alloc((gvi_total_num+1)*sizeof(NODE_T));   �	while (fscanf(fin, "%d %d %d %lf %d", &(lvp_node[gvi_total_num].node_id), &(lvp_node[gvi_total_num].type), &(lvp_node[gvi_total_num].parent_id), &(lvp_node[gvi_total_num].distance), &(lvp_node[gvi_total_num].phase)))   	{   		if (feof(fin))   		{   			printf("read file end!\n");   				break;   		}   �		printf("%d: %d %d %d %lf %d\n", gvi_total_num, lvp_node[gvi_total_num].node_id, lvp_node[gvi_total_num].type, lvp_node[gvi_total_num].parent_id, lvp_node[gvi_total_num].distance, lvp_node[gvi_total_num].phase);   		/* calculate TYPE nums */   .		if (lvp_node[gvi_total_num].type == TYPE_HE)   		{   			gvi_HE_num += 1;   		}   4		else if (lvp_node[gvi_total_num].type == TYPE_CPE)   		{   			gvi_CPE_num += 1;   		}   6		else if (lvp_node[gvi_total_num].type == TYPE_NOISE)   		{   	 		gvi_NOISE_num += 1;   		}   2		else if (lvp_node[gvi_total_num].type == TYPE_X)   		{   			gvi_X_num += 1;   		}   		else   		{   d	 		printf("error occurs in node[%d], it's type=%d\n", gvi_total_num, lvp_node[gvi_total_num].type);   		}   		   		gvi_total_num += 1;   V		lvp_node = (NODE_T *)op_prg_mem_realloc(lvp_node, (gvi_total_num+1)*sizeof(NODE_T));   	}       E	if (gvi_total_num != gvi_HE_num+gvi_CPE_num+gvi_NOISE_num+gvi_X_num)   	{       p		sprintf(lvc_msg, "gvi_total_num: %d, sum: %d", gvi_total_num, gvi_HE_num+gvi_CPE_num+gvi_NOISE_num+gvi_X_num);   f		op_sim_end("err: gvi_total_num != gvi_HE_num+gvi_CPE_num+gvi_NOISE_num+gvi_X_num", lvc_msg, "", "");   	}   	else   	{   �		sprintf(lvc_msg, "gvi_total_num:%d, gvi_HE_num:%d, gvi_CPE_num:%d, gvi_NOISE_num:%d, gvi_X_num:%d", gvi_total_num, gvi_HE_num, gvi_CPE_num, gvi_NOISE_num, gvi_X_num);   /		op_sim_message("read file success", lvc_msg);   	}   	   %	/* generate distance_phase_matrix */    	//* find common father node *//   9	///* generate reverse list: leaf->parent->...->root *///   P	lvpp_rlist = (Prg_List **)op_prg_mem_alloc(gvi_total_num * sizeof(Prg_List *));   >	for (lvi_index_i=0; lvi_index_i<gvi_total_num; lvi_index_i++)   	{   .		lvpp_rlist[lvi_index_i] = prg_list_create();   `		prg_list_insert(lvpp_rlist[lvi_index_i], &(lvp_node[lvi_index_i].node_id), PRGC_LISTPOS_TAIL);   R		lvp_tmp_id = (int *)prg_list_access(lvpp_rlist[lvi_index_i], PRGC_LISTPOS_TAIL);   4		printf("rlist[%d]: %d", lvi_index_i, *lvp_tmp_id);   		while (*lvp_tmp_id != 0)   		{   c			prg_list_insert(lvpp_rlist[lvi_index_i], &(lvp_node[*lvp_tmp_id].parent_id), PRGC_LISTPOS_TAIL);   S			lvp_tmp_id = (int *)prg_list_access(lvpp_rlist[lvi_index_i], PRGC_LISTPOS_TAIL);   			printf("->%d", *lvp_tmp_id);   		}   @		printf(" (len:%d)\n", prg_list_size(lvpp_rlist[lvi_index_i]));   	}   	   q	svpp_distance_phase_matrix = (DISTANCE_PHASE_T **)op_prg_mem_alloc(gvi_total_num * sizeof(DISTANCE_PHASE_T **));   #	// init must success before use!!!   U	for (lvi_index_i=0; lvi_index_i<gvi_HE_num+gvi_CPE_num+gvi_NOISE_num; lvi_index_i++)   	{   {		svpp_distance_phase_matrix[lvi_index_i] = (DISTANCE_PHASE_T *)op_prg_mem_alloc(gvi_total_num * sizeof(DISTANCE_PHASE_T));   	}       U	for (lvi_index_i=0; lvi_index_i<gvi_HE_num+gvi_CPE_num+gvi_NOISE_num; lvi_index_i++)   	{   		   V		for (lvi_index_j=0; lvi_index_j<gvi_HE_num+gvi_CPE_num+gvi_NOISE_num; lvi_index_j++)   		{   B			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_X = 0;   B			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Y = 0;   B			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Z = 0;   F			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_total = 0;   J			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_X = PHASE_ABC;   J			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_Y = PHASE_ABC;   J			svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_Z = PHASE_ABC;   !			/* find nearest common node */   "			if (lvi_index_i != lvi_index_j)   			{   				lvi_index_common_i = 0;   				lvi_index_common_j = 0;   f				lvi_len_sub = prg_list_size(lvpp_rlist[lvi_index_i+1]) - prg_list_size(lvpp_rlist[lvi_index_j+1]);   				if (lvi_len_sub > 0)   				{   &					lvi_index_common_i = lvi_len_sub;   				}   					else    				{   '					lvi_index_common_j = -lvi_len_sub;   				}   �				while (*((int *)prg_list_access(lvpp_rlist[lvi_index_i+1], lvi_index_common_i)) != *((int *)prg_list_access(lvpp_rlist[lvi_index_j+1], lvi_index_common_j)))   				{   					lvi_index_common_i += 1;   					lvi_index_common_j += 1;   				}   (				/* generate distance phase matrix */   �				printf("[%d]->[%d](lvi_index_common_i=%d, lvi_index_common_j=%d) ", lvi_index_i, lvi_index_j, lvi_index_common_i, lvi_index_common_j);   U				for (lvi_index_temp_i=0; lvi_index_temp_i<lvi_index_common_i; lvi_index_temp_i++)   				{   k					if (lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_i+1], lvi_index_temp_i)].phase != PHASE_ABC)   					{   �						svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_X += lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_i+1], lvi_index_temp_i)].distance;   �						svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_X = lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_i+1], lvi_index_temp_i)].phase;   					}   						else   					{   �						svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Y += lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_i+1], lvi_index_temp_i)].distance;   					}   				}   U				for (lvi_index_temp_j=0; lvi_index_temp_j<lvi_index_common_j; lvi_index_temp_j++)   				{   k					if (lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_j+1], lvi_index_temp_j)].phase != PHASE_ABC)   					{   �						svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Z += lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_j+1], lvi_index_temp_j)].distance;   �						svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_Z = lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_j+1], lvi_index_temp_j)].phase;   					}   						else   					{   �						svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Y += lvp_node[*(int *)prg_list_access(lvpp_rlist[lvi_index_j+1], lvi_index_temp_j)].distance;   					}   				}   �				svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_total = svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_X + svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Y + svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Z;  				printf("pha_X=%d, pha_Y=%d, pha_Z=%d, dis_X=%lf, dis_Y=%lf, dis_Z=%lf, dis_total=%lf\n", svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_X, svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_Y, svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].pha_Z, svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_X, svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Y, svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Z, svpp_distance_phase_matrix[lvi_index_i][lvi_index_j].dis_total);   			}   		}   	}   	   	op_prg_mem_free(lvp_node);   U	for (lvi_index_i=0; lvi_index_i<gvi_HE_num+gvi_CPE_num+gvi_NOISE_num; lvi_index_i++)   	{   ,		op_prg_list_free(lvpp_rlist[lvi_index_i]);   	}   	op_prg_mem_free(lvpp_rlist);   $	printf("leave topology_init...\n");   "	FRET(svpp_distance_phase_matrix);   }           >/************************************************************/   >/* Author: jiaying.lu                                       */   >/* Last Update: 2014.11.24                                  */   >/* Remarks:  propagation_attenuation_matrix[0] -> node[1]   */   >/************************************************************/   static void   �propagation_attenuation_generate(DISTANCE_PHASE_T const **distance_phase_matrix, double **propagation_attenuation_matrix, int attenu_num)   {   	double a0, a1, k, f;   	int lvi_index_i, lvi_index_j;   	   )	FIN(propagation_attenuation_generate());   X	printf("enter propagation_attenuation_generate, param [attenu_num=%d].\n", attenu_num);   	   )	//formula: A(f,d) = e^(-(a0 + a1*f^k)*d)   	a0 = 0;   	a1 = 8.75 * pow(10, -10);   	k = 1;   	f = 5 * pow(10, 6);   	   	   ;	for (lvi_index_i=0; lvi_index_i<attenu_num; lvi_index_i++)   	{   <		for (lvi_index_j=0; lvi_index_j<attenu_num; lvi_index_j++)   		{   "			if (lvi_index_i == lvi_index_j)   			{   A				propagation_attenuation_matrix[lvi_index_i][lvi_index_j] = 0;   			}   			else   			{   �				propagation_attenuation_matrix[lvi_index_i][lvi_index_j] = exp(-(a0 + a1*pow(f, k))*distance_phase_matrix[lvi_index_i][lvi_index_j].dis_total);   			}   �			printf("propagation_attenuation_matrix[%d][%d] =%0.4lf\n", lvi_index_i, lvi_index_j, propagation_attenuation_matrix[lvi_index_i][lvi_index_j]);   		}   	}   	   7	printf("leave propagation_attenuation_generate...\n");   	FOUT;   }           >/************************************************************/   >/* Author: jiaying.lu                                       */   >/* Last Update: 2014.11.25                                  */   </* Remarks:                                             	 */   >/************************************************************/   static void   �impedance_correlation_generate(DISTANCE_PHASE_T const **distance_phase_matrix, double **impedance_correlation_matrix, int impedance_correlation_num)   {   	int a;   	int lvi_index_i, lvi_index_j;   	double lvd_row_sum;   	   '	FIN(impedance_correlation_generate());   t	printf("enter impedance_correlation_generate, param [impedance_correlation_num=%d].\n", impedance_correlation_num);   	   	//formula: c = e^(-d/a)   	a = 15;   	   J	for (lvi_index_i=0; lvi_index_i<impedance_correlation_num; lvi_index_i++)   	{   		lvd_row_sum = 0;   K		for (lvi_index_j=0; lvi_index_j<impedance_correlation_num; lvi_index_j++)   		{   "			if (lvi_index_i == lvi_index_j)   			{   ?				impedance_correlation_matrix[lvi_index_i][lvi_index_j] = 0;   			}   			else   			{   �				impedance_correlation_matrix[lvi_index_i][lvi_index_j] = exp(-distance_phase_matrix[lvi_index_i][lvi_index_j].dis_total / a);   			}   I			lvd_row_sum += impedance_correlation_matrix[lvi_index_i][lvi_index_j];   		}   		   		// row normalization   K		for (lvi_index_j=0; lvi_index_j<impedance_correlation_num; lvi_index_j++)   		{   I			impedance_correlation_matrix[lvi_index_i][lvi_index_j] /= lvd_row_sum;   �			printf("impedance_correlation_matrix[%d][%d] =%0.4lf\n", lvi_index_i, lvi_index_j, impedance_correlation_matrix[lvi_index_i][lvi_index_j]);   		}   	}   	   5	printf("leave impedance_correlation_generate...\n");   	FOUT;   }           >/************************************************************/   >/* Author: jiaying.lu                                       */   >/* Last Update: 2014.11.27                                  */   </* Remarks:                                             	 */   >/************************************************************/   static void   �phase_coupling_parameter_generate(DISTANCE_PHASE_T const **distance_phase_matrix, double **phase_coupling_parameter_matrix, int phase_coupling_num)   {   
	double k;   	int lvi_index_i, lvi_index_j;   	   (	FIN(phase_coupling_parameter_generate);   5	printf("enter phase_coupling_parameter_generate\n");   	   1	//formula: c = k*dis_Y / (dis_x + dis_Y + dis_Z)   
	k = 0.5;	   	   C	for (lvi_index_i=0; lvi_index_i<phase_coupling_num; lvi_index_i++)   	{   D		for (lvi_index_j=0; lvi_index_j<phase_coupling_num; lvi_index_j++)   		{   "			if (lvi_index_i == lvi_index_j)   			{   B				phase_coupling_parameter_matrix[lvi_index_i][lvi_index_j] = 1;   			}   			else   			{   �				phase_coupling_parameter_matrix[lvi_index_i][lvi_index_j] = k*distance_phase_matrix[lvi_index_i][lvi_index_j].dis_Y / distance_phase_matrix[lvi_index_i][lvi_index_j].dis_total;   			}   �			printf("phase_coupling_parameter_matrix[%d][%d]=%lf\n", lvi_index_i, lvi_index_j, phase_coupling_parameter_matrix[lvi_index_i][lvi_index_j]);   		}   	}   	   8	printf("leave phase_coupling_parameter_generate...\n");   	FOUT;   }       >/************************************************************/   >/* Author: jiaying.lu                                       */   >/* Last Update: 2014.11.27                                  */   </* Remarks:                                             	 */   >/************************************************************/   static void   eimpedance_vector_init(double *impedance_vector, int impedance_num, double mean, double std_deviation)   {   	int lvi_index_i;   [	Distribution *lvp_normal_dist = op_dist_load("normal", mean, std_deviation*std_deviation);   	   	FIN(impedance_vector_init());   )	printf("enter impedance_vector_init\n");   	   >	for (lvi_index_i=0; lvi_index_i<impedance_num; lvi_index_i++)   	{   C		impedance_vector[lvi_index_i] = op_dist_outcome(lvp_normal_dist);   U		printf("impedance_vector[%d] = %lf\n", lvi_index_i, impedance_vector[lvi_index_i]);   	}   	   ,	printf("leave impedance_vector_init...\n");   	FOUT;   }           >/************************************************************/   >/* Author: jiaying.lu                                       */   >/* Last Update: 2014.11.27                                  */   @/* Remarks:             pack                                	 */   >/************************************************************/   static void   gimpedance_vector_update(double *impedance_vector, int impedance_num, double mean, double std_deviation)   {   	int lvi_index_i;   	double a, b;   X	Distribution *lvp_normal_dist = op_dist_load("normal", 0, std_deviation*std_deviation);   	    	FIN(impedance_vector_update());   +	printf("enter impedance_vector_update\n");   	   /	// formula: y(t) = a*x(t-1) + sqrt(1-a^2)*x(t)   
	a = 0.75;   	b = sqrt(1 - a*a);   	   >	for (lvi_index_i=0; lvi_index_i<impedance_num; lvi_index_i++)   	{   u		impedance_vector[lvi_index_i] = mean + a*(impedance_vector[lvi_index_i]-mean) + b*op_dist_outcome(lvp_normal_dist);   U		printf("impedance_vector[%d] = %lf\n", lvi_index_i, impedance_vector[lvi_index_i]);   	}   	   .	printf("leave impedance_vector_update...\n");   	FOUT;   }           >/************************************************************/   >/* Author: jiaying.lu                                       */   >/* Last Update: 2014.11.27                                  */   >/* Remarks: Used when calculate SINR                        */   >/*          Free impedance_space_vector after calculate.    */   >/************************************************************/   static void   �impedance_space_relate_generate(double const *impedance_vector, double const **impedance_correlation_matrix, double *impedance_space_vector, int impedance_num)   {   	FIN(impedance_space_relate);   	   	FOUT;   }           >/************************************************************/   >/* Author: jiaying.lu                                       */   >/* Last Update: 2014.11.27                                  */   >/* Remarks: Used when calculate SINR                        */   >/*          Free phase_coupling_coefficient after calculate.*/   >/************************************************************/   static void   �phase_coupling_coefficient_generate(double const **phase_coupling_parameter_matrix, double **phase_coupling_coefficient_matrix, int phase_coupling_num, double std_deviation)   {   *	FIN(phase_coupling_coefficient_generate);   	   	FOUT;   }                                            �          
   init   
       J   G    gvoid_channel_id = op_id_self();   FILE *lvp_fin;   int lvi_index_i;   #double lvd_mean, lvd_std_deviation;       "printf("enter CHANNEL.init...\n");   /** //show current dir   Cprintf("file=%s,func=%s,line=%d\n",__FILE__,__FUNCTION__,__LINE__);   system("dir");   **/           /* step 1: topology init. */   H// global varieble HE_num, CPE_num, NOISE_num, X_num and total_num init.   2if ((lvp_fin=fopen("./network.txt", "r")) == NULL)   {   2	op_sim_end("err: Can not open file", "", "", "");   }   else   {   	printf("lvp_fin != NULL\n");   5	svpp_distance_phase_matrix = topology_init(lvp_fin);   ,	printf("total_num == %d\n", gvi_total_num);   }   fclose(lvp_fin);           5/* step 2: generate propagation_attenuation_matrix */   4// malloc memory for propagation_attenuation_matrix.   }svpp_propagation_attenuation_matrix = (double **)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num+gvi_NOISE_num) * sizeof(double *));   Vfor (lvi_index_i=0; lvi_index_i<(gvi_HE_num+gvi_CPE_num+gvi_NOISE_num); lvi_index_i++)   {   �	svpp_propagation_attenuation_matrix[lvi_index_i] = (double *)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num+gvi_NOISE_num) * sizeof(double));   }   �propagation_attenuation_generate(svpp_distance_phase_matrix, svpp_propagation_attenuation_matrix, gvi_HE_num+gvi_CPE_num+gvi_NOISE_num);           3/* step 3: generate impedance_correlation_matrix */   2// malloc memory for impedance_correlation_matrix.   msvpp_impedance_correlation_matrix = (double **)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num) * sizeof(double *));   Lfor (int lvi_index_i=0; lvi_index_i<(gvi_HE_num+gvi_CPE_num); lvi_index_i++)   {   x	svpp_impedance_correlation_matrix[lvi_index_i] = (double *)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num) * sizeof(double));   }   vimpedance_correlation_generate(svpp_distance_phase_matrix, svpp_impedance_correlation_matrix, gvi_HE_num+gvi_CPE_num);           6/* step 4: generate phase_coupling_parameter_matrix */   5// malloc memory for phase_coupling_parameter_matrix.   ~svpp_phase_coupling_parameter_matrix = (double **)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num+gvi_NOISE_num) * sizeof(double *));   Zfor (int lvi_index_i=0; lvi_index_i<(gvi_HE_num+gvi_CPE_num+gvi_NOISE_num); lvi_index_i++)   {   �	svpp_phase_coupling_parameter_matrix[lvi_index_i] = (double *)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num+gvi_NOISE_num) * sizeof(double));   }   �phase_coupling_parameter_generate(svpp_distance_phase_matrix, svpp_phase_coupling_parameter_matrix, gvi_HE_num+gvi_CPE_num+gvi_NOISE_num);           '/* step 5: generate impedance_vector */   &// malloc memory for impedance_vector.   ]svp_impedance_vector = (double *)op_prg_mem_alloc((gvi_HE_num+gvi_CPE_num) * sizeof(double));   lvd_mean = 90;   lvd_std_deviation = 4.5;   aimpedance_vector_init(svp_impedance_vector, gvi_HE_num+gvi_CPE_num, lvd_mean, lvd_std_deviation);           -/* set self intrpt, update impedance_vector*/   ?printf("init end. And current sim time: %ld\n", op_sim_time());   Iop_intrpt_schedule_self(op_sim_time()+10, INTRPT_CHANNEL_TIME_TO_UPDATE);        /* send channel inited intrpt */   Fop_intrpt_schedule_mcast_global(op_sim_time(), INTRPT_CHANNEL_INITED);   J                     
   ����   
          pr_state        �   �          
   idle   
       
      !printf("enter CHANNEL.idle..\n");   )//op_sim_end("success~", "end!", "", "");   
       J      char lvc_err_msg[25];       if (TIME_TO_UPDATE)   {   Q	printf("CHANNEL.idle.Exit receive INTRPT: TIME_TO_UPDATE. Enter next state.\n");   }   else if (INTRPT_CHANNEL_INITED)   {   T	printf("CHANNEL.idle.Exit receive INTRPT: INTRPT_CHANNEL_INITED. Stay in idle.\n");   }   else   {   @	sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());   x	op_sim_end("Error: Unexpected INTRPT is received at \"idle\" state!", "Error source module: CHANNEL", lvc_err_msg, "");   }   J           ����             pr_state        v   Z          
   update   
       
      #double lvd_mean, lvd_std_deviation;       /* update impedance_vector */   lvd_mean = 90;   lvd_std_deviation = 4.5;   cimpedance_vector_update(svp_impedance_vector, gvi_HE_num+gvi_CPE_num, lvd_mean, lvd_std_deviation);   Iop_intrpt_schedule_self(op_sim_time()+10, INTRPT_CHANNEL_TIME_TO_UPDATE);   
                     
   ����   
          pr_state        v            
   receive_PPDU   
                                   
   ����   
          pr_state        �  �          
   	send_PPDU   
                                   
   ����   
          pr_state                       g   �        �  �   �          
   tr_0   
       ����          ����          
    ����   
          ����                       pr_transition                 z     �   �  `   R          
   tr_6   
       
   TIME_TO_UPDATE   
       ����          
    ����   
          ����                       pr_transition              #   �     p   h  �   �          
   tr_7   
       ����          ����          
    ����   
          ����                       pr_transition                  �     �   �  g            
   tr_8   
       
   
PPDU_START   
       ����          
    ����   
          ����                       pr_transition      	        !   �     n    �   �          
   tr_9   
       ����          ����          
    ����   
          ����                       pr_transition      
        �  )     �   �  �  v          
   tr_10   
       
   PPDU_END   
       ����          
    ����   
          ����                       pr_transition              �  3     �  �  �   �          
   tr_11   
       ����          ����          
    ����   
          ����                       pr_transition              �   �     �   �  �   v  z   �  �   �          
   tr_12   
       
   default   
       ����          
    ����   
          ����                       pr_transition                           PLC_Channel   PLC_data   PLC_func                    