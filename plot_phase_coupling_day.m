function [] = plot_phase_coupling_day(phase_coupling_parameter_matrix)
%    ����һ��ʱ���ڣ���λ���ϵ���ı仯����
%
%    �������ֹ��߾��롢��������Ϊ�Ա��������
%
%    Args:
%        phase_coupling_parameter_matrix: ��λ��ϲ���    

    %% ��������
    y1 = [];    % �洢1,1��λ�ı仯����
    y2 = [];    % �洢2,1��λ�ı仯����, �ڵ�3->2, para=0.1364 
    y3 = [];    % �洢2,1��λ�ı仯����, �ڵ�6->5, para=0.3644
    for t = 16:0.1:22
        phase_coupling_coefficient_matrix = phase_coupling_coefficient_init(phase_coupling_parameter_matrix);
        y1 = [y1, phase_coupling_coefficient_matrix(3,2,1,1)];
        y2 = [y2, phase_coupling_coefficient_matrix(3,2,2,1)];
        y3 = [y3, phase_coupling_coefficient_matrix(6,5,2,1)];       
    end
    
    %% ��ͼ
    figure;
    x = 16:0.1:22;
    plot(x,y1,x,y2,x,y3);
    title('��λ���ϵ����ʱ��仯����');
    xlabel('ʱ��(h)');
    ylabel('��λ��ϲ���');
    legend('ͬ��', '��ͬ�࣬���߾����', '��ͬ�࣬���߾��볤');
end