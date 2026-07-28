#include "Engine/Core/TileHeatMap.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "VertexUtils.hpp"
#include <Engine/Math/MathUtils.hpp>
#include "ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
TileHeatMap::TileHeatMap( IntVec2 const& dimensions )
	: m_dimensions( dimensions )
{
	m_values = new float [ m_dimensions.x * m_dimensions.y ] {};
}


//-----------------------------------------------------------------------------------------------
TileHeatMap::~TileHeatMap()
{
	delete [] m_values;
	m_values = nullptr;
}


//-----------------------------------------------------------------------------------------------
void TileHeatMap::SetAllValues( float value )
{
	int size = m_dimensions.x * m_dimensions.y;
	for ( int valueIndex = 0; valueIndex < size; ++ valueIndex )
	{
		m_values [ valueIndex ] = value;
	}
}


//-----------------------------------------------------------------------------------------------
void TileHeatMap::AddVertsForDebugDraw( std::vector<Vertex>& verts, AABB2 totalBounds, FloatRange valueRange /*= FloatRange( 0.f, 1.f )*/, Rgba8 lowColor /*= Rgba8( 0, 0, 0, 100 )*/, Rgba8 highColor /*= Rgba8( 255, 255, 255, 100 )*/, float specialValue /*= 999999.f*/, Rgba8 specialColor /*= Rgba8( 255, 0, 255 ) */ ) const
{
	float highestNonSpecialValue = GetHighestNonSpecialValue( specialValue );
	for ( int rowIndex = 0; rowIndex < totalBounds.m_maxs.y - totalBounds.m_mins.y; ++ rowIndex )
	{
		for ( int colIndex = 0; colIndex < totalBounds.m_maxs.x - totalBounds.m_mins.x; ++ colIndex )
		{
			float row = static_cast< float >( rowIndex );
			float col = static_cast< float >( colIndex );

			Vec2 tileMins = Vec2( col + totalBounds.m_mins.x, row + totalBounds.m_mins.y );
			Vec2 tileMaxs = Vec2( col + totalBounds.m_mins.x + 1.f, row + totalBounds.m_mins.y + 1.f );

			int tileIndex = rowIndex * m_dimensions.x + colIndex;
			Rgba8 color;
			if ( m_values [ tileIndex ] == specialValue )
			{
				color = specialColor;
			}
			else
			{
				float fraction = RangeMapClamped( m_values [ tileIndex ], 0.f, highestNonSpecialValue, valueRange.m_min, valueRange.m_max );
				color = Interpolate( lowColor, highColor, fraction );
			}

			AddVertsForAABB2D( verts, AABB2( tileMins, tileMaxs ), color );
		}
	}
}


//-----------------------------------------------------------------------------------------------
float TileHeatMap::GetHighestNonSpecialValue( float specialValue /*= 999999.f */ ) const
{
	float maxHeatValue = 0.f;
	for ( int heatValueIndex = 0; heatValueIndex < m_dimensions.x * m_dimensions.y; ++ heatValueIndex )
	{
		if ( m_values[ heatValueIndex ] > maxHeatValue && m_values [ heatValueIndex ] < specialValue )
		{

			maxHeatValue = m_values [ heatValueIndex ];
		}
	}

	return maxHeatValue;
}


//-----------------------------------------------------------------------------------------------
float TileHeatMap::GetHeatValue( int index ) const
{
	return m_values [ index ];
}


//-----------------------------------------------------------------------------------------------
void TileHeatMap::SetHeatValue( int index, float value )
{
	m_values [ index ] = value;
}


//-----------------------------------------------------------------------------------------------
void TileHeatMap::AddHeatValue( int index, float value )
{
	m_values [ index ] += value;
}
