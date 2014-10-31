function [impedance_correlation_matrix] = impedance_correlation_generate(distance_phase_matrix)
%    �����迹���ϵ������
%
%    Args:
%        distance_phase_matrix: �ڶ������ɵľ���������Ԫ�����
%
%    Returns:
%        impedance_correlation_matrix: �迹���ϵ�����󣬷���ά��HE_num+CPE_num+noise_num
%          �����е�Ԫ��Ϊ������

    %% ȫ�ֱ�������
    global HE_num CPE_num noise_num;
    
    correlation_num = HE_num + CPE_num + noise_num;
    
    %% �迹���ϵ�����㹫ʽ
    %  c = e^(-d/a)
    a = 30;
    
    %% �������ɹ���
    impedance_correlation_matrix = zeros(correlation_num);
    for i = 1:correlation_num
        for j = 1:correlation_num
            if i ~= j
                d = distance_phase_matrix{i, j}.dis_X + distance_phase_matrix{i, j}.dis_Y + distance_phase_matrix{i, j}.dis_Z;
                impedance_correlation_matrix(i, j) = exp(-d/a);
            end
        end
    end
    
    fprintf('[impedance_correlation success] impedance_correlation_matrix generate success!!\n');
end
