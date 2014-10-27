function [propagation_attenuation_matrix] = impedance_correlation_generate(distance_phase_matrix)
%    ���ɴ���˥������
%    
%    ������ 
%
%    Args:
%        distance_phase_matrix: �ڶ������ɵľ���������Ԫ�����
%
%    Returns:
%        impedance_correlation_matrix: �迹��ؾ��󣬷���ά��HE_num+CPE_num+noise_num
%          �����е�Ԫ��Ϊ������

    %% ȫ�ֱ�������
    global HE_num CPE_num noise_num;
    
    attenu_num = HE_num + CPE_num + noise_num;
    
    %% ��M��Zimmermann�ŵ�ģ�͵ļ�ģ����Ϊ����˥��ģ��
    % A(f,d) = e^(-(a0 + a1*f^k)*d)
    a0= 0;
    a1 = 8.75e-10;
    k= 1;
    f = 5e6;    %Ƶ��5MHz

    %% �������ɹ���
    propagation_attenuation_matrix = zeros(attenu_num);
    for i = 1:attenu_num
        for j = 1:attenu_num
            if i ~= j
                d = distance_phase_matrix{i, j}.dis_X + distance_phase_matrix{i, j}.dis_Y + distance_phase_matrix{i, j}.dis_Z;
                propagation_attenuation_matrix(i, j) = exp(-(a0+a1*f^k)*d);
            end
        end
    end
    
    fprintf('[impedance_correlation success] propagation_attenuation_matrix generate success!!')
end