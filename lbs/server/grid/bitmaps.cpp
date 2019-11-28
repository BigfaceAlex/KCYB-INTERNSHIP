#include "server/grid/bitmaps.h"

uint8_t bitmap[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

int lbs_bitmap_init(lbs_bitmap_t* lbs_bitmap, uint32_t bits_num){
	if(lbs_bitmap == NULL){
		return -1;
	}
	uint32_t byte_num = bits_num / 8 + (bits_num % 8 == 0 ? 0 : 1);
	lbs_bitmap->bits = (uint8_t*)malloc(byte_num);
	memset(lbs_bitmap->bits, 0, byte_num);
    lbs_bitmap->bits_num = bits_num;
    return 0;
}

int lbs_bitmap_destroy(lbs_bitmap_t* lbs_bitmap) {
  if (lbs_bitmap == NULL){
  		return -1;
  }
  free(lbs_bitmap->bits);
  return 0;
}

int lbs_bitmap_setbit(lbs_bitmap_t* lbs_bitmap, uint32_t pos){
	if (lbs_bitmap == NULL || pos >= lbs_bitmap->bits_num){
    		return -1;
  	}
  	uint32_t idp = pos / 8;
  	uint32_t offset = pos % 8;
  	lbs_bitmap->bits[idp] |= bitmap[offset];
    return 0;
}

int lbs_bitmap_isset(lbs_bitmap_t* lbs_bitmap, uint32_t pos){
	if (lbs_bitmap == NULL || pos >= lbs_bitmap->bits_num){
    		return -1;
  	}
  	uint32_t idp = pos / 8;
  	uint32_t offset = pos % 8;
  	if(lbs_bitmap->bits[idp] & bitmap[offset]){
  		return 1;
  	}
  	else{
  		return 0;
  	}
}
