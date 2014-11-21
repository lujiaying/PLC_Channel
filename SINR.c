#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*  枚举类型定义 */
typedef enum NODE_TYPE_T
{
    TYPE_HE = 0,
    TYPE_CPE = 1,
    TYPE_NOISE = 2,
    TYPE_X = 3,
} NODE_TYPE_T;

typedef enum PHASE_T
{
    PHASE_ABC = 0,
    PHASE_A = 1,
    PHASE_B = 2,
    PHASE_C = 3,
} PHASE_T;
/* end 枚举类型定义 */


/* 数据结构定义 */
typedef struct NODE_T
{
    int node_id;
    NODE_TYPE_T type;
    int parent_id;
    double distance;
    PHASE_T phase;
} NODE_T;

typedef struct NODE
{
    int node_id;
    struct NODE *next;
} NODE;

typedef struct DISTANCE_PHASE_T
{
    double dis_x;
    double dix_Y;
    double dis_Z;
    PHASE_T pha_X;
    PHASE_T pha_Y;
    PHASE_T pha_Z;
} DISTANCE_PHASE_T;
/* end 数据结构定义 */


/* 全局变量 */
int HE_num = 0, CPE_num = 0, NOISE_num = 0, X_num = 0;
/* end 全局变量 */


/* 函数定义 */
void
initList(NODE **pNode)
{
    *pNode = NULL;
}

NODE *
getLTail(NODE *pHead)
{
    while (pHead->next != NULL)
    {
        pHead = pHead->next;
    }

    return pHead;
}

NODE *
appendList(NODE *pHead, int id)
{
    NODE *p1, *pTail;

    p1 = (NODE *)malloc(sizeof(NODE));

    if (p1 == NULL)
    {
        printf("malloc error\n");
        exit(0);
    }

    p1->node_id = id;
    p1->next = NULL;
    if (pHead == NULL)
    {
        pHead = p1; 
    }
    else
    {
        pTail = getLTail(pHead);
        pTail->next = p1;
    }

    return pHead;
}

int
getLength(NODE *pHead)
{
    int len = 0;

    while (pHead->next != NULL)
    {
        len += 1;
        pHead = pHead->next;
    }

    return len;
}

int
getElem(NODE *pHead, int pos)
{
    // pos ~ [0, lenof(pHead)-1]
    assert(pos < getLength(pHead));

    for (int i=0; i<pos; i++)
    {
        pHead = pHead->next;    
    }

    return pHead->node_id;
}

void
deleteList(NODE *pHead)
{
    NODE *ptmp;

    ptmp = pHead;
    while (ptmp != NULL)
    {
        pHead = ptmp->next;
        free(ptmp);
        ptmp = pHead;
    }
}

int
topology_init(FILE *fin, DISTANCE_PHASE_T *distance_phase_matrix)
{
    int node_id, parent_id;
    NODE_TYPE_T node_type;
    double distance;
    PHASE_T phase;
    NODE_T *lvp_node;
    int total_num;
    extern int HE_num, CPE_num, NOISE_num, X_num;

    /* read from file */
    total_num = 0;
    lvp_node = (NODE_T *)malloc((total_num+1)*sizeof(NODE_T));
    while (fscanf(fin, "%d %d %d %lf %d", &node_id, &node_type, &parent_id, &distance, &phase))
    {
        if (feof(fin))
        {
            printf("end!\n");
            break;
        }
        printf("%d %d %d %lf %d\n", node_id, node_type, parent_id, distance, phase);
        lvp_node[total_num].node_id = node_id;
        lvp_node[total_num].type = node_type;
        lvp_node[total_num].parent_id = parent_id;
        lvp_node[total_num].distance = distance;
        lvp_node[total_num].phase = phase;
        total_num += 1;
        lvp_node = (NODE_T *)realloc(lvp_node, (total_num+1)*sizeof(NODE_T));
    }
    //printf("total_num == %d\n", total_num);
    /* calculate TYPE nums */
    for (int i=0; i<total_num; i++)
    {
        //printf("%d\n", lvp_node[i].node_id);
        if (lvp_node[i].type == TYPE_HE)
        {
            HE_num += 1;
        }
        else if (lvp_node[i].type == TYPE_CPE)
        {
            CPE_num += 1;
        }
        else if (lvp_node[i].type == TYPE_NOISE)
        {
            NOISE_num += 1;
        }
        else if (lvp_node[i].type == TYPE_X)
        {
            X_num += 1;
        }
        else
        {
            printf("error occurs in node[%d], it's type=%d", i, lvp_node[i].type);
        }
    }
    assert(total_num == HE_num+CPE_num+NOISE_num+X_num);

    /* generate distance_phase_matrix */
    DISTANCE_PHASE_T *distance_phase_matrix[total_num][total_num];
    //* find common father node *//
    ///* generate link:leaf->...->root *///
    NODE *lvp_heads[total_num];
    NODE *lvp_tail;
    for (int i=0; i<total_num; i++)
    {
        initList(&lvp_heads[i]);
        lvp_heads[i] = appendList(lvp_heads[i], lvp_node[i].node_id);
        lvp_tail = getLTail(lvp_heads[i]);
        while (lvp_tail->node_id != 0)
        {
            lvp_heads[i] = appendList(lvp_heads[i], lvp_node[lvp_tail->node_id].parent_id);
            printf("%d-> ", lvp_tail->node_id);
            lvp_tail = getLTail(lvp_heads[i]);
        }
        lvp_heads[i] = appendList(lvp_heads[i], 0);
        printf("0  |--len of %d: %d--|\n", i, getLength(lvp_heads[i]));
    }
    ///* find nearest common node *///
    int len_diff;
    for (int i=0; i<total_num; i++)
    {
        for (int j=0; j<total_num; j++)
        {
            len_diff = getLength(lvp_heads[i]) - getLength(lvp_heads[j]); 
            int h = 0, k = 0;
            if (len_diff > 0)
            {
                h = len_diff;    
            }
            else if (len_diff < 0)
            {
                k = -len_diff;
            }
            else
            {
                assert(h == k);
            }
            
            while (getElem(lvp_heads[i], h) != getElem(lvp_heads[j], k))
            {
                h += 1;
                k += 1;
            }
            assert(getElem(lvp_heads[i], h) == getElem(lvp_heads[j], k));
            // 上步获得了i,j交叉点的下标。
            // 从i，j出发，以起始点的相位作为pha_X, pha_Z
            distance_phase_matrix[i][j] = (DISTANCE_PHASE_T *)malloc(1*sizeof(DISTANCE_PHASE_T));
            distance_phase_matrix[i][j]->pha_X = lvp_node[i].phase;
            distance_phase_matrix[i][j]->pha_Z = lvp_node[j].phase;
        }
    }

    // 释放内存
    for (int i=0; i<total_num; i++)
    {
        deleteList(lvp_heads[i]);
    }
    free(lvp_node);
    return total_num;
}
/* end 函数定义 */


int
main()
{
    FILE *lvp_fin;
    DISTANCE_PHASE_T *lvp_distance_phase_matrix;
    int total_num;

    if ((lvp_fin=fopen("./test.txt", "r")) == NULL)
    {
        printf("can not open file\n");
        exit(0);
    }
    else
    {
        total_num = topology_init(lvp_fin, lvp_distance_phase_matrix);
        printf("total_num == %d\n", total_num);
    }

    fclose(lvp_fin);
    return 1;
}
