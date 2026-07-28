#pragma once

//-----------------------------------------------------------------------------------------------
struct Rgba8
{
public:
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;

	static const Rgba8 WHITE;
	static const Rgba8 RED;
	static const Rgba8 CYAN;
	static const Rgba8 GREEN;
	static const Rgba8 BLUE;
	static const Rgba8 YELLOW;
	static const Rgba8 GREY;
	static const Rgba8 TRANSPARENT_BLACK;
	static const Rgba8 BLACK;
	static const Rgba8 MAGENTA;
	static const Rgba8 ORANGE;
	static const Rgba8 PEACH;

	// Catppuccin
	static const Rgba8 CATPPUCCIN_RED;
	static const Rgba8 CATPPUCCIN_GREEN;
	static const Rgba8 CATPPUCCIN_BLUE;
	static const Rgba8 CATPPUCCIN_YELLOW;
	static const Rgba8 CATPPUCCIN_PINK;
	static const Rgba8 CATPPUCCIN_SKY;
	static const Rgba8 CATPPUCCIN_PEACH;
	static const Rgba8 CATPPUCCIN_MAUVE;
	static const Rgba8 CATPPUCCIN_TEAL;

public:
	Rgba8() = default;
	explicit Rgba8( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte = 255 );
	~Rgba8() = default;

	void SetFromText( char const* text );
	void GetAsFloats( float* colorAsFloats ) const;
	void ScaleRGBA( float colorScale = 1.0f, float alphaScale = 1.0f );
	bool IsRGBEqual( Rgba8 const& compare ) const;

	bool operator==( Rgba8 const& compare ) const;
};

//-----------------------------------------------------------------------------------------------
Rgba8 Interpolate( Rgba8 start, Rgba8 end, float fractionOfEnd );