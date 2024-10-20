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
#include<assert.h>

using namespace DirectX;

ID3DBlob* vs_blob_ptr = NULL;
ID3DBlob* ps_blob_ptr = NULL;
ID3DBlob* error_blob = NULL;

float vertex_data_array[] = {
0.0f,  0.5f,  0.0f,
0.5f, -0.5f,  0.0f,
-0.5f, -0.5f,  0.0f,
};
UINT vertex_stride = 3 * sizeof(float);
UINT vertex_offset = 0;
UINT vertex_count = 3;

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
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3DBlob* VS_Buffer;
ID3DBlob* PS_Buffer;
ID3D11InputLayout* vertLayout;
ID3DBlob* errorBuffer = NULL;

ID3D11Buffer* vertex_buffer_ptr = NULL;

float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;
HRESULT hr;

bool InitializeDirect3dApp(HINSTANCE hInstance);

const int Width = 1920;
const int Height = 1080;

bool InitializeWindow(HINSTANCE hInstance,
    int ShowWnd,
    int width, int height,
    bool windowed);

int messageloop();

LRESULT CALLBACK WndProc(HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam);

int WINAPI WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
    if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true))
    {
        MessageBox(0, L"An error has ocurred during window initialization!",
            L"Error", MB_OK);
        return 0;
    }

    if (!InitializeDirect3dApp(hInstance)) {
        MessageBox(0, L"An error has occurred during Direct3D initialization!", L"Error", MB_OK);
        return 0;
    }

    InitializeDirect3dApp(hInstance);
    messageloop();

    return 0;
}

bool InitializeWindow(HINSTANCE hInstance,
    int ShowWnd,
    int width, int height,
    bool windowed)
{
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);    //Size of our windows class
    wc.style = CS_HREDRAW | CS_VREDRAW;    //class styles
    wc.lpfnWndProc = WndProc;    //Default windows procedure function
    wc.cbClsExtra = NULL;    //Extra bytes after our wc structure
    wc.cbWndExtra = NULL;    //Extra bytes after our windows instance
    wc.hInstance = hInstance;    //Instance to current application
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);    //Title bar Icon
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);    //Default mouse Icon
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);    //Window bg color
    wc.lpszMenuName = NULL;    //Name of the menu attached to our window
    wc.lpszClassName = WndClassName;    //Name of our windows class
    wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); //Icon in your taskbar

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"An error has ocurred during class registration!",
        L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    hwnd = CreateWindowEx(
        NULL,
        WndClassName,
        windowName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width,
        height,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
    {

        MessageBox(NULL, L"An error has ocurred during window creation!",
        L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    ShowWindow(hwnd, ShowWnd);
    UpdateWindow(hwnd);

    return true;
}

//Initializes D3D11
bool InitializeDirect3dApp(HINSTANCE hInstance)
{
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3DCOMPILE_DEBUG; // add more debug output
#endif

    DXGI_MODE_DESC bufferDesc;

    ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

    bufferDesc.Width = Width;
    bufferDesc.Height = Height;
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc.BufferCount = 3; 
    swapChainDesc.BufferDesc.Width = Width;
    swapChainDesc.BufferDesc.Height = Height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);
    if (FAILED(hr))
    {
        MessageBox(NULL, L"An error has ocurred during D3D11 Device Creation!",
            TEXT("Error"), MB_OK);
        return 0;
    }

    ID3D11Texture2D* BackBuffer;
    SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
    if (FAILED(hr))
    {
        MessageBox(NULL, L"An error has ocurred during getting buffer!",
            TEXT("Error"), MB_OK);
        return 0;
    }

    hr = d3d11Device->CreateRenderTargetView(BackBuffer, nullptr, &renderTargetView);
    BackBuffer->Release();
    if (FAILED(hr))
    {
        MessageBox(NULL, L"An error has ocurred during creatting render target!",
            TEXT("Error"), MB_OK);
        return 0;
    }

    // COMPILE VERTEX SHADER
    HRESULT hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0",
        flags, 0, &vs_blob_ptr, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            OutputDebugStringA((char*)error_blob->GetBufferPointer());
            error_blob->Release();
        }
        if (vs_blob_ptr) { vs_blob_ptr->Release(); }
        assert(false);
    }

    // COMPILE PIXEL SHADER
    hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0",
        flags, 0, &ps_blob_ptr, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            OutputDebugStringA((char*)error_blob->GetBufferPointer());
            error_blob->Release();
        }
        if (ps_blob_ptr) { ps_blob_ptr->Release(); }
        assert(false);
    }

    hr = d3d11Device->CreateVertexShader(
        vs_blob_ptr->GetBufferPointer(),
        vs_blob_ptr->GetBufferSize(),
        NULL,
        &VS);
    assert(SUCCEEDED(hr));

    hr = d3d11Device->CreatePixelShader(
        ps_blob_ptr->GetBufferPointer(),
        ps_blob_ptr->GetBufferSize(),
        NULL,
        &PS);
    assert(SUCCEEDED(hr));

    ID3D11InputLayout* input_layout_ptr = NULL;
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
      { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      /*
      { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      */
    };
    hr = d3d11Device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vs_blob_ptr->GetBufferPointer(),
        vs_blob_ptr->GetBufferSize(), &input_layout_ptr);
    assert(SUCCEEDED(hr));

    { /*** load mesh data into vertex buffer **/
        D3D11_BUFFER_DESC vertex_buff_descr = {};
        vertex_buff_descr.ByteWidth = sizeof(vertex_data_array);
        vertex_buff_descr.Usage = D3D11_USAGE_DEFAULT;
        vertex_buff_descr.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA sr_data = { 0 };
        sr_data.pSysMem = vertex_data_array;
        HRESULT hr = d3d11Device->CreateBuffer(&vertex_buff_descr, &sr_data, &vertex_buffer_ptr);
        assert(SUCCEEDED(hr));
    }

    return true;
}

int messageloop() {
    MSG msg = { 0 };
    bool should_close = false;
    while (!should_close) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT) { break; }
        {
            float background_colour[4] = { 0x64 / 255.0f, 0x95 / 255.0f, 0xED / 255.0f, 1.0f };
            d3d11DevCon->ClearRenderTargetView(renderTargetView, background_colour);

            RECT winRect;
            GetClientRect(hwnd, &winRect);
            D3D11_VIEWPORT viewport = {
              0.0f,
              0.0f,
              (FLOAT)(winRect.right - winRect.left),
              (FLOAT)(winRect.bottom - winRect.top),
              0.0f,
              1.0f };
            d3d11DevCon->RSSetViewports(1, &viewport);

            d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, NULL);

            d3d11DevCon->IASetInputLayout(vertLayout);
            d3d11DevCon->IASetVertexBuffers(0, 1, &vertex_buffer_ptr, &vertex_stride, &vertex_offset);

            d3d11DevCon->VSSetShader(VS, NULL, 0);
            d3d11DevCon->PSSetShader(PS, NULL, 0);

            d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            d3d11DevCon->Draw(vertex_count, 0);

            SwapChain->Present(1, 0);

        }
    }
    return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {

    case WM_KEYDOWN:

        if (wParam == VK_ESCAPE) {
            if (MessageBox(0, Exit,
                Exit2, MB_YESNO | MB_ICONQUESTION) == IDYES)

                DestroyWindow(hwnd);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd,
        msg,
        wParam,
        lParam);
}