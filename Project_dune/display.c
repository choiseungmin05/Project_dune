/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"
#include "common.h"

// ����� ������� �»��(topleft) ��ǥ
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

// �� ������ �Ӽ��� ����
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

// 'B', 'P', 'R' ���ڸ� ����ϴ� �Լ�
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
	// �迭�� ����Ͽ� ���ڸ� ���
	for (int i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
		printc(padd(map_pos, elements[i].pos), elements[i].ch, elements[i].color);
	}

	// �߰� ��ҵ� ���
	drawAdditionalElements();
}


// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);

}

//����â
void status_wd(int x, int y, int width, int height) {
	POSITION pos;
	set_color(COLOR_DEFAULT);

	// ��� �׵θ�
	pos.row = y;
	for (int j = 0; j < width; j++) {
		pos.column = x + j;
		printc(pos, '#', COLOR_DEFAULT);
	}

	// �߰� �κ�
	for (int i = 1; i < height - 1; i++) {
		pos.row = y + i;

		// ���� �׵θ�
		pos.column = x;
		printc(pos, '#', COLOR_DEFAULT);

		// ���� ����
		for (int j = 1; j < width - 1; j++) {
			pos.column = x + j;
			printc(pos, ' ', COLOR_DEFAULT);
		}

		// ���� �׵θ�
		pos.column = x + width - 1;
		printc(pos, '#', COLOR_DEFAULT);
	}

	// �ϴ� �׵θ�
	pos.row = y + height - 1;
	for (int j = 0; j < width; j++) {
		pos.column = x + j;
		printc(pos, '#', COLOR_DEFAULT);
	}
}

//�ý��� �޼���
void system_wd(int x, int y, int width, int height) {
	POSITION pos;
	set_color(COLOR_DEFAULT);

	// ��� �׵θ�
	pos.row = y;
	for (int j = 0; j < width; j++) {
		pos.column = x + j;
		printc(pos, '#', COLOR_DEFAULT);
	}

	// �߰� �κ�
	for (int i = 1; i < height - 1; i++) {
		pos.row = y + i;

		// ���� �׵θ�
		pos.column = x;
		printc(pos, '#', COLOR_DEFAULT);

		// ���� ����
		for (int j = 1; j < width - 1; j++) {
			pos.column = x + j;
			printc(pos, ' ', COLOR_DEFAULT);
		}

		// ���� �׵θ�
		pos.column = x + width - 1;
		printc(pos, '#', COLOR_DEFAULT);
	}

	// �ϴ� �׵θ�
	pos.row = y + height - 1;
	for (int j = 0; j < width; j++) {
		pos.column = x + j;
		printc(pos, '#', COLOR_DEFAULT);
	}
}


//���â
void command_wd(int x, int y, int width, int height) {
	POSITION pos;
	set_color(COLOR_DEFAULT);

	// ��� �׵θ�
	pos.row = y;
	for (int j = 0; j < width; j++) {
		pos.column = x + j;
		printc(pos, '#', COLOR_DEFAULT);
	}

	// �߰� �κ�
	for (int i = 1; i < height - 1; i++) {
		pos.row = y + i;

		// ���� �׵θ�
		pos.column = x;
		printc(pos, '#', COLOR_DEFAULT);

		// ���� ����
		for (int j = 1; j < width - 1; j++) {
			pos.column = x + j;
			printc(pos, ' ', COLOR_DEFAULT);
		}

		// ���� �׵θ�
		pos.column = x + width - 1;
		printc(pos, '#', COLOR_DEFAULT);
	}

	// �ϴ� �׵θ�
	pos.row = y + height - 1;
	for (int j = 0; j < width; j++) {
		pos.column = x + j;
		printc(pos, '#', COLOR_DEFAULT);
	}
}

// ����â�� ����� �Լ�
void clear_status_line(POSITION pos) {
	gotoxy(pos); // �ش� ��ġ�� Ŀ���� �̵�
	set_color(COLOR_DEFAULT); // �⺻ �������� ����
	for (int i = 0; i < 57; i++) { // �־��� �ʺ�ŭ ���
		printf(" "); // �� �������� ����
	}
}



