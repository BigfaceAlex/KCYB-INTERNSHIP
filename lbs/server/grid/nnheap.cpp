#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nnheap.h"

#define CAPACITY 100

int lbs_nnheap_init(lbs_nnheap_t* lbs_nnheap)
{
	if (lbs_nnheap == NULL)  return -1;  //注意报错
	lbs_nnheap->capacity = CAPACITY;
	lbs_nnheap->size = 0;
	lbs_nnheap->heap_nodes = (lbs_heapnode_t*)malloc(sizeof(lbs_heapnode_t)*lbs_nnheap->capacity);
	return 0;
}

int lbs_nnheap_destroy(lbs_nnheap_t* lbs_nnheap)
{
	if (lbs_nnheap == NULL) return -1;
	free(lbs_nnheap->heap_nodes);
	return 0;
}

void Heapsort_bottom(lbs_nnheap_t* a, int size)
{
	uint32_t i = size; //2i
	if (i <= 0) return;
	uint32_t parent = (i - 1) / 2; //i
	if (a->heap_nodes[parent].distance > a->heap_nodes[i].distance) {
		lbs_heapnode_t temp = a->heap_nodes[parent];
		a->heap_nodes[parent] = a->heap_nodes[i];
		a->heap_nodes[i] = temp;
		Heapsort_bottom(a, parent);
	}
}

void Heapsort_top(lbs_nnheap_t* a)
{
	uint32_t i = 1, min, branch;
	for (;;) {
		if (2 * i > a->size) return;
		min = 2 * i; branch = 2 * i + 1;
		if (branch <= a->size) {
			if (a->heap_nodes[min - 1].distance > a->heap_nodes[branch - 1].distance)
				min = branch;
			}	
		if (a->heap_nodes[i - 1].distance > a->heap_nodes[min - 1].distance) {
			lbs_heapnode_t temp = a->heap_nodes[min - 1];
			a->heap_nodes[min - 1] = a->heap_nodes[i - 1];
			a->heap_nodes[i - 1] = temp;
			i = min;
		}
		else break;
	}
}

int lbs_nnheap_insert(lbs_nnheap_t* lbs_nnheap, lbs_mov_node_t* lbs_mov_node,
	int cell_id, uint8_t is_grid, double distance)
{
	if (lbs_nnheap == NULL) return -1;  //注意报错
	if (lbs_nnheap->size == lbs_nnheap->capacity) {  //超出范围，重新分配空间
		lbs_nnheap->capacity = lbs_nnheap->capacity * 2;
		lbs_nnheap->heap_nodes = (lbs_heapnode_t*)realloc(lbs_nnheap->heap_nodes,
			sizeof(lbs_heapnode_t)*lbs_nnheap->capacity);
	}		
	uint32_t i;
	i=lbs_nnheap->size;
	lbs_nnheap->heap_nodes[i].cell_id = cell_id;
	lbs_nnheap->heap_nodes[i].distance = distance;
	lbs_nnheap->heap_nodes[i].is_grid = is_grid;
	lbs_nnheap->heap_nodes[i].node = lbs_mov_node;
	lbs_nnheap->size++;	
	Heapsort_bottom(lbs_nnheap, lbs_nnheap->size - 1); //插入是从下往上调整
  return 0;
}

lbs_heapnode_t* lbs_nnheap_top(lbs_nnheap_t* lbs_nnheap)
{
	if (lbs_nnheap->size > 0)
		return lbs_nnheap->heap_nodes;
	else return NULL;
}

void lbs_nnheap_pop(lbs_nnheap_t* lbs_nnheap)
{
	if (lbs_nnheap->size == 0)
		return;	
	lbs_nnheap->heap_nodes[0] = lbs_nnheap->heap_nodes[lbs_nnheap->size - 1];
	lbs_nnheap->size--;
	Heapsort_top(lbs_nnheap); //删除从上往下调整	
}
