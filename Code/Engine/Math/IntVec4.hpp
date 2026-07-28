#pragma once

//-----------------------------------------------------------------------------------------------
struct IntVec4
{
public:
    int x = 0;
    int y = 0;
    int z = 0;
    int w = 0;

    static IntVec4 const ZERO;

public:
    IntVec4()  = default;
    ~IntVec4() = default;

    explicit IntVec4( int initialX, int initialY, int initialZ, int initialW );
    IntVec4( IntVec4 const& copyFrom );
};