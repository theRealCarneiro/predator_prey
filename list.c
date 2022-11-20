#include <stdlib.h>
#include <string.h>
#include <stdio.h>


typedef struct list {
	int size;
	int used;
	creature* array;
} list;

list create_list(int size){
	list l = {size, 0, malloc(sizeof(int) * size)};
	return l;
}

void append_list(list* l, int x) {
	if (l->used + 1 >= l->size) {
		// realoc
	}

	l->array[++l->used] = x;
}

void remove_list(list* l, int id) {
	int size_for_copy = l->size - id;
	memmove(&l->array[id], &l->array[id + 1], (l->used - id - 1) * sizeof(int));
	l->used--;
}

void print_list(list l) {
	for (int i = 0; i < l.used; i++)
		printf("%d ", l.array[i]);
	printf("\n");
}

int main() {
	list l = create_list(5);
	append_list(&l, 1);
	append_list(&l, 2);
	append_list(&l, 3);
	append_list(&l, 4);
	print_list(l);
	remove_list(&l, 1);
	print_list(l);
}
