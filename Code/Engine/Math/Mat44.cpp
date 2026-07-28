#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
Mat44::Mat44()
{
    m_values[ Ix ] = 1.f;
    m_values[ Jx ] = 0.f;
    m_values[ Kx ] = 0.f;
    m_values[ Tx ] = 0.f;
    m_values[ Iy ] = 0.f;
    m_values[ Jy ] = 1.f;
    m_values[ Ky ] = 0.f;
    m_values[ Ty ] = 0.f;
    m_values[ Iz ] = 0.f;
    m_values[ Jz ] = 0.f;
    m_values[ Kz ] = 1.f;
    m_values[ Tz ] = 0.f;
    m_values[ Iw ] = 0.f;
    m_values[ Jw ] = 0.f;
    m_values[ Kw ] = 0.f;
    m_values[ Tw ] = 1.f;
}

//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D )
    : Mat44()
{
    m_values[ Ix ] = iBasis2D.x;
    m_values[ Iy ] = iBasis2D.y;

    m_values[ Jx ] = jBasis2D.x;
    m_values[ Jy ] = jBasis2D.y;

    m_values[ Tx ] = translation2D.x;
    m_values[ Ty ] = translation2D.y;
}

//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D )
    : Mat44()
{
    m_values[ Ix ] = iBasis3D.x;
    m_values[ Iy ] = iBasis3D.y;
    m_values[ Iz ] = iBasis3D.z;

    m_values[ Jx ] = jBasis3D.x;
    m_values[ Jy ] = jBasis3D.y;
    m_values[ Jz ] = jBasis3D.z;

    m_values[ Kx ] = kBasis3D.x;
    m_values[ Ky ] = kBasis3D.y;
    m_values[ Kz ] = kBasis3D.z;

    m_values[ Tx ] = translation3D.x;
    m_values[ Ty ] = translation3D.y;
    m_values[ Tz ] = translation3D.z;
}

//-----------------------------------------------------------------------------------------------
Mat44::Mat44( float const* sixteenValuesBasisMajor )
{
    for ( int i = 0; i < 16; ++i )
    {
        m_values[ i ] = sixteenValuesBasisMajor[ i ];
    }
}

