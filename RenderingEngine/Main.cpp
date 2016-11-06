#include <Windows.h>
#include <memory>
#include "D3DApplication.h"

/*
 * Might call this: Noobie
 * as i'm a noobie and noobies are not great but strive to be better
 * and that's exactly what this engine's doing.
 */

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

Vertex MakeVert(XMFLOAT3 Pos, XMFLOAT4 Color)
{
	Vertex v;
	v.Pos = Pos;
	v.Color = Color;
}

class TriangleApp : public D3DApplication
{
public:
	TriangleApp(HINSTANCE hInstance);
	TriangleApp& operator= (const TriangleApp& rhs) = delete;
	~TriangleApp();

	virtual bool Init() override;
private:
	virtual void Draw(const GameTimer &gt) override;
	
	void BuildDescriptorHeaps();
	void BuidConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildTriangleGeometry();
	void BuildPSO();

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;


};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	try
	{
		D3DApplication d3dApp = D3DApplication(hInstance);
		d3dApp.Init();

		return d3dApp.Run();
	}
	catch (DxException &e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}