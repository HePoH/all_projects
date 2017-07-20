#include "../include/cmplx_num.h"

char* PLUGIN_NAME = "div";

cmplx_num* cn_div(cmplx_num* first, cmplx_num* second){
        cmplx_num* ans = 0;
        ans = (cmplx_num*) malloc(sizeof(cmplx_num));

        if (ans == NULL)
                return 0;

        if (second->a == 0 && second->b == 0)
                return 0;

        ans->a = (first->a * second->a + first->b * second->b) / (second->a * second->a + second->b * second->b);
        ans->b = (second->a * first->b - first->a * second->b) / (second->a * second->a + second->b * second->b);

        return ans;
}
