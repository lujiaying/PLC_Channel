function [distance_matrix] = distance_matrix_generate(distance_phase_matrix)
%    ���ɾ������
%
%    Args:
%        distance_phase_matrix: ������λ��Ԫ�����
%
%    Returns:
%        distance_matrix: �������ά��ΪHE_num+CPE_num

    %% ȫ�ֱ�������
    global HE_num CPE_num;
    
    distance_num = HE_num + CPE_num;
    
    %% ���ɾ������
    distance_matrix = zeros(distance_num);
    
    for i = 1:distance_num
        for j = 1:distance_num
            if i ~= j
                distance_matrix(i,j) = distance_phase_matrix{i,j}.dis_X +  distance_phase_matrix{i,j}.dis_Y + distance_phase_matrix{i,j}.dis_Z;
        
            end
        end
    end
    
    fprintf('[distance_matrix success] distance_matrix generate success!!\n');       
end