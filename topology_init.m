function [node_vector] = topology_init(network_topology_file)
%     初始化网络拓扑
%     
%     Args:
%       network_topology_file: 用txt描述的格式化的网络拓扑
%  
%     Returns:
%       node_vector: 网络拓扑向量，行数HE_num+CPE_num+noise_num+X_num
%         列数5

    % 全局变量声明
    global HE_num CPE_num noise_num X_num;

    % 载入txt文件
    node_vector = load(network_topology_file);
    
    % 统计HE_num, CPE_num, noise_num, X_num
    % type_code HE=1, CPE=2, noise=3, X=4
    type_column = node_vector(:,2);
    for i = 1:length(type_column)
        if type_column(i) == 1
            HE_num = HE_num + 1;
        elseif type_column(i) == 2
            CPE_num = CPE_num + 1;
        elseif type_column(i) == 3
            noise_num = noise_num + 1;
        elseif type_column(i) == 4
            X_num = X_num + 1;            
        end
    end