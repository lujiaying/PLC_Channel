% ȫ�ֱ�������
global HE_num CPE_num noise_num X_num;
HE_num = 0;
CPE_num = 0;
noise_num = 0;
X_num = 0;

% ��һ��
node_vector = topology_init('test.txt');

% �ڶ���
distance_phase_matrix = distance_phase_generate(node_vector);