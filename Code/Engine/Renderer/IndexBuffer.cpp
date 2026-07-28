#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>


//----------------------------------------------------------------------------------------------
IndexBuffer::IndexBuffer( unsigned int size, unsigned int stride )
	: m_size( size )
	, m_stride( stride )
{
	Create();
}


//----------------------------------------------------------------------------------------------
IndexBuffer::~IndexBuffer()
{
	DX_SAFE_RELEASE( m_buffer );
}


//----------------------------------------------------------------------------------------------
void IndexBuffer::Create()
{
	UINT indexBufferSize        = ( UINT ) m_size;
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage             = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth         = indexBufferSize;
	bufferDesc.BindFlags         = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags    = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr;
	hr = g_engine->m_render->m_device->CreateBuffer( &bufferDesc, nullptr, &m_buffer );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create index buffer." );
	}
}


//----------------------------------------------------------------------------------------------
void IndexBuffer::Resize( unsigned int size )
{
	DX_SAFE_RELEASE( m_buffer );
	m_size = size;
	Create();
}


//----------------------------------------------------------------------------------------------
unsigned int IndexBuffer::GetSize()
{
	return m_size;
}


//----------------------------------------------------------------------------------------------
unsigned int IndexBuffer::GetStride()
{
	return m_stride;
}


//----------------------------------------------------------------------------------------------
unsigned int IndexBuffer::GetCount()
{
	return m_size / m_stride;
}
