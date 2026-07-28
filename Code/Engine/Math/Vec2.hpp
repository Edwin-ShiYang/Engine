#pragma once


//-----------------------------------------------------------------------------------------------
struct Vec2
{
public: 
	float x = 0.f;
	float y = 0.f;

public:
	/* Construction / Destruction */
	~Vec2() {}												
	Vec2() {}											
	Vec2( Vec2 const& copyFrom );							  
	explicit Vec2( float initialX, float initialY );		  

	/* Static const */
	static const Vec2 ZERO;
	static const Vec2 ONE;
	static const Vec2 UP;
	static const Vec2 DOWN;
	static const Vec2 LEFT;
	static const Vec2 RIGHT;

	/* Static methods */
	static Vec2 const MakeFromPolarDegrees( float orientationDegrees, float length = 1.f );
	static Vec2 const MakeFromPolarRadians( float orentationRadians, float length = 1.f);


	/* Accessor ( const ) */ 
	float            GetLength()                                               const;
	float            GetLengthSquared()                                        const;
	float            GetOrientationDegrees()                                   const;
	float	         GetOrientationRadians()                                   const;
	Vec2    const    GetRotatedBy90Degrees()                                   const;
	Vec2    const    GetRotatedByMinus90Degrees()                              const;
	Vec2    const    GetClamped( float maxLength )                             const;
	Vec2    const    GetNormalized()                                           const;
	Vec2    const    GetReflected( Vec2 const& normalOfSurfaceToReflectOffOf ) const;


	/* Accessor */
	Vec2    const    GetRotatedByDegrees( float rotationDegrees );
	Vec2    const    GetRotatedByRadians( float rotationRadians );


	/* Mutators ( non-const ) */ 
	void             SetOrientationDegrees( float newOrientationDegrees );
	void             SetOrientationRadians( float newOrientationRadians );
	void             SetPolarDegrees( float newOrientationDegrees, float newLength );
	void             SetPolarRadians( float newOrientationRadians, float newLength );
	void             Rotate90Degrees();
	void             RotateMinus90Degrees();
	void             RotateDegrees( float rotationDegrees );
	void             RotateRadians( float rotationRadians );
	void             SetLength( float newLength );
	void             ClampLength( float maxLength );
	void             Normalize();
	float            NormalizeAndGetPreviousLength();
	void             Reflect( Vec2 const& normalOfSurfaceToReflectOffOf );
				     
	void             SetFromText( char const* text );
				     
				     
	/* Operators (    const ) */
	bool		     operator==( Vec2 const& compare )                        const;
	bool		     operator!=( Vec2 const& compare )                        const;
	Vec2    const	 operator+( Vec2 const& vecToAdd )                        const;
	Vec2    const	 operator-( Vec2 const& vecToSubtract )                   const;
	Vec2    const	 operator-()                                              const;
	Vec2    const	 operator*( float uniformScale )                          const;
	Vec2    const	 operator*( Vec2 const& vecToMultiply )                   const;
	Vec2    const	 operator/( float inverseScale )                          const;


	/* Operators ( self-mutating / non-const ) */
	void		     operator+=( Vec2 const& vecToAdd );	
	void		     operator-=( Vec2 const& vecToSubtract );
	void		     operator*=( const float uniformScale );
	void		     operator/=( const float uniformDivisor );
	void		     operator=( Vec2 const& copyFrom );
	 

	/* Friends */
	friend Vec2 const operator*( float uniformScale, Vec2 const& vecToScale );	// float * vec2
};


