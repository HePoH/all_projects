#include "../include/cmplx_num.h"

char PLUGIN_NAME[] = "sub.so";
char MAIN_FUNCTION[] = "cn_sub";
char MENU_TITLE[] = "[Sub]traction";

cmplx_num* cn_sub(cmplx_num* first, cmplx_num* second){
        cmplx_num* ans = 0;
        ans = (cmplx_num*) malloc(sizeof(cmplx_num));

        if (ans == NULL)
                return 0;

        ans->a = first->a - second->a;
        ans->b = first->b - second->b;

        return ans;
}
