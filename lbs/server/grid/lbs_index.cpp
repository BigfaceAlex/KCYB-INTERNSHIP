#include "server/grid/lbs_index.h"

#include <stdio.h>
#include <stdlib.h>

#include "common/log.h"

#include "server/grid/lbs_distance.h"

#include "server/grid/lbs_grid.h" 
#include "server/grid/nnheap.h"
#include "server/grid/bitmaps.h"

#define LBS_LON_MIN 116
#define LBS_LON_MAX 117
#define LBS_LAT_MIN 39
#define LBS_LAT_MAX 41

#define LBS_ROW_NUM 200
#define LBS_COL_NUM 100

lbs_grid_t lbs_grid;

// 初始化网格索引
int lbs_grid_index_init() {
	// TODO: by student
  lbs_grid_init(&lbs_grid, LBS_LON_MIN, LBS_LON_MAX, LBS_LAT_MIN, LBS_LAT_MAX, LBS_ROW_NUM, LBS_COL_NUM);
  return 0;
}

// 更新接口[出租车位置更新]
int lbs_grid_index_update(double lon,double lat, uint64_t timestamp,uint32_t id) {
  // TODO: by student
    lbs_grid_update(&lbs_grid,lon,lat,timestamp,id);
  return 0;
}

// 范围查询接口[查询某一范围内的所有出租车信息]
int lbs_grid_index_range_query(double lon1,double lon2,double lat1,double lat2,lbs_res_node_t* output) {
  // TODO: by student
  
  if(output==NULL){
  	return -1;
  }
  
  int rowLeft = lbs_grid_cell_row(&lbs_grid,lat1);
  int rowRight = lbs_grid_cell_row(&lbs_grid,lat2);
  int colLeft = lbs_grid_cell_col(&lbs_grid,lon1);
  int colRight = lbs_grid_cell_col(&lbs_grid,lon2);

  printf("row=%d %d col=%d %d\n", rowLeft, rowRight, colLeft, colRight);
  for(int row = rowLeft; row<=rowRight; row++){
  	for(int col = colLeft; col<=colRight; col++){
      int cell_id = lbs_grid_cell_id(&lbs_grid, row, col);
	  	lbs_cell_t* cell  = lbs_grid_cell(&lbs_grid, cell_id);//current cell

	  	lbs_queue_t* head = &(cell->dammy_node.queue);
        lbs_queue_t* p    = head->next;
        for (; p != head; p = p->next) {
          lbs_mov_node_t* mov_node = (lbs_mov_node_t*)p;
          if (mov_node->lon > lon2 || mov_node->lon < lon1 || mov_node->lat > lat2 || mov_node->lat < lat1) continue;
          lbs_res_node_t* result = (lbs_res_node_t*)malloc(sizeof(lbs_res_node_t));
          result->lon = mov_node->lon;
          result->lat = mov_node->lat;
          result->id = mov_node->id;
          result->timestamp = mov_node->timestamp;
          lbs_queue_insert_head(&(output->queue), &(result->queue));
      	}	
	  }
  }
  return 0;
}

double lbs_cell_distance(double lon1, double lon2, double lat1, double lat2, double lon, double lat) {
	if (lon >= lon1 && lon <= lon2 && lat >= lat1 && lat <= lat2)	//in the grid
	{
		return 0;
	} else if (lon >= lon1 && lon <= lon2 && lat >= lat2)	//north
	{
		return lbs_distance(0, lat, 0, lat2);
	} else if (lon > lon1 && lon < lon2 && lat < lat1)	//south
	{
		return lbs_distance(0, lat, 0, lat1);
	} else if (lat > lat1 && lat < lat2 && lon < lon1)	//west
	{
		return lbs_distance(lon, 0, lon1, 0);
	} else if (lat > lat1 && lat < lat2 && lon > lon2)	//east
	{
		return lbs_distance(lon, 0, lon2, 0);
	} else	if (lon < lon1 && lat > lat2)	//north west
	{
		return lbs_distance(lon, lat, lon1, lat2);
	} else if (lon > lon2 && lat > lat2)	//north east
	{
		return lbs_distance(lon, lat, lon2, lat2);
	} else if (lon < lon1 && lat < lat1)	//south west
	{
		return lbs_distance(lon, lat, lon1, lat1);
	} else if (lon > lon2 && lat < lat1)	//south east
	{
		return lbs_distance(lon, lat, lon2, lat1);
	}
  return 0;
}

// NN查询接口[查询离lon,lat最近的出租车]
int lbs_grid_index_nn_query(double lon, double lat, lbs_res_node_t* output) {
  // TODO: by student
	int row = lbs_grid_cell_row(&lbs_grid, lat);
	int col = lbs_grid_cell_col(&lbs_grid, lon);
	int cell_id = lbs_grid_cell_id(&lbs_grid, row, col);
	
	lbs_nnheap_t lbs_nnheap;
	lbs_nnheap_init(&lbs_nnheap);
	
	lbs_bitmap_t lbs_bitmap;
	lbs_bitmap_init(&lbs_bitmap, LBS_ROW_NUM * LBS_COL_NUM);
	
	lbs_mov_node_t* moveNode = (lbs_mov_node_t *)(&(lbs_grid.cell[cell_id].dammy_node));
	//调用insert 插入
	 lbs_nnheap_insert(&lbs_nnheap, moveNode, cell_id, 1, 0.00 );
 
	while(1){
		
	lbs_heapnode_t* heap_node = lbs_nnheap_top(&lbs_nnheap);
	if (heap_node->is_grid == 1){
		lbs_cell_t* cell  = lbs_grid_cell(&lbs_grid, heap_node->cell_id);
	
	 	lbs_queue_t* head = &(cell->dammy_node.queue);
        lbs_queue_t* p = head->next;
       	for (; p != head; p = p->next) {
         	lbs_mov_node_t* mod_node = (lbs_mov_node_t*)p;
         	lbs_nnheap_insert(&lbs_nnheap, mod_node, cell_id, 0, lbs_distance(mod_node->lon, mod_node->lat, lon, lat));
       	}
       	//insert cells in nnheap
     	for(int a=-1; a<=1; a++){
	     	for(int b=-1;b<=1;b++){
	     		int id = cell_id + a*lbs_grid.col_num +b;
	     		int row, col;
		 		lbs_grid_cell_row_col(&lbs_grid, id, &row, &col);
		 		double lat1 = row * lbs_grid.cell_height + lbs_grid.lat_min;
    			double lat2 = lat1 + lbs_grid.cell_height;
    			double lon1 = col * lbs_grid.cell_width + lbs_grid.lon_min;
		 		double lon2 = lon1 + lbs_grid.cell_width;
	     		if (lbs_bitmap_isset(&lbs_bitmap, id) == 0){
		     		lbs_nnheap_insert(&lbs_nnheap, (lbs_mov_node_t*)(&(lbs_grid.cell[id].dammy_node)),id, 1, lbs_cell_distance(lon1, lat1, lon2, lat2, lon, lat) );//?????
		     		lbs_bitmap_setbit(&lbs_bitmap, id);
		     	}
	     	}
	     }
	     
	}else if (heap_node->is_grid == 0){
		lbs_res_node_t* result = (lbs_res_node_t*)malloc(sizeof(lbs_res_node_t));
  		result->lon = heap_node->node->lon;
  		result->lat = heap_node->node->lat;
    	result->id = heap_node->node->id;
     	result->timestamp = heap_node->node->timestamp;
      	lbs_queue_insert_head(&(output->queue), &(result->queue));
        break;
	}
  lbs_nnheap_pop(&lbs_nnheap);
	} 
	
	return 0;
}
