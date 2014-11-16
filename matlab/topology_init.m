function [node_vector] = topology_init(network_topology_file)
%     ��ʼ����������
%     
%     Args:
%       network_topology_file: ��txt�����ĸ�ʽ������������
%  
%     Returns:
%       node_vector: ������������������root_num+HE_num+CPE_num+noise_num+X_num
%         ������Ԫ��Ϊnode�ṹ��:node {id, type, parent_id, distance, phase}
%         ����Ԫ�ص�˳��root-HE-CPE-noise-X
%     

    %% ȫ�ֱ�������
    global root_num HE_num CPE_num noise_num X_num;
    
    % ����root_num, root: id=0, type=0, parent_id=-1
    root_num = 1;

    %% ����txt�ļ�
    node_matrix = load(network_topology_file);
    
    %% ������txt��Ӧ��node_vector
    total_num = length(node_matrix);
    node_vector = cell(total_num, 1);
    for i = 1:total_num
        node.id = node_matrix(i, 1);
        node.type = node_matrix(i, 2);
        % ͳ��HE_num, CPE_num, noise_num, X_num
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
   
    %% ���node_vector��˳���Ƿ��� root-HE-CPE-noise-X
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
    