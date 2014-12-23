MIL_3_Tfile_Hdr_ 145A 140A modeler 9 5498CDEC 5498DA85 7 lu-wspn lu 0 0 none none 0 0 none 51260E5E 1A36 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                     ��g�      @   D   H                *  .  2             	   begsim intrpt             ����      doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����             Distribution *	\svp_exp_dist;        Distribution *	\svp_normal_dist;           
   #include "PLC_Channel.h"       s#define PPDU_TIME_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_START))   p#define PPDU_END			((op_intrpt_type() == OPC_INTRPT_REMOTE) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_END))			       #define PPDU_TIME_DELTA		1   #define PPDU_POWER_MEAN		398   #define PPDU_POWER_SD		1       Objid gvoid_PHY;                                              �            
   init   
       J   
   double lvd_ppdu_time;   ?svp_exp_dist = op_dist_load("exponential", PPDU_TIME_DELTA, 0);   ^svp_normal_dist = op_dist_load("normal", PPDU_POWER_MEAN, PPDU_POWER_SD);      //398mw = 26dBm       gvoid_PHY = op_id_self();       /* schedule first PPDU */   .lvd_ppdu_time = op_dist_outcome(svp_exp_dist);   iprintf("[PHY.init] current time:%lf, first PPDU time:%lf\n", op_sim_time(), op_sim_time()+lvd_ppdu_time);   Pop_intrpt_schedule_self(op_sim_time()+lvd_ppdu_time, INTRPT_CHANNEL_PPDU_START);   J                     
   ����   
          pr_state                    
   idle   
       
      printf("enter PHY.idle.\n");   
       
      char lvc_err_msg[25];       printf("Leave PHY.idle. ");       if (PPDU_TIME_START)   {   )	printf("receive INTRPT: PPDU_START.\n");   }   else if (PPDU_END)   {   .	printf("receive REMOTE INTRPT: PPDU_END.\n");   }   else if (INTRPT_CHANNEL_INITED)   {   A	printf("receive MCAST INTRPT: CHANNEL_INITED. Stay in idle.\n");   }   else   {   @	sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());   }	op_sim_end("Error: Unexpected INTRPT is received at \"PHY.idle.Exit\" state!", "Error source module: PHY", lvc_err_msg, "");   }   
           ����             pr_state        �   �          
   send_PPDU_2Channel   
       J      Ici *lvp_ici;   double lvd_next_ppdu_time;   PPDU_T *lvp_ppdu;       /* send PPDU to channel*/    lvp_ici = op_ici_create("PPDU");   // set PPDU attr   5lvp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));   lvp_ppdu->type = 0;   %lvp_ppdu->start_time = op_sim_time();   8lvp_ppdu->end_time = op_sim_time() + op_dist_uniform(5);   glvp_ppdu->transmitter_node_index = 1 + (int)(op_dist_uniform(gvi_CPE_num)+0.5);	//HE index start from 2   Llvp_ppdu->receiver_node_index = 1 + (int)(op_dist_uniform(gvi_CPE_num)+0.5);   :lvp_ppdu->power_linear = op_dist_outcome(svp_normal_dist);   �printf("[PHY.send_PPDU_2Channel] transmitter:%d, receiver:%d, power_linear:%lf, start_time:%lf, end_time:%lf\n", lvp_ppdu->transmitter_node_index, lvp_ppdu->receiver_node_index, lvp_ppdu->power_linear, lvp_ppdu->start_time, lvp_ppdu->end_time);       3op_ici_attr_set_ptr(lvp_ici, "PPDU_ptr", lvp_ppdu);   op_ici_install(lvp_ici);   Sop_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_START, gvoid_channel);   op_ici_install(OPC_NIL);       /* schedule next PPDU */   3lvd_next_ppdu_time = op_dist_outcome(svp_exp_dist);   {printf("[PHY.send_PPDU_2Channel] current time:%lf, next PPDU time:%lf\n", op_sim_time(), op_sim_time()+lvd_next_ppdu_time);   Uop_intrpt_schedule_self(op_sim_time()+lvd_next_ppdu_time, INTRPT_CHANNEL_PPDU_START);   J                     
   ����   
          pr_state        �  �          
   receive_Channel_PPDU   
       
      Ici *lvp_ici;   PPDU_T *lvp_PPDU;   int lvi_actual_receiver_index;       /* receive PPDU from Channel*/   lvp_ici = op_intrpt_ici();   0op_ici_attr_get(lvp_ici, "PPDU_ptr", &lvp_PPDU);   op_ici_destroy(lvp_ici);   lvp_ici = OPC_NIL;       ;printf("\n============================================\n");   8printf("|            receive_channel_PPDU         |\n");   kprintf("transmitter:%d, receiver:%d, \n", lvp_PPDU->transmitter_node_index, lvp_PPDU->receiver_node_index);   xprintf("power_linear:%lf, PPDU_sinr_dB:%lf\n", lvp_PPDU->power_linear, lvp_PPDU->actual_receiver_array[0].PPDU_sinr_dB);   Sprintf("start_time:%lf, end_time:%lf\n", lvp_PPDU->start_time, lvp_PPDU->end_time);   9printf("============================================\n");       /* free memory */   ~for (lvi_actual_receiver_index = 0; lvi_actual_receiver_index < lvp_PPDU->actual_receiver_number; lvi_actual_receiver_index++)   {   X	prg_list_free(lvp_PPDU->actual_receiver_array[lvi_actual_receiver_index].segment_sinr);   }   1op_prg_mem_free(lvp_PPDU->actual_receiver_array);   prg_mem_free(lvp_PPDU);   
                     
   ����   
          pr_state                        �        �                
   tr_0   
       ����          ����          
    ����   
          ����                       pr_transition              i   �         �   �          
   tr_1   
       
   PPDU_TIME_START   
       ����          
    ����   
          ����                       pr_transition              r   �     �   �              
   tr_2   
       ����          ����          
    ����   
          ����                       pr_transition              q  K         �  �          
   tr_3   
       
   PPDU_END   
       ����          
    ����   
          ����                       pr_transition              e  Z     �  �              
   tr_4   
       ����          ����          
    ����   
          ����                       pr_transition               �   �        �  #   �   �   �   �   �          
   tr_5   
       
   default   
       ����          
    ����   
          ����                       pr_transition                                             