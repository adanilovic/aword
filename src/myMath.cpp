#include <stdint.h>
#include "myMath.hpp"

uint32_t factorial( uint32_t n )
{
    if( n == 0 )
    {
        return 1;
    }
    else if( n == 1 )
    {
        return n;
    }
    else
    {
        return n * factorial( n - 1 );
    }
}
