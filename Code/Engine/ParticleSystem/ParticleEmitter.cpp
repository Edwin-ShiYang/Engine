#include "Engine/ParticleSystem/ParticleEmitter.hpp"

#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"

static RandomNumberGenerator s_rng;

//----------------------------------------------------------------------------------------------
ParticleEmitter::ParticleEmitter( Vec3 const& position )
    : m_position( position )
{
}

//----------------------------------------------------------------------------------------------
void ParticleEmitter::Spawn()
{
    Particle p;
    p.m_position = m_position;

    float       pitch = s_rng.MakeRandomFloatInRange( -180.f, 180.f );
    float       yaw   = s_rng.MakeRandomFloatInRange( -180.f, 180.f );

    EulerAngles orientation = EulerAngles( yaw, pitch, 0.f );
    p.m_velocity            = orientation.GetForwardDir_IFwd_JLeft_KUp().GetNormalized();

    p.m_startColor = Rgba8::CATPPUCCIN_GREEN;
    p.m_lifetime   = 10.f;
    m_particles.push_back( p );
}

//----------------------------------------------------------------------------------------------
void ParticleEmitter::Update()
{
    float deltaSeconds = static_cast< float >( Clock::GetSystemClock().GetDeltaSeconds() );

    m_spawnTimer += deltaSeconds;

    if ( m_spawnTimer >= m_spawnInterval )
    {
        Spawn();
        m_spawnTimer -= m_spawnInterval;
    }

    for ( int i = 0; i < static_cast< int >( m_particles.size() ); )
    {
        Particle& p  = m_particles[ i ];
        p.m_position = p.m_position + ( p.m_velocity * deltaSeconds * 20.f );
        p.m_age += deltaSeconds;

        if ( p.m_age >= p.m_lifetime )
        {
            m_particles.erase( m_particles.begin() + i );
        }
        else
        {
            ++i;
        }
    }
}

//----------------------------------------------------------------------------------------------
void ParticleEmitter::Render( Camera* camera ) const
{
    std::vector< Vertex > verts;
    AddVertsForAABB3D( verts, AABB3( -0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.1f ) );

    g_engine->m_render->SetModelConstants( GetModelToWorldTransform(), Rgba8::CATPPUCCIN_MAUVE );
    g_engine->m_render->BindTexture( g_defaultWhiteTexture );
    g_engine->m_render->BindShader( ShaderType::Default );
    g_engine->m_render->DrawVertexArray( verts );

    std::vector< Vertex > v;
    AddVertsForQuad3D( v, Vec3( 0.f, -0.5f, -0.5f ), Vec3( 0.f, 0.5f, -0.5f ), Vec3( 0.f, 0.5f, 0.5f ), Vec3( 0.f, -0.5f, 0.5f ) );
    //Texture* t = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/PolygonParticles_Smoke_01.png" );
    Texture* t = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/VFX/PolygonParticles_Sparkle.png" );

    for ( int i = 0; i < static_cast< int >( m_particles.size() ); ++i )
    {
        Particle const& p = m_particles[ i ];

        Mat44           m = GetBillboard( BillboardType::FULL_FACING, camera->GetCameraToWorldTransform(), p.m_position );
        g_engine->m_render->BindShader( ShaderType::PBRLitStatic );
        g_engine->m_render->SetMaterialConstants( 0.f, 0.f, 1.0f, 2.f, Rgba8::RED );
        g_engine->m_render->SetBlendMode( BlendMode::ALPHA );

        g_engine->m_render->BindTextureWithSampler( { t, SamplerMode::POINT_CLAMP, ShaderResourceSlot::DIFFUSE } );
        g_engine->m_render->BindTextureWithSampler( { g_defaultNormalTexture, SamplerMode::POINT_CLAMP, ShaderResourceSlot::NORMAL } );
        g_engine->m_render->BindTextureWithSampler( { g_defaultSGETexture, SamplerMode::POINT_CLAMP, ShaderResourceSlot::SPEC_GLOSS_EMIT } );
        g_engine->m_render->BindTextureWithSampler( { g_defaultAmbientOcclusionTexture, SamplerMode::POINT_CLAMP, ShaderResourceSlot::AMBIENT_OCCLUSION } );
        g_engine->m_render->BindTextureWithSampler( { g_defaultMetallicTexture, SamplerMode::POINT_CLAMP, ShaderResourceSlot::METALLIC } );
        g_engine->m_render->BindTextureWithSampler( { g_defaultRoughnessTexture, SamplerMode::POINT_CLAMP, ShaderResourceSlot::ROUGHNESS } );

        g_engine->m_render->SetModelConstants( m );
        g_engine->m_render->DrawVertexArray( v );

        g_engine->m_render->UnbindPBRTextures();
        g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
        g_engine->m_render->SetMaterialConstants();
        g_engine->m_render->BindShader( ShaderType::Default );
    }
}

//----------------------------------------------------------------------------------------------
Mat44 ParticleEmitter::GetModelToWorldTransform() const
{
    Mat44 modelToWorldTransform;

    Mat44 rotation = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
    rotation.Orthonormalize_XFwd_YLeft_ZUp();

    Mat44 scaleMatrix = Mat44::MakeNonUniformScale3D( m_scale );

    modelToWorldTransform.AppendTranslation3D( m_position );
    modelToWorldTransform.Append( rotation );
    modelToWorldTransform.Append( scaleMatrix );
    return modelToWorldTransform;
}