function [impedance_vector] = impedance_vector_init(mean, std_deviation)
%    初始化阻抗向量
%
%    Args:
%        mean: 阻抗的均值
%        std_deviation: 阻抗的标准差
%
%    Returns:
%        impedance_vector: 阻抗向量，向量维数HE_num+CPE_num
%          方阵中的元素为浮点数    

    %% 全局变量声明
    global HE_num CPE_num;
    
    impedance_num = HE_num + CPE_num;
    
    %% 阻抗初始化公式
    % idle ~ N(90, 2.5^2)    busy ~ N(20, 4.5^2)
%     mean_idle = 90;
%     deviation_idle = 4.5;
%     mean_busy = 20;
%     deviation_busy = 2.5;
    
    %% 生成阻抗向量
    impedance_vector = zeros(impedance_num,1);
    
    for i = 1:impedance_num
        impedance_vector(i) = normrnd(mean, std_deviation);
    end
    fprintf('[impedance_vector success] impedance_vector init success!!\n');
    
end
