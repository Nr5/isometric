typedef struct Entity{
	int32_t pos_x;
	int32_t pos_y;
	struct Entity* target;
}Entity;

void entity_move(Entity* entity){
	if (!entity->target)return;
	int	entity_pos_x_rel = entity->pos_x /64 - map_position[0];
	int	entity_pos_y_rel = entity->pos_y /64 - map_position[1];	
	
	worldmap_entities[entity_pos_y_rel * MAP_WIDTH + entity_pos_x_rel] = 0;
	uint32_t dx = entity->target->pos_x - entity->pos_x;
	uint32_t dy = entity->target->pos_y - entity->pos_y;
	int distance = sqrt( dx * dx + dy * dy );
	
	if (distance >16 && entity_pos_x_rel >= 0 && entity_pos_x_rel < MAP_WIDTH && entity_pos_y_rel >= 0 && entity_pos_y_rel < MAP_WIDTH ){
		int new_x = entity->pos_x+( (entity->target->pos_x - entity->pos_x) /(distance/16))  ;
		int new_y = entity->pos_y+( (entity->target->pos_y - entity->pos_y) /(distance/16))  ;
		if ( 
			(int)(worldmap_ground[(int) ((new_y / subtile) - map_position[1]) * MAP_WIDTH +((int) entity->pos_x / subtile) - map_position[0]] ) 	&&
			(int)(worldmap_ground[(int) ((new_y / subtile + !!(new_y % subtile) ) - map_position[1]) * MAP_WIDTH +((int) entity->pos_x / subtile) - map_position[0]] ) 	&&
			(int)(worldmap_ground[(int) ((new_y / subtile + !!(new_y % subtile) ) - map_position[1]) * MAP_WIDTH +(int) ((entity->pos_x / subtile) - map_position[0] + !!(entity->pos_x % subtile)) ] ) &&
			(int)(worldmap_ground[(int) ((new_y / subtile)     - map_position[1]) * MAP_WIDTH +  (int) ((entity->pos_x / subtile) - map_position[0] + !!(entity->pos_x % subtile)) ] )	&&
		


			!(int)(worldmap_entities[(int) ((new_y / subtile) - map_position[1]) * MAP_WIDTH +((int) entity->pos_x / subtile) - map_position[0]]  ) 	&&
			!(int)(worldmap_entities[(int) ((new_y / subtile + !!(new_y % subtile) ) - map_position[1]) * MAP_WIDTH +((int) entity->pos_x / subtile) - map_position[0]] ) 	&&
			!(int)(worldmap_entities[(int) ((new_y / subtile + !!(new_y % subtile) ) - map_position[1]) * MAP_WIDTH +(int) ((entity->pos_x / subtile) - map_position[0] + !!(entity->pos_x % subtile)) ] ) &&
			!(int)(worldmap_entities[(int) ((new_y / subtile)     - map_position[1]) * MAP_WIDTH +  (int) ((entity->pos_x / subtile) - map_position[0] + !!(entity->pos_x % subtile)) ] )
		   ){
			entity->pos_y=new_y;
		}
		if ( 
			(int)(worldmap_ground[(int) ((entity->pos_y / subtile) - map_position[1])     * MAP_WIDTH +(int) (new_x / subtile) - map_position[0]] )  	&&
			(int)(worldmap_ground[(int) ((entity->pos_y / subtile) - map_position[1] +  !!(entity->pos_y % subtile) ) * MAP_WIDTH +(int) (new_x / subtile) - map_position[0]] ) 	&&
			(int)(worldmap_ground[(int) ((entity->pos_y / subtile) - map_position[1] +  !!(entity->pos_y % subtile) ) * MAP_WIDTH +(int) (new_x / subtile - map_position[0] +  !!(new_x % subtile) ) ]  ) &&
			(int)(worldmap_ground[(int) ((entity->pos_y / subtile) - map_position[1] )    * MAP_WIDTH +(int) (new_x / subtile - map_position[0] +  !!(new_x % subtile) ) ]  ) 
		&&	
			!(int)(worldmap_entities[(int) ((entity->pos_y / subtile) - map_position[1])     * MAP_WIDTH +(int) (new_x / subtile) - map_position[0]]  ) 	&&
			!(int)(worldmap_entities[(int) ((entity->pos_y / subtile) - map_position[1] +  !!(entity->pos_y % subtile) ) * MAP_WIDTH +(int) (new_x / subtile) - map_position[0]]  ) 	&&
			!(int)(worldmap_entities[(int) ((entity->pos_y / subtile) - map_position[1] +  !!(entity->pos_y % subtile) ) * MAP_WIDTH +(int) (new_x / subtile - map_position[0] +  !!(new_x % subtile) ) ]) &&
			!(int)(worldmap_entities[(int) ((entity->pos_y / subtile) - map_position[1] )    * MAP_WIDTH +(int) (new_x / subtile - map_position[0] +  !!(new_x % subtile) ) ]  )
			){	
			entity->pos_x=new_x;
		}
	
	}else{
		entity->pos_x=entity->target->pos_x;
		entity->pos_y=entity->target->pos_y;
	}
	

	worldmap_entities[(entity->pos_y /subtile - map_position[1]) * MAP_WIDTH +(entity->pos_x / subtile - map_position[0])] = 1;
}

