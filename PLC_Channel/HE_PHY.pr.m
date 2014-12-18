MIL_3_Tfile_Hdr_ 145A 140A modeler 9 548E3DDE 5492A862 9 lu-wspn lu 0 0 none none 0 0 none A7ABFED4 1E77 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                     ��g�      @   D   H      �  S  W  [  _  k  o  s  �           	   begsim intrpt             ����      doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����             int	\svi_HE_index;              #include "PLC_def.h"   #include "PLC_Channel.h"           d#define SYS_INIT			((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_SYS_INIT))   s#define PPDU_TIME_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_START))   p#define PPDU_END			((op_intrpt_type() == OPC_INTRPT_REMOTE) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_END))			                                                         
   init   
       J      Distribution *lvp_exp_dist;   double lvd_ppdu_time;       /* find self node index */   =for (svi_HE_index=0; svi_HE_index<gvi_HE_num; svi_HE_index++)   {   6	if (gvp_node_objid[svi_HE_index].PHY == op_id_self())   	{   		break;   	}   }   �//printf("svi_HE_index:%d, gvp_node_objid[svi_HE_index].PHY:%d, op_id_self:%d\n", svi_HE_index, gvp_node_objid[0].PHY, op_id_self());   if (svi_HE_index == gvi_HE_num)   {   p	op_sim_end("HE index exceed gvi_HE_num",  "Error source module: HE_PHY", "Error source state: init.enter", "");   }       /* schedule first PPDU */   1lvp_exp_dist = op_dist_load("exponential", 1, 0);   .lvd_ppdu_time = op_dist_outcome(lvp_exp_dist);   iprintf("[PHY.init] current time:%lf, first PPDU time:%lf\n", op_sim_time(), op_sim_time()+lvd_ppdu_time);   Pop_intrpt_schedule_self(op_sim_time()+lvd_ppdu_time, INTRPT_CHANNEL_PPDU_START);   J                     
   ����   
          pr_state        �            
   idle   
       
      printf("enter PHY.idle.\n");   
       
      char lvc_err_msg[25];       printf("Leave PHY.idle. ");       if (PPDU_TIME_START)   {   )	printf("receive INTRPT: PPDU_START.\n");   }   else if (PPDU_END)   {   .	printf("receive REMOTE INTRPT: PPDU_END.\n");   }   else if (INTRPT_CHANNEL_INITED)   {   A	printf("receive MCAST INTRPT: CHANNEL_INITED. Stay in idle.\n");   }   else   {   @	sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());   }	op_sim_end("Error: Unexpected INTRPT is received at \"PHY.idle.Exit\" state!", "Error source module: PHY", lvc_err_msg, "");   }   
           ����             pr_state        :   �          
   send_PPDU_2Channel   
       
      Ici *lvp_ici;   Distribution *lvp_dist;   double lvd_next_ppdu_time;   PPDU_T *lvp_ppdu;       /* send PPDU to channel*/    lvp_ici = op_ici_create("PPDU");   5lvp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));   lvp_ppdu->type = 0;   %lvp_ppdu->start_time = op_sim_time();   8lvp_ppdu->end_time = op_sim_time() + op_dist_uniform(5);   0lvp_ppdu->transmitter_node_index = svi_HE_index;   Ulvp_ppdu->receiver_node_index = gvi_HE_num + 1 + (int)(op_dist_uniform(gvi_CPE_num));   Ilvp_dist = op_dist_load("normal", 3.16, 1);    //mean power 35dBm = 3.16w   3lvp_ppdu->power_linear = op_dist_outcome(lvp_dist);   op_dist_unload(lvp_dist);   �printf("[PHY.send_PPDU_2Channel] transmitter:%d, receiver:%d, power_linear:%lf, start_time:%lf, end_time:%lf\n", lvp_ppdu->transmitter_node_index, lvp_ppdu->receiver_node_index, lvp_ppdu->power_linear, lvp_ppdu->start_time, lvp_ppdu->end_time);       /op_ici_attr_set(lvp_ici, "PPDU_ptr", lvp_ppdu);   op_ici_install(lvp_ici);   Sop_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_START, gvoid_channel);   op_ici_install(OPC_NIL);       /* schedule next PPDU */   -lvp_dist = op_dist_load("exponential", 1, 0);   /lvd_next_ppdu_time = op_dist_outcome(lvp_dist);   op_dist_unload(lvp_dist);   {printf("[PHY.send_PPDU_2Channel] current time:%lf, next PPDU time:%lf\n", op_sim_time(), op_sim_time()+lvd_next_ppdu_time);   Uop_intrpt_schedule_self(op_sim_time()+lvd_next_ppdu_time, INTRPT_CHANNEL_PPDU_START);   
                     
   ����   
          pr_state        :  �          
   receive_Channel_PPDU   
       
      Ici *lvp_ici;   PPDU_T *lvp_PPDU;       /* receive PPDU from Channel*/   lvp_ici = op_intrpt_ici();   0op_ici_attr_get(lvp_ici, "PPDU_ptr", &lvp_PPDU);   op_ici_destroy(lvp_ici);   lvp_ici = OPC_NIL;       ;printf("\n============================================\n");   8printf("|            receive_channel_PPDU         |\n");   kprintf("transmitter:%d, receiver:%d, \n", lvp_PPDU->transmitter_node_index, lvp_PPDU->receiver_node_index);   xprintf("power_linear:%lf, PPDU_sinr_dB:%lf\n", lvp_PPDU->power_linear, lvp_PPDU->actual_receiver_array[0].PPDU_sinr_dB);   Sprintf("start_time:%lf, end_time:%lf\n", lvp_PPDU->start_time, lvp_PPDU->end_time);   9printf("============================================\n");       /* free memory */   1op_prg_mem_free(lvp_PPDU->actual_receiver_array);   prg_mem_free(lvp_PPDU);   
                     
   ����   
          pr_state         Z            
   wait_sys_init   
                     
      char lvc_err_msg[25];       &printf("Leave HE_PHY.wait_sys_init ");       if (SYS_INIT)   {   -	printf("receive MCAST INTRPT: SYS_INIT.\n");   }   else   {   @	sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());   �	op_sim_end("Error: Unexpected INTRPT is received at \"HE_PHY.wait_sys_init.Exit\" state!", "Error source module: HE_PHY", lvc_err_msg, "");   }   
           ����             pr_state                      N       
    �            
   tr_0   
       ����          ����          
    ����   
          ����                       pr_transition              �   �     �    4   �          
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
          ����                       pr_transition              {   �     �   �  �   �  R   �  s   �          
   tr_5   
       
   default   
       ����          
    ����   
          ����                       pr_transition                �        Z                
   tr_6   
       
   SYS_INIT   
       ����          
    ����   
          ����                       pr_transition               Z   �      l     �   �   =   �   N            
   tr_7   
       
   default   
       ����          
    ����   
          ����                       pr_transition                                             