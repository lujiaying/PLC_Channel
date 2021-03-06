MIL_3_Tfile_Hdr_ 145A 140A modeler 9 547F0BD1 549CCEF8 22 lu-wspn lu 0 0 none none 0 0 none 1BCEC925 19A0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                    ��g�      @   D   H      �  |  �  �  �  �  �  �  �           	   begsim intrpt             ����      doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����             int	\svi_noise_index;       PPDU_T *	\svp_ppdu;       double	\svd_pulse_start_time;       double	\svd_pulse_start_power;              #include "PLC_def.h"   #include "PLC_Channel.h"       s#define NEXT_PULSE_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_NOISE_PULSE_START))   q#define POWER_UPDATE			((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_NOISE_POWER_UPDATE))                                              �   �          
   init   
       
      Ici *lvp_ici;   Distribution *lvp_exp_dist;   double lvd_pulse_time;       
/* init */   1svi_noise_index = *(int *)op_pro_argmem_access();   wprintf("[pulse_noise_generater.init] generate pulse_noise_generater process success, index is %d.  ", svi_noise_index);            /* send noise PPDU to Channel */    lvp_ici = op_ici_create("PPDU");   5svp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));   svp_ppdu->type = 3;   %svp_ppdu->start_time = op_sim_time();   svp_ppdu->end_time = -1;   'svp_ppdu->PPDU_index = svi_noise_index;   Rsvp_ppdu->transmitter_node_index = 1 + gvi_HE_num + gvi_CPE_num + svi_noise_index;       /op_ici_attr_set(lvp_ici, "PPDU_ptr", svp_ppdu);   op_ici_install(lvp_ici);   Sop_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_START, gvoid_channel);   op_ici_install(OPC_NIL);           /* schedule first pulse */   1lvp_exp_dist = op_dist_load("exponential", 1, 0);   /lvd_pulse_time = op_dist_outcome(lvp_exp_dist);   aprintf("current time:%lf, first pulse time: %lf\n", op_sim_time(), op_sim_time()+lvd_pulse_time);   Pop_intrpt_schedule_self(op_sim_time()+lvd_pulse_time, INTRPT_NOISE_PULSE_START);   op_dist_unload(lvp_exp_dist);   
                     
   ����   
          pr_state        J   �          
   idle   
                     
      char lvc_err_msg[25];       -printf("Leave pulse_noise_generater.idle. ");       if (POWER_UPDATE)   {   =	printf("receive INTRPT: POWER_UPDATE. Enter next state.\n");   }   else if (NEXT_PULSE_START)   {   A	printf("receive INTRPT: NEXT_PULSE_START. Enter next state.\n");   }   else   {   @	sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());   �	op_sim_end("Error: Unexpected INTRPT is received at \"pulse_noise_generater.idle.Exit\" state!", "Error source module: CHANNEL", lvc_err_msg, "");   }   
           ����             pr_state        �   Z          
   update_noise   
       J      double ne_exponential_para;       /* update noise power */   ne_exponential_para = 0.00075;   osvp_ppdu->power_linear = exp((svd_pulse_start_time-op_sim_time())/ne_exponential_para) * svd_pulse_start_power;   {//printf("[noise %d] current time:%lf, update pulse power: %lf\n", svi_noise_index, op_sim_time(), svp_ppdu->power_linear);       &/* schedule next noise power update */   // after 20ms, power drop to 0.   2if ((op_sim_time()-svd_pulse_start_time) <= 0.020)   {   L    op_intrpt_schedule_self(op_sim_time()+0.001, INTRPT_NOISE_POWER_UPDATE);   }   else   {   �	printf("[noise %d] current time:%lf, and pulse start time:%lf. Already passed 20ms.\n", svi_noise_index, op_sim_time(), svd_pulse_start_time);   }   J                     
   ����   
          pr_state        �  J          
   set_next_pulse   
       
      Distribution *lvp_dist;   double lvd_pulse_time;            /* set PPDU current pulse attr*/   Ilvp_dist = op_dist_load("normal", 158.4893, 1);    // 158.4893 mw = 22dBm   3svp_ppdu->power_linear = op_dist_outcome(lvp_dist);   /svd_pulse_start_power = svp_ppdu->power_linear;   op_dist_unload(lvp_dist);   Wprintf("[noise %d] start pulse power: %lf\n", svi_noise_index, svp_ppdu->power_linear);           !/* schedule noise power update */   %svd_pulse_start_time = op_sim_time();   \printf("[noise %d] current pulse start time: %lf, ", svi_noise_index, svd_pulse_start_time);   Iop_intrpt_schedule_self(svd_pulse_start_time, INTRPT_NOISE_POWER_UPDATE);           /* schedule next pulse */   -lvp_dist = op_dist_load("exponential", 1, 0);   +lvd_pulse_time = op_dist_outcome(lvp_dist);   @printf(" next pulse time: %lf\n", op_sim_time()+lvd_pulse_time);   Pop_intrpt_schedule_self(op_sim_time()+lvd_pulse_time, INTRPT_NOISE_PULSE_START);   op_dist_unload(lvp_dist);   
                     
   ����   
          pr_state                        �   �      �   �  F   �          
   tr_0   
       ����          ����          
    ����   
          ����                       pr_transition              �   z     D   �  �   K          
   tr_5   
       
   POWER_UPDATE   
       ����          
    ����   
          ����                       pr_transition              �   �     �   k  ^   �          
   tr_6   
       ����          ����          
    ����   
          ����                       pr_transition              �  	     ^   �     @          
   tr_7   
       
   NEXT_PULSE_START   
       ����          
    ����   
          ����                       pr_transition              �       �  ^  P   �          
   tr_8   
       ����          ����          
    ����   
          ����                       pr_transition      	        #   ;     U   �  U   F     c  =   �          
   tr_9   
       
   default   
       ����          
    ����   
          ����                       pr_transition         
                                    