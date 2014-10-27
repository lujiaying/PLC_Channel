function [distance_phase_matrix, leaf2leaf_link_matrix] = distance_phase_generate(node_vector)
%     ���ɾ�����Ԫ�����
%     
%     Args:
%       node_vector: ��һ����ʼ���õ���������������
%         ������Ԫ��Ϊnode�ṹ��:node {id, type, parent_id, distance, phase} 
%  
%     Returns:
%       distance_phase_matrix: ����������Ԫ����󣬷���ά��HE_num+CPE_num+noise_num
%         �����е�Ԫ��Ϊdistance_phase�ṹ��

    %% ȫ�ֱ�������
    global root_num HE_num CPE_num noise_num X_num;
    
    distance_num = HE_num + CPE_num + noise_num;
    total_num = root_num + HE_num + CPE_num + noise_num + X_num;
    
    %% �󹫹����ڵ�
    % ˼·�� ��Ҷ�ӱ��������ڵ㣬�õ���������������ĵ�һ������㡣
        % �ֽ�1: �õ�root��HE, CPE, noise�ķ�������
    leaf2root_link = -1 .* ones(distance_num, total_num);
    len_link = zeros(distance_num, 1);
    for i = (1+root_num):(1+distance_num)      % distance_phase_matrix��HE��
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
    
        % �ֽ�2: ������Ľ����
        % 1. ���������������һ���ڵ��ַ��ͬ�����н���
        % 2. ������������ĳ��ȷֱ���m��n,��m>=n��
           % �ϳ��������ƶ�m-n���ڵ㣬ʹ����������ĩ�˶��룬Ȼ��Ӷ���Ľڵ㿪ʼ�Ƚ���������
           % �õ��ĵ�һ����ȵĽڵ���ǽ���㡣
    leaf2leaf_link_matrix = cell(distance_num);      %�㵽���·������
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
            % �ϲ��õ���i, j�������±꣬��������һ��i-j��·������
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
    
    %% ͨ��·����������distance_phase_matrix
    distance_phase_matrix = cell(distance_num);
    % ����λ��ΪX,Y,Z�ķֶ�����
    X_start_index = 1;
    Y_start_index = 2;
    for i = 1:distance_num
        for j = 1:distance_num
            if i == j
                distance_phase_matrix{i, j} = 0;
            else
                temp_len_leaf2leaf_link = length(leaf2leaf_link_matrix{i, j});
                distance_phase_matrix{i, j}.pha_X = node_vector{leaf2leaf_link_matrix{i, j}(X_start_index)+1}.phase;
                distance_phase_matrix{i, j}.pha_Y = 0;
                distance_phase_matrix{i, j}.pha_Z = node_vector{leaf2leaf_link_matrix{i, j}(temp_len_leaf2leaf_link)+1}.phase;
                distance_phase_matrix{i, j}.dis_X = 0;
                distance_phase_matrix{i, j}.dis_Y = 0;
                distance_phase_matrix{i, j}.dis_Z = 0;
                % �ҳ�Y�ķֶε�
                Z_start_index = temp_len_leaf2leaf_link;
                for l = Y_start_index:temp_len_leaf2leaf_link
                    if node_vector{leaf2leaf_link_matrix{i, j}(l) + 1}.phase ~= 0
                        Z_start_index = l;
                    end
                end
                % �������������ͷ�ڵ��dis_X
                if leaf2leaf_link_matrix{i, j}(X_start_index) == 1
                    distance_phase_matrix{i, j}.dis_X = node_vector{leaf2leaf_link_matrix{i, j}(Y_start_index)+1}.distance;
                    if node_vector{leaf2leaf_link_matrix{i, j}(Y_start_index)+1}.parent_id ~= 1
                        fprintf(['i=' num2str(i) ', j=' num2str(j) '\n']);
                        error(['[distance phase error] id=' num2str(leaf2leaf_link_matrix{i, j}(Y_start_index)) ', parent_id should = 1']);                                               
                    end 
                else
                    for temp_l = X_start_index:Y_start_index - 1
                        distance_phase_matrix{i, j}.dis_X = distance_phase_matrix{i, j}.dis_X + node_vector{leaf2leaf_link_matrix{i,j}(temp_l)+1}.distance;                
                    end
                end
                % �������������ͷ�ڵ��dis_Z
                if leaf2leaf_link_matrix{i, j}(temp_len_leaf2leaf_link) == 1
                    distance_phase_matrix{i, j}.dis_Z = node_vector{leaf2leaf_link_matrix{i, j}(Z_start_index-1)+1}.distance;
                    if node_vector{leaf2leaf_link_matrix{i, j}(temp_len_leaf2leaf_link-1)+1}.parent_id ~= 1
                        fprintf(['i=' num2str(i) ', j=' num2str(j) '\n']);
                        error(['[distance phase error] id=' num2str(leaf2leaf_link_matrix{i, j}(temp_len_leaf2leaf_link)+1 -1) ', parent_id should = 1']);                       
                    end 
                else
                    for temp_l = temp_len_leaf2leaf_link:-1:Z_start_index
                        distance_phase_matrix{i, j}.dis_Z = distance_phase_matrix{i, j}.dis_Z + node_vector{leaf2leaf_link_matrix{i,j}(temp_l)+1}.distance;                
                    end
                end
                % ����dis_Y
                    % ��ǰ����
                for temp_l = Y_start_index:Z_start_index-1
                    if node_vector{leaf2leaf_link_matrix{i, j}(temp_l)+1}.parent_id == leaf2leaf_link_matrix{i, j}(temp_l + 1) && node_vector{leaf2leaf_link_matrix{i, j}(temp_l)+1}.parent_id ~= 1
                        distance_phase_matrix{i, j}.dis_Y = distance_phase_matrix{i, j}.dis_Y + node_vector{leaf2leaf_link_matrix{i, j}(temp_l)+1}.distance;
                    end
                end
                    % �Ӻ���ǰ
                for temp_l = Z_start_index-1:-1:Y_start_index
                    if node_vector{leaf2leaf_link_matrix{i, j}(temp_l)+1}.parent_id == leaf2leaf_link_matrix{i, j}(temp_l - 1) && node_vector{leaf2leaf_link_matrix{i, j}(temp_l)+1}.parent_id ~= 1
                        distance_phase_matrix{i, j}.dis_Y = distance_phase_matrix{i, j}.dis_Y + node_vector{leaf2leaf_link_matrix{i, j}(temp_l)+1}.distance;
                    end
                end                
            end
        end
    end
    fprintf('[distance_phase success] distance_phase_matrix generate success!!\n');