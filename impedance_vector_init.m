function [impedance_vector] = impedance_vector_init()
%    ��ʼ���迹����
%
%    Args:
%        None
%
%    Returns:
%        impedance_vector: �迹����������ά��HE_num+CPE_num
%          �����е�Ԫ��Ϊ������    

    %% ȫ�ֱ�������
    global HE_num CPE_num;
    
    impedance_num = HE_num + CPE_num;
    
    %% �迹��ʼ����ʽ
    % idle ~ N(90, 2.5^2)    busy ~ N(20, 4.5^2)
    mean_idle = 90;
    deviation_idle = 2.5;
    mean_busy = 20;
    deviation_idle = 4.5;
    
    %% �����迹����
    impedance_vector = zeros(impedance_num,1);
    
    for i = 1:impedance_num
        impedance_vector(i) = normrnd(mean_idle, deviation_idle);
    end
    
    fprintf('[impedance_vector success] impedance_vector init success!!\n');
end
