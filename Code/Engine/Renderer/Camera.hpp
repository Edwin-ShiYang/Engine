#pragma once


//----------------------------------------------------------------------------------------------
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/EulerAngles.hpp"


//----------------------------------------------------------------------------------------------
class EulerAngles;


//----------------------------------------------------------------------------------------------
struct Viewport
{
	float x      = 0.0f;
	float y      = 0.0f;
	float width  = 1.0f;
	float height = 1.0f;
};


//----------------------------------------------------------------------------------------------
class Camera
{
public:
	enum Mode
	{
		eMode_Orthographic,
		eMode_Perspective,
		eMode_Count
	};
	
	Camera() = default;
	Camera( const Vec2& orthographicBottomLeft, const Vec2& orthographicTopRight );
	~Camera() = default;


	void SetOrthographicView( Vec2 const& bottomLeft, Vec2 const& topRight, float near = 0.0f, float far = 1.0f );
	void SetPerspectiveView( float aspect, float fov, float near, float far );
	
	void SetPositionAndOrientation( const Vec3& position, const EulerAngles& orientation );
	void SetPosition( const Vec3& position );
	Vec3 GetPosition() const;
	void SetOrientation( const EulerAngles& orientation );
	EulerAngles GetOrientation() const;

	Mat44 GetCameraToWorldTransform() const;
	Mat44 GetWorldToCameraTransform() const;

	void  SetCameraToRenderTransform( const Mat44& m );
	Mat44 GetCameraToRenderTransform() const;

	Mat44 GetRenderToClipTransform() const;

	Vec2 GetOrthographicBottomLeft() const;
	Vec2 GetOrthographicTopRight() const;
	void Translate2D( const Vec2& translation2D );
	
	Mat44 GetOrthographicMatrix() const;
	Mat44 GetPerspectiveMatrix() const;
	Mat44 GetProjectionMatrix() const;

protected:
	Mode        m_mode = eMode_Orthographic;

	Vec3        m_position;
	EulerAngles m_orientation;

	Vec2        m_orthographicBottomLeft;
	Vec2        m_orthographicTopRight;
	float       m_orthographicNear;
	float       m_orthographicFar;

	float       m_perspectiveAspect;
	float       m_perspectiveFOV;
	float       m_perspectiveNear;
	float       m_perspectiveFar;

	Mat44       m_cameraToRenderTransform;
};