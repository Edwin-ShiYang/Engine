#pragma once

class Texture;

//-----------------------------------------------------------------------------------------------
struct Material
{
    Texture const* m_diffuseTexture          = nullptr;
    Texture const* m_normalTexture           = nullptr;
    Texture const* m_roughnessTexture        = nullptr;
    Texture const* m_metallicTexture         = nullptr;
    Texture const* m_ambientOcclusionTexture = nullptr;
    Texture const* m_specGlossEmissTexture   = nullptr;
};