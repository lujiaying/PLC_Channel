%% ȫ�ֱ�������
global root_num HE_num CPE_num noise_num X_num;
root_num = 0;
HE_num = 0;
CPE_num = 0;
noise_num = 0;
X_num = 0;

% �迹����
mean_idle = 90;
deviation_idle = 4.5;
mean_busy = 20;
deviation_busy = 2.5;

%% log start
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
impedance_vector = impedance_vector_init(mean_idle, deviation_idle);

%% ���߲�  ���迹�������пռ���ػ�
impedance_space_vector = impedance_space_relate(impedance_vector, impedance_correlation_matrix);

%% �ڰ˲�  ��ʼ����λ���ϵ������
phase_coupling_coefficient_matrix = phase_coupling_coefficient_init(phase_coupling_parameter_matrix);

%% ���迹��ʱ��仯��ͼ
plot_impedance_day(impedance_vector, impedance_correlation_matrix);

%% �����ϵ����ʱ��仯��ͼ
plot_phase_coupling_day(phase_coupling_parameter_matrix);

%% log end
fprintf('[end] SINR calculate end...\n')