function [] = plot_impedance_day(impedance_vector, impedance_correlation_matrix)
%    ����һ��ʱ���ڣ��迹�ı仯����
%
%    Args:
%        impedance_vector: ��ʼ�����迹����
%        impedance_correlation_matrix: �迹�����ϵ������

    %% ȫ�ֱ�������
    global HE_num CPE_num noise_num;
    
    impedance_num = HE_num + CPE_num + noise_num;
    
    %% ��ʱ������迹����
    %  ʱ��12:00~22:00, ÿСʱ���10��
    %  ��ʱ12:00~16:00 ������16:00~18:00 æʱ18:00~22:00
    %  æʱ����ʱ�Ĺ�ʽ: y(t) = a*x(t-1) + (1-a)^(1/2)*x(t)
    a = 0.75;
    b = sqrt(1-a^2);
    y = [];
    impedance_vector_next_time = impedance_vector_init();
    % TODO: ˼�� a*N(u,x) + (1-a)*N(u,x) �� a*N(u,x) + sqrt(1-a^2)*N(u,x)������
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