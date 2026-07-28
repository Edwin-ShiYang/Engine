#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Engine.hpp"
#include <d3d11.h>

//----------------------------------------------------------------------------------------------
ConstantBuffer::ConstantBuffer( size_t size )
	: m_size( size )
{
	Create();
}

//----------------------------------------------------------------------------------------------
void ConstantBuffer::Create()
{
	UINT constantBufferSize = ( UINT ) m_size;
	D3D11_BUFFER_DESC bufferDesc = {};

	bufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth      = constantBufferSize;
	bufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr;
	hr = g_engine->m_render->m_device->CreateBuffer( &bufferDesc, nullptr, &m_buffer );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create constant buffer." );
	}
}

//----------------------------------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer()
{
	DX_SAFE_RELEASE( m_buffer );
}