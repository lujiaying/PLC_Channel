%% 全局变量声明
global root_num HE_num CPE_num noise_num X_num;
root_num = 0;
HE_num = 0;
CPE_num = 0;
noise_num = 0;
X_num = 0;

%% log
fprintf('[start] SINR calculate start... print log:\n')

%% 第一步
node_vector = topology_init('test.txt');

%% 第二步
%distance_phase_matrix = distance_phase_generate(node_vector);
[distance_phase_matrix, leaf2leaf_link_matrix] = distance_phase_generate(node_vector);

%% 第三步
propagation_attenuation_matrix = impedance_correlation_generate(distance_phase_matrix);

%% end
fprintf('[end] SINR calculate end...\n')