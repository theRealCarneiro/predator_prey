#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "creature.h"


// Malloc list 
list create_list(int size){
	list l = {size, 0, malloc(sizeof(creature) * size)};
	return l;
}

// Free list
void destroy_list(list l) {
	free(l.array);
}

// Add creature to list, reallocs it if necessary
void append_creature(list* l, creature c) {

	if (l->used + 1 >= l->size){
		l->array = realloc(l->array, l->size * 2 * sizeof(creature));
		l->size *= 2;
	}

	c.id = l->used;
	l->array[l->used++] = c;
}

// removes creatures that have invalid type
void fix_list(list* l) {
	int j = 0;
	int size = l->used;
	for (int i = 0; i < size; i++) {
		if (l->array[i].type != -1) {
			if(i != j){
				l->array[i].id = j;
				l->array[j] = l->array[i];
			}
			j++;
			
		} else l->used--;
	}
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
	printf("%s[%d][%d]-[%d][%d](%d)(%d):", type, c.x, c.y, c.oldx, c.oldy, c.hung_age, c.proc_age);
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
int orient_check(settings s, creature* grid, int p[4], int x, int y, int type) {
	int P = 0;
	if (x - 1 >= 0 && grid[(x - 1) * s.C + y].type == type) {
		p[0] = 1;
		P++;
	}
	if (y + 1 < s.C && grid[x * s.C + y + 1].type == type) {
		p[1] = 1;
		P++;
	}
	if (x + 1 < s.L && grid[(x + 1) * s.C + y].type == type) {
		p[2] = 1;
		P++;
	}
	if (y - 1 >= 0 && grid[x * s.C + y - 1].type == type) {
		p[3] = 1;
		P++;
	}
	return P;
}

// Moves a specific creature (does not update grid)
creature move_creature(settings s, creature* grid, creature c) {

	int P = 0;
	int p[4] = {0, 0, 0, 0};

	// check grids around
	if (c.type == FOX)
		P = orient_check(s, grid, p, c.x, c.y, BUNNY);

	if (P == 0)
		P = orient_check(s, grid, p, c.x, c.y, EMPTY);

	if (P == 0)
		return c;

	int pos = (s.C_GEN + c.x + c.y) % P;

	int tmp = 0, i = 0;
	for (i = 0; i < 4; i++) {
		if (p[i] == 1) {
			if (tmp == pos)
				break;
			tmp++;
		}
	}

	if (i == 0)			// north
		c.x--;
	else if (i == 1)	// east
		c.y++;
	else if (i == 2)	// south
		c.x++;
	else if (i == 3)	// west
		c.y--;

	return c;
}

// Moves a list of creatures (does not update grid)
void move_creatures(settings s, creature* grid, list* l) {
	# pragma omp parallel for
	for (int i = 0; i < l->used; i++){
		l->array[i] = move_creature(s, grid, l->array[i]);
	}
}

// Make two creatures brawl and see who wins
int brawl(creature atack, creature defense) {
	if (atack.type == BUNNY) {
		if (defense.type == EMPTY)
			return 1;

		if (atack.proc_age > defense.proc_age)
			return 1;
	}

	else if(atack.type == FOX) {
		if (defense.type != FOX)
			return 1;

		if (atack.proc_age > defense.proc_age)
			return 1;

		else if (atack.proc_age == defense.proc_age && atack.hung_age < defense.hung_age)
			return 1;
	}

	return 0;
}

// Moves creatures in grid and solves conflicts
void solve_conflict(settings s, creature* grid, list* l_other, list* l) {

	int used = l->used;

	# pragma omp parallel for
	for (int i = 0; i < used; i++) {
		int ax, ay, oldx, oldy;
		creature atack, defense, old_position;

		oldx = l->array[i].oldx;
		oldy = l->array[i].oldy;
		ax = l->array[i].x;
		ay = l->array[i].y;

		# pragma omp critical (grid)
		{
			old_position = l->array[i];
			atack = l->array[i];
			defense = grid[ax * s.C + ay];
		}

		old_position.x = oldx;
		old_position.y = oldy;

		if (atack.type == FOX)
			atack.hung_age++;

		// starvation check, if gonna atack bunny, don't starve
		if (atack.hung_age >= s.GEN_FOOD_FOX && defense.type != BUNNY) {

			// set to empty so it loses brawl
			atack.type = EMPTY;
			atack.proc_age = 0;
			atack.hung_age = 0;
		}

		// procreate
		old_position.proc_age = 0;
		old_position.hung_age = 0;
		int proc_age = atack.type == BUNNY ? s.GEN_PROC_BUNNY : s.GEN_PROC_FOX;
		if (++atack.proc_age > proc_age){
			atack.proc_age = 0;
			old_position.type = atack.type;

			# pragma omp critical (list)
			append_creature(l, old_position);
		} else {
			old_position.type = EMPTY;
		} 

		# pragma omp critical (grid)
		copy_creature(&grid[oldx * s.C + oldy], old_position);

		if (brawl(atack, defense) == 1){

			if (atack.type == FOX && defense.type == BUNNY) {
				atack.hung_age = 0;

				// set type as invalid so we can remove it later
				# pragma omp critical (other_list)
				l_other->array[defense.id].type = -1;
			}

			// remove creature that lost
			if (defense.type == atack.type) {

				// only case this will happen is if a fox already ate a bunny
				if (defense.hung_age == 0)
					atack.hung_age = 0;

				// set type as invalid so we can remove it later
				# pragma omp critical (list)
				l->array[i].type = -1;
			}

			# pragma omp critical (list)
			l->array[i] = atack;

			# pragma omp critical (grid)
			copy_creature(&grid[ax * s.C + ay], l->array[i]);

		}

		// lost brawl/starved
		else {

			# pragma omp critical (list)
			l->array[i] = atack;

			// set type as invalid so we can remove it later
			# pragma omp critical (list)
			l->array[i].type = -1;
		}
	}

	fix_list(l);
	fix_list(l_other);
	
	// update grid
	# pragma omp parallel for
	for (int i = 0; i < l->used; i++) {
		l->array[i].oldx = l->array[i].x;
		l->array[i].oldy = l->array[i].y;
		grid[l->array[i].x * s.C + l->array[i].y] = l->array[i];
	}
}

// Print current state of grid
void print_grid(settings s, creature* grid){
	for (int i = 0; i < s.L; i++) {
		for (int j = 0; j < s.C; j++) {
			switch(grid[i * s.C + j].type){
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
