MIL_3_Tfile_Hdr_ 145A 140A modeler 9 547C1F9B 547E6597 14 lu-wspn lu 0 0 none none 0 0 none 86C54CFF 1266 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                    ��g�      @   D   H      �  B  F  J  N  Z  ^  b  �           	   begsim intrpt         
   ����   
   doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����                     #include "PLC_Channel.h"       o#define CHANNEL_INITED		((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_CHANNEL_INITED))   s#define PPDU_NEXT_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_START))   j#define PPDU_END		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_END))       Objid gvoid_noise_generater;                                              �   �          
   wait_channel_init   
                                   
    ����   
          pr_state        �   �          
   idle   
       
       
                         ����             pr_state        J   �          
   init   
       J   
   #Distribution *lvp_exponential_dist;   double lvd_send_time;       %gvoid_noise_generater = op_id_self();       /* set FIRST PPDU send time */   9lvp_exponential_dist = op_dist_load("exponential", 1, 0);   Dlvd_send_time = op_sim_time()+op_dist_outcome(lvp_exponential_dist);   Bop_intrpt_schedule_self(lvd_send_time, INTRPT_CHANNEL_PPDU_START);   Mprintf("[noise_generater.init] first PPDU send time = %lf\n", lvd_send_time);   J                     
   ����   
          pr_state        �   Z          
   receive_PPDU   
                                   
   ����   
          pr_state        �  J          
   	send_PPDU   
       J      /* send PPDU to Channel */   Ici *lvp_ici;   PPDU_T *lvp_ppdu;   Distribution *lvp_normal_dist;       ,lvp_ici = op_ici_create("PULSE_NOISE_PPDU");   5lvp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));   1lvp_normal_dist = op_dist_load("normal", -22, 1);       lvp_ppdu->type = 3;   :lvp_ppdu->power_linear = op_dist_outcome(lvp_normal_dist);   %lvp_ppdu->start_time = op_sim_time();   *lvp_ppdu->end_time = op_sim_time() + 0.01;   /op_ici_attr_set(lvp_ici, "PPDU_ptr", lvp_ppdu);   op_ici_install(lvp_ici);   Sop_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_START, gvoid_channel);   op_ici_install(OPC_NIL);   J                     
   ����   
          pr_state                       �   �      �   �  M   �          
   tr_1   
       
   CHANNEL_INITED   
       ����          
    ����   
          ����                       pr_transition              �   �     D   �  �   �          
   tr_2   
       ����          ����          
    ����   
          ����                       pr_transition                 �   h      �   �   �   }   o   }   �   �          
   tr_3   
       
   default   
       ����          
    ����   
          ����                       pr_transition              L           �  �   O          
   tr_5   
       
   PPDU_END   
       ����          
    ����   
          ����                       pr_transition              `   �     �   i     �          
   tr_6   
       ����          ����          
    ����   
          ����                       pr_transition              l          �  �  ;          
   tr_7   
       J   PPDU_NEXT_START   J       ����          
    ����   
          ����                       pr_transition              S       �  V     �          
   tr_8   
       ����          ����          
    ����   
          ����                       pr_transition      	        �   r     �   �  �   }  �   �  �   �          
   tr_9   
       
   default   
       ����          
    ����   
          ����                       pr_transition         
                                    