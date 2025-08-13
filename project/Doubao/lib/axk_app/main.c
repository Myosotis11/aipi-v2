#include "bl616_glb.h"

int main(void)
{
    extern int axk_init(void);
    axk_init();

    extern int app_main(void);
    app_main();

    return 0;
}
