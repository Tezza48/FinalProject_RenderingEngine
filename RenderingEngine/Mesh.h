#pragma once

#include <D3D11.h>
#include "Vertex.h"
#include "utils.h"



class Mesh
{
protected:

	ID3D11Buffer *pVBuffer;
	ID3D11Buffer *pIBuffer;

public:
	Mesh();
	Mesh(VERTEX *vertices, int *indices,
		ID3D11Device *device, ID3D11DeviceContext *context);
	~Mesh();

	Mesh operator= (const Mesh &rhs) const;

	ID3D11Buffer *GetVBuffer() const;
	ID3D11Buffer *GetIBuffer() const;
};

