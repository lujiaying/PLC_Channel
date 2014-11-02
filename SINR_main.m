%% 全局变量声明
global root_num HE_num CPE_num noise_num X_num;
root_num = 0;
HE_num = 0;
CPE_num = 0;
noise_num = 0;
X_num = 0;

%% log
fprintf('[start] SINR calculate start... print log:\n')

%% 第一步  初始化网络拓扑
node_vector = topology_init('test.txt');

%% 第二步  生成距离相位六元组矩阵
[distance_phase_matrix, leaf2leaf_link_matrix] = distance_phase_generate(node_vector);

%% 第三步  生成传输衰减矩阵
propagation_attenuation_matrix = propagation_attenuation_generate(distance_phase_matrix);

%% 第四步  生成阻抗相关系数矩阵
impedance_correlation_matrix = impedance_correlation_generate(distance_phase_matrix);

%% 第五步  生成相位耦合参数矩阵
phase_coupling_parameter_matrix = phase_coupling_parameter_generate(distance_phase_matrix);

%% 第六步  初始化阻抗向量
impedance_vector = impedance_vector_init();

%% 第七步  对阻抗向量进行空间相关化
distance_matrix = distance_matrix_generate(distance_phase_matrix);
impedance_space_vector = impedance_space_relate(impedance_vector, distance_matrix);

%% 第八步  初始化相位耦合系数矩阵
phase_coupling_coefficient_matrix = phase_coupling_coefficient_init(phase_coupling_parameter_matrix);

%% end
fprintf('[end] SINR calculate end...\n')