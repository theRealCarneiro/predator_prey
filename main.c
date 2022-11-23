#include <string.h>
#include <stdio.h>
#include "creature.h"

void read_settings(settings* s) {
	scanf("%d %d %d %d %d %d %d",
		&s->GEN_PROC_BUNNY,
		&s->GEN_PROC_FOX,
		&s->GEN_FOOD_FOX,
		&s->N_GEN, &s->L, &s->C, &s->N
	);
}

void read_grid(settings s, creature* grid, list* bunny, list* fox, list* rock) {
	int x, y;
	char obj[10];

	while(scanf("%s %d %d\n", obj, &x, &y) == 3){

		// rock
		if (!strcmp("ROCHA", obj)) {
			grid[x * s.C + y].type = ROCK;
			append_creature(rock, grid[x * s.C + y]);
		} 

		// bunny
		else if (!strcmp("COELHO", obj)) {
			grid[x * s.C + y].type = BUNNY;
			append_creature(bunny, grid[x * s.C + y]);
		}

		// fox
		else if (!strcmp("RAPOSA", obj)) {
			grid[x * s.C + y].type = FOX;
			append_creature(fox, grid[x * s.C + y]);
		}

	}
}

int main() {
	list rock, bunny, fox;
	settings s;
	read_settings(&s);

	creature grid[s.L * s.C];
	for (int i = 0; i < s.L; i++) {
		for (int j = 0; j < s.C; j++) {
			creature c = {i, j, i, j, EMPTY, 0, 0, 0};
			grid[i * s.C + j] = c;
		}
	}

	bunny = create_list(s.L * s.C);
	fox = create_list(s.L * s.C);
	rock = create_list(s.L * s.C);
	read_grid(s, grid, &bunny, &fox, &rock);

	for (s.C_GEN = 0; s.C_GEN < s.N_GEN; s.C_GEN++) {
		move_creatures(s, grid, &bunny);
		solve_conflict(s, grid, &fox, &bunny);
		move_creatures(s, grid, &fox);
		solve_conflict(s, grid, &bunny, &fox);
	}

	s.N = rock.used + bunny.used + fox.used;

	printf("%d %d %d %d %d %d %d\n",
		s.GEN_PROC_BUNNY, s.GEN_PROC_FOX, s.GEN_FOOD_FOX, 0, s.L, s.C, s.N
	);

	for (int i = 0; i < rock.used; i++) {
		printf("ROCHA %d %d\n", rock.array[i].x, rock.array[i].y);
	}

	for (int i = 0; i < bunny.used; i++) {
		printf("COELHO %d %d\n", bunny.array[i].x, bunny.array[i].y);
	}

	for (int i = 0; i < fox.used; i++) {
		printf("RAPOSA %d %d\n", fox.array[i].x, fox.array[i].y);
	}

	destroy_list(rock);
	destroy_list(bunny);
	destroy_list(fox);

	return 0;
}
