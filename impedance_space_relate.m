function [impedance_space_vector] = impedance_space_relate(impedance_vector, distance_matrix)
%    对阻抗向量进行空间相关化
%
%    Args:
%        impedance_vector: 当前时刻的时间相关化后的阻抗向量
%        distance_matrix: 距离矩阵
%
%    Returns:
%        impedance_space_vector: 经过空间相关化后的阻抗向量, 向量维数和impedance_vector一样,HE_num+CPE_num
%          方阵中的元素为浮点数

    %% 全局变量声明
    global HE_num CPE_num;
    
    impedance_num = HE_num + CPE_num;
    
    %% 空间相关化公式
    % space_vector = R * vector, 其中R为HE_num+CPE_num的方阵，元素为空间相关系数s
    % s = e^(-d/a)     
    a = 15;
    
    %% 生成空间相关化后的阻抗
    % 生成空间相关矩阵
    space_relate_matrix = ones(impedance_num);
    for i = 1:impedance_num
        for j = 1:impedance_num
            space_relate_matrix(i,j) = exp(-distance_matrix(i,j)/a);
        end
    end
    fprintf('[impedance_space_relate success] space_relate_matrix generate success!\n');

end