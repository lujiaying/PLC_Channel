MIL_3_Tfile_Hdr_ 145A 140A modeler 9 5498CDEC 54C1C372 1F lu-wspn lu 0 0 none none 0 0 none 1BF969E 21E4 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                     ��g�      @   D   H      	$  �  �  �  �  �  �  �  	           	   begsim intrpt         
   ����   
   doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����             Distribution *	\svp_exp_dist;        Distribution *	\svp_normal_dist;       int	\svi_node_index;       PHASE_T	\sve_work_phase;              #include "PLC_def.h"   #include "PLC_Channel.h"   #include "PLC_func.h"       f#define SYS_INITED			((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_SYS_INIT))   s#define PPDU_TIME_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_START))   p#define PPDU_END			((op_intrpt_type() == OPC_INTRPT_REMOTE) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_END))			       #define PPDU_DELTA_TIME		1   #define PPDU_LIVE_TIME		5   #define PPDU_POWER_MEAN		398   #define PPDU_POWER_SD		1       Objid gvoid_PHY;       $static void PPDU_attr_set(PPDU_T *);   "static int self_index_find(Objid);      >/************************************************************/   >/* Author: jiaying.lu                                       */   >/* Last Update: 2014.12.23                                  */    /* Remarks:        											*/   >/************************************************************/   static void   PPDU_attr_set(PPDU_T *ppdu)   {   !	double lvd_prob_broadcast = 0.2;   	   	FIN(PPDU_attr_set());   	   	// set PPDU attr   	ppdu->type = 0;   "	ppdu->start_time = op_sim_time();   B	ppdu->end_time = op_sim_time() + op_dist_uniform(PPDU_LIVE_TIME);   /	ppdu->transmitter_node_index = svi_node_index;   	// send broadcast or unicast   1	if (op_dist_uniform(1.0) < lvd_prob_broadcast) {   <		ppdu->receiver_node_index = PPDU_RECEIVER_INDEX_BROADCAST;   	}   	else {   5		ppdu->receiver_node_index = PPDU_RECEIVER_INDEX_HE;   	}   7	ppdu->power_linear = op_dist_outcome(svp_normal_dist);   #	ppdu->send_phase = sve_work_phase;   	   	FOUT;   }                                                    
   init   
       J      double lvd_ppdu_time;   3Objid lvoid_NODE_id = op_topo_parent(op_id_self());       ?svp_exp_dist = op_dist_load("exponential", PPDU_DELTA_TIME, 0);   ^svp_normal_dist = op_dist_load("normal", PPDU_POWER_MEAN, PPDU_POWER_SD);      //398mw = 26dBm           /* find self index */   8svi_node_index = _global_self_index_find(lvoid_NODE_id);   /* set PHY Objid */   6gvoid_node_oids[svi_node_index].phy_id = op_id_self();   /* get work phase */   Bop_ima_obj_attr_get(lvoid_NODE_id, "work_phase", &sve_work_phase);   /* set power on flag */   Cop_ima_obj_attr_set(lvoid_NODE_id, "power_on", NODE_POWER_ON_FLAG);   Tprintf("[PHY.init] NODE index:%d, work_phase:%d\n", svi_node_index, sve_work_phase);       /* schedule first PPDU */   .lvd_ppdu_time = op_dist_outcome(svp_exp_dist);   �printf("[PHY.init] NODE index:%d, current time:%lf, first PPDU time:%lf\n", svi_node_index, op_sim_time(), op_sim_time()+lvd_ppdu_time);   Pop_intrpt_schedule_self(op_sim_time()+lvd_ppdu_time, INTRPT_CHANNEL_PPDU_START);   J                     
   ����   
          pr_state        �            
   idle   
       
      printf("enter PHY.idle.\n");   
       
      char lvc_err_msg[25];       printf("Leave PHY.idle. ");       if (PPDU_TIME_START)   {   )	printf("receive INTRPT: PPDU_START.\n");   }   else if (PPDU_END)   {   .	printf("receive REMOTE INTRPT: PPDU_END.\n");   }   else if (INTRPT_CHANNEL_INITED)   {   A	printf("receive MCAST INTRPT: CHANNEL_INITED. Stay in idle.\n");   }   else   {   @	sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());   }	op_sim_end("Error: Unexpected INTRPT is received at \"PHY.idle.Exit\" state!", "Error source module: PHY", lvc_err_msg, "");   }   
           ����             pr_state        �   �          
   send_PPDU_2Channel   
       
      Ici *lvp_ici;   double lvd_next_ppdu_time;   PPDU_T *lvp_ppdu;       /* send PPDU to channel*/    lvp_ici = op_ici_create("PPDU");   5lvp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));   PPDU_attr_set(lvp_ppdu);   �printf("[PHY.send_PPDU_2Channel] transmitter:%d, receiver:%d, power_linear:%lf, start_time:%lf, end_time:%lf\n", lvp_ppdu->transmitter_node_index, lvp_ppdu->receiver_node_index, lvp_ppdu->power_linear, lvp_ppdu->start_time, lvp_ppdu->end_time);       3op_ici_attr_set_ptr(lvp_ici, "PPDU_ptr", lvp_ppdu);   op_ici_install(lvp_ici);   Sop_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_START, gvoid_channel);   op_ici_install(OPC_NIL);       /* schedule next PPDU */   3lvd_next_ppdu_time = op_dist_outcome(svp_exp_dist);   {printf("[PHY.send_PPDU_2Channel] current time:%lf, next PPDU time:%lf\n", op_sim_time(), op_sim_time()+lvd_next_ppdu_time);   Uop_intrpt_schedule_self(op_sim_time()+lvd_next_ppdu_time, INTRPT_CHANNEL_PPDU_START);   
                     
   ����   
          pr_state        :  �          
   receive_Channel_PPDU   
       
      Ici *lvp_ici;   PPDU_T *lvp_PPDU;   int lvi_actual_receiver_index;       /* receive PPDU from Channel*/   lvp_ici = op_intrpt_ici();   0op_ici_attr_get(lvp_ici, "PPDU_ptr", &lvp_PPDU);   op_ici_destroy(lvp_ici);   lvp_ici = OPC_NIL;       ;printf("\n============================================\n");   8printf("|            receive_channel_PPDU         |\n");   kprintf("transmitter:%d, receiver:%d, \n", lvp_PPDU->transmitter_node_index, lvp_PPDU->receiver_node_index);   xprintf("power_linear:%lf, PPDU_sinr_dB:%lf\n", lvp_PPDU->power_linear, lvp_PPDU->actual_receiver_array[0].PPDU_sinr_dB);   Sprintf("start_time:%lf, end_time:%lf\n", lvp_PPDU->start_time, lvp_PPDU->end_time);   9printf("============================================\n");       /* free memory */   ~for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvp_PPDU->actual_receiver_number; lvi_actual_receiver_index++)   {   X	prg_list_free(lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].segment_sinr);   }   1op_prg_mem_free(lvp_PPDU->actual_receiver_array);   prg_mem_free(lvp_PPDU);   
                     
   ����   
          pr_state         Z            
   wait_sys_inited   
                                       ����             pr_state                      N       
    �            
   tr_0   
       ����          ����          
    ����   
          ����                       pr_transition              �   �     �    �   �          
   tr_1   
       
   PPDU_TIME_START   
       ����          
    ����   
          ����                       pr_transition              �   �        �  �            
   tr_2   
       ����          ����          
    ����   
          ����                       pr_transition              �  K     �    <  �          
   tr_3   
       
   PPDU_END   
       ����          
    ����   
          ����                       pr_transition              �  Z     2  �  �            
   tr_4   
       ����          ����          
    ����   
          ����                       pr_transition              p   �     �   �  �   �  R   �  s   �          
   tr_5   
       
   default   
       ����          
    ����   
          ����                       pr_transition                �         X                
   tr_6   
       
   
SYS_INITED   
       ����          
    ����   
          ����                       pr_transition               b   �      h     ~   �   :   �   G   �          
   tr_7   
       
   default   
       ����          
    ����   
          ����                       pr_transition                           PLC_func                    