MIL_3_Tfile_Hdr_ 145A 140A modeler 9 546EFE1B 5471DCF9 69 lu-wspn lu 0 0 none none 0 0 none 13826F9D 4E0A 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                    ��g�      @   D   H      EM  K�  K�  K�  K�  K�  L  L  EA           	   begsim intrpt         
   ����   
   doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����             List *	\svlist_channel;       Stathandle	\svgstat_SINR;       'Stathandle	\svgstat_active_MPDU_number;       2DISTANCE_PHASE_T ***	\svppp_distance_phase_matrix;           :   #include "PLC_def.h"   #include "PLC_data.h"   #include "PLC_func.h"   //#include "PLC_Channel.h"       e#define SYS_INIT				((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_SYS_INIT))   l#define PHY_TXSTART				((op_intrpt_type() == OPC_INTRPT_REMOTE) && (op_intrpt_code() == INTRPT_PHY_TXSTART))   f#define PHY_TXEND				((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_PHY_TXEND))       
/* enum */   typedef enum NODE_TYPE_T   {       TYPE_HE = 0,       TYPE_CPE = 1,       TYPE_NOISE = 2,       TYPE_X = 3,   } NODE_TYPE_T;       typedef enum PHASE_T   {       PHASE_ABC = 0,       PHASE_A = 1,       PHASE_B = 2,       PHASE_C = 3,   
} PHASE_T;   /* end enum */           /* struct */   typedef struct NODE_T   {       int node_id;       NODE_TYPE_T type;       int parent_id;       double distance;       PHASE_T phase;   	} NODE_T;           typedef struct DISTANCE_PHASE_T   {       double dis_X;       double dis_Y;       double dis_Z;       PHASE_T pha_X;       PHASE_T pha_Y;       PHASE_T pha_Z;   } DISTANCE_PHASE_T;   /* end struct */           :static double MPDU_receive_power_calculate(int, MPDU_T *);   %static void PHY_medium_refresh(void);   ,static void MPDU_sinr_segment_refresh(void);   *static void MPDU_sinr_calculate(MPDU_T *);   5static DISTANCE_PHASE_T *** topology_init(FILE *fin);       Eint HE_num = 0, CPE_num = 0, NOISE_num = 0, X_num = 0, total_num = 0;  �   >/************************************************************/   ;/* Author: Li Yong	                                     	*/   >/* Last Update: 2014.08.25                                  */   ;/* Remarks:                                             	*/   >/************************************************************/   static double   KMPDU_receive_power_calculate(int lvi_receiver_node_index, MPDU_T *lvp_MPDU)   {   	double lvd_pathloss_linear;   	   %	FIN(MPDU_receive_power_calculate());   	   y	lvd_pathloss_linear = pow(10.0, gvo_pathloss_matrix_dB[lvp_MPDU->transmitter_node_index][lvi_receiver_node_index]/10.0);          4	FRET(lvp_MPDU->power_linear / lvd_pathloss_linear);   }           >/************************************************************/   ;/* Author: Li Yong	                                     	*/   >/* Last Update: 2014.08.25                                  */   ;/* Remarks:                                             	*/   >/************************************************************/   static void   PHY_medium_refresh()	   {   	int lvi_HE_index;   	int lvi_CPE_index;   	int lvi_node_index;   	int lvi_MPDU_index;   	int lvi_MPDU_number;   	MPDU_T *lvp_MPDU;   	double lvd_sum_power_linear;   	double lvd_sum_power_dBm;   	   	FIN(PHY_medium_refresh());   	   4	lvi_MPDU_number = op_prg_list_size(svlist_channel);   	   E	for (lvi_HE_index = 0; lvi_HE_index < gvi_HE_number; lvi_HE_index++)   	{   A		if (gvo_HE_property[lvi_HE_index].PHY_poweron_flag == OPC_TRUE)   		{	   !			lvi_node_index = lvi_HE_index;   			lvd_sum_power_linear = 0.0;   		   O			for (lvi_MPDU_index = 0; lvi_MPDU_index < lvi_MPDU_number; lvi_MPDU_index++)   			{   L				lvp_MPDU = (MPDU_T *)op_prg_list_access(svlist_channel, lvi_MPDU_index);   S				lvd_sum_power_linear += MPDU_receive_power_calculate(lvi_node_index, lvp_MPDU);   			}   		   9			lvd_sum_power_linear += CONST_HE_THERMAL_NOISE_LINEAR;   A			lvd_sum_power_dBm = 10.0 * log10(lvd_sum_power_linear) + 30.0;   Y			op_stat_write(gvo_HE_property[lvi_HE_index].PHY_medium_stathandle, lvd_sum_power_dBm);   		}	   	}	   	   I	for (lvi_CPE_index = 0; lvi_CPE_index < gvi_CPE_number; lvi_CPE_index++)   	{   C		if (gvo_CPE_property[lvi_CPE_index].PHY_poweron_flag == OPC_TRUE)   		{	   2			lvi_node_index = lvi_CPE_index + gvi_HE_number;   			lvd_sum_power_linear = 0.0;   		   O			for (lvi_MPDU_index = 0; lvi_MPDU_index < lvi_MPDU_number; lvi_MPDU_index++)   			{   L				lvp_MPDU = (MPDU_T *)op_prg_list_access(svlist_channel, lvi_MPDU_index);   S				lvd_sum_power_linear += MPDU_receive_power_calculate(lvi_node_index, lvp_MPDU);   			}       :			lvd_sum_power_linear += CONST_CPE_THERMAL_NOISE_LINEAR;   A			lvd_sum_power_dBm = 10.0 * log10(lvd_sum_power_linear) + 30.0;   [			op_stat_write(gvo_CPE_property[lvi_CPE_index].PHY_medium_stathandle, lvd_sum_power_dBm);   		}   	}	   	   	FOUT;   }	           >/************************************************************/   ;/* Author: Li Yong                                      	*/   >/* Last Update: 2014.08.25                                  */   ;/* Remarks:                                             	*/   >/************************************************************/   static void   MPDU_sinr_segment_refresh()   {   	int lvi_MPDU_number;   	int lvi_MPDU_index_1;   	int lvi_MPDU_index_2;   	MPDU_T *lvp_MPDU_1;   	MPDU_T *lvp_MPDU_2;    	double lvd_signal_power_linear;   &	double lvd_interference_power_linear;   	double lvd_noise_power_linear;   	double lvd_sinr_linear;   "	SEGMENT_SINR_T *lvp_segment_sinr;    	int lvi_actual_receiver_number;   	int lvi_actual_receiver_index;   $	int lvi_actual_receiver_node_index;   	   "	FIN(MPDU_sinr_segment_refresh());   	   4	lvi_MPDU_number = op_prg_list_size(svlist_channel);   	   S	for (lvi_MPDU_index_1 = 0; lvi_MPDU_index_1 < lvi_MPDU_number; lvi_MPDU_index_1++)   	{   N		lvp_MPDU_1 = (MPDU_T *)op_prg_list_access(svlist_channel, lvi_MPDU_index_1);   		   B		lvi_actual_receiver_number = lvp_MPDU_1->actual_receiver_number;   7		/* calculate for each actual receiver of each MPDU */   z		for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvi_actual_receiver_number; lvi_actual_receiver_index++)   		{   v			lvi_actual_receiver_node_index = lvp_MPDU_1->actual_receiver[lvi_actual_receiver_index].actual_receiver_node_index;   		   !			lvd_signal_power_linear = 0.0;   '			lvd_interference_power_linear = 0.0;   6			if (lvi_actual_receiver_node_index < gvi_HE_number)   			{   >		    	lvd_noise_power_linear = CONST_HE_THERMAL_NOISE_LINEAR;   			}	   			else   			{   ?		    	lvd_noise_power_linear = CONST_CPE_THERMAL_NOISE_LINEAR;   			}	   			/* loop for all MPDUs */   U			for (lvi_MPDU_index_2 = 0; lvi_MPDU_index_2 < lvi_MPDU_number; lvi_MPDU_index_2++)   			{   P				lvp_MPDU_2 = (MPDU_T *)op_prg_list_access(svlist_channel, lvi_MPDU_index_2);   				/* signal */   -				if (lvi_MPDU_index_1 == lvi_MPDU_index_2)   				{   *			    	if (lvd_signal_power_linear > 0.0)   					{   �						op_sim_end("Error: Duplicate signal MPDU!", "Error source module: PLC_CHANNEL", "Error source function: MPDU_sinr_segment_refresh()", "");   					}	   						else   					{   i						lvd_signal_power_linear = MPDU_receive_power_calculate(lvi_actual_receiver_node_index, lvp_MPDU_2);   					}	   				}	   !				/* otherwise, interference */   				else   				{   o					lvd_interference_power_linear += MPDU_receive_power_calculate(lvi_actual_receiver_node_index, lvp_MPDU_2);   				}   			}   			   h			lvd_sinr_linear = lvd_signal_power_linear / (lvd_interference_power_linear + lvd_noise_power_linear);   Q			lvp_segment_sinr = (SEGMENT_SINR_T *)op_prg_mem_alloc(sizeof(SEGMENT_SINR_T));   E			lvp_segment_sinr->segment_sinr_dB = 10.0 * log10(lvd_sinr_linear);   8			lvp_segment_sinr->segment_start_time = op_sim_time();   			op_prg_list_insert(lvp_MPDU_1->actual_receiver[lvi_actual_receiver_index].segment_sinr, lvp_segment_sinr, OPC_LISTPOS_TAIL);   		}   	}   	   	FOUT;   }           >/************************************************************/   ;/* Author: Li Yong                                      	*/   >/* Last Update: 2014.08.25                                  */   ;/* Remarks:                                             	*/   >/************************************************************/   static void   %MPDU_sinr_calculate(MPDU_T *lvp_MPDU)   {   	int lvi_sinr_segment_number;   	int lvi_sinr_segment_index;   "	SEGMENT_SINR_T *lvp_segment_sinr;   	double lvd_sinr_dB;   	double lvd_segment_end_time;    	int lvi_actual_receiver_number;   	int lvi_actual_receiver_index;   	List* lvlist_segment_sinr;   	   	FIN(MPDU_sinr_calculate());   	   ?	lvi_actual_receiver_number = lvp_MPDU->actual_receiver_number;   	   6	/* calculate for each actual receiver of each MPDU */   {   for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvi_actual_receiver_number; lvi_actual_receiver_index++)   	{   Z		lvlist_segment_sinr = lvp_MPDU->actual_receiver[lvi_actual_receiver_index].segment_sinr;   B		lvi_sinr_segment_number = op_prg_list_size(lvlist_segment_sinr);   ,		lvd_segment_end_time = lvp_MPDU->end_time;   		lvd_sinr_dB = 0.0;   		   s		for (lvi_sinr_segment_index = lvi_sinr_segment_number - 1; lvi_sinr_segment_index >= 0; lvi_sinr_segment_index--)   		{   h			lvp_segment_sinr = (SEGMENT_SINR_T *)op_prg_list_access(lvlist_segment_sinr, lvi_sinr_segment_index);   �			lvd_sinr_dB += lvp_segment_sinr->segment_sinr_dB * (lvd_segment_end_time - lvp_segment_sinr->segment_start_time);    // sum of time-weighted dB value   ?			lvd_segment_end_time = lvp_segment_sinr->segment_start_time;   		}   	   ~		lvp_MPDU->actual_receiver[lvi_actual_receiver_index].MPDU_sinr_dB = lvd_sinr_dB/(lvp_MPDU->end_time - lvp_MPDU->start_time);   	   "		/* write MPDU SINR statistics */   a		op_stat_write(svgstat_SINR, lvp_MPDU->actual_receiver[lvi_actual_receiver_index].MPDU_sinr_dB);   f		if (lvp_MPDU->actual_receiver[lvi_actual_receiver_index].actual_receiver_node_index < gvi_HE_number)   		{	   �			op_stat_write(gvo_HE_property[lvp_MPDU->actual_receiver[lvi_actual_receiver_index].actual_receiver_node_index].PHY_SINR_stathandle, lvp_MPDU->actual_receiver[lvi_actual_receiver_index].MPDU_sinr_dB);   		}	   		else   		{	   �			op_stat_write(gvo_CPE_property[lvp_MPDU->actual_receiver[lvi_actual_receiver_index].actual_receiver_node_index - gvi_HE_number].PHY_SINR_stathandle, lvp_MPDU->actual_receiver[lvi_actual_receiver_index].MPDU_sinr_dB);   		}	   	}   	   	FOUT;   }       >/************************************************************/   >/* Author: jiaying.lu                                      	*/   >/* Last Update: 2014.11.16                                  */   ;/* Remarks:                                             	*/   >/************************************************************/   static DISTANCE_PHASE_T ***   topology_init(FILE *fin)   {   	NODE_T *lvp_node;   9	extern int HE_num, CPE_num, NOISE_num, X_num, total_num;   )	Prg_List **lvpp_rlist;    //reverse list   	int lvi_len_sub;   ?	Prg_List *lvp_tmp_list ,***lvppp_nlist;    //node to node list   (	int lvi_index_Y_start, lvi_index_Y_end;   	   	FIN(topology_init());   !	printf("enter topology_init\n");   	total_num = 0;   	lvi_index_Y_start = 0;          	/* read from file */   E	lvp_node = (NODE_T *)op_prg_mem_alloc((total_num+1)*sizeof(NODE_T));   �	while (fscanf(fin, "%d %d %d %lf %d", &(lvp_node[total_num].node_id), &(lvp_node[total_num].type), &(lvp_node[total_num].parent_id), &(lvp_node[total_num].distance), &(lvp_node[total_num].phase)))   	{   		if (feof(fin))   		{   			printf("read file end!\n");   				break;   		}   �		printf("%d: %d %d %d %lf %d\n", total_num, lvp_node[total_num].node_id, lvp_node[total_num].type, lvp_node[total_num].parent_id, lvp_node[total_num].distance, lvp_node[total_num].phase);   		/* calculate TYPE nums */   *		if (lvp_node[total_num].type == TYPE_HE)   		{   			HE_num += 1;   		}   0		else if (lvp_node[total_num].type == TYPE_CPE)   		{   			CPE_num += 1;   		}   2		else if (lvp_node[total_num].type == TYPE_NOISE)   		{   	 		NOISE_num += 1;   		}   .		else if (lvp_node[total_num].type == TYPE_X)   		{   			X_num += 1;   		}   		else   		{   \	 		printf("error occurs in node[%d], it's type=%d\n", total_num, lvp_node[total_num].type);   		}   		   		total_num += 1;   R		lvp_node = (NODE_T *)op_prg_mem_realloc(lvp_node, (total_num+1)*sizeof(NODE_T));   	}       1	if (total_num != HE_num+CPE_num+NOISE_num+X_num)   	{   		char err_msg[20];   X		sprintf(err_msg, "total_num: %d, sum: %d", total_num, HE_num+CPE_num+NOISE_num+X_num);   R		op_sim_end("err: total_num != HE_num+CPE_num+NOISE_num+X_num", err_msg, "", "");   	}   	   %	/* generate distance_phase_matrix */    	//* find common father node *//   9	///* generate reverse list: leaf->parent->...->root *///   L	lvpp_rlist = (Prg_List **)op_prg_mem_alloc(total_num * sizeof(Prg_List *));    	for (int i=0; i<total_num; i++)   	{   $		lvpp_rlist[i] = prg_list_create();   L		prg_list_insert(lvpp_rlist[i], &(lvp_node[i].node_id), PRGC_LISTPOS_TAIL);   M		int *lvp_tmp_id = (int *)prg_list_access(lvpp_rlist[i], PRGC_LISTPOS_TAIL);   *		printf("rlist[%d]: %d", i, *lvp_tmp_id);   		while (*lvp_tmp_id != 0)   		{   Y			prg_list_insert(lvpp_rlist[i], &(lvp_node[*lvp_tmp_id].parent_id), PRGC_LISTPOS_TAIL);   I			lvp_tmp_id = (int *)prg_list_access(lvpp_rlist[i], PRGC_LISTPOS_TAIL);   			printf("->%d", *lvp_tmp_id);   		}   6		printf(" (len:%d)\n", prg_list_size(lvpp_rlist[i]));   	}   	   	   #	///* find nearest common node *///   O	lvppp_nlist = (Prg_List ***)op_prg_mem_alloc(total_num * sizeof(Prg_List **));   #	// init must success before use!!!    	for (int i=0; i<total_num; i++)   	{   Q		lvppp_nlist[i] = (Prg_List **)op_prg_mem_alloc(total_num * sizeof(Prg_List *));   	}    	for (int i=0; i<total_num; i++)   	{   		   !		for (int j=0; j<total_num; j++)   		{   			if (i != j)   			{   N				lvi_len_sub = prg_list_size(lvpp_rlist[i]) - prg_list_size(lvpp_rlist[j]);   				int h = 0, k = 0;   				if (lvi_len_sub > 0)   				{   					h = lvi_len_sub;   				}   					else    				{   					k = -lvi_len_sub;   				}   f				while (*((int *)prg_list_access(lvpp_rlist[i], h)) != *((int *)prg_list_access(lvpp_rlist[j], k)))   				{   					h += 1;   					k += 1;   				}   $				/* generate node to node list */   !				printf("(h=%d, k=%d)", h, k);   %				lvp_tmp_list = prg_list_create();   )				printf("list [%d] to [%d]: |", i, j);   *				for (int temp_h=0; temp_h<h; temp_h++)   				{   >					//printf("access lvpp_rlist[%d], temp_h=%d\n",i, temp_h);   f					prg_list_insert(lvp_tmp_list, (int *)prg_list_access(lvpp_rlist[i], temp_h) , PRGC_LISTPOS_TAIL);   D					printf("%d->", *(int *)prg_list_access(lvpp_rlist[i], temp_h));   				}   +				for (int temp_k=k; temp_k>=0; temp_k--)   				{   >					//printf("access lvpp_rlist[%d], temp_k=%d\n",j, temp_k);   f					prg_list_insert(lvp_tmp_list, (int *)prg_list_access(lvpp_rlist[j], temp_k) , PRGC_LISTPOS_TAIL);   D					printf("%d->", *(int *)prg_list_access(lvpp_rlist[j], temp_k));   				}   				printf("|\n");   %				lvppp_nlist[i][j] = lvp_tmp_list;   				lvp_tmp_list = NULL;   			}   		}   	}   	   	//* calculate dis_Y*//   $	// init svppp_distance_phase_matrix   o	svppp_distance_phase_matrix = (DISTANCE_PHASE_T ***)op_prg_mem_alloc(total_num * sizeof(DISTANCE_PHASE_T **));   #	// init must success before use!!!    	for (int i=0; i<total_num; i++)   	{   q		svppp_distance_phase_matrix[i] = (DISTANCE_PHASE_T **)op_prg_mem_alloc(total_num * sizeof(DISTANCE_PHASE_T *));   	}    	for (int i=0; i<total_num; i++)   	{   !		for (int j=0; j<total_num; j++)   		{   r			svppp_distance_phase_matrix[i][j] = (DISTANCE_PHASE_T *)op_prg_mem_alloc(total_num * sizeof(DISTANCE_PHASE_T));   			if (i == j)   			{   1				svppp_distance_phase_matrix[i][j]->dis_X = 0;   1				svppp_distance_phase_matrix[i][j]->dis_Y = 0;   1				svppp_distance_phase_matrix[i][j]->dis_Z = 0;   9				svppp_distance_phase_matrix[i][j]->pha_X = PHASE_ABC;   9				svppp_distance_phase_matrix[i][j]->pha_Y = PHASE_ABC;   9				svppp_distance_phase_matrix[i][j]->pha_Z = PHASE_ABC;   			}   			else   			{   f				for (lvi_index_Y_start=0; lvi_index_Y_start<prg_list_size(lvppp_nlist[i][j]); lvi_index_Y_start++)   				{   d					if (lvp_node[*(int *)prg_list_access(lvppp_nlist[i][j], lvi_index_Y_start)].phase == PHASE_ABC)   					{   						break;   					}   				}   `				for (lvi_index_Y_end=prg_list_size(lvppp_nlist[i][j]); lvi_index_Y_end>0; lvi_index_Y_end--)   				{   b					if (lvp_node[*(int *)prg_list_access(lvppp_nlist[i][j], lvi_index_Y_end)].phase == PHASE_ABC)   					{   						break;   					}   				}   R				if (lvi_index_Y_start==prg_list_size(lvppp_nlist[i][j]) && lvi_index_Y_end==0)   				{   					lvi_index_Y_start = -1;   					lvi_index_Y_end = -1;   .					printf("lvppp_nlist[%d][%d] dis_Y=0!\n");   				}   			}   		}   	}   	   	   !	printf("leave topology_init\n");   	op_prg_mem_free(lvp_node);    	for (int i=0; i<total_num; i++)   	{   "		op_prg_list_free(lvpp_rlist[i]);   !		for (int j=0; i<total_num; i++)   		{   '			op_prg_list_free(lvppp_nlist[i][j]);   		}   "		op_prg_mem_free(lvppp_nlist[i]);   	}   	op_prg_mem_free(lvpp_rlist);   	op_prg_mem_free(lvppp_nlist);   #	FRET(svppp_distance_phase_matrix);   }                                          �   �          
   init   
       J   0   extern int total_num;       FILE *lvp_fin;       2if ((lvp_fin=fopen("./network.txt", "r")) == NULL)   {   2	op_sim_end("err: Can not open file", "", "", "");   }   else   {   	printf("lvp_fin != NULL\n");   6	svppp_distance_phase_matrix = topology_init(lvp_fin);   (	printf("total_num == %d\n", total_num);   }       fclose(lvp_fin);       /**   //test for point   
int m = 3;   
int n = 4;   EPrg_List ***a = (Prg_List ***)prg_mem_alloc(m * sizeof(Prg_List **));   for (int i=0; i<m; i++)   {   ?	*(a + i) = (Prg_List **)prg_mem_alloc(n * sizeof(Prg_List *));   }   "Prg_List *tmp = prg_list_create();   ,prg_list_insert(tmp, &m, PRGC_LISTPOS_TAIL);   //a[1][3] = tmp;   Bprintf("before: a[1]:%p,  a[1][3]=%p\n", *(a+1), *(*(a + 1) + 3));   *(*(a + 1) + 3) = tmp;   (printf("a[1][3]=%p\n", *(*(a + 1) + 3));   prg_list_free(a[1][3]);   for (int i=0; i<m; i++)   {   	prg_mem_free(a[i]);   }   prg_mem_free(a);   **/       /**   //test for List    Prg_List *a = prg_list_create();   int a0 = 0, a1 = 1;   +prg_list_insert(a, &a0, PRGC_LISTPOS_TAIL);   +prg_list_insert(a, &a1, PRGC_LISTPOS_TAIL);   Xprintf("a: [%d]->[%d]\n", *(int *)prg_list_access(a, 0), *(int *)prg_list_access(a, 1));   **/   J                     
   ����   
          pr_state           �          
   idle   
       
      'op_sim_end("success~", "end!", "", "");   
                         ����             pr_state                        �   �      �   �     �          
   tr_0   
       ����          ����          
    ����   
          ����                       pr_transition                                             