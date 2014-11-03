function [impedance_space_vector] = impedance_space_relate(impedance_vector, impedance_correlation_matrix)
%    ���迹�������пռ���ػ�
%
%    Args:
%        impedance_vector: ��ǰʱ�̵�ʱ����ػ�����迹����
%        impedance_correlation_matrix: �迹���ϵ������
%
%    Returns:
%        impedance_space_vector: �����ռ���ػ�����迹����, ����ά����impedance_vectorһ��,HE_num+CPE_num
%          �����е�Ԫ��Ϊ������

    %% ȫ�ֱ�������
    global HE_num CPE_num;
    
    impedance_num = HE_num + CPE_num;
    
    %% �ռ���ػ���ʽ
    % space_vector = R^(1/2) * vector, ����RΪHE_num+CPE_num�ķ���Ԫ��Ϊ�ռ����ϵ��
    %% ���ɿռ���ػ�����迹
    impedance_space_vector = impedance_correlation_matrix * impedance_vector;
    fprintf('[impedance_space_relate success] impedance space vector generate success!\n');
end