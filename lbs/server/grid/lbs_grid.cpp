#include "server/grid/lbs_grid.h"

//网络的初始化
int lbs_grid_init(lbs_grid_t* lbs_grid, double lon1, double lon2, double lat1, double lat2, int row_num, int col_num){
	lbs_grid->row_num = row_num;
	lbs_grid->col_num = col_num;
	lbs_grid->lon_min = lon1;
	lbs_grid->lat_min = lat1;
	lbs_grid->cell_width = (lon2-lon1)/col_num;
	lbs_grid->cell_height = (lat2-lat1)/row_num;
	
	lbs_hashtable_init(&lbs_grid->hash_table);
	
	lbs_grid->cell = (lbs_cell_t *)malloc(row_num * col_num *sizeof(lbs_cell_t));
	for(int i=0; i<row_num*col_num;i++)
	{
		 lbs_queue_init(&(lbs_grid->cell[i].dammy_node.queue));
	}
	return 0;
	
}
//网格的删除
//int lbs_grid_destory(lbs_grid_t* lbs_grid);
//更新移动位置
int lbs_grid_update(lbs_grid_t* lbs_grid, double lon, double lat, uint64_t timestamp, uint32_t id){
	
	int cell_id = lbs_grid_cell_id(lbs_grid, lbs_grid_cell_row(lbs_grid, lat), lbs_grid_cell_col(lbs_grid, lon));//current cell id
	
	lbs_hashnode_t*  hashnode = lbs_hashtable_get(&lbs_grid->hash_table, id);//get hash node
	
	if (hashnode == NULL) {
		lbs_mov_node_t * mov_node = new lbs_mov_node_t;
		mov_node->lat = lat;
		mov_node->lon = lon;
		mov_node->id = id;
		lbs_hashtable_set(&lbs_grid->hash_table, id, mov_node, cell_id);
		lbs_queue_insert_head( &(lbs_grid->cell[cell_id].dammy_node.queue), &(mov_node->queue) );
	}
	else {
		if (hashnode->cell_id == cell_id) {
			hashnode->mov_node->lat = lat;
			hashnode->mov_node->lon = lon;
		}else {
			hashnode->mov_node->lat = lat;
			hashnode->mov_node->lon = lon;
			hashnode->cell_id = cell_id;
			lbs_queue_remove(&(hashnode->mov_node->queue));
			lbs_queue_insert_head( &(lbs_grid->cell[cell_id].dammy_node.queue), &(hashnode->mov_node->queue));
		}
	}
	return 0;
}
//计算cell row
int lbs_grid_cell_row(lbs_grid_t* lbs_grid,double lat){
	int row = (int)((lat - lbs_grid->lat_min)/lbs_grid->cell_height);
	return row;
}
//计算 cell col
int lbs_grid_cell_col(lbs_grid_t* lbs_grid, double lon){
	int col = (int)((lon - lbs_grid->lon_min)/lbs_grid->cell_width);
	return col;
}
//计算 cell id
int lbs_grid_cell_id(lbs_grid_t* lbs_grid,int cell_row, int cell_col){
	int id = (int)(lbs_grid->col_num*cell_row + cell_col);
	return id;
}
//计算row和col
void lbs_grid_cell_row_col(lbs_grid_t* lbs_grid, int cell_id, int* cell_row, int* cell_col){
	*cell_row = cell_id/lbs_grid->col_num;
	*cell_col = cell_id%lbs_grid->col_num;
}
//获取cell id里面的cell 
lbs_cell_t* lbs_grid_cell(lbs_grid_t* lbs_grid, int cell_id){
	return(lbs_grid->cell + cell_id);
}
