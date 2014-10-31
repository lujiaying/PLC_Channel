function [impedance_space_vector] = impedance_space_relate(impedance_vector, distance_matrix)
%    ���迹�������пռ���ػ�
%
%    Args:
%        impedance_vector: ��ǰʱ�̵�ʱ����ػ�����迹����
%        distance_matrix: �������
%
%    Returns:
%        impedance_space_vector: �����ռ���ػ�����迹����, ����ά����impedance_vectorһ��,HE_num+CPE_num
%          �����е�Ԫ��Ϊ������

    %% ȫ�ֱ�������
    global HE_num CPE_num;
    
    impedance_num = HE_num + CPE_num;
    
    %% �ռ���ػ���ʽ
    % space_vector = R * vector, ����RΪHE_num+CPE_num�ķ���Ԫ��Ϊ�ռ����ϵ��s
    % s = e^(-d/a)     
    a = 15;
    
    %% ���ɿռ���ػ�����迹
    % ���ɿռ���ؾ���
    space_relate_matrix = ones(impedance_num);
    for i = 1:impedance_num
        for j = 1:impedance_num
            space_relate_matrix(i,j) = exp(-distance_matrix(i,j)/a);
        end
    end
    fprintf('[impedance_space_relate success] space_relate_matrix generate success!\n');

end