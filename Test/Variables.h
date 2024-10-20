#pragma once
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "user32")             
#pragma comment( lib, "dxgi.lib")        
#include <windows.h>
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <dxgi.h>

using namespace DirectX;

ID3DBlob* vs_blob_ptr = NULL;
ID3DBlob* ps_blob_ptr = NULL;
ID3DBlob* error_blob = NULL;
ID3D11InputLayout* input_layout_ptr = NULL;

struct Vertex    //Overloaded Vertex Structure
{
    Vertex() {}
    Vertex(float x, float y, float z,
        float cr, float cg, float cb, float ca)
        : pos(x, y, z), color(cr, cg, cb, ca) {}

    XMFLOAT3 pos;
    XMFLOAT4 color;
};

Vertex v[] =
{
    Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
    Vertex(-1.0f, +1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f),
    Vertex(+1.0f, +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
    Vertex(+1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f),
    Vertex(-1.0f, -1.0f, +1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
    Vertex(-1.0f, +1.0f, +1.0f, 1.0f, 1.0f, 1.0f, 1.0f),
    Vertex(+1.0f, +1.0f, +1.0f, 1.0f, 0.0f, 1.0f, 1.0f),
    Vertex(+1.0f, -1.0f, +1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
};

DWORD indices[] = {
    // front face
    0, 1, 2,
    0, 2, 3,

    // back face
    4, 6, 5,
    4, 7, 6,

    // left face
    4, 5, 1,
    4, 1, 0,

    // right face
    3, 2, 6,
    3, 6, 7,

    // top face
    1, 5, 6,
    1, 6, 2,

    // bottom face
    4, 0, 3,
    4, 3, 7
};

UINT vertex_stride = sizeof(indices);
UINT vertex_offset = 0;
UINT vertex_count = 36;

UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

LPCTSTR WndClassName = L"D3D11 Engine";
HWND hwnd = NULL;
LPCWSTR windowName = L"D3D11 Engine";
LPCWSTR Exit = L"Are you sure you want to exit? Any unsaved progress will be lost!";
LPCWSTR Exit2 = L"Exit";

IDXGISwapChain* SwapChain = NULL;
ID3D11Device* d3d11Device = NULL;
ID3D11DeviceContext* d3d11DevCon = NULL;
ID3D11RenderTargetView* renderTargetView = NULL;

ID3D11Buffer* triangleVertBuffer;
ID3D11VertexShader* VS = NULL;
ID3D11PixelShader* PS = NULL;
ID3DBlob* VS_Buffer;
ID3DBlob* PS_Buffer;
ID3D11InputLayout* vertLayout;
ID3DBlob* errorBuffer = NULL;
ID3D11Buffer* squareIndexBuffer;
ID3D11Buffer* squareVertBuffer;
ID3D11Buffer* vertex_buffer_ptr = NULL;

ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;

ID3D11Buffer* cbPerObjectBuffer;

XMMATRIX WVP;
XMMATRIX World;
XMMATRIX camView;
XMMATRIX camProjection;

XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;

struct cbPerObject
{
    XMMATRIX  WVP;
};

cbPerObject cbPerObj;

XMMATRIX cube1World;
XMMATRIX cube2World;
XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;
float rot = 0.01f;

HRESULT hr;