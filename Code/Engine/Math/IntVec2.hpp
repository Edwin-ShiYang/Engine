#pragma once


//-----------------------------------------------------------------------------------------------
struct IntVec2
{
public:
	int x = 0;
	int y = 0;

	static const IntVec2 ZERO;
	static const IntVec2 INVALID;

public:
	/* Construction / Destruction */
	~IntVec2() {};
	IntVec2() {};
	IntVec2( const IntVec2& copyFrom );
	explicit IntVec2 ( int initialX, int initialY );


	/* Accessors( const ) */
	float			GetLength()                   const;
	int				GetTaxicabLength()            const;
	int				GetLengthSquared()            const;
	float			GetOrientationRadians()       const;
	float			GetOrientationDegrees()       const;
	IntVec2 const	GetRotatedBy90Degrees()       const;
	IntVec2 const   GetRotatedByMinus90Degrees()  const;

	void            SetFromText( char const* text );

	/* Mutators ( non-const ) */
	void            Rotate90Degrees();
	void            RotateMinus90Degrees();


	/* Operators ( self-mutating / non-const ) */
	void            operator=( const IntVec2& copyFrom );
	IntVec2 const	operator+( IntVec2 const& intVecToAdd )                 const;
	IntVec2 const	operator-( IntVec2 const& intVecToSubtract )            const;
	IntVec2 const	operator-()                                              const;
	IntVec2 const	operator/ ( int inverseScale )                          const;
	bool		    operator==( IntVec2 const& compare )                    const;
	bool		    operator!=( IntVec2 const& compare )                    const;
	void		    operator+=( IntVec2 const& vecToAdd );
};


//-----------------------------------------------------------------------------------------------	
IntVec2 const GetAsIntVec2( float x, float y );