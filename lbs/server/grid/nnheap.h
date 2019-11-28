#ifndef SERVER_GRID_LBS_NN_HEAP_H_
#define SERVER_GRID_LBS_NN_HEAP_H_

#include "lbs_defs.h"

typedef struct lbs_heapnode_s {
	double distance;  //����
	uint8_t is_grid;  //1��������  0���ƶ�����
	int cell_id;
	lbs_mov_node_t* node;
}lbs_heapnode_t;

typedef struct lbs_nnheap_s {
	uint32_t capacity;  //����
	uint32_t size;  //Ŀǰ�Ĵ�С
	lbs_heapnode_t *heap_nodes;
}lbs_nnheap_t;

/*��ʼ����*/
int lbs_nnheap_init(lbs_nnheap_t* lbs_nnheap);

/*����*/
int lbs_nnheap_destroy(lbs_nnheap_t* lbs_nnheap);

/*����*/
int lbs_nnheap_insert(lbs_nnheap_t* lbs_nnheap, lbs_mov_node_t* lbs_mov_node,
	                  int cell_id, uint8_t is_grid, double distance);

/*��ȡ��distance��С��lbs_heapnode_t*/
lbs_heapnode_t* lbs_nnheap_top(lbs_nnheap_t* lbs_nnheap);

/*ɾ���Ѷ�Ԫ��*/
void lbs_nnheap_pop(lbs_nnheap_t* lbs_nnheap);

#endif // SERVER_GRID_LBS_NN_HEAP_H_

