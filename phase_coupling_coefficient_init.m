function [phase_coupling_coefficient_matrix] = phase_coupling_coefficient_init(phase_coupling_parameter_matrix)
%    初始化相位耦合系数矩阵
%
%    相位耦合系数的均值为相位耦合参数，服从正态分布。
%
%    Args:
%        phase_coupling_parameter_matrix: 第五步生成的相位耦合参数矩阵
%
%    Returns:
%        phase_coupling_coefficient_matrix: 相位耦合系数矩阵，方阵维数HE_num+CPE_num
%          矩阵中元素为浮点数

    %% 全局变量声明
    global HE_num CPE_num;
    
    phase_num = HE_num + CPE_num;
    
    %% 相位耦合系数公式
    %  相位耦合系数 ~ N(相位耦合参数, 1^2)
    phase_coupling_coefficient_matrix = ones(phase_num);
    for i = 1:phase_num
        for j = 1:phase_num
            if i ~= j
                phase_coupling_coefficient_matrix(i, j) = normrnd(phase_coupling_parameter_matrix(i, j), 1);
                if phase_coupling_coefficient_matrix(i, j) < 0
                    phase_coupling_coefficient_matrix(i, j) = 0;
                end
            end
        end
    end
    fprintf('[phase_coupling_coefficient success] phase_coupling_coefficient_matrix init success!!\n');
    
end