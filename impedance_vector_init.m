function [impedance_vector] = impedance_vector_init()   
%     ��ʼ���迹����
%     
%     �������ڷ���������ʱ����ƽ������迹�������и��¡�
%     
%     Args:
%         None
%         
%     Returns:
%         impedance_vector: �迹����������ά��HE_num+CPE_num
%           �����е�Ԫ��Ϊ������

    %% ȫ�ֱ�������
    global HE_num CPE_num;
    
    %% 
    impedance_vector = zeros(HE_num+CPE_num);
    
    
end
