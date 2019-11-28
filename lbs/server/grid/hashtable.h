#ifndef H_HASHTABLE_H_
#define H_HASHTABLE_H_

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "server/grid/lbs_defs.h"

#define HASHSIZE 200000
#define NULLKEY -10000000
#define Capacity 20000
#define determine NULL

typedef struct lbs_hashnode_s 
{	
	lbs_queue_t queue;
	lbs_mov_node_t* mov_node;
	int cell_id;
	/* data */
}lbs_hashnode_t;

typedef struct lbs_hashtable_s
{
	pthread_mutex_t mutex;
	int size;
	int capacity;
	lbs_hashnode_t* hash_nodes;
	/* data */
}lbs_hashtable_t;

int lbs_hashtable_init(lbs_hashtable_t* lbs_hashtable);

int lbs_hashtable_destroy(lbs_hashtable_t* lbs_hashtable);

int lbs_hashtable_set(lbs_hashtable_t* lbs_hashtable, uint32_t id, lbs_mov_node_t* lbs_mov_node, int cell_id);

lbs_hashnode_t* lbs_hashtable_get(lbs_hashtable_t* lbs_hash_table,uint32_t id);

#endif
