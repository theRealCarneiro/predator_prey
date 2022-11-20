#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "creature.h"


list create_list(int size){
	list l = {size, 0, malloc(sizeof(creature) * size)};
	return l;
}

void destroy_list(list l) {
	free(l.array);
}

void append_list(list* l, creature c) {
	if (l->used + 1 >= l->size) {
		l->array = realloc(l->array, l->size * 2 * sizeof(creature));
		l->size *= 2;
	}

	l->array[l->used++] = c;
	/*printf("%d %d %d\n", c.type, c.x, c.y);*/
}

void remove_id(list* l, int id) {
	memmove(&l->array[id], &l->array[id + 1], (l->used - id - 1) * sizeof(creature));
	l->used--;
}

void remove_creature(list* l, creature c) {
	for (int i = 0; i < l->used; i++) {
		if(l->array[i].x == c.x && l->array[i].y == c.y &&
				l->array[i].proc_age == c.proc_age &&
				l->array[i].hung_age == c.hung_age &&
				l->array[i].type == c.type){ 
			memmove(&l->array[i], &l->array[i + 1], (l->used - i - 1) * sizeof(creature));
			l->used--;
		}
	}
}

creature create_creature(int x, int y, int type) {
	creature c = {x, y, type, 0, 0};
	return c;
}

void print_list(list* l) {
	for (int i = 0; i < l->used; i++){
		char* type = l->array[i].type == FOX ? "FOX" : "BUNNY";
		printf("%s[%d][%d]:", type, l->array[i].x, l->array[i].y);
	}
	printf("\n");
}

// 0: north, 1: east, 2: south, 3: west, -1: invalid
// returns the type of creature
int orient_check(int L, int C, creature grid[L][C], int x, int y, int orient) {
		if (orient == 0 && x - 1 >= 0) {
			return grid[x - 1][y].type;
		}
		else if (orient == 1 && y + 1 < C) {
			return grid[x][y + 1].type;
		}
		else if (orient == 2 && x + 1 < L) {
			return grid[x + 1][y].type;
		}
		else if (orient == 3 && y - 1 >= 0) {
			return grid[x][y - 1].type;
		}
		return -1;
}

creature move_creature(int L, int C, creature grid[L][C], int x, int y, int cur_gen) {
	int P = 0;
	int p[4] = {0, 0, 0, 0};
	creature currentc = grid[x][y];

	// check grids around
	if (currentc.type == FOX) {
		for (int i = 0; i < 4; i++) {
			if (orient_check(L, C, grid, x, y, i) == BUNNY) {
				p[i] = 1;
				P++;
			}
		}
	}
	if ((currentc.type == FOX && P == 0) || currentc.type == BUNNY) {
		for (int i = 0; i < 4; i++) {
			int type = orient_check(L, C, grid, x, y, i);
			if (type == EMPTY) {
				p[i] = 1;
				P++;
			}
		}
	}

	if (P == 0) {
		return currentc;
	}

	creature new_pos = currentc;

	int pos = (cur_gen + x + y) % P;

	/*printf("%d %d %d %d -- ", p[0], p[1], p[2], p[3]);*/
	int tmp = 0, i = 0;
	for (i = 0; i < 4; i++) {
		if (p[i] == 1) {
			if (tmp == pos)
				break;
			tmp++;
		}
	}
	/*printf("%d[%d][%d]:", grid[x][y].type, grid[x][y].x, grid[x][y].y);*/

	// north
	if (i == 0)
		new_pos.x--;

	// east
	else if (i == 1)
		new_pos.y++;
	
	// south
	else if (i == 2)
		new_pos.x++;
	
	// west
	else if (i == 3)
		new_pos.y--;

	/*printf("[%d][%d]\n", new_pos.x, new_pos.y);*/
	return new_pos;
}

list move_creatures(int L, int C, creature grid[L][C], int cur_gen, list l) {
	list l_new = create_list(l.size);
	creature c;
	int x, y;
	for (int i = 0; i < l.used; i++){

		x = l.array[i].x;
		y = l.array[i].y;

		append_list(&l_new, move_creature(L, C, grid, x, y, cur_gen));
	}
	/*print_list(&l_new);*/
	return l_new;
}

// 0 means atack lost, 1 means atack wins
int brawl(creature atack, creature defense) {
	switch (atack.type) {
		case BUNNY:
			if (defense.type == EMPTY) return 1;
			return (atack.proc_age > defense.proc_age) ? 1 : 0;
			break;
		case FOX:
			if (defense.type != FOX) return 1;
			return (atack.proc_age > defense.proc_age || 
					(atack.proc_age == defense.proc_age &&
					atack.hung_age < defense.hung_age)) ? 1 : 0;
			break;
	}
	return 0;
}

void solve_conflict(int L, int C, creature grid[L][C], int cur_gen, int proc_age,
		int hung_age, list l_old, list* l_other, list* l) {

	// list for removing creatures
	creature c;
	list r = create_list(l->used);
	for (int i = 0; i < l->used; i++){
		int ax = l->array[i].x;
		int ay = l->array[i].y;

		ax = l->array[i].x;
		ay = l->array[i].y;

		creature atack = l->array[i];
		creature defense = grid[ax][ay];

		if (brawl(atack, defense)) {

			// get creature position on grid
			int oldx = l_old.array[i].x;
			int oldy = l_old.array[i].y;
			creature* old_creature = &(grid[oldx][oldy]);

			// can reproduce
			if (atack.proc_age++ >= proc_age){
				atack.proc_age = 0;
				old_creature->hung_age = 0;
				append_list(l, *old_creature);

			// cant reproduce
			} else {
				old_creature->type = EMPTY;
				old_creature->proc_age = 0;
				old_creature->hung_age = 0;
			} 

			if (atack.type == FOX) {

				// reset hunger
				if (defense.type == BUNNY) {
					atack.hung_age = 0;
					remove_creature(l_other, defense);
				} else {
					atack.hung_age++;
				} 

				// death from starvation
				if (atack.hung_age >= hung_age) {
					creature tmp = atack;
					atack.hung_age = 0;
					atack.proc_age = 0;
					atack.type = EMPTY;
					append_list(&r, tmp);
				}
			}

			// remove creature that lost
			if (defense.type != EMPTY)
				append_list(&r, defense);


			// update grid
			grid[ax][ay] = atack;
		}
	} 

	// remove dead creatures
	for (int i = 0; i < r.used; i++)
		remove_creature(l, r.array[i]);

	destroy_list(r);
}

void print_grid(int L, int C, creature grid[L][C]){
	for (int i = 0; i < L; i++) {
		for (int j = 0; j < C; j++) {
			switch(grid[i][j].type){
				case EMPTY:
					printf("- ");
					break;
				case ROCK:
					printf("* ");
					break;
				case BUNNY:
					printf("C ");
					break;
				case FOX:
					printf("R ");
					break;
			}			
		}
		printf("\n");
	}
	printf("\n");
}
