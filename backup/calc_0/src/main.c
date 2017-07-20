#include "../include/cmplx_num.h"

int file_select(const struct dirent *entry) {
	return strcmp(entry->d_name, ".so");
}

struct dirent** find_libs(char* path, int* fc) {
	struct dirent** fnl;
	*fc = scandir(path, &fnl, file_select, alphasort);

	return (*fc > 0) ? fnl : NULL;
}

void* load_lib(char* path){
	void *ext_lib;
	cmplx_num* (*func)(cmplx_num*, cmplx_num*);

	ext_lib = dlopen(path, RTLD_NOW);
	if (!ext_lib){
		fprintf(stderr, "dlopen() error: %s\n", dlerror());
		return 1;
	};

	func = dlsym(ext_library, argv[1]);
	dlclose(ext_library);
}

int main(){
	char c;
	int i = 0, lc = 0;
	struct dirent** libs = NULL;
	cmplx_num *first = 0, *second = 0, *ans = 0;
	cmplx_num* (**func)(cmplx_num*, cmplx_num*);

	first = (cmplx_num*) malloc(sizeof(cmplx_num));
	second = (cmplx_num*) malloc(sizeof(cmplx_num));

	if (first == NULL || second == NULL)
		return 0;

	printf("\033[2J");
	printf("\033[0;0f");

	libs = find_libs("plugins", &lc);
	func = malloc(lc * sizeof(void(*)(cmplx_num*, cmplx_num*));

	for (i = 0; i < lc; i++)
		func[i] = malloc(sizeof...);

	func[i] = (cmplx_num*) load_lib("/home/2017d_medvedev/work/calc/plugins/add.so");

	for (i = 0; i < lc; i++)
		printf("Lib: %s\n", libs[i]->d_name);

	getchar();
	/*while(c != 'q'){
		printf("\033[2J");
		printf("\033[0;0f");

		printf("\n[ Arithmetic operations on complex numbers ]\n");
		printf(" 1. [Add]ition\n 2. [Sub]traction\n 3. [Mul]tiplication\n 4. [Div]ision\n 5. [Ex]it\n");
		printf("\n > Select an arithmetic operation: ");
		scanf("%c", &c);

		switch (c){
			printf("\033[2J");
			printf("\033[0;0f");

			case '1': printf("\nInput the first complex number: ");
				scanf("%lf %lf", &first->a, &first->b);

				printf("Input the second complex number: ");
				scanf("%lf %lf", &second->a, &second->b);

				ans = cn_add(first, second);
				(!ans) ? printf("Error") : printf("\nResult add: %.1f + %.1fi", ans->a, ans->b);
				free(ans);

				break;

			case '2': printf("\nInput the first complex number: ");
				scanf("%lf %lf", &first->a, &first->b);

				printf("Input the second complex number: ");
				scanf("%lf %lf", &second->a, &second->b);

				ans = cn_sub(first, second);
				(!ans) ? printf("Error") : printf("\nRsult sub: %.1f + %.1fi", ans->a, ans->b);
				free(ans);

				break;

			case '3': printf("\nInput the first complex number: ");
				scanf("%lf %lf", &first->a, &first->b);

				printf("Input the second complex number: ");
				scanf("%lf %lf", &second->a, &second->b);

				ans = cn_mul(first, second);
				(!ans) ? printf("Error") : printf("\nRsult mul: num = %.1f + %.1fi", ans->a, ans->b);
				free(ans);

				break;

			case '4': printf("\nInput the first complex number: ");
				scanf("%lf %lf", &first->a, &first->b);

				printf("Input the second complex number: ");
				scanf("%lf %lf", &second->a, &second->b);

				ans = cn_div(first, second);
				(!ans) ? printf("Error") : printf("\nResult div: num = %.1f + %.1fi", ans->a, ans->b);
				free(ans);

				break;
			case '5': case 'q': return;
			default: break;
		}

		getchar();
		getchar();
	}*/

	return 0;
}
