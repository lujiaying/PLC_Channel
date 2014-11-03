function [propagation_attenuation_matrix] = propagation_attenuation_generate(distance_phase_matrix)
%    生成传输衰减矩阵
%    
%    第三步 
%
%    Args:
%        distance_phase_matrix: 第二步生成的距离向量六元组矩阵
%
%    Returns:
%        propagation_attenuation_matrix: 传输衰减矩阵，方阵维数HE_num+CPE_num+noise_num
%          矩阵中的元素为浮点数

    %% 全局变量声明
    global HE_num CPE_num noise_num;
    
    attenu_num = HE_num + CPE_num + noise_num;
    
    %% 以M．Zimmermann信道模型的简化模型作为传输衰减模型
    %  A(f,d) = e^(-(a0 + a1*f^k)*d)
    a0= 0;
    a1 = 8.75e-10;
    k= 1;
    f = 5e6;    %频率5MHz

    %% 矩阵生成过程
    propagation_attenuation_matrix = ones(attenu_num);
    for i = 1:attenu_num
        for j = 1:attenu_num
            if i ~= j
                d = distance_phase_matrix{i, j}.dis_X + distance_phase_matrix{i, j}.dis_Y + distance_phase_matrix{i, j}.dis_Z;
                propagation_attenuation_matrix(i, j) = exp(-(a0+a1*f^k)*d);
            end
        end
    end
    
    fprintf('[propagation_attenuation success] propagation_attenuation_matrix generate success!!\n');
end