//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec4 const& iBasis3D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D )
    : Mat44()
{
    m_values[ Ix ] = iBasis3D.x;
    m_values[ Iy ] = iBasis3D.y;
    m_values[ Iz ] = iBasis3D.z;
    m_values[ Iw ] = iBasis3D.w;

    m_values[ Jx ] = jBasis4D.x;
    m_values[ Jy ] = jBasis4D.y;
    m_values[ Jz ] = jBasis4D.z;
    m_values[ Jw ] = jBasis4D.w;

    m_values[ Kx ] = kBasis4D.x;
    m_values[ Ky ] = kBasis4D.y;
    m_values[ Kz ] = kBasis4D.z;
    m_values[ Kw ] = kBasis4D.w;

    m_values[ Tx ] = translation4D.x;
    m_values[ Ty ] = translation4D.y;
    m_values[ Tz ] = translation4D.z;
    m_values[ Tw ] = translation4D.w;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeTranslation2D( Vec2 const& translationXY )
{
    Mat44 mat44;
    mat44.AppendTranslation2D( translationXY );

    return mat44;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeTranslation3D( Vec3 const& translationXYZ )
{
    Mat44 mat44;
    mat44.AppendTranslation3D( translationXYZ );

    return mat44;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeUniformScale2D( float uniformScaleXY )
{
    Mat44 mat44;
    mat44.AppendScaleUniform2D( uniformScaleXY );

    return mat44;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeUniformScale3D( float uniformScaleXYZ )
{
    Mat44 mat44;
    mat44.AppendScaleUniform3D( uniformScaleXYZ );

    return mat44;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeNonUniformScale2D( Vec2 const& nonUniformScaleXY )
{
    Mat44 mat44;
    mat44.AppendScaleNonUniform2D( nonUniformScaleXY );

    return mat44;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeNonUniformScale3D( Vec3 const& nonUniformScaleXYZ )
{
    Mat44 mat44;
    mat44.AppendScaleNonUniform3D( nonUniformScaleXYZ );

    return mat44;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeZRotationDegrees( float rotationDegreesAboutZ )
{
    Mat44 mat44;
    mat44.AppendZRotation( rotationDegreesAboutZ );

    return mat44;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeYRotationDegrees( float rotationDegreesAboutY )
{
    Mat44 mat44;
    mat44.AppendYRotation( rotationDegreesAboutY );

    return mat44;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeXRotationDegrees( float rotationDegreesAboutX )
{
    Mat44 mat44;
    mat44.AppendXRotation( rotationDegreesAboutX );

    return mat44;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeOrthoProjection( float left, float right, float bottom, float top, float zNear, float zFar )
{
    Mat44 orthoProjectionMatrix;

    orthoProjectionMatrix.m_values[ Ix ] = 2.f / ( right - left );
    orthoProjectionMatrix.m_values[ Tx ] = ( left + right ) / ( left - right );

    orthoProjectionMatrix.m_values[ Jy ] = 2.f / ( top - bottom );
    orthoProjectionMatrix.m_values[ Ty ] = ( top + bottom ) / ( bottom - top );

    orthoProjectionMatrix.m_values[ Kz ] = 1.f / ( zFar - zNear );
    orthoProjectionMatrix.m_values[ Tz ] = zNear / ( zNear - zFar );

    return orthoProjectionMatrix;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakePerspectiveProjection( float fovYDegrees, float aspect, float zNear, float zFar )
{
    Mat44 perspective;

    float c      = CosDegrees( fovYDegrees * 0.5f );
    float s      = SinDegrees( fovYDegrees * 0.5f );
    float scaleY = c / s;
    float scaleX = scaleY / aspect;

    float scaleZ     = zFar / ( zFar - zNear );
    float translateZ = ( zNear * zFar ) / ( zNear - zFar );

    perspective.m_values[ Ix ] = scaleX;
    perspective.m_values[ Jy ] = scaleY;
    perspective.m_values[ Kz ] = scaleZ;
    perspective.m_values[ Kw ] = 1.f;
    perspective.m_values[ Tz ] = translateZ;
    perspective.m_values[ Tw ] = 0.f;

    return perspective;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeCameraToRenderTransform()
{
    Mat44 cameraToRenderTransform;
    cameraToRenderTransform.SetIJKT4D(
        Vec4( 0.0f, 0.0f, 1.0f, 0.0f ),
        Vec4( -1.0f, 0.0f, 0.0f, 0.0f ),
        Vec4( 0.0f, 1.0f, 0.0f, 0.0f ),
        Vec4( 0.0f, 0.0f, 0.0f, 1.0f ) );
    return cameraToRenderTransform;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeLookAtTransform( Vec3 const& position, Vec3 const& targetPosition )
{
    Mat44 lookAtMatrix;
    Vec3  iBasis = ( targetPosition - position ).GetNormalized();
    Vec3  jBasis;
    Vec3  kBasis;

    if ( abs( DotProduct3D( iBasis, Vec3::WORLD_UP ) ) < 0.99999f )
    {
        jBasis = CrossProduct3D( Vec3::WORLD_UP, iBasis ).GetNormalized();
        kBasis = CrossProduct3D( iBasis, jBasis );
    }
    else
    {
        kBasis = CrossProduct3D( iBasis, Vec3::WORLD_LEFT ).GetNormalized();
        jBasis = CrossProduct3D( kBasis, iBasis );
    }

    lookAtMatrix.SetIJKT3D( iBasis, jBasis, kBasis, position );
    return lookAtMatrix;
}

//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::TransformVectorQuantity2D( Vec2 const& vectorQuantityXY ) const
{
    float x = m_values[ Ix ] * vectorQuantityXY.x + m_values[ Jx ] * vectorQuantityXY.y;
    float y = m_values[ Iy ] * vectorQuantityXY.x + m_values[ Jy ] * vectorQuantityXY.y;

    return Vec2( x, y );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::TransformVectorQuantity3D( Vec3 const& vectorQuantityXYZ ) const
{
    float x = m_values[ Ix ] * vectorQuantityXYZ.x + m_values[ Jx ] * vectorQuantityXYZ.y + m_values[ Kx ] * vectorQuantityXYZ.z;
    float y = m_values[ Iy ] * vectorQuantityXYZ.x + m_values[ Jy ] * vectorQuantityXYZ.y + m_values[ Ky ] * vectorQuantityXYZ.z;
    float z = m_values[ Iz ] * vectorQuantityXYZ.x + m_values[ Jz ] * vectorQuantityXYZ.y + m_values[ Kz ] * vectorQuantityXYZ.z;

    return Vec3( x, y, z );
}

//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::TransformPosition2D( Vec2 const& positionXY ) const
{
    float x = m_values[ Ix ] * positionXY.x + m_values[ Jx ] * positionXY.y;
    float y = m_values[ Iy ] * positionXY.x + m_values[ Jy ] * positionXY.y;

    x += m_values[ Tx ];
    y += m_values[ Ty ];

    return Vec2( x, y );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::TransformPosition3D( Vec3 const& position3D ) const
{
    float x = m_values[ Ix ] * position3D.x + m_values[ Jx ] * position3D.y + m_values[ Kx ] * position3D.z;
    float y = m_values[ Iy ] * position3D.x + m_values[ Jy ] * position3D.y + m_values[ Ky ] * position3D.z;
    float z = m_values[ Iz ] * position3D.x + m_values[ Jz ] * position3D.y + m_values[ Kz ] * position3D.z;

    x += m_values[ Tx ];
    y += m_values[ Ty ];
    z += m_values[ Tz ];

    return Vec3( x, y, z );
}

//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::TransformHomogeneous3D( Vec4 const& homogeneousPoint3D ) const
{
    float x = m_values[ Ix ] * homogeneousPoint3D.x + m_values[ Jx ] * homogeneousPoint3D.y + m_values[ Kx ] * homogeneousPoint3D.z + m_values[ Tx ] * homogeneousPoint3D.w;
    float y = m_values[ Iy ] * homogeneousPoint3D.x + m_values[ Jy ] * homogeneousPoint3D.y + m_values[ Ky ] * homogeneousPoint3D.z + m_values[ Ty ] * homogeneousPoint3D.w;
    float z = m_values[ Iz ] * homogeneousPoint3D.x + m_values[ Jz ] * homogeneousPoint3D.y + m_values[ Kz ] * homogeneousPoint3D.z + m_values[ Tz ] * homogeneousPoint3D.w;
    float w = m_values[ Iw ] * homogeneousPoint3D.x + m_values[ Jw ] * homogeneousPoint3D.y + m_values[ Kw ] * homogeneousPoint3D.z + m_values[ Tw ] * homogeneousPoint3D.w;

    return Vec4( x, y, z, w );
}

//-----------------------------------------------------------------------------------------------
float* Mat44::GetAsFloatArray()
{
    return m_values;
}

//-----------------------------------------------------------------------------------------------
float const* Mat44::GetAsFloatArray() const
{
    return m_values;
}

//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::GetIBasis2D() const
{
    return Vec2( m_values[ Ix ], m_values[ Iy ] );
}

//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::GetJBasis2D() const
{
    return Vec2( m_values[ Jx ], m_values[ Jy ] );
}

//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::GetTranslation2D() const
{
    return Vec2( m_values[ Tx ], m_values[ Ty ] );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetIBasis3D() const
{
    return Vec3( m_values[ Ix ], m_values[ Iy ], m_values[ Iz ] );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetJBasis3D() const
{
    return Vec3( m_values[ Jx ], m_values[ Jy ], m_values[ Jz ] );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetKBasis3D() const
{
    return Vec3( m_values[ Kx ], m_values[ Ky ], m_values[ Kz ] );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetTranslation3D() const
{
    return Vec3( m_values[ Tx ], m_values[ Ty ], m_values[ Tz ] );
}

//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetIBasis4D() const
{
    return Vec4( m_values[ Ix ], m_values[ Iy ], m_values[ Iz ], m_values[ Iw ] );
}

//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetJBasis4D() const
{
    return Vec4( m_values[ Jx ], m_values[ Jy ], m_values[ Jz ], m_values[ Jw ] );
}

//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetKBasis4D() const
{
    return Vec4( m_values[ Kx ], m_values[ Ky ], m_values[ Kz ], m_values[ Kw ] );
}

//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetTranslation4D() const
{
    return Vec4( m_values[ Tx ], m_values[ Ty ], m_values[ Tz ], m_values[ Tw ] );
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::GetOrthonormalInverse() const
{
    Mat44 inverse = *this;
    inverse.Transpose();  // undo rotation

    Vec3 translation = GetTranslation3D();
    Vec3 iBasis      = GetIBasis3D();
    Vec3 jBasis      = GetJBasis3D();
    Vec3 kBasis      = GetKBasis3D();

    inverse.m_values[ Tx ] = -( iBasis.x * translation.x + iBasis.y * translation.y + iBasis.z * translation.z );
    inverse.m_values[ Ty ] = -( jBasis.x * translation.x + jBasis.y * translation.y + jBasis.z * translation.z );
    inverse.m_values[ Tz ] = -( kBasis.x * translation.x + kBasis.y * translation.y + kBasis.z * translation.z );

    inverse.m_values[ Iw ] = 0.0f;
    inverse.m_values[ Jw ] = 0.0f;
    inverse.m_values[ Kw ] = 0.0f;
    inverse.m_values[ Tw ] = 1.0f;

    return inverse;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetTranslation2D( Vec2 const& translationXY )
{
    m_values[ Tx ] = translationXY.x;
    m_values[ Ty ] = translationXY.y;
    m_values[ Tz ] = 0.f;
    m_values[ Tw ] = 1.f;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetTranslation3D( Vec3 const& translationXYZ )
{
    m_values[ Tx ] = translationXYZ.x;
    m_values[ Ty ] = translationXYZ.y;
    m_values[ Tz ] = translationXYZ.z;
    m_values[ Tw ] = 1.f;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJ2D( Vec2 const& iBasis2D, Vec2 const& jBasis2D )
{
    m_values[ Ix ] = iBasis2D.x;
    m_values[ Iy ] = iBasis2D.y;
    m_values[ Iz ] = 0.f;
    m_values[ Iw ] = 0.f;

    m_values[ Jx ] = jBasis2D.x;
    m_values[ Jy ] = jBasis2D.y;
    m_values[ Jz ] = 0.f;
    m_values[ Jw ] = 0.f;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJT2D( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY )
{
    m_values[ Ix ] = iBasis2D.x;
    m_values[ Iy ] = iBasis2D.y;
    m_values[ Iz ] = 0.f;
    m_values[ Iw ] = 0.f;

    m_values[ Jx ] = jBasis2D.x;
    m_values[ Jy ] = jBasis2D.y;
    m_values[ Jz ] = 0.f;
    m_values[ Jw ] = 0.f;

    m_values[ Tx ] = translationXY.x;
    m_values[ Ty ] = translationXY.y;
    m_values[ Tz ] = 0.f;
    m_values[ Tw ] = 1.f;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJK3D( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D )
{
    m_values[ Ix ] = iBasis3D.x;
    m_values[ Jx ] = jBasis3D.x;
    m_values[ Kx ] = kBasis3D.x;
    m_values[ Iy ] = iBasis3D.y;
    m_values[ Jy ] = jBasis3D.y;
    m_values[ Ky ] = kBasis3D.y;
    m_values[ Iz ] = iBasis3D.z;
    m_values[ Jz ] = jBasis3D.z;
    m_values[ Kz ] = kBasis3D.z;
    m_values[ Iw ] = 0.f;
    m_values[ Jw ] = 0.f;
    m_values[ Kw ] = 0.f;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJKT3D( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translationXYZ )
{
    m_values[ Ix ] = iBasis3D.x;
    m_values[ Jx ] = jBasis3D.x;
    m_values[ Kx ] = kBasis3D.x;
    m_values[ Tx ] = translationXYZ.x;
    m_values[ Iy ] = iBasis3D.y;
    m_values[ Jy ] = jBasis3D.y;
    m_values[ Ky ] = kBasis3D.y;
    m_values[ Ty ] = translationXYZ.y;
    m_values[ Iz ] = iBasis3D.z;
    m_values[ Jz ] = jBasis3D.z;
    m_values[ Kz ] = kBasis3D.z;
    m_values[ Tz ] = translationXYZ.z;
    m_values[ Iw ] = 0.f;
    m_values[ Jw ] = 0.f;
    m_values[ Kw ] = 0.f;
    m_values[ Tw ] = 1.f;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJKT4D( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D )
{
    m_values[ Ix ] = iBasis4D.x;
    m_values[ Jx ] = jBasis4D.x;
    m_values[ Kx ] = kBasis4D.x;
    m_values[ Tx ] = translation4D.x;
    m_values[ Iy ] = iBasis4D.y;
    m_values[ Jy ] = jBasis4D.y;
    m_values[ Ky ] = kBasis4D.y;
    m_values[ Ty ] = translation4D.y;
    m_values[ Iz ] = iBasis4D.z;
    m_values[ Jz ] = jBasis4D.z;
    m_values[ Kz ] = kBasis4D.z;
    m_values[ Tz ] = translation4D.z;
    m_values[ Iw ] = iBasis4D.w;
    m_values[ Jw ] = jBasis4D.w;
    m_values[ Kw ] = kBasis4D.w;
    m_values[ Tw ] = translation4D.w;
}

//-----------------------------------------------------------------------------------------------
void Mat44::Transpose()
{
    Mat44  copy      = *this;
    float* oldValues = copy.m_values;
    m_values[ Iy ]   = oldValues[ Jx ];
    m_values[ Iz ]   = oldValues[ Kx ];
    m_values[ Iw ]   = oldValues[ Tx ];
    m_values[ Jx ]   = oldValues[ Iy ];
    m_values[ Kx ]   = oldValues[ Iz ];
    m_values[ Tx ]   = oldValues[ Iw ];

    m_values[ Jz ] = oldValues[ Ky ];
    m_values[ Jw ] = oldValues[ Ty ];
    m_values[ Ky ] = oldValues[ Jz ];
    m_values[ Ty ] = oldValues[ Jw ];

    m_values[ Kw ] = oldValues[ Tz ];
    m_values[ Tz ] = oldValues[ Kw ];
}

//-----------------------------------------------------------------------------------------------
void Mat44::Orthonormalize_XFwd_YLeft_ZUp()
{
    Mat44 copy = *this;

    Vec3  iBasis = GetIBasis3D();
    Vec3  jBasis = GetJBasis3D();
    Vec3  kBasis = GetKBasis3D();

    // i-basis
    Vec3  normalizedIBasis = iBasis.GetNormalized();

    //k-basis
    float lengthKOnI = DotProduct3D( kBasis, normalizedIBasis );
    Vec3  KOnIVector = normalizedIBasis * lengthKOnI;
    kBasis -= KOnIVector;
    Vec3  normalizedKBasis = kBasis.GetNormalized();

    //j-basis
    float lengthJOnI = DotProduct3D( copy.GetJBasis3D(), normalizedIBasis );
    Vec3  JOnIVector = normalizedIBasis * lengthJOnI;
    jBasis -= JOnIVector;

    float lengthJOnK = DotProduct3D( copy.GetJBasis3D(), normalizedKBasis );
    Vec3  JOnKVector = normalizedKBasis * lengthJOnK;
    jBasis -= JOnKVector;
    Vec3 normalizedJBasis = jBasis.GetNormalized();

    this->SetIJK3D( normalizedIBasis, normalizedJBasis, normalizedKBasis );
}

//-----------------------------------------------------------------------------------------------
void Mat44::Append( Mat44 const& appendThis )
{
    Mat44  copy      = *this;
    float* oldValues = copy.m_values;

    m_values[ Ix ] = oldValues[ Ix ] * appendThis.m_values[ Ix ] + oldValues[ Jx ] * appendThis.m_values[ Iy ] + oldValues[ Kx ] * appendThis.m_values[ Iz ] + oldValues[ Tx ] * appendThis.m_values[ Iw ];
    m_values[ Iy ] = oldValues[ Iy ] * appendThis.m_values[ Ix ] + oldValues[ Jy ] * appendThis.m_values[ Iy ] + oldValues[ Ky ] * appendThis.m_values[ Iz ] + oldValues[ Ty ] * appendThis.m_values[ Iw ];
    m_values[ Iz ] = oldValues[ Iz ] * appendThis.m_values[ Ix ] + oldValues[ Jz ] * appendThis.m_values[ Iy ] + oldValues[ Kz ] * appendThis.m_values[ Iz ] + oldValues[ Tz ] * appendThis.m_values[ Iw ];
    m_values[ Iw ] = oldValues[ Iw ] * appendThis.m_values[ Ix ] + oldValues[ Jw ] * appendThis.m_values[ Iy ] + oldValues[ Kw ] * appendThis.m_values[ Iz ] + oldValues[ Tw ] * appendThis.m_values[ Iw ];

    m_values[ Jx ] = oldValues[ Ix ] * appendThis.m_values[ Jx ] + oldValues[ Jx ] * appendThis.m_values[ Jy ] + oldValues[ Kx ] * appendThis.m_values[ Jz ] + oldValues[ Tx ] * appendThis.m_values[ Jw ];
    m_values[ Jy ] = oldValues[ Iy ] * appendThis.m_values[ Jx ] + oldValues[ Jy ] * appendThis.m_values[ Jy ] + oldValues[ Ky ] * appendThis.m_values[ Jz ] + oldValues[ Ty ] * appendThis.m_values[ Jw ];
    m_values[ Jz ] = oldValues[ Iz ] * appendThis.m_values[ Jx ] + oldValues[ Jz ] * appendThis.m_values[ Jy ] + oldValues[ Kz ] * appendThis.m_values[ Jz ] + oldValues[ Tz ] * appendThis.m_values[ Jw ];
    m_values[ Jw ] = oldValues[ Iw ] * appendThis.m_values[ Jx ] + oldValues[ Jw ] * appendThis.m_values[ Jy ] + oldValues[ Kw ] * appendThis.m_values[ Jz ] + oldValues[ Tw ] * appendThis.m_values[ Jw ];

    m_values[ Kx ] = oldValues[ Ix ] * appendThis.m_values[ Kx ] + oldValues[ Jx ] * appendThis.m_values[ Ky ] + oldValues[ Kx ] * appendThis.m_values[ Kz ] + oldValues[ Tx ] * appendThis.m_values[ Kw ];
    m_values[ Ky ] = oldValues[ Iy ] * appendThis.m_values[ Kx ] + oldValues[ Jy ] * appendThis.m_values[ Ky ] + oldValues[ Ky ] * appendThis.m_values[ Kz ] + oldValues[ Ty ] * appendThis.m_values[ Kw ];
    m_values[ Kz ] = oldValues[ Iz ] * appendThis.m_values[ Kx ] + oldValues[ Jz ] * appendThis.m_values[ Ky ] + oldValues[ Kz ] * appendThis.m_values[ Kz ] + oldValues[ Tz ] * appendThis.m_values[ Kw ];
    m_values[ Kw ] = oldValues[ Iw ] * appendThis.m_values[ Kx ] + oldValues[ Jw ] * appendThis.m_values[ Ky ] + oldValues[ Kw ] * appendThis.m_values[ Kz ] + oldValues[ Tw ] * appendThis.m_values[ Kw ];

    m_values[ Tx ] = oldValues[ Ix ] * appendThis.m_values[ Tx ] + oldValues[ Jx ] * appendThis.m_values[ Ty ] + oldValues[ Kx ] * appendThis.m_values[ Tz ] + oldValues[ Tx ] * appendThis.m_values[ Tw ];
    m_values[ Ty ] = oldValues[ Iy ] * appendThis.m_values[ Tx ] + oldValues[ Jy ] * appendThis.m_values[ Ty ] + oldValues[ Ky ] * appendThis.m_values[ Tz ] + oldValues[ Ty ] * appendThis.m_values[ Tw ];
    m_values[ Tz ] = oldValues[ Iz ] * appendThis.m_values[ Tx ] + oldValues[ Jz ] * appendThis.m_values[ Ty ] + oldValues[ Kz ] * appendThis.m_values[ Tz ] + oldValues[ Tz ] * appendThis.m_values[ Tw ];
    m_values[ Tw ] = oldValues[ Iw ] * appendThis.m_values[ Tx ] + oldValues[ Jw ] * appendThis.m_values[ Ty ] + oldValues[ Kw ] * appendThis.m_values[ Tz ] + oldValues[ Tw ] * appendThis.m_values[ Tw ];
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendZRotation( float degreesRotationAboutZ )
{
    float radians = ConvertDegreesToRadians( degreesRotationAboutZ );

    float oldIx = m_values[ Ix ];
    float oldIy = m_values[ Iy ];
    float oldIz = m_values[ Iz ];
    float oldIw = m_values[ Iw ];

    float oldJx = m_values[ Jx ];
    float oldJy = m_values[ Jy ];
    float oldJz = m_values[ Jz ];
    float oldJw = m_values[ Jw ];

    float cos = cosf( radians );
    float sin = sinf( radians );

    m_values[ Ix ] = oldIx * cos + oldJx * sin;
    m_values[ Iy ] = oldIy * cos + oldJy * sin;

    m_values[ Jx ] = oldIx * -sin + oldJx * cos;
    m_values[ Jy ] = oldIy * -sin + oldJy * cos;

    m_values[ Iz ] = oldIz * cos + oldJz * sin;
    m_values[ Iw ] = oldIw * cos + oldJw * sin;

    m_values[ Jz ] = oldIz * -sin + oldJz * cos;
    m_values[ Jw ] = oldIw * -sin + oldJw * cos;
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendYRotation( float degreesRotationAboutY )
{
    float radians = ConvertDegreesToRadians( degreesRotationAboutY );

    float oldIx = m_values[ Ix ];
    float oldIy = m_values[ Iy ];
    float oldIz = m_values[ Iz ];
    float oldIw = m_values[ Iw ];

    float oldKx = m_values[ Kx ];
    float oldKy = m_values[ Ky ];
    float oldKz = m_values[ Kz ];
    float oldKw = m_values[ Kw ];

    float cos = cosf( radians );
    float sin = sinf( radians );

    m_values[ Ix ] = oldIx * cos + oldKx * -sin;
    m_values[ Kx ] = oldIx * sin + oldKx * cos;

    m_values[ Iy ] = oldIy * cos + oldKy * -sin;
    m_values[ Ky ] = oldIy * sin + oldKy * cos;

    m_values[ Iz ] = oldIz * cos + oldKz * -sin;
    m_values[ Kz ] = oldIz * sin + oldKz * cos;

    m_values[ Iw ] = oldIw * cos + oldKw * -sin;
    m_values[ Kw ] = oldIw * sin + oldKw * cos;
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendXRotation( float degreesRotationAboutX )
{
    float radians = ConvertDegreesToRadians( degreesRotationAboutX );

    float oldJx = m_values[ Jx ];
    float oldJy = m_values[ Jy ];
    float oldJz = m_values[ Jz ];
    float oldJw = m_values[ Jw ];

    float oldKx = m_values[ Kx ];
    float oldKy = m_values[ Ky ];
    float oldKz = m_values[ Kz ];
    float oldKw = m_values[ Kw ];

    float cos = cosf( radians );
    float sin = sinf( radians );

    m_values[ Jx ] = oldJx * cos + oldKx * sin;
    m_values[ Kx ] = oldJx * -sin + oldKx * cos;

    m_values[ Jy ] = oldJy * cos + oldKy * sin;
    m_values[ Ky ] = oldJy * -sin + oldKy * cos;

    m_values[ Jz ] = oldJz * cos + oldKz * sin;
    m_values[ Kz ] = oldJz * -sin + oldKz * cos;

    m_values[ Jw ] = oldJw * cos + oldKw * sin;
    m_values[ Kw ] = oldJw * -sin + oldKw * cos;
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendTranslation2D( Vec2 const& translationXY )
{
    float oldIx = m_values[ Ix ];
    float oldIy = m_values[ Iy ];
    float oldIz = m_values[ Iz ];
    float oldIw = m_values[ Iw ];

    float oldJx = m_values[ Jx ];
    float oldJy = m_values[ Jy ];
    float oldJz = m_values[ Jz ];
    float oldJw = m_values[ Jw ];

    m_values[ Tx ] += oldIx * translationXY.x + oldJx * translationXY.y;
    m_values[ Ty ] += oldIy * translationXY.x + oldJy * translationXY.y;
    m_values[ Tz ] += oldIz * translationXY.x + oldJz * translationXY.y;
    m_values[ Tw ] += oldIw * translationXY.x + oldJw * translationXY.y;
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendTranslation3D( Vec3 const& translationXYZ )
{
    float oldIx = m_values[ Ix ];
    float oldIy = m_values[ Iy ];
    float oldIz = m_values[ Iz ];
    float oldIw = m_values[ Iw ];

    float oldJx = m_values[ Jx ];
    float oldJy = m_values[ Jy ];
    float oldJz = m_values[ Jz ];
    float oldJw = m_values[ Jw ];

    float oldKx = m_values[ Kx ];
    float oldKy = m_values[ Ky ];
    float oldKz = m_values[ Kz ];
    float oldKw = m_values[ Kw ];

    m_values[ Tx ] += oldIx * translationXYZ.x + oldJx * translationXYZ.y + oldKx * translationXYZ.z;
    m_values[ Ty ] += oldIy * translationXYZ.x + oldJy * translationXYZ.y + oldKy * translationXYZ.z;
    m_values[ Tz ] += oldIz * translationXYZ.x + oldJz * translationXYZ.y + oldKz * translationXYZ.z;
    m_values[ Tw ] += oldIw * translationXYZ.x + oldJw * translationXYZ.y + oldKw * translationXYZ.z;
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleUniform2D( float uniformScaleXY )
{
    float oldIx = m_values[ Ix ];
    float oldIy = m_values[ Iy ];
    float oldIz = m_values[ Iz ];
    float oldIw = m_values[ Iw ];

    float oldJx = m_values[ Jx ];
    float oldJy = m_values[ Jy ];
    float oldJz = m_values[ Jz ];
    float oldJw = m_values[ Jw ];

    m_values[ Ix ] = oldIx * uniformScaleXY;
    m_values[ Iy ] = oldIy * uniformScaleXY;
    m_values[ Iz ] = oldIz * uniformScaleXY;
    m_values[ Iw ] = oldIw * uniformScaleXY;

    m_values[ Jx ] = oldJx * uniformScaleXY;
    m_values[ Jy ] = oldJy * uniformScaleXY;
    m_values[ Jz ] = oldJz * uniformScaleXY;
    m_values[ Jw ] = oldJw * uniformScaleXY;
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleUniform3D( float uniformScaleXYZ )
{
    float oldIx = m_values[ Ix ];
    float oldIy = m_values[ Iy ];
    float oldIz = m_values[ Iz ];
    float oldIw = m_values[ Iw ];

    float oldJx = m_values[ Jx ];
    float oldJy = m_values[ Jy ];
    float oldJz = m_values[ Jz ];
    float oldJw = m_values[ Jw ];

    float oldKx = m_values[ Kx ];
    float oldKy = m_values[ Ky ];
    float oldKz = m_values[ Kz ];
    float oldKw = m_values[ Kw ];

    m_values[ Ix ] = oldIx * uniformScaleXYZ;
    m_values[ Iy ] = oldIy * uniformScaleXYZ;
    m_values[ Iz ] = oldIz * uniformScaleXYZ;
    m_values[ Iw ] = oldIw * uniformScaleXYZ;

    m_values[ Jx ] = oldJx * uniformScaleXYZ;
    m_values[ Jy ] = oldJy * uniformScaleXYZ;
    m_values[ Jz ] = oldJz * uniformScaleXYZ;
    m_values[ Jw ] = oldJw * uniformScaleXYZ;

    m_values[ Kx ] = oldKx * uniformScaleXYZ;
    m_values[ Ky ] = oldKy * uniformScaleXYZ;
    m_values[ Kz ] = oldKz * uniformScaleXYZ;
    m_values[ Kw ] = oldKw * uniformScaleXYZ;
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleNonUniform2D( Vec2 const& nonUniformScaleXY )
{
    float oldIx = m_values[ Ix ];
    float oldIy = m_values[ Iy ];
    float oldIz = m_values[ Iz ];
    float oldIw = m_values[ Iw ];

    float oldJx = m_values[ Jx ];
    float oldJy = m_values[ Jy ];
    float oldJz = m_values[ Jz ];
    float oldJw = m_values[ Jw ];

    m_values[ Ix ] = oldIx * nonUniformScaleXY.x;
    m_values[ Iy ] = oldIy * nonUniformScaleXY.x;
    m_values[ Iz ] = oldIz * nonUniformScaleXY.x;
    m_values[ Iw ] = oldIw * nonUniformScaleXY.x;

    m_values[ Jx ] = oldJx * nonUniformScaleXY.y;
    m_values[ Jy ] = oldJy * nonUniformScaleXY.y;
    m_values[ Jz ] = oldJz * nonUniformScaleXY.y;
    m_values[ Jw ] = oldJw * nonUniformScaleXY.y;
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleNonUniform3D( Vec3 const& nonUniformScaleXYZ )
{
    float oldIx = m_values[ Ix ];
    float oldIy = m_values[ Iy ];
    float oldIz = m_values[ Iz ];
    float oldIw = m_values[ Iw ];

    float oldJx = m_values[ Jx ];
    float oldJy = m_values[ Jy ];
    float oldJz = m_values[ Jz ];
    float oldJw = m_values[ Jw ];

    float oldKx = m_values[ Kx ];
    float oldKy = m_values[ Ky ];
    float oldKz = m_values[ Kz ];
    float oldKw = m_values[ Kw ];

    m_values[ Ix ] = oldIx * nonUniformScaleXYZ.x;
    m_values[ Iy ] = oldIy * nonUniformScaleXYZ.x;
    m_values[ Iz ] = oldIz * nonUniformScaleXYZ.x;
    m_values[ Iw ] = oldIw * nonUniformScaleXYZ.x;

    m_values[ Jx ] = oldJx * nonUniformScaleXYZ.y;
    m_values[ Jy ] = oldJy * nonUniformScaleXYZ.y;
    m_values[ Jz ] = oldJz * nonUniformScaleXYZ.y;
    m_values[ Jw ] = oldJw * nonUniformScaleXYZ.y;

    m_values[ Kx ] = oldKx * nonUniformScaleXYZ.z;
    m_values[ Ky ] = oldKy * nonUniformScaleXYZ.z;
    m_values[ Kz ] = oldKz * nonUniformScaleXYZ.z;
    m_values[ Kw ] = oldKw * nonUniformScaleXYZ.z;
}