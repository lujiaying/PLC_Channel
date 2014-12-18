MIL_3_Tfile_Hdr_ 145A 140A modeler 9 548E3FD3 54924435 16 lu-wspn lu 0 0 none none 0 0 none EEF0CA25 1462 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                    ��g�      @   D   H        >  B  F  J  V  Z  ^             	   begsim intrpt         
   ����   
   doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����                     #include "PLC_def.h"   #include "PLC_Channel.h"       o#define CHANNEL_INITED		((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_CHANNEL_INITED))       NODE_OID_T *gvp_node_objid;                                              Z   �          
   wait_channel_init   
       
      2printf("enter global.wait_channel_init.enter \n");   
       
      char lvc_err_msg[25];       4printf("Leave global_init.wait_channel_init.exit ");       if (CHANNEL_INITED)   {   3	printf("receive MCAST INTRPT: CHANNEL_INITED.\n");   }   else   {   @	sprintf(lvc_err_msg, "Error intrpt_code=%d", op_intrpt_code());   �	op_sim_end("Error: Unexpected INTRPT is received at \"global.wait_channel_init.Exit\" state!", "Error source module: CHANNEL", lvc_err_msg, "");   }   
       
    ����   
          pr_state           �          
   	NODE_init   
       J   .   int lvi_NODE_index;   char lvch_NODE_name[12];   char lvch_string[100];   "Objid lvoid_NODE_id, lvoid_subnet;       Qgvp_node_objid = (NODE_OID_T *)op_prg_mem_alloc(gvi_HE_num * sizeof(NODE_OID_T));   <lvoid_subnet = op_topo_parent(op_topo_parent(op_id_self()));       /* HE_init */   Gfor (lvi_NODE_index = 0; lvi_NODE_index < gvi_HE_num; lvi_NODE_index++)   {   	/* Objid node_id */   	memset(lvch_NODE_name, 0, 12);   6	sprintf(lvch_NODE_name, "HE_%d.PHY", lvi_NODE_index);   R	lvoid_NODE_id = op_id_from_name(lvoid_subnet, OPC_OBJTYPE_NDFIX, lvch_NODE_name);   A	printf("%d: lvoid_NODE_id=%d\n", lvi_NODE_index, lvoid_NODE_id);   (	if (lvoid_NODE_id == OPC_OBJID_INVALID)   	{   		memset(lvch_string, 0, 100);   G		sprintf(lvch_string, "Error: HE_%d can't be found!", lvi_NODE_index);   g		op_sim_end(lvch_string, "Error source module: global_init", "Error source state: HE_init.enter", "");   	}   	else   	{   5		gvp_node_objid[lvi_NODE_index].PHY = lvoid_NODE_id;   	}   }	       /* CPE_init */   \for (lvi_NODE_index = gvi_HE_num; lvi_NODE_index < gvi_HE_num+gvi_CPE_num; lvi_NODE_index++)   {   	/* Objid node_id */   	memset(lvch_NODE_name, 0, 12);   >	sprintf(lvch_NODE_name, "CPE_%d", lvi_NODE_index-gvi_HE_num);   R	lvoid_NODE_id = op_id_from_name(lvoid_subnet, OPC_OBJTYPE_NDFIX, lvch_NODE_name);   (	if (lvoid_NODE_id == OPC_OBJID_INVALID)   	{   		memset(lvch_string, 0, 100);   G		sprintf(lvch_string, "Error: HE_%d can't be found!", lvi_NODE_index);   g		op_sim_end(lvch_string, "Error source module: global_init", "Error source state: HE_init.enter", "");   	}   	else   	{   5		gvp_node_objid[lvi_NODE_index].PHY = lvoid_NODE_id;   	}   }	   J                     
   ����   
          pr_state        �   �          
   send_sys_init   
       
      char lvch_string[100];       memset(lvch_string, 0, 100);   Usprintf(lvch_string, "%d HEs and %d CPEs are initialized.", gvi_HE_num, gvi_CPE_num);   Dop_sim_message("Simulation initialization completes.", lvch_string);   @op_intrpt_schedule_mcast_global(op_sim_time(), INTRPT_SYS_INIT);   
                     
   ����   
          pr_state        �   �          
   idle   
                                       ����             pr_state                        �   �      V   �     �          
   tr_0   
       
   CHANNEL_INITED   
       ����          
    ����   
          ����                       pr_transition                 �        �  &   �  �   �  �   �          
   tr_5   
       
   default   
       ����          
    ����   
          ����                       pr_transition              �   �     �   �  �   �          
   tr_6   
       ����          ����          
    ����   
          ����                       pr_transition                 V   �      h   �   �   �   &   �   H   �          
   tr_7   
       
   default   
       ����          
    ����   
          ����                       pr_transition      	        C   �        �  w   �          
   tr_9   
       ����          ����          
    ����   
          ����                       pr_transition         
                                    