MIL_3_Tfile_Hdr_ 145A 140A modeler 9 547F0BF2 5481662C 5 lu-wspn lu 0 0 none none 0 0 none DCFC5B91 C2F 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                      ��g�      @   D   H      �  	�  
  
  
  
#  
'  
+  �           	   begsim intrpt         
   ����   
   doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����                     #include "PLC_Channel.h"       o#define CHANNEL_INITED		((op_intrpt_type() == OPC_INTRPT_MCAST) && (op_intrpt_code() == INTRPT_CHANNEL_INITED))   s#define PPDU_NEXT_START		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_START))   j#define PPDU_END		((op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code() == INTRPT_CHANNEL_PPDU_END))       Objid gvoid_noise_generater;                                              �   �          
   wait_channel_init   
                                   
    ����   
          pr_state        �   �          
   idle   
       
       
                         ����             pr_state        J   �          
   init   
       
      int lvi_noise_index;   Prohandle lvpro_child_process;   	   %gvoid_noise_generater = op_id_self();       Tprintf("[pulse_noise_manager.init] start generate pulse_noise_generater process\n");   Ifor (lvi_noise_index=0; lvi_noise_index<gvi_NOISE_num; lvi_noise_index++)   {   G	lvpro_child_process = op_pro_create("pulse_noise_generater", OPC_NIL);   6	op_pro_invoke(lvpro_child_process, &lvi_noise_index);   }   
                     
   ����   
          pr_state                       �   �      �   �  M   �          
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
          ����                       pr_transition      
            m        �  '   �  �   �  �   �          
   tr_10   
       
   default   
       ����          
    ����   
          ����                       pr_transition               pulse_noise_generater                                