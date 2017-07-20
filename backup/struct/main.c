# include <stdio.h>

struct test{
	char c;
	int b;
} __attribute__((packed));

int main() {
	struct test* t;
	char  a[10] = {'a', 10, 0, 0, 0, 'b', 0, 0, 0, 0};

	t = (struct test *)a;
	printf("\nChar: %c\nInt: %d\n\n", t->c, t->b);
	t++;
	printf("Char: %c\nInt: %d\n", t->c, t->b);
	return 0;
}
