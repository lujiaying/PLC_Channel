function [] = plot_phase_coupling_day(phase_coupling_parameter_matrix)
%    画出一天时间内，相位耦合系数的变化曲线
%
%    着重体现共线距离、相间耦合作为自变量的情况
%
%    Args:
%        phase_coupling_parameter_matrix: 相位耦合参数    

    %% 生成曲线
    y1 = [];    % 存储1,1相位的变化曲线
    y2 = [];    % 存储2,1相位的变化曲线, 节点3->2, para=0.1364 
    y3 = [];    % 存储2,1相位的变化曲线, 节点6->5, para=0.3644
    for t = 16:0.1:22
        phase_coupling_coefficient_matrix = phase_coupling_coefficient_init(phase_coupling_parameter_matrix);
        y1 = [y1, phase_coupling_coefficient_matrix(3,2,1,1)];
        y2 = [y2, phase_coupling_coefficient_matrix(3,2,2,1)];
        y3 = [y3, phase_coupling_coefficient_matrix(6,5,2,1)];       
    end
    
    %% 画图
    figure;
    x = 16:0.1:22;
    plot(x,y1,x,y2,x,y3);
    title('相位耦合系数随时间变化曲线');
    xlabel('时间(h)');
    ylabel('相位耦合参数');
    legend('同相', '不同相，共线距离短', '不同相，共线距离长');
end