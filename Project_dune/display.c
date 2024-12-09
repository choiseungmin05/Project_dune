/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"
#include "common.h"

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

// 각 문자의 속성을 정의
MapElement elements[] = {
	{'R', {10, 20}, COLOR_GRAY},
	{'R', {6, 50}, COLOR_GRAY},
	{'R', {15, 50}, COLOR_GRAY},
	{'R', {15, 55}, COLOR_GRAY},
	{'5', {12, 1}, COLOR_ORANGE},
	{'5', {6, 58}, COLOR_ORANGE},
	{'W', {4, 5}, COLOR_YELLOW},
	{'W', {13, 52}, COLOR_YELLOW},
	{'H', {14, 1}, COLOR_BLUE},
	{'H', {3, 58}, COLOR_RED},
};

// 'B', 'P', 'R' 문자를 출력하는 함수
void drawAdditionalElements() {
	for (int h_b = 1; h_b < 3; h_b++) {
		for (int w_b = 1; w_b < 3; w_b++) {
			POSITION pos1_b = { 17 - h_b, w_b };
			printc(padd(map_pos, pos1_b), 'B', COLOR_BLUE);
			POSITION pos2_b = { h_b, 59 - w_b };
			printc(padd(map_pos, pos2_b), 'B', COLOR_RED);
			POSITION pos1_p = { 17 - h_b, w_b + 2 };
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
	// 배열을 사용하여 문자를 출력
	for (int i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
		printc(padd(map_pos, elements[i].pos), elements[i].ch, elements[i].color);
	}

	// 추가 요소들 출력
	drawAdditionalElements();
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

//상태창
void status_wd(int x, int y, int width, int height) {
	POSITION pos;
	set_color(COLOR_DEFAULT);

	// 상단 테두리
	pos.row = y;
	for (int j = 0; j < width; j++) {
		pos.column = x + j;
		printc(pos, '#', COLOR_DEFAULT);
	}

	// 중간 부분
	for (int i = 1; i < height - 1; i++) {
		pos.row = y + i;

		// 좌측 테두리
		pos.column = x;
		printc(pos, '#', COLOR_DEFAULT);

		// 내부 공백
		for (int j = 1; j < width - 1; j++) {
			pos.column = x + j;
			printc(pos, ' ', COLOR_DEFAULT);
		}

		// 우측 테두리
		pos.column = x + width - 1;
		printc(pos, '#', COLOR_DEFAULT);
	}

	// 하단 테두리
	pos.row = y + height - 1;
	for (int j = 0; j < width; j++) {
		pos.column = x + j;
		printc(pos, '#', COLOR_DEFAULT);
	}
}

//시스템 메세지
void system_wd(int x, int y, int width, int height) {
	POSITION pos;
	set_color(COLOR_DEFAULT);

	// 상단 테두리
	pos.row = y;
	for (int j = 0; j < width; j++) {
		pos.column = x + j;
		printc(pos, '#', COLOR_DEFAULT);
	}

	// 중간 부분
	for (int i = 1; i < height - 1; i++) {
		pos.row = y + i;

		// 좌측 테두리
		pos.column = x;
		printc(pos, '#', COLOR_DEFAULT);

		// 내부 공백
		for (int j = 1; j < width - 1; j++) {
			pos.column = x + j;
			printc(pos, ' ', COLOR_DEFAULT);
		}

		// 우측 테두리
		pos.column = x + width - 1;
		printc(pos, '#', COLOR_DEFAULT);
	}

	// 하단 테두리
	pos.row = y + height - 1;
	for (int j = 0; j < width; j++) {
		pos.column = x + j;
		printc(pos, '#', COLOR_DEFAULT);
	}
}


//명령창
void command_wd(int x, int y, int width, int height) {
	POSITION pos;
	set_color(COLOR_DEFAULT);

	// 상단 테두리
	pos.row = y;
	for (int j = 0; j < width; j++) {
		pos.column = x + j;
		printc(pos, '#', COLOR_DEFAULT);
	}

	// 중간 부분
	for (int i = 1; i < height - 1; i++) {
		pos.row = y + i;

		// 좌측 테두리
		pos.column = x;
		printc(pos, '#', COLOR_DEFAULT);

		// 내부 공백
		for (int j = 1; j < width - 1; j++) {
			pos.column = x + j;
			printc(pos, ' ', COLOR_DEFAULT);
		}

		// 우측 테두리
		pos.column = x + width - 1;
		printc(pos, '#', COLOR_DEFAULT);
	}

	// 하단 테두리
	pos.row = y + height - 1;
	for (int j = 0; j < width; j++) {
		pos.column = x + j;
		printc(pos, '#', COLOR_DEFAULT);
	}
}

// 상태창을 지우는 함수
void clear_status_line(POSITION pos) {
	gotoxy(pos); // 해당 위치로 커서를 이동
	set_color(COLOR_DEFAULT); // 기본 색상으로 설정
	for (int i = 0; i < 57; i++) { // 주어진 너비만큼 비움
		printf(" "); // 빈 공간으로 덮기
	}
}



// 객체 정보를 출력하는 함수
void display_object_info(POSITION curr) {
	set_color(COLOR_DEFAULT);
	POSITION status_pos = { 2, 62 }; // 상태창 위치 설정


	gotoxy(status_pos); // 상태창 위치로 커서 이동

	// 커서 위치에 따라 객체 정보를 가져와 출력
	if (curr.row >= 0 && curr.row < MAP_HEIGHT && curr.column >= 0 && curr.column < MAP_WIDTH) {
		for (int i = 0; i < 10; i++) { // 10줄을 지울 수 있도록 설정
			clear_status_line((POSITION) { 2 + i, 62 }, 57); // 각 줄을 지웁니다.
		}
		if (map[0][curr.row][curr.column] == 'B') {
			gotoxy((POSITION) { 2, 62 });
			printf("건물: 본진(아트에이디스)\n");
			gotoxy((POSITION) { 3, 62 });
			printf("설명: \n");
			gotoxy((POSITION) { 4, 62 });
			printf("건설비용: 없음\n");
			gotoxy((POSITION) { 5, 62 });
			printf("내구도: 50\n");
			gotoxy((POSITION) { 6, 62 });
			printf("명령어: H:하베스터 생산\n");
		}
		else if (map[0][curr.row][curr.column] == 'P') {
			gotoxy((POSITION) { 2, 62 });
			printf("건물: 장판\n");
			gotoxy((POSITION) { 3, 62 });
			printf("설명: 건물 짓기 전에 깔기\n");
			gotoxy((POSITION) { 4, 62 });
			printf("건설비용: 1\n");
			gotoxy((POSITION) { 5, 62 });
			printf("내구도: 없음\n");
			gotoxy((POSITION) { 6, 62 });
			printf("명령어: 없음\n");
		}
		else if (map[0][curr.row][curr.column] == 'R') {
			gotoxy((POSITION) { 2, 62 });
			printf("바위\n");
			gotoxy((POSITION) { 3, 62 });
			printf("설명: 샌드윔은 통과할 수 없음\n");
		}
		else if (map[1][curr.row][curr.column] == 'W') {
			gotoxy((POSITION) { 2, 62 });
			printf("유닉: 샌드윔\n");
			gotoxy((POSITION) { 3, 62 });
			printf("생산비용: 없음\n");
			gotoxy((POSITION) { 4, 62 });
			printf("인구수: 없음\n");
			gotoxy((POSITION) { 5, 62 });
			printf("이동주기: 2500\n");
			gotoxy((POSITION) { 6, 62 });
			printf("공격력: 무한대\n");
			gotoxy((POSITION) { 7, 62 });
			printf("공격주기: 10000\n");
			gotoxy((POSITION) { 8, 62 });
			printf("체력: 무한대\n");
			gotoxy((POSITION) { 9, 62 });
			printf("시야: 무한대\n");
			gotoxy((POSITION) { 10, 62 });
			printf("명령어: 없음\n");
		}
		else if (map[0][curr.row][curr.column] == '5') {
			gotoxy((POSITION) { 2, 62 });
			printf("유닉: 스파이스\n");
		}
		else if (map[1][curr.row][curr.column] == 'H') {
			gotoxy((POSITION) { 2, 62 });
			printf("유닉: 하베스터\n");
			gotoxy((POSITION) { 3, 62 });
			printf("생산비용: 5\n");
			gotoxy((POSITION) { 4, 62 });
			printf("인구수: 5\n");
			gotoxy((POSITION) { 5, 62 });
			printf("이동주기: 2000\n");
			gotoxy((POSITION) { 6, 62 });
			printf("공격력: 없음\n");
			gotoxy((POSITION) { 7, 62 });
			printf("공격주기: 없음\n");
			gotoxy((POSITION) { 8, 62 });
			printf("체력: 70\n");
			gotoxy((POSITION) { 9, 62 });
			printf("시야: 0\n");
			gotoxy((POSITION) { 10, 62 });
			printf("명령어: H:Harvest, M:Move\n");
		}
		else {
			gotoxy((POSITION) { 2, 62 });
			printf("설명:기본 지형(빈칸),건물을 지을 수 없음\n");
		}
	}
	else {
		gotoxy((POSITION) { 2, 62 });
		printf("설명:잘못된 위치입니다.\n");
	}
}

// 명령어 정보를 출력하는 함수
void display_command_info(POSITION curr) {
	set_color(COLOR_DEFAULT);
	POSITION command_pos = { 20, 62 }; // 명령어 위치 설정
	int command_width = 80; // 명령어의 너비

	// 명령어 영역 지우기
	clear_status_line(command_pos, command_width);

	gotoxy(command_pos); // 명령어 위치로 커서 이동

	// 커서 위치에 따라 명령어 정보를 가져와 출력
	if (curr.row >= 0 && curr.row < MAP_HEIGHT && curr.column >= 0 && curr.column < MAP_WIDTH) {
		if (map[0][curr.row][curr.column] == 'B') {
			printf("명령어:H: 하베스터 생산\n");
		}
		else if (map[1][curr.row][curr.column] == 'H') {
			printf("명령어: H:Harvest, M:Move\n");
		}
		else {
			printf("\n");
		}
	}
	else {
		printf("명령어:잘못된 위치입니다.\n");
	}
}