
#include "m32_math.h"

float m32_coshf (const float x) __z88dk_fastcall
{
    float y;

    y = m32_expf(x);
    return 0.5*(y + m32_inv(y));
}
