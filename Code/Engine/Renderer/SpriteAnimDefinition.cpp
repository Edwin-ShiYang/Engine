#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/XmlUtils.hpp"


//-----------------------------------------------------------------------------------------------
SpriteAnimDefinition::SpriteAnimDefinition( SpriteSheet const& sheet, int startSpriteIndex, int endSpriteIndex, float framesPerSecond, SpriteAnimPlaybackType playbackType /*= SpriteAnimPlaybackType::LOOP */ )
	: m_spriteSheet( sheet )
	, m_startSpriteIndex( startSpriteIndex )
	, m_endSpriteIndex( endSpriteIndex )
	, m_framesPerSecond( framesPerSecond )
	, m_playbackType( playbackType )
{
}


//-----------------------------------------------------------------------------------------------
SpriteAnimDefinition::SpriteAnimDefinition( SpriteSheet const& sheet, float framesPerSecond, SpriteAnimPlaybackType playbackType /*= SpriteAnimPlaybackType::LOOP */ )
	: m_spriteSheet( sheet )
	, m_framesPerSecond( framesPerSecond )
	, m_playbackType( playbackType )
{

}

//-----------------------------------------------------------------------------------------------
float SpriteAnimDefinition::GetDuration() const
{
	int numOfSprite = m_endSpriteIndex - m_startSpriteIndex + 1;
	if ( m_framesPerSecond <= 0.0f )
	{
		return 0.0f;
	}
	return static_cast< float >( numOfSprite ) / m_framesPerSecond;
}

//-----------------------------------------------------------------------------------------------
bool SpriteAnimDefinition::LoadFromXmlElement( const XmlElement& element )
{
	m_startSpriteIndex = ParseXmlAttribute( element, "startFrame", m_startSpriteIndex );
	m_endSpriteIndex   = ParseXmlAttribute( element, "endFrame",   m_endSpriteIndex   );

	return true;
}


//-----------------------------------------------------------------------------------------------
SpriteDefinition const& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{
	int spriteIndex = -1;
	int numOfSprite = m_endSpriteIndex - m_startSpriteIndex + 1;

	if ( m_playbackType == SpriteAnimPlaybackType::LOOP )
	{
		spriteIndex = ( static_cast< int > ( seconds * m_framesPerSecond ) % numOfSprite ) + m_startSpriteIndex;
	}

	if ( m_playbackType == SpriteAnimPlaybackType::ONCE )
	{
		spriteIndex = static_cast< int > ( seconds * m_framesPerSecond ) + m_startSpriteIndex;
		if ( spriteIndex > m_endSpriteIndex )
		{
			spriteIndex = m_endSpriteIndex;
		}
	}

	if ( m_playbackType == SpriteAnimPlaybackType::PINGPONG )
	{
		int range = numOfSprite + ( numOfSprite - 2 );
		int currentIndex = static_cast< int >( seconds * m_framesPerSecond ) % range;

		if ( currentIndex < numOfSprite )
		{
			spriteIndex = currentIndex + m_startSpriteIndex;
		}
		else
		{
			spriteIndex = ( range - currentIndex ) + m_startSpriteIndex;
		}
	}


	return m_spriteSheet.GetSpriteDef( spriteIndex );
}
