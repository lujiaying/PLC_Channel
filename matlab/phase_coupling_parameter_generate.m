function [phase_coupling_parameter_matrix] = phase_coupling_parameter_generate(distance_phase_matrix)
%    生成相位耦合参数矩阵
%
%    Args:
%        distance_phase_matrix: 第二步生成的距离向量六元组矩阵
%
%    Returns:
%        phase_coupling_parameter_matrix: 相位耦合参数矩阵，方阵维数HE_num+CPE_num
%          矩阵中的元素为浮点数

    %% 全局变量声明
    global HE_num CPE_num;
    
    coupling_num = HE_num + CPE_num;

    %% 耦合系数计算公式
    % c = k*dis_Y / (dis_x + dis_Y + dis_Z)
    k = 0.5;
    
    %% 生成相位耦合参数矩阵
    phase_coupling_parameter_matrix = ones(coupling_num);
    
    for i = 1:coupling_num
        for j = 1:coupling_num
            if i ~= j
                phase_coupling_parameter_matrix(i, j) = k * distance_phase_matrix{i, j}.dis_Y / (distance_phase_matrix{i, j}.dis_X+distance_phase_matrix{i, j}.dis_Y+distance_phase_matrix{i, j}.dis_Z);
            end
        end
    end
    
    fprintf('[phase_coupling_parameter success] phase_coupling_parameter_matrix generate success!!\n');
end