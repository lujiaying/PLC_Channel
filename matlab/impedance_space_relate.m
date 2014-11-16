function [impedance_space_vector] = impedance_space_relate(impedance_vector, impedance_correlation_matrix)
%    对阻抗向量进行空间相关化
%
%    Args:
%        impedance_vector: 当前时刻的时间相关化后的阻抗向量
%        impedance_correlation_matrix: 阻抗相关系数矩阵
%
%    Returns:
%        impedance_space_vector: 经过空间相关化后的阻抗向量, 向量维数和impedance_vector一样,HE_num+CPE_num
%          方阵中的元素为浮点数

    %% 全局变量声明
    global HE_num CPE_num;
    
    impedance_num = HE_num + CPE_num;
    
    %% 空间相关化公式
    % space_vector = R^(1/2) * vector, 其中R为HE_num+CPE_num的方阵，元素为空间相关系数
    %% 生成空间相关化后的阻抗
    impedance_space_vector = impedance_correlation_matrix * impedance_vector;
    fprintf('[impedance_space_relate success] impedance space vector generate success!\n');
end