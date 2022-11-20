#include <stdio.h>
#include "../creature.h"

list l;

int test_create_list() {
	l = create_list(5);
	if (l.used != 0) 
		return 1;

	if (l.size != 5) 
		return 1;

	return 0;
}

int test_append_list() {
	creature c = {0, 0, 0, 0, 0};

	append_list(&l, c);
	c.x = 1;
	append_list(&l, c);
	c.x = 2;
	print_list(&l);
	/*printf("%d\n", l.array[1].x);*/
	/*append_list(l, c);*/
	/*c.x = 3;*/
	/*append_list(l, c);*/
	/*c.x = 4;*/
	/*append_list(l, c);*/
	/*c.x = 5;*/
	/*append_list(l, c);*/

	/*if (l.used != 6) */
		/*return 1;*/

	/*if (l.size != 10) */
		/*return 1;*/

	return 0;
}

int test_remove_id_list() {


	remove_id(&l, 1);

	if (l.array[1].x == 1) 
		return 1;

	if (l.used != 5) 
		return 1;

	creature c = {1, 0, 0, 0, 0};
	append_list(&l, c);

	if (l.array[5].x != 1) 
		return 1;

	return 0;
}

int main() {
	if (test_create_list())
		fprintf(stderr, "Failed create_list");

	if (test_append_list())
		fprintf(stderr, "Failed append_list");

	if (test_remove_id_list())
		fprintf(stderr, "Failed remove_id");
}
