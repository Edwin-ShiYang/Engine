#pragma once


//----------------------------------------------------------------------------------------------
struct ID3D11Buffer;


//----------------------------------------------------------------------------------------------
class IndexBuffer
{
	friend class Renderer;

public:
	IndexBuffer( unsigned int size, unsigned int stride );
	IndexBuffer( const IndexBuffer& copy ) = delete;
	virtual ~IndexBuffer();

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