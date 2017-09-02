#ifndef __RANDOMNUMBERGENERATOR_HPP__
#define __RANDOMNUMBERGENERATOR_HPP__

inline float RandomNumberGenerator::uniform01Impl( void )
{
	// TODO - Use a better RNG
    const float one_over_RM = 1.0f / (float) RAND_MAX;
	return (float) rand() * one_over_RM;
}

inline float RandomNumberGenerator::uniform01( void )
{
    if( cache_next >= CACHE_SIZE ) {
        buildCache();
    }
    
    return cache[ cache_next++ ];
}

inline float RandomNumberGenerator::uniformRange( float min, float max )
{
	return uniform01() * (max - min) + min;
}



#endif
