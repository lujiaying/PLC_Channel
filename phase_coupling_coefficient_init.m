function [phase_coupling_coefficient_matrix] = phase_coupling_coefficient_init(phase_coupling_parameter_matrix)
%    ��ʼ����λ���ϵ������
%
%    ��λ���ϵ���ľ�ֵΪ��λ��ϲ�����������̬�ֲ���
%
%    Args:
%        phase_coupling_parameter_matrix: ���岽���ɵ���λ��ϲ�������
%
%    Returns:
%        phase_coupling_coefficient_matrix: ��λ���ϵ�����󣬷���ά��HE_num+CPE_num
%          ������Ԫ��Ϊ������

    %% ȫ�ֱ�������
    global HE_num CPE_num;
    
    phase_num = HE_num + CPE_num;
    
    %% ��λ���ϵ����ʽ
    %  ��λ���ϵ�� ~ N(��λ��ϲ���, 1^2)
    phase_coupling_coefficient_matrix = ones(phase_num, phase_num, 4, 4);
    for i = 1:phase_num
        for j = 1:phase_num
            for h = 1:4
                for k = 1:4
                    if i ~= j && h ~=k
                        phase_coupling_coefficient_matrix(i, j, h, k) = normrnd(phase_coupling_parameter_matrix(i, j), 1);
                        if phase_coupling_coefficient_matrix(i, j, h, k) < 0
                            phase_coupling_coefficient_matrix(i, j, h, k) = 0;
                        end
                    end
                end
            end
        end
    end
    fprintf('[phase_coupling_coefficient success] phase_coupling_coefficient_matrix init success!!\n');
    
end