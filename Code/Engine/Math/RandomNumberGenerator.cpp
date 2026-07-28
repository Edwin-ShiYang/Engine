#include "Engine/Math/RandomNumberGenerator.hpp"
#include <cstdlib> 




//-----------------------------------------------------------------------------------------------
int RandomNumberGenerator::RollRandomIntLessThan( int maxNotInclusive )
{
	return rand() % maxNotInclusive;
}


//-----------------------------------------------------------------------------------------------
int RandomNumberGenerator::RollRandomIntInRange( int minInclusive, int maxInclusive )
{
	return minInclusive + rand() % ( maxInclusive - minInclusive + 1 );
}


//-----------------------------------------------------------------------------------------------
float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
	return static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
}


//-----------------------------------------------------------------------------------------------
float RandomNumberGenerator::RollRandomFloatInRange( float minInclusive, float maxInclusive )
{
	return minInclusive + RollRandomFloatZeroToOne() * ( maxInclusive - minInclusive );
}


//-----------------------------------------------------------------------------------------------
float RandomNumberGenerator::MakeRandomFloatInRange( float minInclusive, float maxInclusive )
{
	float zeorToOne = static_cast< float >( rand() ) / static_cast< float >( RAND_MAX );
	return minInclusive + zeorToOne * ( maxInclusive - minInclusive );
}
