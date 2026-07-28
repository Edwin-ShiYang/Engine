#pragma once


//-----------------------------------------------------------------------------------------------
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>


struct Vertex;
struct AABB2;

//-----------------------------------------------------------------------------------------------
class TileHeatMap
{
public:
	TileHeatMap( IntVec2 const& dimensions );
	~TileHeatMap();
	
	void  SetAllValues( float value = 1.0f );
	void  AddVertsForDebugDraw( std::vector<Vertex>& verts, AABB2 totalBounds, FloatRange valueRange = FloatRange( 0.f, 1.f ), Rgba8 lowColor = Rgba8( 0, 0, 0, 100 ), Rgba8 highColor = Rgba8( 255, 255, 255, 100 ), float specialValue = 999999.f, Rgba8 specialColor = Rgba8( 255, 0, 255 ) ) const;
	float GetHighestNonSpecialValue( float specialValue = 999999.f ) const;

	float GetHeatValue( int index ) const;
	void  SetHeatValue( int index, float value );
	
	float*    m_values = nullptr;
	IntVec2   m_dimensions;


private:
	void  AddHeatValue( int index, float value );
};