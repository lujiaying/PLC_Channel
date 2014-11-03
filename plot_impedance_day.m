function [] = plot_impedance_day(impedance_vector, impedance_correlation_matrix)
%    画出一天时间内，阻抗的变化曲线
%
%    Args:
%        impedance_vector: 初始化的阻抗向量
%        impedance_correlation_matrix: 阻抗的相关系数矩阵

    %% 全局变量声明
    global HE_num CPE_num noise_num;
    
    impedance_num = HE_num + CPE_num + noise_num;
    
    %% 随时间更新阻抗向量
    %  时长12:00~22:00, 每小时描点10个
    %  闲时12:00~16:00 过渡期16:00~18:00 忙时18:00~22:00
    %  忙时和闲时的公式: y(t) = a*x(t-1) + (1-a)^(1/2)*x(t)
    a = 0.75;
    b = sqrt(1-a^2);
    y = [];
    impedance_vector_next_time = impedance_vector_init();
    % TODO: 思考 a*N(u,x) + (1-a)*N(u,x) 与 a*N(u,x) + sqrt(1-a^2)*N(u,x)的区别
    for t=12:0.1:15.9
        impedance_vector_now = impedance_vector_next_time;
        impedance_vector_next_time = impedance_vector_init();
        impedance_vector = a*impedance_vector_now + b*impedance_vector_next_time;
        impedance_space_vector = impedance_space_relate(impedance_vector, impedance_correlation_matrix);
        y = [y impedance_space_vector];
    end
    
    x = 12:0.1:15.9;
    plot(x, y);
    
end