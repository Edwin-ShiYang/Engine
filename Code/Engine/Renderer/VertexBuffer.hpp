#pragma once


//----------------------------------------------------------------------------------------------
struct ID3D11Buffer;


//----------------------------------------------------------------------------------------------
class VertexBuffer
{
	friend class Renderer;

public:
	VertexBuffer( unsigned int size, unsigned int stride );
	VertexBuffer( const VertexBuffer& copy ) = delete;
	virtual ~VertexBuffer();

	void Create();
	void Resize( unsigned int size );

	unsigned int GetSize();
	unsigned int GetStride();
	unsigned int GetCount();

private:
	ID3D11Buffer* m_buffer = nullptr;
	unsigned int  m_size   = 0;
	unsigned int  m_stride = 0;
};