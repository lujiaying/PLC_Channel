function [impedance_vector] = impedance_vector_init()   
%     初始化阻抗向量
%     
%     后续：在仿真中随着时间的推进，对阻抗向量进行更新。
%     
%     Args:
%         None
%         
%     Returns:
%         impedance_vector: 阻抗向量，向量维数HE_num+CPE_num
%           方阵中的元素为浮点数

    %% 全局变量声明
    global HE_num CPE_num;
    
    %% 
    impedance_vector = zeros(HE_num+CPE_num);
    
    
end
