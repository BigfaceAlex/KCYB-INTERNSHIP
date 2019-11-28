#include "server/grid/hashtable.h"

int lbs_hashtable_init(lbs_hashtable_t* lbs_hashtable){
	if (lbs_hashtable == NULL){ 
		return -1;
	}
	lbs_hashtable->size = 0;
  lbs_hashtable->capacity = Capacity;
  lbs_hashtable->hash_nodes = (lbs_hashnode_t*)malloc(sizeof(lbs_hashnode_t) * Capacity);
  for (int i = 0; i < lbs_hashtable->capacity; ++i) {
    lbs_hashnode_t* node = lbs_hashtable->hash_nodes + i;
    lbs_queue_init(&(node->queue));
  }
  return 0;
}

int lbs_hashtable_destroy(lbs_hashtable_t* lbs_hashtable) {
  if (lbs_hashtable == NULL){
   	return -1;
  }
  free(lbs_hashtable->hash_nodes);
  return 0;
}

int lbs_hashtable_set(lbs_hashtable_t* lbs_hashtable, uint32_t id, lbs_mov_node_t* lbs_mov_node, int cell_id) {
	if (lbs_hashtable == NULL){
  	 	return -1;
  	 }
	uint32_t position = id % Capacity;
    lbs_hashnode_t* head = lbs_hashtable->hash_nodes + position;
    lbs_hashnode_t* new_hash_node = (lbs_hashnode_t*)malloc(sizeof(lbs_hashnode_t));
    new_hash_node->mov_node = lbs_mov_node;
    new_hash_node->cell_id = cell_id;
    lbs_queue_insert_head(&(head->queue), &(new_hash_node->queue)); 
    return 0;
}

lbs_hashnode_t* lbs_hashtable_get(lbs_hashtable_t* lbs_hash_table,uint32_t id){
	uint32_t position = id % Capacity;
  lbs_hashnode_t* head = lbs_hash_table->hash_nodes + position;
	lbs_hashnode_t* next = (lbs_hashnode_t*)head->queue.next;
	while(next != head){
		if(id == next->mov_node->id)
			return next;
    next = (lbs_hashnode_t*)next->queue.next;
	}
	return NULL;
}
