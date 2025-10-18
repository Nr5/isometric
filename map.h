#include <stdint.h>
#define MAP_WIDTH 1024
int subtile = 64;

int32_t  global_position[2]={128*64,128*64};
int32_t glob_pos2[2]={128*64,128*64};
int32_t  map_position[2]={10000,10000};
uint8_t worldmap_ground[MAP_WIDTH*MAP_WIDTH * 4];
uint8_t worldmap_entities[MAP_WIDTH*MAP_WIDTH];


