#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <Windows.h>
#include <d3d11.h>


//----------------------------------------------------------------------------------------------
VertexBuffer::VertexBuffer( unsigned int size, unsigned int stride )
	: m_size( size )
	, m_stride( stride )
{
	Create();
}


//----------------------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	DX_SAFE_RELEASE( m_buffer );
}


//----------------------------------------------------------------------------------------------
void VertexBuffer::Create()
{
	UINT vertexBufferSize = ( UINT ) m_size;
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage             = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth         = vertexBufferSize;
	bufferDesc.BindFlags         = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags    = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr;
	hr = g_engine->m_render->m_device->CreateBuffer( &bufferDesc, nullptr, &m_buffer );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create vertex buffer." );
	}
}


//----------------------------------------------------------------------------------------------
void VertexBuffer::Resize( unsigned int size )
{
	DX_SAFE_RELEASE( m_buffer );
	m_size = size;
	Create();
}


//----------------------------------------------------------------------------------------------
unsigned int VertexBuffer::GetSize()
{
	return m_size;
}


//----------------------------------------------------------------------------------------------
unsigned int VertexBuffer::GetStride()
{
	return m_stride;
}


//----------------------------------------------------------------------------------------------
unsigned int VertexBuffer::GetCount()
{
	return m_size / m_stride;
}