// ��ü ������ ����ϴ� �Լ�
void display_object_info(POSITION curr) {
	set_color(COLOR_DEFAULT);
	POSITION status_pos = { 2, 62 }; // ����â ��ġ ����


	gotoxy(status_pos); // ����â ��ġ�� Ŀ�� �̵�

	// Ŀ�� ��ġ�� ���� ��ü ������ ������ ���
	if (curr.row >= 0 && curr.row < MAP_HEIGHT && curr.column >= 0 && curr.column < MAP_WIDTH) {
		for (int i = 0; i < 10; i++) { // 10���� ���� �� �ֵ��� ����
			clear_status_line((POSITION) { 2 + i, 62 }, 57); // �� ���� ����ϴ�.
		}
		if (map[0][curr.row][curr.column] == 'B') {
			gotoxy((POSITION) { 2, 62 });
			printf("�ǹ�: ����(��Ʈ���̵�)\n");
			gotoxy((POSITION) { 3, 62 });
			printf("����: \n");
			gotoxy((POSITION) { 4, 62 });
			printf("�Ǽ����: ����\n");
			gotoxy((POSITION) { 5, 62 });
			printf("������: 50\n");
			gotoxy((POSITION) { 6, 62 });
			printf("��ɾ�: H:�Ϻ����� ����\n");
		}
		else if (map[0][curr.row][curr.column] == 'P') {
			gotoxy((POSITION) { 2, 62 });
			printf("�ǹ�: ����\n");
			gotoxy((POSITION) { 3, 62 });
			printf("����: �ǹ� ���� ���� ���\n");
			gotoxy((POSITION) { 4, 62 });
			printf("�Ǽ����: 1\n");
			gotoxy((POSITION) { 5, 62 });
			printf("������: ����\n");
			gotoxy((POSITION) { 6, 62 });
			printf("��ɾ�: ����\n");
		}
		else if (map[0][curr.row][curr.column] == 'R') {
			gotoxy((POSITION) { 2, 62 });
			printf("����\n");
			gotoxy((POSITION) { 3, 62 });
			printf("����: �������� ����� �� ����\n");
		}
		else if (map[1][curr.row][curr.column] == 'W') {
			gotoxy((POSITION) { 2, 62 });
			printf("����: ������\n");
			gotoxy((POSITION) { 3, 62 });
			printf("������: ����\n");
			gotoxy((POSITION) { 4, 62 });
			printf("�α���: ����\n");
			gotoxy((POSITION) { 5, 62 });
			printf("�̵��ֱ�: 2500\n");
			gotoxy((POSITION) { 6, 62 });
			printf("���ݷ�: ���Ѵ�\n");
			gotoxy((POSITION) { 7, 62 });
			printf("�����ֱ�: 10000\n");
			gotoxy((POSITION) { 8, 62 });
			printf("ü��: ���Ѵ�\n");
			gotoxy((POSITION) { 9, 62 });
			printf("�þ�: ���Ѵ�\n");
			gotoxy((POSITION) { 10, 62 });
			printf("��ɾ�: ����\n");
		}
		else if (map[0][curr.row][curr.column] == '5') {
			gotoxy((POSITION) { 2, 62 });
			printf("����: �����̽�\n");
		}
		else if (map[1][curr.row][curr.column] == 'H') {
			gotoxy((POSITION) { 2, 62 });
			printf("����: �Ϻ�����\n");
			gotoxy((POSITION) { 3, 62 });
			printf("������: 5\n");
			gotoxy((POSITION) { 4, 62 });
			printf("�α���: 5\n");
			gotoxy((POSITION) { 5, 62 });
			printf("�̵��ֱ�: 2000\n");
			gotoxy((POSITION) { 6, 62 });
			printf("���ݷ�: ����\n");
			gotoxy((POSITION) { 7, 62 });
			printf("�����ֱ�: ����\n");
			gotoxy((POSITION) { 8, 62 });
			printf("ü��: 70\n");
			gotoxy((POSITION) { 9, 62 });
			printf("�þ�: 0\n");
			gotoxy((POSITION) { 10, 62 });
			printf("��ɾ�: H:Harvest, M:Move\n");
		}
		else {
			gotoxy((POSITION) { 2, 62 });
			printf("����:�⺻ ����(��ĭ),�ǹ��� ���� �� ����\n");
		}
	}
	else {
		gotoxy((POSITION) { 2, 62 });
		printf("����:�߸��� ��ġ�Դϴ�.\n");
	}
}

// ��ɾ� ������ ����ϴ� �Լ�
void display_command_info(POSITION curr) {
	set_color(COLOR_DEFAULT);
	POSITION command_pos = { 20, 62 }; // ��ɾ� ��ġ ����
	int command_width = 80; // ��ɾ��� �ʺ�

	// ��ɾ� ���� �����
	clear_status_line(command_pos, command_width);

	gotoxy(command_pos); // ��ɾ� ��ġ�� Ŀ�� �̵�

	// Ŀ�� ��ġ�� ���� ��ɾ� ������ ������ ���
	if (curr.row >= 0 && curr.row < MAP_HEIGHT && curr.column >= 0 && curr.column < MAP_WIDTH) {
		if (map[0][curr.row][curr.column] == 'B') {
			printf("��ɾ�:H: �Ϻ����� ����\n");
		}
		else if (map[1][curr.row][curr.column] == 'H') {
			printf("��ɾ�: H:Harvest, M:Move\n");
		}
		else {
			printf("\n");
		}
	}
	else {
		printf("��ɾ�:�߸��� ��ġ�Դϴ�.\n");
	}
}