#include "Engine/Math/IntVec4.hpp"

//-----------------------------------------------------------------------------------------------
IntVec4 const IntVec4::ZERO( 0, 0, 0, 0 );

//-----------------------------------------------------------------------------------------------
IntVec4::IntVec4( int initialX, int initialY, int initialZ, int initialW )
    : x( initialX )
    , y( initialY )
    , z( initialZ )
    , w( initialW )
{
}

//-----------------------------------------------------------------------------------------------
IntVec4::IntVec4( IntVec4 const& copyFrom )
    : x( copyFrom.x )
    , y( copyFrom.y )
    , z( copyFrom.z )
    , w( copyFrom.w )
{
}