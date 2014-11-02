%% ȫ�ֱ�������
global root_num HE_num CPE_num noise_num X_num;
root_num = 0;
HE_num = 0;
CPE_num = 0;
noise_num = 0;
X_num = 0;

%% log
fprintf('[start] SINR calculate start... print log:\n')

%% ��һ��  ��ʼ����������
node_vector = topology_init('test.txt');

%% �ڶ���  ���ɾ�����λ��Ԫ�����
[distance_phase_matrix, leaf2leaf_link_matrix] = distance_phase_generate(node_vector);

%% ������  ���ɴ���˥������
propagation_attenuation_matrix = propagation_attenuation_generate(distance_phase_matrix);

%% ���Ĳ�  �����迹���ϵ������
impedance_correlation_matrix = impedance_correlation_generate(distance_phase_matrix);

%% ���岽  ������λ��ϲ�������
phase_coupling_parameter_matrix = phase_coupling_parameter_generate(distance_phase_matrix);

%% ������  ��ʼ���迹����
impedance_vector = impedance_vector_init();

%% ���߲�  ���迹�������пռ���ػ�
distance_matrix = distance_matrix_generate(distance_phase_matrix);
impedance_space_vector = impedance_space_relate(impedance_vector, distance_matrix);

%% �ڰ˲�  ��ʼ����λ���ϵ������
phase_coupling_coefficient_matrix = phase_coupling_coefficient_init(phase_coupling_parameter_matrix);

%% end
fprintf('[end] SINR calculate end...\n')