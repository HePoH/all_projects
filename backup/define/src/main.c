#include <stdio.h>

int main() {
	#ifdef HELLO
		printf("Hello world\n");
	#elif GOODBUE
		printf("Goodbue world\n");
	#endif

	return 0;
}
