#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Texture.hpp"

//-----------------------------------------------------------------------------------------------
SpriteSheet::SpriteSheet( Texture& texture, IntVec2 const& simpleGridLayout )
    : m_texture( texture )
    , m_simpleGridLayout( simpleGridLayout )
{
    GenerateSpriteDefs();
}

//-----------------------------------------------------------------------------------------------
SpriteSheet::~SpriteSheet()
{
}

//-----------------------------------------------------------------------------------------------
Texture& SpriteSheet::GetTexture() const
{
    return m_texture;
}

//-----------------------------------------------------------------------------------------------
int SpriteSheet::GetNumSprites() const
{
    return m_simpleGridLayout.x * m_simpleGridLayout.y;
}

//-----------------------------------------------------------------------------------------------
SpriteDefinition const& SpriteSheet::GetSpriteDef( int spriteIndex ) const
{
    return m_spriteDefs[ spriteIndex ];
}

//-----------------------------------------------------------------------------------------------
void SpriteSheet::GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex ) const
{
    GetSpriteDef( spriteIndex ).GetUVs( out_uvAtMins, out_uvAtMaxs );
}

//-----------------------------------------------------------------------------------------------
AABB2 SpriteSheet::GetSpriteUVs( int spriteIndex ) const
{
    return GetSpriteDef( spriteIndex ).GetUVs();
}

//-----------------------------------------------------------------------------------------------
AABB2 SpriteSheet::GetSpriteUVs( IntVec2 spriteCoords ) const
{
    int spriteIndex = spriteCoords.y + ( spriteCoords.x * m_simpleGridLayout.x );
    return GetSpriteDef( spriteIndex ).GetUVs();
}

//-----------------------------------------------------------------------------------------------
void SpriteSheet::GenerateSpriteDefs()
{
    IntVec2 textureDimensions = GetTexture().GetDimensions();

    float   nudge = 1.0f / 128.f;

    float   widthPerTexel  = 1.f / static_cast< float >( textureDimensions.x );
    float   heightPerTexel = 1.f / static_cast< float >( textureDimensions.y );

    float   uNudge = widthPerTexel * nudge;
    float   vNudge = heightPerTexel * nudge;

    m_spriteDefs.reserve( GetNumSprites() );

    for ( int spriteY = 0; spriteY < m_simpleGridLayout.y; ++spriteY )
    {
        for ( int spriteX = 0; spriteX < m_simpleGridLayout.x; ++spriteX )
        {
            int  spriteIndex = spriteX + ( spriteY * m_simpleGridLayout.x );

            Vec2 uvAtMins;
            uvAtMins.x = static_cast< float >( spriteX ) / static_cast< float >( m_simpleGridLayout.x ) + uNudge;
            uvAtMins.y = static_cast< float >( m_simpleGridLayout.y - 1 - spriteY ) / static_cast< float >( m_simpleGridLayout.y ) + vNudge;

            Vec2 uvAtMaxs;
            uvAtMaxs.x = static_cast< float >( spriteX + 1 ) / static_cast< float >( m_simpleGridLayout.x ) - uNudge;
            uvAtMaxs.y = static_cast< float >( m_simpleGridLayout.y - spriteY ) / static_cast< float >( m_simpleGridLayout.y ) - vNudge;

            SpriteDefinition definition = SpriteDefinition( *this, spriteIndex, uvAtMins, uvAtMaxs );
            m_spriteDefs.push_back( definition );
        }
    }
}