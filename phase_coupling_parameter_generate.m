function [phase_coupling_parameter_matrix] = phase_coupling_parameter_generate(distance_phase_matrix)
%    ������λ��ϲ�������
%
%    Args:
%        distance_phase_matrix: �ڶ������ɵľ���������Ԫ�����
%
%    Returns:
%        phase_coupling_parameter_matrix: ��λ��ϲ������󣬷���ά��HE_num+CPE_num
%          �����е�Ԫ��Ϊ������

    %% ȫ�ֱ�������
    global HE_num CPE_num;
    
    coupling_num = HE_num + CPE_num;

    %% ���ϵ�����㹫ʽ
    % c = k*dis_Y / (dis_x + dis_Y + dis_Z)
    k = 0.5;
    
    %% ������λ��ϲ�������
    phase_coupling_parameter_matrix = ones(coupling_num);
    
    for i = 1:coupling_num
        for j = 1:coupling_num
            if i ~= j
                phase_coupling_parameter_matrix(i, j) = k * distance_phase_matrix{i, j}.dis_Y / (distance_phase_matrix{i, j}.dis_X+distance_phase_matrix{i, j}.dis_Y+distance_phase_matrix{i, j}.dis_Z);
            end
        end
    end
    
    fprintf('[phase_coupling_parameter success] phase_coupling_parameter_matrix generate success!!\n');
end