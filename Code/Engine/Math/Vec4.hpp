#pragma once


//-----------------------------------------------------------------------------------------------
struct Vec4 
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

	static const Vec4 ZERO;

public:
	Vec4() = default; // The default constructor will use the member variables default values
	~Vec4() {}
	Vec4( Vec4 const& copyFrom );
	explicit Vec4( float initialX, float initialY, float initialZ, float initialW );

	void operator= ( Vec4 const& copyFrom );
	Vec4 const operator-() const;
	
	float GetLength() const;



	void  Normalize();
	Vec4  GetNormalized() const;
};