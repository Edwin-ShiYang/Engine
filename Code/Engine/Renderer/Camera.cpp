#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec4.hpp"


//-----------------------------------------------------------------------------------------------
Camera::Camera( const Vec2& orthographicBottomLeft, const Vec2& orthographicTopRight )
	: m_orthographicBottomLeft( orthographicBottomLeft )
	, m_orthographicTopRight( orthographicTopRight )
{
}


//-----------------------------------------------------------------------------------------------
void Camera::Translate2D( const Vec2& translation2D )
{
	m_orthographicBottomLeft.x += translation2D.x;
	m_orthographicBottomLeft.y += translation2D.y;

	m_orthographicTopRight.x += translation2D.x;
	m_orthographicTopRight.y += translation2D.y;
}


//-----------------------------------------------------------------------------------------------
Vec2 Camera::GetOrthographicBottomLeft() const
{
	return m_orthographicBottomLeft;
}


//-----------------------------------------------------------------------------------------------
Vec2 Camera::GetOrthographicTopRight() const
{
	return m_orthographicTopRight;
}


//-----------------------------------------------------------------------------------------------
void Camera::SetOrthographicView( Vec2 const& bottomLeft, Vec2 const& topRight, float near /*= 0.0f*/, float far /*= 1.0f */ )
{
 	m_mode                   = eMode_Orthographic;
	m_orthographicBottomLeft = bottomLeft;
	m_orthographicTopRight   = topRight;
	m_orthographicNear       = near;
	m_orthographicFar        = far;
}


//-----------------------------------------------------------------------------------------------
void Camera::SetPerspectiveView( float aspect, float fov, float near, float far )
{
	m_mode              = eMode_Perspective;
	m_perspectiveAspect = aspect;
	m_perspectiveFOV    = fov;
	m_perspectiveNear   = near;
	m_perspectiveFar    = far;
}


void Camera::SetPositionAndOrientation( const Vec3& position, const EulerAngles& orientation )
{
	m_position = position;
	m_orientation = orientation;
}

void Camera::SetPosition( const Vec3& position )
{
	m_position = position;
}


Vec3 Camera::GetPosition() const
{
	return m_position;
}

void Camera::SetOrientation( const EulerAngles& orientation )
{
	m_orientation = orientation;
}

EulerAngles Camera::GetOrientation() const
{
	return m_orientation;
}

//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetCameraToWorldTransform() const
{
	Mat44 cameraToWorldMatrix;

	Mat44 rotation = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	cameraToWorldMatrix.AppendTranslation3D( m_position );
	cameraToWorldMatrix.Orthonormalize_XFwd_YLeft_ZUp();
	cameraToWorldMatrix.Append( rotation );

	return cameraToWorldMatrix;
}


//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetWorldToCameraTransform() const
{
	return GetCameraToWorldTransform().GetOrthonormalInverse();
}


//-----------------------------------------------------------------------------------------------
void Camera::SetCameraToRenderTransform( const Mat44& m )
{
	m_cameraToRenderTransform = m;
}

//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetCameraToRenderTransform() const
{
	return m_cameraToRenderTransform;
}

Mat44 Camera::GetRenderToClipTransform() const
{
	return GetProjectionMatrix();
}


//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetOrthographicMatrix() const
{
	Mat44 orthographicMatrix = Mat44::MakeOrthoProjection( m_orthographicBottomLeft.x, m_orthographicTopRight.x, m_orthographicBottomLeft.y, m_orthographicTopRight.y, m_orthographicNear, m_orthographicFar );
	return orthographicMatrix;
}


//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetPerspectiveMatrix() const
{
	Mat44 perspectiveMatrix = Mat44::MakePerspectiveProjection( m_perspectiveFOV, m_perspectiveAspect, m_perspectiveNear, m_perspectiveFar );
	return perspectiveMatrix;
}


//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetProjectionMatrix() const
{
	return m_mode == eMode_Orthographic ? GetOrthographicMatrix() : GetPerspectiveMatrix();
}
