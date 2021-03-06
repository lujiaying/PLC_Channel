MIL_3_Tfile_Hdr_ 145A 140A modeler 9 549FC9EE 54A1FE63 15 lu-wspn lu 0 0 none none 0 0 none 7E8894D4 21E2 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                    ��g�      @   D   H      	  b  f  �  �  �  �  �  �           	   begsim intrpt             ����      doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����             PPDU_T *	\svp_ppdu;       double	\svd_pulse_start_time;       double	\svd_pulse_start_power;       int	\svi_node_index;       PHASE_T	\sve_work_phase;        Distribution *	\svp_normal_dist;       Distribution *	\svp_exp_dist;       Stathandle	\svstat_power;              #include "PLC_def.h"   #include "PLC_Channel.h"   #include "PLC_func.h"       g#define SYS_INITED				((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_SYS_INIT))   s#define NEXT_PULSE_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_NOISE_PULSE_START))   q#define POWER_UPDATE			((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_NOISE_POWER_UPDATE))        #define NOISE_POWER_MEAN			0.006    #define NOISE_POWER_MIN				0.001   !#define NOISE_PULSE_TIME_DELTA		1   &#define NOISE_UPDATE_TIME_DELTA		0.001   #define PULSE_TIME_MIN				0.020   (#define NOISE_POWER_UPDATE_PARA		0.00075      >/************************************************************/   >/* Author: jiaying.lu                                       */   >/* Last Update: 2014.12.28                                  */    /* Remarks:        											*/   >/************************************************************/   static void   PPDU_attr_set(PPDU_T *ppdu)   {   	FIN(PPDU_attr_set());   	   	// set PPDU attr   	ppdu->type = PPDU_TYPE_NOISE;   "	ppdu->start_time = op_sim_time();   	ppdu->end_time = -1;   /	ppdu->transmitter_node_index = svi_node_index;   	ppdu->receiver_node_index = 0;   7	ppdu->power_linear = op_dist_outcome(svp_normal_dist);   *	if (ppdu->power_linear < NOISE_POWER_MIN)   	{   '		ppdu->power_linear = NOISE_POWER_MIN;   	}   #	ppdu->send_phase = sve_work_phase;   	   	FOUT;   }                                         �   �          
   init   
       J       Ici *lvp_ici;   double lvd_pulse_time;   3Objid lvoid_NODE_id = op_topo_parent(op_id_self());       Vsvp_normal_dist = op_dist_load("normal", NOISE_POWER_MEAN, 1);    // 0.006 mw = -22dBm   Fsvp_exp_dist = op_dist_load("exponential", NOISE_PULSE_TIME_DELTA, 0);       esvstat_power = op_stat_reg("NOISE PHY.Pulse Noise Power (dBm)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);       /* find self index */   8svi_node_index = _global_self_index_find(lvoid_NODE_id);   /* set PHY Objid */   6gvoid_node_oids[svi_node_index].phy_id = op_id_self();   /* get work phase */   Bop_ima_obj_attr_get(lvoid_NODE_id, "work_phase", &sve_work_phase);   \printf("[pulse_noise.init] NODE index:%d, work_phase:%d\n", svi_node_index, sve_work_phase);        /* send noise PPDU to Channel */    lvp_ici = op_ici_create("PPDU");   5svp_ppdu = (PPDU_T *)prg_mem_alloc(1*sizeof(PPDU_T));   PPDU_attr_set(svp_ppdu);       /op_ici_attr_set(lvp_ici, "PPDU_ptr", svp_ppdu);   op_ici_install(lvp_ici);   Sop_intrpt_schedule_remote(op_sim_time(), INTRPT_CHANNEL_PPDU_START, gvoid_channel);   op_ici_install(OPC_NIL);           /* schedule first pulse */   /lvd_pulse_time = op_dist_outcome(svp_exp_dist);   aprintf("current time:%lf, first pulse time: %lf\n", op_sim_time(), op_sim_time()+lvd_pulse_time);   Pop_intrpt_schedule_self(op_sim_time()+lvd_pulse_time, INTRPT_NOISE_PULSE_START);   J                     
   ����   
          pr_state        �   �          
   idle   
                     
      char lvc_err_msg[25];       -printf("Leave pulse_noise_generater.idle. ");       if (POWER_UPDATE)   {   ?	//printf("receive INTRPT: POWER_UPDATE. Enter next state.\n");   	;   }   else if (NEXT_PULSE_START)   {   A	printf("receive INTRPT: NEXT_PULSE_START. Enter next state.\n");   }   else   {   @	sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());   �	op_sim_end("Error: Unexpected INTRPT is received at \"pulse_noise_generater.idle.Exit\" state!", "Error source module: CHANNEL", lvc_err_msg, "");   }   
           ����             pr_state        :   Z          
   update_noise   
       J      /* update noise power */   ssvp_ppdu->power_linear = exp((svd_pulse_start_time-op_sim_time())/NOISE_POWER_UPDATE_PARA) * svd_pulse_start_power;   z//printf("[noise %d] current time:%lf, update pulse power: %lf\n", svi_node_index, op_sim_time(), svp_ppdu->power_linear);   >op_stat_write(svstat_power, 10*log10(svp_ppdu->power_linear));       &/* schedule next noise power update */   // after 20ms, power drop to 0.   ;if ((op_sim_time()-svd_pulse_start_time) <= PULSE_TIME_MIN)   {   ^    op_intrpt_schedule_self(op_sim_time()+NOISE_UPDATE_TIME_DELTA, INTRPT_NOISE_POWER_UPDATE);   }   else   {   �	printf("[noise %d] current time:%lf, and pulse start time:%lf. Already passed 20ms.\n", svi_node_index, op_sim_time(), svd_pulse_start_time);   }   J                     
   ����   
          pr_state        :  J          
   set_next_pulse   
       J      double lvd_pulse_time;        /* set PPDU current pulse attr*/   :svp_ppdu->power_linear = op_dist_outcome(svp_normal_dist);   /svd_pulse_start_power = svp_ppdu->power_linear;   Uprintf("[node %d] start pulse power: %lf\n", svi_node_index, svp_ppdu->power_linear);   >op_stat_write(svstat_power, 10*log10(svp_ppdu->power_linear));           !/* schedule noise power update */   %svd_pulse_start_time = op_sim_time();   Zprintf("[node %d] current pulse start time: %lf, ", svi_node_index, svd_pulse_start_time);   Iop_intrpt_schedule_self(svd_pulse_start_time, INTRPT_NOISE_POWER_UPDATE);           /* schedule next pulse */   /lvd_pulse_time = op_dist_outcome(svp_exp_dist);   Qif (lvd_pulse_time < PULSE_TIME_MIN)			//next pulse time should gt PULSE_TIME_MIN   {   !	lvd_pulse_time = PULSE_TIME_MIN;   }   @printf(" next pulse time: %lf\n", op_sim_time()+lvd_pulse_time);   Pop_intrpt_schedule_self(op_sim_time()+lvd_pulse_time, INTRPT_NOISE_PULSE_START);   J                     
   ����   
          pr_state         Z   �          
   wait_sys_inited   
                                       ����             pr_state                      +   �      �   �  �   �          
   tr_0   
       ����          ����          
    ����   
          ����                       pr_transition              �   z     �   �  )   K          
   tr_5   
       
   POWER_UPDATE   
       ����          
    ����   
          ����                       pr_transition              �   �     3   k  �   �          
   tr_6   
       ����          ����          
    ����   
          ����                       pr_transition                	     �   �  <  @          
   tr_7   
       
   NEXT_PULSE_START   
       ����          
    ����   
          ����                       pr_transition              �       0  ^  �   �          
   tr_8   
       ����          ����          
    ����   
          ����                       pr_transition      	        _   ;     �   �  �   F  ?   c  y   �          
   tr_9   
       
   default   
       ����          
    ����   
          ����                       pr_transition      
         [   h      h   �   ~   �   :      G   �          
   tr_10   
       
   default   
       ����          
    ����   
          ����                       pr_transition                �   �      Y   �   �   �          
   tr_11   
       ����          ����          
    ����   
          ����                       pr_transition                   Pulse Noise Power (dBm)           	NOISE PHY����   discrete        ԲI�%��}          PLC_func                    