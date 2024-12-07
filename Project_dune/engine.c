#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

ElementInfo element_info[MAP_HEIGHT][MAP_WIDTH];

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
POSITION sample_obj_next_position(void);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };



RESOURCE resource = { 
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.speed = 300,
	.next_move_time = 300
};

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor);

	// 각 섹션 그리기
	status_wd(61, 1, STATUS_W, STATUS_H);
	system_wd(0, 19, SYSTEM_W, SYSTEM_H);
	command_wd(61, 19, COMMAND_W, COMMAND_H);


	while (1) {
		// 키 입력 확인
		KEY key = get_key();

		// 방향키 입력 처리
		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			// 방향키 외의 입력 처리
			switch (key) {
			case k_quit:
				outro();
				break;
			case k_space: {  // 스페이스바 입력 처리
				POSITION curr = cursor.current;
				display_object_info(curr);  // 객체 정보 출력
				display_command_info(curr);  // 명령어 정보 출력
				break;
			}
			case k_escape: {  // Esc 키 입력 처리
				POSITION command_pos = { 20,  62 };
				clear_status_line(command_pos, 80);
				for (int i = 0; i < 10; i++) { // 10줄을 지울 수 있도록 설정
					clear_status_line((POSITION) { 2 + i, 62 }, 57); // 각 줄을 지웁니다.
				}
				break;
			}
			case k_none:
			case k_undef:
			default:
				break;
			}
		}

		// 샘플 오브젝트 동작
		sample_obj_move();

		// 화면 출력
		display(resource, map, cursor);
		Sleep(TICK);  // 주기적인 업데이트를 위해 대기
		sys_clock += 10; // 시스템 시계 업데이트
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");		
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}


void init(void) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[0][i][j] = ' ';  // 기본값 설정
		}
	}
	// layer 0(map[0])에 지형 생성
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT -1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH-1; j++) {
			map[0][i][j] = ' ';
			for (int h_b = 1; h_b < 3; h_b++) {
				for (int w_b = 1; w_b < 3; w_b++) {
					map[0][17 - h_b][w_b] = 'B';
					map[0][h_b][59 - w_b] = 'B';
					map[0][17 - h_b][w_b + 2] = 'P';
					map[0][h_b][57 - w_b] = 'P';
					map[0][9 - h_b][30 - w_b] = 'R';
					map[0][9 + h_b][30 + w_b] = 'R';
				}

			}
			map[0][10][20] = 'R';
			map[0][6][50] = 'R';
			map[0][15][55] = 'R';
			map[0][12][1] = '5';
			map[0][6][58] = '5';
		}
	}

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
			map[1][14][1] = 'H';
			map[1][3] [58] = 'H';
			map[1][4][5] = 'W';
			map[1][13][52] = 'W';
		}
	}

	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';

	// ElementInfo 배열 초기화
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			element_info[i][j] = (ElementInfo){ "", "없음", "없음" }; // 기본값
		}
	}

	// 각 위치에 대한 정보 설정
	element_info[17][20] = (ElementInfo){ "", "없음", "50" }; // B 위치
	element_info[17][58] = (ElementInfo){ "건물짓기 전에 깔기", "1", "없음" }; // P 위치
	// 나머지 위치는 필요에 따라 설정
}

DIRECTION last_direction = d_stay;  // 마지막으로 입력된 방향
int last_time = 0;  // 마지막 입력 시간
// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos;

	// 연속 입력을 감지하기 위해 같은 방향키가 두 번 눌렸는지 확인
	if (last_direction == dir && (sys_clock - last_time < 100)) { // 100ms 이내에 같은 키 입력
		// 4칸 이동
		new_pos = curr;
		for (int i = 0; i < 4; i++) {
			new_pos = pmove(new_pos, dir);

			// 이동 가능한 위치인지 확인
			if (new_pos.row < 1 || new_pos.row > MAP_HEIGHT - 2 || \
				new_pos.column < 1 || new_pos.column > MAP_WIDTH - 2) {
				break;  // 맵의 경계를 넘으면 중단
			}
		}
	}
	else {
		// 일반적인 1칸 이동
		new_pos = pmove(curr, dir);
	}

	// 위치 유효성 검사 후 커서 이동
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}

	// 마지막 방향 및 시간 업데이트
	last_direction = dir;
	last_time = sys_clock;
}


/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}
	
	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}
	
	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {
		
		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.speed;
}


