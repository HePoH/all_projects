#include "../include/cmplx_num.h"

char* PLUGIN_NAME = "add";

cmplx_num* cn_add(cmplx_num* first, cmplx_num* second){
        cmplx_num* ans = 0;
        ans = (cmplx_num*) malloc(sizeof(cmplx_num));

        if (ans == NULL)
                return 0;

        ans->a = first->a + second->a;
        ans->b = first->b + second->b;

        return ans;
}
