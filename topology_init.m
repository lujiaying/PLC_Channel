function [node_vector] = topology_init(network_topology_file)
%     初始化网络拓扑
%     
%     Args:
%       network_topology_file: 用txt描述的格式化的网络拓扑
%  
%     Returns:
%       node_vector: 网络拓扑向量，行数root_num+HE_num+CPE_num+noise_num+X_num
%         向量的元素为node结构体:node {id, type, parent_id, distance, phase}
%         向量元素的顺序root-HE-CPE-noise-X
%     

    %% 全局变量声明
    global root_num HE_num CPE_num noise_num X_num;
    
    % 设置root_num, root: id=0, type=0, parent_id=-1
    root_num = 1;

    %% 载入txt文件
    node_matrix = load(network_topology_file);
    
    %% 生成与txt对应的node_vector
    total_num = length(node_matrix);
    node_vector = cell(total_num, 1);
    for i = 1:total_num
        node.id = node_matrix(i, 1);
        node.type = node_matrix(i, 2);
        % 统计HE_num, CPE_num, noise_num, X_num
        % type_code root=0, HE=1, CPE=2, noise=3, X=4
        if node.type == 1
            HE_num = HE_num + 1;
        elseif node.type == 2
            CPE_num = CPE_num + 1;
        elseif node.type == 3
            noise_num = noise_num + 1;
        elseif node.type == 4
            X_num = X_num + 1;
        end
        node.parent_id = node_matrix(i ,3);
        node.distance = node_matrix(i, 4);
        node.phase = node_matrix(i, 5);
        node_vector{i} = node;
    end
    fprintf('[init success] node_vector load success\n')
   
    %% 检测node_vector的顺序是否是 root-HE-CPE-noise-X
    for i = 1:total_num
        if node_vector{i}.id ~= i-1
           error(['[init error] node_vector{' num2str(i) '}.id ==' num2str(node_vector{i}.id)]); 
        end
        if and(i>root_num, i<=root_num+HE_num)
            if node_vector{i}.type ~= 1
                error(['[init error] node_vector{' num2str(i) '}.type ==' num2str(node_vector{i}.type) 'should == 1']);
            end
        end
        if and(i>root_num+HE_num, i<=root_num+HE_num+CPE_num)
            if node_vector{i}.type ~= 2
                error(['[init error] node_vector{' num2str(i) '}.type ==' num2str(node_vector{i}.type) 'should == 2']);
            end
        end
        if and(i>root_num+HE_num+CPE_num, i<=root_num+HE_num+CPE_num+noise_num)
            if node_vector{i}.type ~= 3
                error(['[init error] node_vector{' num2str(i) '}.type ==' num2str(node_vector{i}.type) 'should == 3']);
            end
        end
        if and(i>root_num+HE_num+CPE_num+noise_num, i<=root_num+HE_num+CPE_num+noise_num+X_num)
            if node_vector{i}.type ~= 4
                error(['[init error] node_vector{' num2str(i) '}.type ==' num2str(node_vector{i}.type) 'should == 4']);
            end
        end
    end
    fprintf('[init success] node_vector generate success!!\n');
    fprintf(['[init success] total_num==' num2str(total_num) '\n'])  
    