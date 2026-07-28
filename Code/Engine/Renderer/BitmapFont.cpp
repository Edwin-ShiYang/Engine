#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/AABB2.hpp"


//------------------------------------------------------------------------------------------------
BitmapFont::BitmapFont( char const* fontFilePathNameWithNoExtension, Texture& fontTexture )
	: m_fontFilePathNameWithNoExtension( fontFilePathNameWithNoExtension )
	, m_fontDefaultAspect( 1.0f )
	, m_fontGlyphsSpriteSheet( fontTexture, IntVec2( 16, 16 ) )
{
}


//------------------------------------------------------------------------------------------------
Texture& BitmapFont::GetTexture()
{
	return m_fontGlyphsSpriteSheet.GetTexture();
}


//------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForText2D( std::vector<Vertex>& vertexArray, Vec2 textMins, float cellHeight, std::string const& text, int& maxGlyphsToDraw, Rgba8 tint, float cellAspectScale )
{
	float cellWidth = cellHeight * cellAspectScale;
	for ( int charIndex = 0; charIndex < static_cast< int >( text.size() ); ++ charIndex )
	{
		if ( maxGlyphsToDraw <= 0 )
		{
			break;
		}

		Vec2 currentCharMins = Vec2( textMins.x + cellWidth * charIndex, textMins.y );
		Vec2 currentCharMaxs = Vec2( currentCharMins.x + cellWidth, currentCharMins.y + cellHeight );

		AABB2 textUVs = GetTextUVs( text [ charIndex ] );
		AddVertsForAABB2D( vertexArray, AABB2( currentCharMins, currentCharMaxs ), tint, textUVs );

		maxGlyphsToDraw--;
	}
}


//------------------------------------------------------------------------------------------------
AABB2 BitmapFont::GetTextUVs( int spriteIndex )
{
	return m_fontGlyphsSpriteSheet.GetSpriteUVs( spriteIndex );
}

//------------------------------------------------------------------------------------------------
float BitmapFont::GetTextWidth( float cellHeight, std::string const& text, float cellAspectScale ) const
{
	return cellHeight * cellAspectScale * static_cast< float > ( text.size() );
}

//------------------------------------------------------------------------------------------------
float BitmapFont::GetMaxLineWidth( std::vector<std::string> const& lines, float cellHeight, float cellAspectScale )
{
	float maxWidth = 0.f;
	for ( int lineIndex = 0; lineIndex < static_cast< int >( lines.size() ); ++ lineIndex )
	{
		float width = GetTextWidth( cellHeight, lines [ lineIndex ], cellAspectScale );
		if ( width > maxWidth )
		{
			maxWidth = width;
		}
	}
	return maxWidth;
}

//------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForTextInBox2D( std::vector<Vertex>& verts, std::string const& text, AABB2 const& box, float cellHeight, Rgba8 tint /*= Rgba8::WHITE*/, float cellAspectScale /*= 1.f*/, Vec2 alignment /*= Vec2( .5f, .5f )*/, TextDrawMode mode /*= TextDrawMode::SHRINK_TO_FIT*/, int maxGlyphsToDraw /*= 99999999 */ )
{
	Strings lines = SplitStringOnDelimiter( text, '\n' );

	float boxWidth  = box.m_maxs.x - box.m_mins.x;
	float boxHeight = box.m_maxs.y - box.m_mins.y;
	int   lineCount  = static_cast< int >( lines.size() );

	float textBlockHeight = static_cast< float >( lineCount ) * cellHeight;
	float textBlockWidth = GetMaxLineWidth( lines, cellHeight, cellAspectScale );

	if ( mode == TextDrawMode::SHRINK_TO_FIT ) 
	{
		float widthScale = boxWidth / textBlockWidth;
		float heightScale = boxHeight / textBlockHeight;

		if ( textBlockWidth > boxWidth && textBlockHeight <= boxHeight )
		{
			cellHeight *= widthScale;
		}

		if ( textBlockHeight > boxHeight && textBlockWidth <= boxWidth )
		{
			cellHeight *= heightScale;
		}


		if ( textBlockWidth > boxWidth  && textBlockHeight > boxHeight )
		{
			cellHeight *= widthScale > heightScale ? heightScale : widthScale;
		}

		textBlockHeight = static_cast< float >( lineCount ) * cellHeight;
	}


	float verticalSpace = boxHeight - textBlockHeight;
	float verticalSpaceToTop = verticalSpace * alignment.y;

	for ( int lineIndex = 0; lineIndex < static_cast < int >( lines.size() ); ++ lineIndex )
	{
		float horizontalSpace = boxWidth - GetTextWidth( cellHeight, lines [ lineIndex ], cellAspectScale );
		float horizontalSpaceToLeft = horizontalSpace * alignment.x;

		float textMinsX = box.m_mins.x + horizontalSpaceToLeft;
		float textMinsY = box.m_mins.y + verticalSpaceToTop + ( cellHeight * ( lines.size() - 1 - lineIndex ) );
		Vec2 textMins = Vec2( textMinsX, textMinsY );
		AddVertsForText2D( verts, textMins, cellHeight, lines [ lineIndex ], maxGlyphsToDraw, tint, cellAspectScale );
	}
}

//------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForText3DAtOriginXForward( std::vector<Vertex>& verts, float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.0f*/, Vec2 const& alignment /*= Vec2( 0.5f, 0.5f )*/, int maxGlyphsToDraw /*= 999 */ )
{
	AddVertsForText2D( verts, Vec2::ZERO, cellHeight, text, maxGlyphsToDraw, tint, cellAspect );

	AABB2 bounds = GetVertexBounds2D( verts );
	float width  = bounds.m_maxs.x - bounds.m_mins.x;
	float height = bounds.m_maxs.y - bounds.m_mins.y;

	Mat44 transformMatrix;
	
	Mat44 orientationMatrix;
	Vec3  iBasis = Vec3( 0.0f, 1.0f, 0.0f );
	Vec3  jBasis = Vec3( 0.0f, 0.0f, 1.0f );
	Vec3  kBasis = Vec3( 1.0f, 0.0f, 0.0f );
	orientationMatrix.SetIJK3D( iBasis, jBasis, kBasis );

	transformMatrix.Append( orientationMatrix );
	transformMatrix.AppendTranslation3D( Vec3( -alignment.x * width, -alignment.y * height, 0.f ) );

	TransformVertexArray3D( verts, transformMatrix );
}


//------------------------------------------------------------------------------------------------
float BitmapFont::GetGlyphAspect( [[maybe_unused]] int glyphUnicode ) const
{
	return m_fontDefaultAspect;
}
