MIL_3_Tfile_Hdr_ 145A 140A modeler 9 54846D7A 54923FA4 21 lu-wspn lu 0 0 none none 0 0 none F5994CC9 1DED 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                    ��g�      @   D   H      �  �  �  �  �  �  �  �  �           	   begsim intrpt             ����      doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����             int	\svi_CPE_index;              #include "PLC_def.h"   #include "PLC_Channel.h"           d#define SYS_INIT			((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_SYS_INIT))   s#define PPDU_TIME_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_START))   p#define PPDU_END			((op_intrpt_type() == OPC_INTRPT_REMOTE) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_END))			                                              Z            
   wait_sys_init   
       
       
       J      char lvc_err_msg[25];       #printf("Leave PHY.wait_sys_init ");       if (SYS_INIT)   {   -	printf("receive MCAST INTRPT: SYS_INIT.\n");   }   else   {   @	sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());   �	op_sim_end("Error: Unexpected INTRPT is received at \"PHY.wait_sys_init.Exit\" state!", "Error source module: PHY", lvc_err_msg, "");   }   J       
    ����   
          pr_state        �            
   idle   
       
      printf("enter PHY.idle.\n");   
       
      char lvc_err_msg[25];       printf("Leave PHY.idle. ");       if (PPDU_TIME_START)   {   )	printf("receive INTRPT: PPDU_START.\n");   }   else if (PPDU_END)   {   .	printf("receive REMOTE INTRPT: PPDU_END.\n");   }   else if (INTRPT_CHANNEL_INITED)   {   A	printf("receive MCAST INTRPT: CHANNEL_INITED. Stay in idle.\n");   }   else   {   @	sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());   }	op_sim_end("Error: Unexpected INTRPT is received at \"PHY.idle.Exit\" state!", "Error source module: PHY", lvc_err_msg, "");   }   
           ����             pr_state        :   �          
   send_PPDU_2Channel   
       
      Ici *lvp_ici;   Distribution *lvp_dist;   double lvd_next_ppdu_time;   PPDU_T *lvp_ppdu;       /* send PPDU to channel*/    lvp_ici = op_ici_create("PPDU");   5lvp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));   lvp_ppdu->type = 0;   %lvp_ppdu->start_time = op_sim_time();   8lvp_ppdu->end_time = op_sim_time() + op_dist_uniform(5);   1lvp_ppdu->transmitter_node_index = svi_CPE_index;   >lvp_ppdu->receiver_node_index = 1;    // for test, only one HE   Ilvp_dist = op_dist_load("normal", 3.16, 1);    //mean power 35dBm = 3.16w   3lvp_ppdu->power_linear = op_dist_outcome(lvp_dist);   op_dist_unload(lvp_dist);   �printf("[PHY.send_PPDU_2Channel] transmitter:%d, receiver:%d, power_linear:%lf, start_time:%lf, end_time:%lf\n", lvp_ppdu->transmitter_node_index, lvp_ppdu->receiver_node_index, lvp_ppdu->power_linear, lvp_ppdu->start_time, lvp_ppdu->end_time);       /op_ici_attr_set(lvp_ici, "PPDU_ptr", lvp_ppdu);   op_ici_install(lvp_ici);   Sop_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_START, gvoid_channel);   op_ici_install(OPC_NIL);       /* schedule next PPDU */   -lvp_dist = op_dist_load("exponential", 1, 0);   /lvd_next_ppdu_time = op_dist_outcome(lvp_dist);   op_dist_unload(lvp_dist);   {printf("[PHY.send_PPDU_2Channel] current time:%lf, next PPDU time:%lf\n", op_sim_time(), op_sim_time()+lvd_next_ppdu_time);   Uop_intrpt_schedule_self(op_sim_time()+lvd_next_ppdu_time, INTRPT_CHANNEL_PPDU_START);   
                     
   ����   
          pr_state        :  �          
   receive_Channel_PPDU   
       
      Ici *lvp_ici;   PPDU_T *lvp_PPDU;       /* receive PPDU from Channel*/   lvp_ici = op_intrpt_ici();   0op_ici_attr_get(lvp_ici, "PPDU_ptr", &lvp_PPDU);   op_ici_destroy(lvp_ici);   lvp_ici = OPC_NIL;       ;printf("\n============================================\n");   8printf("|            receive_channel_PPDU         |\n");   kprintf("transmitter:%d, receiver:%d, \n", lvp_PPDU->transmitter_node_index, lvp_PPDU->receiver_node_index);   xprintf("power_linear:%lf, PPDU_sinr_dB:%lf\n", lvp_PPDU->power_linear, lvp_PPDU->actual_receiver_array[0].PPDU_sinr_dB);   Sprintf("start_time:%lf, end_time:%lf\n", lvp_PPDU->start_time, lvp_PPDU->end_time);   9printf("============================================\n");       /* free memory */   1op_prg_mem_free(lvp_PPDU->actual_receiver_array);   prg_mem_free(lvp_PPDU);   
                     
   ����   
          pr_state                    
   init   
       
      Distribution *lvp_exp_dist;   double lvd_ppdu_time;       /* find self nodex index */   Ufor (svi_CPE_index=gvi_HE_num; svi_CPE_index<gvi_HE_num+gvi_CPE_num; svi_CPE_index++)   {   7	if (gvp_node_objid[svi_CPE_index].PHY == op_id_self())   	{   		break;   	}   }    if (svi_CPE_index == gvi_HE_num)   {   r	op_sim_end("CPE index exceed gvi_HE_num",  "Error source module: CPE_PHY", "Error source state: init.enter", "");   }       /* schedule first PPDU */   1lvp_exp_dist = op_dist_load("exponential", 1, 0);   .lvd_ppdu_time = op_dist_outcome(lvp_exp_dist);   iprintf("[PHY.init] current time:%lf, first PPDU time:%lf\n", op_sim_time(), op_sim_time()+lvd_ppdu_time);   Pop_intrpt_schedule_self(op_sim_time()+lvd_ppdu_time, INTRPT_CHANNEL_PPDU_START);   
                     
   ����   
          pr_state                     �   �     �    4   �          
   tr_1   
       
   PPDU_TIME_START   
       ����          
    ����   
          ����                       pr_transition              �   �     >   �  �            
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
          ����                       pr_transition                 [   �      j   �   �   �   1   �   L   �          
   tr_6   
       
   default   
       ����          
    ����   
          ����                       pr_transition              L           �            
   tr_8   
       ����          ����          
    ����   
          ����                       pr_transition      	          �        Y                
   tr_9   
       
   SYS_INIT   
       ����          
    ����   
          ����                       pr_transition         
                                    