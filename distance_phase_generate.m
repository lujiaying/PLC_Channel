function [leaf2leaf_link_matrix,distance_phase_matrix] = distance_phase_generate(node_vector)
%     生成距离六元组矩阵
%     
%     Args:
%       node_vector: 第一步初始化得到的网络拓扑向量
%         向量的元素为node结构体:node {id, type, parent_id, distance, phase} 
%  
%     Returns:
%       distance_phase_matrix: 距离向量六元祖矩阵，方阵维数HE_num+CPE_num+noise_num
%         矩阵中的元素为distance_phase结构体

    %% 全局变量声明
    global root_num HE_num CPE_num noise_num X_num;
    
    distance_num = HE_num + CPE_num + noise_num;
    total_num = root_num + HE_num + CPE_num + noise_num + X_num;
    distance_phase_matrix = cell(distance_num);
    
    %% 求公共父节点
    % 思路： 由叶子遍历到根节点，得到两个链表，求链表的第一个交叉点。
        % 分解1: 得到root到HE, CPE, noise的反向链表
    leaf2root_link = -1 .* ones(distance_num, total_num);
    len_link = zeros(distance_num, 1);
    for i = (1+root_num):(1+distance_num)      % distance_phase_matrix从HE起
        j = 1;
        leaf2root_link(i-1, j) = node_vector{i}.id;
        j = j + 1;
        temp_pid = node_vector{i}.parent_id;
        while temp_pid ~= 0
            leaf2root_link(i-1, j) = temp_pid;
            j = j + 1;
            temp_pid = node_vector{temp_pid+1}.parent_id;
        end
        leaf2root_link(i-1, j) = 0;
        len_link(i-1) = j;
    end
    fprintf('[distance_phase success] generate node_link success:\n');
    leaf2root_link
    len_link
    
        % 分解2: 求链表的交叉点
        % 1. 如果两个链表的最后一个节点地址相同，则有交叉
        % 2. 假设两个链表的长度分别是m和n,且m>=n。
           % 较长的链表移动m-n个节点，使两个链表在末端对齐，然后从对齐的节点开始比较两个链表。
           % 得到的第一个相等的节点就是交叉点。
    
    parent_id_common_matrix = ones(distance_num);
    leaf2leaf_link_matrix = cell(distance_num);      %点到点的路径矩阵
    for i = 1:distance_num
        for j = 1:distance_num
            diff = len_link(i) - len_link(j);
            h = 1;
            k = 1;
            if diff > 0
                h = diff + 1;
            elseif diff < 0
                k = -diff + 1;
            end
            while leaf2root_link(i, h) ~= leaf2root_link(j, k)
                h = h + 1;
                k = k + 1;
            end
            % 上步得到了i, j交叉点的下标，下面生成一条i-j的路径链表
            leaf2leaf_link = [];
            for temp_h = 1:h-1
                leaf2leaf_link = [leaf2leaf_link leaf2root_link(i, temp_h)];
            end
            for temp_k = k:-1:1
                leaf2leaf_link = [leaf2leaf_link leaf2root_link(j, temp_k)];
            end
            leaf2leaf_link_matrix{i, j} = leaf2leaf_link;           
        end
    end
    fprintf('[distance_phase success] leaf2leaf_link_matrix generate success\n');
    
    %% 通过路径矩阵，生成distance_phase_matrix
    
    
    
    
    
    
    
    
        
    