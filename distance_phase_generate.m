function [distance_phase_matrix] = distance_phase_generate(node_vector)
%     生成距离六元组矩阵
%     
%     Args:
%       node_vector: 第一步初始化得到的网络拓扑向量
%  
%     Returns:
%       distance_phase_matrix: 距离向量六元祖矩阵，方阵维数HE_num+CPE_num+noise_num
%         矩阵中的元素为distance_phase结构体

    % 全局变量声明
    global HE_num CPE_num noise_num X_num;
    
    distance_phase_matrix = ones(HE_num+CPE_num+noise_num, HE_num+CPE_num+noise_num);
    