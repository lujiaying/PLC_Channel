MIL_3_Tfile_Hdr_ 145A 140A modeler 9 547F0BD1 547F1E73 7 lu-wspn lu 0 0 none none 0 0 none 6CAFE88D FAD 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                      ��g�      @   D   H      �  �  �  �  �  �  �  �  |           	   begsim intrpt             ����      doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����             int	\svi_noise_index;       PPDU_T *	\svp_ppdu;              #include "PLC_Channel.h"       s#define NEXT_PULSE_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_NOISE_PULSE_START))   q#define POWER_UPDATE			((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_NOISE_POWER_UPDATE))                                              �   �          
   init   
       J      Ici *lvp_ici;   Distribution *lvp_normal_dist;   double lvd_pulse_time;       
/* init */   1svi_noise_index = *(int *)op_pro_argmem_access();   vprintf("[pulse_noise_generater.init] generate pulse_noise_generater process success, index is %d\n", svi_noise_index);       /* send PPDU to Channel */   ,lvp_ici = op_ici_create("PULSE_NOISE_PPDU");   5svp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));   1lvp_normal_dist = op_dist_load("normal", -22, 1);       svp_ppdu->type = 3;   :svp_ppdu->power_linear = op_dist_outcome(lvp_normal_dist);    op_dist_unload(lvp_normal_dist);   %svp_ppdu->start_time = op_sim_time();   *svp_ppdu->end_time = op_sim_time() + 0.01;   /op_ici_attr_set(lvp_ici, "PPDU_ptr", svp_ppdu);   op_ici_install(lvp_ici);   Sop_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_START, gvoid_channel);   op_ici_install(OPC_NIL);       /* schedule first pulse */   4lvp_normal_dist = op_dist_load("exponential", 1, 0);   2lvd_pulse_time = op_dist_outcome(lvp_normal_dist);   >printf("first pulse time: %lf", op_sim_time()+lvd_pulse_time);   Pop_intrpt_schedule_self(op_sim_time()+lvd_pulse_time, INTRPT_NOISE_PULSE_START);   J                     
   ����   
          pr_state        J   �          
   idle   
                                       ����             pr_state        �   Z          
   update_noise   
                                   
   ����   
          pr_state        �  J          J   set_next_pulse   J                                   
   ����   
          pr_state                        �   �      �   �  F   �          
   tr_0   
       ����          ����          
    ����   
          ����                       pr_transition              �   z     D   �  �   K          
   tr_5   
       J   POWER_UPDATE   J       ����          
    ����   
          ����                       pr_transition              �   �     �   k  ^   �          
   tr_6   
       ����          ����          
    ����   
          ����                       pr_transition              �  	     ^   �     @          
   tr_7   
       J   NEXT_PULSE_START   J       ����          
    ����   
          ����                       pr_transition              �       �  ^  P   �          
   tr_8   
       ����          ����          
    ����   
          ����                       pr_transition         	                                    