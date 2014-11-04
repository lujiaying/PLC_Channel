function [] = plot_impedance_day(impedance_vector, impedance_correlation_matrix)
%    画出一天时间内，阻抗的变化曲线
%
%    着重体现时间相关性和空间相关性
%
%    Args:
%        impedance_vector: 初始化的阻抗向量
%        impedance_correlation_matrix: 阻抗的相关系数矩阵

    %% 阻抗参数
    mean_idle = 90;
    deviation_idle = 4.5;
    mean_busy = 20;
    deviation_busy = 2.5;
    
    %% 随时间更新阻抗向量
    %  时长12:00~22:00, 每小时描点10个
    %  闲时12:00~16:00 过渡期16:00~18:00 忙时18:00~22:00
        % 忙时和闲时的公式: y(t) = a*x(t-1) + (1-a)^(1/2)*x(t)
    a = 0.75;
    b = sqrt(1-a^2);
        % 过渡期的公式: y(t) = c*y(t-1) + (1-c)*x(t)
    c = 0.8;
    y = [];
    % TODO: 思考 a*N(u,x) + (1-a)*N(u,x) 与 a*N(u,x) + sqrt(1-a^2)*N(u,x)的区别
    %% 闲时
    for t=12:0.1:15.9
        impedance_vector_next_time = impedance_vector_init(mean_idle, deviation_idle);
        impedance_vector = a*(impedance_vector-mean_idle) + b*(impedance_vector_next_time-mean_idle) + mean_idle;
        impedance_space_vector = impedance_space_relate(impedance_vector, impedance_correlation_matrix);
        y = [y impedance_space_vector];
    end
    %% 过渡期
    for t=16.0:0.1:17.9
        impedance_vector_next_time = impedance_vector_init(mean_busy, deviation_busy);
        impedance_vector = c*impedance_vector + (1-c)*impedance_vector_next_time;
        impedance_space_vector = impedance_space_relate(impedance_vector, impedance_correlation_matrix);
        y = [y impedance_space_vector];
    end
    
    %% 忙时
    for t=18.0:0.1:22.0
        impedance_vector_next_time = impedance_vector_init(mean_busy, deviation_busy);
        impedance_vector = a*(impedance_vector-mean_busy) + b*(impedance_vector_next_time-mean_busy) + mean_busy;
        impedance_space_vector = impedance_space_relate(impedance_vector, impedance_correlation_matrix);
        y = [y impedance_space_vector];       
    end
    
    %% 画图
    x = 12.0 : 0.1: 22.0;
    plot(x, y(4,:), x, y(5,:), x, y(6,:));
    title('阻抗随时间变化曲线');
    xlabel('时间(h)');
    ylabel('阻抗(Ω)');
    legend('4号节点', '5号节点', '6号节点');
end