#include "../include/cmplx_num.h"

char* PLUGIN_NAME = "mul";

cmplx_num* cn_mul(cmplx_num* first, cmplx_num* second){
        cmplx_num* ans = 0;
        ans = (cmplx_num*) malloc(sizeof(cmplx_num));

        if (ans == NULL)
                return 0;

        ans->a = first->a * second->a - first->b * second->b;
        ans->b = first->a * second->b + first->b * second->a;

        return ans;
}
