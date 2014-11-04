function [] = plot_impedance_day(impedance_vector, impedance_correlation_matrix)
%    ����һ��ʱ���ڣ��迹�ı仯����
%
%    ��������ʱ������ԺͿռ������
%
%    Args:
%        impedance_vector: ��ʼ�����迹����
%        impedance_correlation_matrix: �迹�����ϵ������

    %% �迹����
    mean_idle = 90;
    deviation_idle = 4.5;
    mean_busy = 20;
    deviation_busy = 2.5;
    
    %% ��ʱ������迹����
    %  ʱ��12:00~22:00, ÿСʱ���10��
    %  ��ʱ12:00~16:00 ������16:00~18:00 æʱ18:00~22:00
        % æʱ����ʱ�Ĺ�ʽ: y(t) = a*x(t-1) + (1-a)^(1/2)*x(t)
    a = 0.75;
    b = sqrt(1-a^2);
        % �����ڵĹ�ʽ: y(t) = c*y(t-1) + (1-c)*x(t)
    c = 0.8;
    y = [];
    % TODO: ˼�� a*N(u,x) + (1-a)*N(u,x) �� a*N(u,x) + sqrt(1-a^2)*N(u,x)������
    %% ��ʱ
    for t=12:0.1:15.9
        impedance_vector_next_time = impedance_vector_init(mean_idle, deviation_idle);
        impedance_vector = a*(impedance_vector-mean_idle) + b*(impedance_vector_next_time-mean_idle) + mean_idle;
        impedance_space_vector = impedance_space_relate(impedance_vector, impedance_correlation_matrix);
        y = [y impedance_space_vector];
    end
    %% ������
    for t=16.0:0.1:17.9
        impedance_vector_next_time = impedance_vector_init(mean_busy, deviation_busy);
        impedance_vector = c*impedance_vector + (1-c)*impedance_vector_next_time;
        impedance_space_vector = impedance_space_relate(impedance_vector, impedance_correlation_matrix);
        y = [y impedance_space_vector];
    end
    
    %% æʱ
    for t=18.0:0.1:22.0
        impedance_vector_next_time = impedance_vector_init(mean_busy, deviation_busy);
        impedance_vector = a*(impedance_vector-mean_busy) + b*(impedance_vector_next_time-mean_busy) + mean_busy;
        impedance_space_vector = impedance_space_relate(impedance_vector, impedance_correlation_matrix);
        y = [y impedance_space_vector];       
    end
    
    %% ��ͼ
    x = 12.0 : 0.1: 22.0;
    plot(x, y(4,:), x, y(5,:), x, y(6,:));
    title('�迹��ʱ��仯����');
    xlabel('ʱ��(h)');
    ylabel('�迹(��)');
    legend('4�Žڵ�', '5�Žڵ�', '6�Žڵ�');
end