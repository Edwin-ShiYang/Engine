#pragma once

//----------------------------------------------------------------------------------------------
enum class ConstantBufferSlot : int
{
    Engine      = 0,
    PerFrame    = 1,
    Camera      = 2,
    Model       = 3,
    Light       = 4,
    PostProcess = 5,
    Prefilter   = 6,
    Skin        = 7,
    Material    = 9
    // use 8 first
};

//----------------------------------------------------------------------------------------------
struct CameraConstants
{
    Mat44 c_renderToClip;
    Mat44 c_cameraToRender;
    Mat44 c_worldToCamera;
    Vec3  c_cameraWorldPos;
    float pad0;
};

//----------------------------------------------------------------------------------------------
struct ModelConstants
{
    Mat44 c_modelToWorld;
    float c_modelTint[ 4 ];
};

//----------------------------------------------------------------------------------------------
struct PostProcessConstants
{
    float c_width;
    float c_height;
    float pad0;
    float pad1;
};

//----------------------------------------------------------------------------------------------
struct PrefilterConstants
{
    float c_roughness;
    float padding0;
    float padding1;
    float padding2;
};

//-----------------------------------------------------------------------------------------------
struct SkinConstants
{
    Mat44 c_skinMatrices[ 128 ];
};

//-----------------------------------------------------------------------------------------------
struct MaterialConstants
{
    float c_metallic;
    float c_roughness;
    float c_ambientOcclusion;
    float pad0;
    float c_emissiveColor[ 4 ];
    float c_emissiveIntensity;
    float pad1;
    float pad2;
    float pad3;
};