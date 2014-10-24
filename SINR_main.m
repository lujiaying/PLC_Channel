% 全局变量声明
global HE_num CPE_num noise_num X_num;
HE_num = 0;
CPE_num = 0;
noise_num = 0;
X_num = 0;

% 第一步
node_vector = topology_init('test.txt');

% 第二步
distance_phase_matrix = distance_phase_generate(node_vector);