/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	// display_system_message()
	// display_object_info()
	// display_commands()
	// ...
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };
				printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
			}
			frontbuf[i][j] = backbuf[i][j];

		}
	}
	POSITION pos_r1 = { 10, 20 };
	printc(padd(map_pos, pos_r1), 'R', COLOR_GRAY);
	POSITION pos_r2 = { 6,50 };
	printc(padd(map_pos, pos_r2), 'R', COLOR_GRAY);
	POSITION pos_r3 = { 15, 50 };
	printc(padd(map_pos, pos_r3), 'R', COLOR_GRAY);
	POSITION pos_r4 = { 15, 55 };
	printc(padd(map_pos, pos_r4), 'R', COLOR_GRAY);
	POSITION pos1_5 = { 12, 1 };
	printc(padd(map_pos, pos1_5), '5', COLOR_ORANGE);
	POSITION pos2_5 = { 6, 58 };
	printc(padd(map_pos, pos2_5), '5', COLOR_ORANGE);
	POSITION pos1_w = { 4, 5 };
	printc(padd(map_pos, pos1_w), 'W', COLOR_YELLOW);
	POSITION pos2_w = { 13, 52 };
	printc(padd(map_pos, pos2_w), 'W', COLOR_YELLOW);
	POSITION pos1_h = { 14, 1 };
	printc(padd(map_pos, pos1_h), 'H', COLOR_BLUE);
	POSITION pos2_h = { 3, 58 };
	printc(padd(map_pos, pos2_h), 'H', COLOR_RED);
	for (int h_b = 1; h_b < 3; h_b++) {
		for (int w_b = 1; w_b < 3; w_b++) {
			POSITION pos1_b = { 17 - h_b, w_b };
			printc(padd(map_pos, pos1_b), 'B', COLOR_BLUE);
			POSITION pos2_b = { h_b, 59-w_b };
			printc(padd(map_pos, pos2_b), 'B', COLOR_RED);
			POSITION pos1_p = { 17 - h_b, w_b+2 };
			printc(padd(map_pos, pos1_p), 'P', COLOR_BLACK);
			POSITION pos2_p = { h_b, 57 - w_b };
			printc(padd(map_pos, pos2_p), 'P', COLOR_BLACK);
			POSITION pos1_r = { 9 - h_b, 30 - w_b };
			printc(padd(map_pos, pos1_r), 'R', COLOR_GRAY);
			POSITION pos2_r = { 9 + h_b, 30 + w_b };
			printc(padd(map_pos, pos2_r), 'R', COLOR_GRAY);

		}
	}
}

// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}