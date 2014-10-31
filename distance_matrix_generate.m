function [distance_matrix] = distance_matrix_generate(distance_phase_matrix)
%    生成距离矩阵
%
%    Args:
%        distance_phase_matrix: 距离相位六元组矩阵
%
%    Returns:
%        distance_matrix: 距离矩阵，维数为HE_num+CPE_num

    %% 全局变量声明
    global HE_num CPE_num;
    
    distance_num = HE_num + CPE_num;
    
    %% 生成距离矩阵
    distance_matrix = zeros(distance_num);
    
    for i = 1:distance_num
        for j = 1:distance_num
            if i ~= j
                distance_matrix(i,j) = distance_phase_matrix{i,j}.dis_X +  distance_phase_matrix{i,j}.dis_Y + distance_phase_matrix{i,j}.dis_Z;
        
            end
        end
    end
    
    fprintf('[distance_matrix success] distance_matrix generate success!!\n');       
end