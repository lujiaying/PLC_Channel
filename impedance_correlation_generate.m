function [impedance_correlation_matrix] = impedance_correlation_generate(distance_phase_matrix)
%    生成阻抗相关系数矩阵
%
%    Args:
%        distance_phase_matrix: 第二步生成的距离向量六元组矩阵
%
%    Returns:
%        impedance_correlation_matrix: 阻抗相关系数矩阵，方阵维数HE_num+CPE_num+noise_num
%          矩阵中的元素为浮点数

    %% 全局变量声明
    global HE_num CPE_num noise_num;
    
    correlation_num = HE_num + CPE_num + noise_num;
    
    %% 阻抗相关系数计算公式
    %  c = e^(-d/a)
    a = 30;
    
    %% 矩阵生成过程
    impedance_correlation_matrix = zeros(correlation_num);
    for i = 1:correlation_num
        for j = 1:correlation_num
            if i ~= j
                d = distance_phase_matrix{i, j}.dis_X + distance_phase_matrix{i, j}.dis_Y + distance_phase_matrix{i, j}.dis_Z;
                impedance_correlation_matrix(i, j) = exp(-d/a);
            end
        end
    end
    
    fprintf('[impedance_correlation success] impedance_correlation_matrix generate success!!\n');
end
