function [distance_phase_matrix] = distance_phase_generate(node_vector)
%     ���ɾ�����Ԫ�����
%     
%     Args:
%       node_vector: ��һ����ʼ���õ���������������
%  
%     Returns:
%       distance_phase_matrix: ����������Ԫ����󣬷���ά��HE_num+CPE_num+noise_num
%         �����е�Ԫ��Ϊdistance_phase�ṹ��

    % ȫ�ֱ�������
    global HE_num CPE_num noise_num X_num;
    
    distance_phase_matrix = ones(HE_num+CPE_num+noise_num, HE_num+CPE_num+noise_num);
    