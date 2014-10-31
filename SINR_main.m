%% ȫ�ֱ�������
global root_num HE_num CPE_num noise_num X_num;
root_num = 0;
HE_num = 0;
CPE_num = 0;
noise_num = 0;
X_num = 0;

%% log
fprintf('[start] SINR calculate start... print log:\n')

%% ��һ��
node_vector = topology_init('test.txt');

%% �ڶ���
%distance_phase_matrix = distance_phase_generate(node_vector);
[distance_phase_matrix, leaf2leaf_link_matrix] = distance_phase_generate(node_vector);

%% ������
propagation_attenuation_matrix = propagation_attenuation_generate(distance_phase_matrix);

%% ���Ĳ�
impedance_correlation_matrix = impedance_correlation_generate(distance_phase_matrix);

%% ���岽
phase_coupling_parameter_matrix = phase_coupling_parameter_generate(distance_phase_matrix);

%% ������
impedance_vector = impedance_vector_init();

%% ���߲�
distance_matrix = distance_matrix_generate(distance_phase_matrix);


%% end
fprintf('[end] SINR calculate end...\n')