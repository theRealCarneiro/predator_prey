#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
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
				l->array[i].hung_age == c.hung_age &&
				l->array[i].proc_age == c.proc_age){ 
			memmove(&l->array[i], &l->array[i + 1], (l->used - i - 1) * sizeof(creature));
			l->used--;
		}
	}
}

creature create_creature(int x, int y, int type) {
	creature c = {x, y, type, 0, 0};
	return c;
}

void print_creature(creature c) {
	char type[6];
	switch (c.type){
		case FOX:
			strcpy(type, "FOX");
			break;
		case BUNNY:
			strcpy(type, "BUNNY");
			break;
		case ROCK:
			strcpy(type, "FOX");
			break;
		case EMPTY:
			strcpy(type, "EMPTY");
			break;
		default:
			strcpy(type, "NULL");
			break;
	}
	printf("%s[%d][%d](%d)(%d):", type, c.x, c.y, c.hung_age, c.proc_age);
}

void print_list(list* l) {
	for (int i = 0; i < l->used; i++){
		print_creature(l->array[i]);
	}
	printf("\n");
}

void copy_creature(creature *old, creature new) {
	old->type = new.type;
	old->proc_age = new.proc_age;
	old->hung_age = new.hung_age;
	old->x = new.x;
	old->y = new.y;
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

creature move_creature(int L, int C, creature grid[L][C], creature c, int cur_gen) {
	int P = 0;
	int p[4] = {0, 0, 0, 0};

	// check grids around
	if (c.type == FOX) {
		for (int i = 0; i < 4; i++) {
			if (orient_check(L, C, grid, c.x, c.y, i) == BUNNY) {
				p[i] = 1;

				P++;
			}
		}
	}
	if ((c.type == FOX && P == 0) || c.type == BUNNY) {
		for (int i = 0; i < 4; i++) {
			int type = orient_check(L, C, grid, c.x, c.y, i);
			if (type == EMPTY) {
				p[i] = 1;

				P++;
			}
		}
	}

	if (P == 0)
		return c;

	int pos = (cur_gen + c.x + c.y) % P;

	int tmp = 0, i = 0;
	for (i = 0; i < 4; i++) {
		if (p[i] == 1) {
			if (tmp == pos)
				break;
			tmp++;
		}
	}

	// north
	if (i == 0)
		c.x--;

	// east
	else if (i == 1)
		c.y++;
	
	// south
	else if (i == 2)
		c.x++;
	
	// west
	else if (i == 3)
		c.y--;

	return c;
}

list move_creatures(int L, int C, creature grid[L][C], int cur_gen, list l) {
	list l_new = create_list(l.size);
	l_new.used = l.used;

	# pragma omp parallel for
	for (int i = 0; i < l.used; i++){

		creature c = move_creature(L, C, grid, l.array[i], cur_gen);
		l_new.array[i] = c;
	}

	return l_new;
}

// 0 means atack lost, 1 means atack wins
int brawl(creature atack, creature defense) {
	switch (atack.type) {
		case BUNNY:
			if (defense.type == EMPTY) {
				return 1;
			}

			return (atack.proc_age > defense.proc_age) ? 1 : 0;
			break;

		case FOX:
			if (defense.type != FOX){
				return 1;
			}

			return (atack.proc_age > defense.proc_age || 
					(atack.proc_age == defense.proc_age &&
					atack.hung_age < defense.hung_age)) ? 1 : 0;
			break;

		default:
			return 0;
	}
	return 0;
}

void solve_conflict(int L, int C, creature grid[L][C], int cur_gen, int proc_age,
		int hung_age, list l_old, list* l_other, list* l) {


	// list for removing creatures
	list dead_same_sp = create_list(l->used);
	list dead_other_sp = create_list(l->used);
	list new_borns = create_list(l->used);

	# pragma omp parallel for
	for (int i = 0; i < l->used; i++) {
		int ax, ay, oldx, oldy;
		creature atack, defense, old_position;

		oldx = l_old.array[i].x;
		oldy = l_old.array[i].y;
		ax = l->array[i].x;
		ay = l->array[i].y;

		# pragma omp critical (grid)
		{
			old_position = l_old.array[i];
			defense = grid[ax][ay];
			atack = l->array[i];
		}

		if (atack.type == FOX)
			atack.hung_age++;

		// if gonna atack bunny, don't starve
		if (atack.hung_age >= hung_age && defense.type != BUNNY) {

			// set to empty so it loses brawl
			atack.type = EMPTY;
			atack.proc_age = 0;
			atack.hung_age = 0;
		}

		// procreate
		old_position.proc_age = 0;
		old_position.hung_age = 0;
		if (++atack.proc_age > proc_age){
			atack.proc_age = 0;
			old_position.type = atack.type;

			# pragma omp critical (new_borns)
			append_list(&new_borns, old_position);
		} else {
			old_position.type = EMPTY;
		} 

		# pragma omp critical (grid)
		copy_creature(&grid[oldx][oldy], old_position);

		if (brawl(atack, defense) == 1){

			if (atack.type == FOX && defense.type == BUNNY) {
				atack.hung_age = 0;

				# pragma omp critical (dead_other_sp)
				append_list(&dead_other_sp, defense);
			}

			// remove creature that lost
			if (defense.type == atack.type) {

				// other fox already ate a bunny
				if (defense.hung_age == 0) {
					atack.hung_age = 0;
				}

				# pragma omp critical (dead_same_sp)
				append_list(&dead_same_sp, defense);
			}

			// update grid
			# pragma omp critical (grid)
			{
				l->array[i] = atack;
				copy_creature(&grid[ax][ay], l->array[i]);
			}

		}

		// lost brawl/starved
		else {

			# pragma omp critical (grid)
			l->array[i] = atack;

			# pragma omp critical (dead_same_sp)
			append_list(&dead_same_sp, atack);
		}
	}

	// remove dead creatures
	for (int i = 0; i < dead_same_sp.used; i++) {
		remove_creature(l, dead_same_sp.array[i]);
	}
	for (int i = 0; i < l->used; i++) {
		grid[l->array[i].x][l->array[i].y] = l->array[i];
	}
	for (int i = 0; i < dead_other_sp.used; i++) {
		remove_creature(l_other, dead_other_sp.array[i]);
	}
	for (int i = 0; i < new_borns.used; i++) {
		append_list(l, new_borns.array[i]);
	}


	destroy_list(dead_same_sp);
	destroy_list(dead_other_sp);
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
