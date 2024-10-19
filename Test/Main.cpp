#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <windows.h>
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <directxmath.h>
#include <d3dcompiler.h>

using namespace DirectX;

UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

LPCTSTR WndClassName = L"D3D11 Engine";
HWND hwnd = NULL;
LPCWSTR windowName = L"D3D11 Engine";
LPCWSTR Exit = L"Are you sure you want to exit? Any unsaved progress will be lost!";
LPCWSTR Exit2 = L"Exit";

struct Vertex
{
    Vertex() {}
    Vertex(float x, float y, float z)
        : pos(x, y, z) {}

    XMFLOAT3 pos;
};

IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;



ID3D11Buffer* triangleVertBuffer;
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3DBlob* VS_Buffer;
ID3DBlob* PS_Buffer;
ID3D11InputLayout* vertLayout;
ID3DBlob* errorBuffer = NULL;

float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;
HRESULT hr;

bool InitializeDirect3dApp(HINSTANCE hInstance);
void ReleaseObjects();
bool InitScene();
void UpdateScene();
void DrawScene();

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
    if (!InitScene()) {
        MessageBox(0, L"An error has occurred during scene initialization!", L"Error", MB_OK);
        return 0;
    }

    InitializeDirect3dApp(hInstance);
    InitScene();
    messageloop();
    ReleaseObjects();

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

    d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, nullptr);

    return true;
}

//Prevents a memory leak
void ReleaseObjects()
{
    SwapChain->Release();
    d3d11Device->Release();
    d3d11DevCon->Release();
    renderTargetView->Release();
    triangleVertBuffer->Release();
    VS->Release();
    PS->Release();
    VS_Buffer->Release();
    PS_Buffer->Release();
    vertLayout->Release();
}

//Initialize the scene
bool InitScene()
{
    DWORD createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    UINT numElements = ARRAYSIZE(layout);

    hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", flags, 0, &VS_Buffer, &errorBuffer);

    hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "main", "ps_5_0", flags, 0, &PS_Buffer, &errorBuffer);

    hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
    if (FAILED(hr)) {
        if (errorBuffer) {
            MessageBox(NULL, (LPCWSTR)errorBuffer->GetBufferPointer(), TEXT("Shader Compilation Error"), MB_OK);
            errorBuffer->Release();
        }
        return false;
    }

    hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
    if (FAILED(hr)) {
        if (errorBuffer) {
            MessageBox(NULL, (LPCWSTR)errorBuffer->GetBufferPointer(), TEXT("Pixel Compilation Error"), MB_OK);
            errorBuffer->Release();
        }
        return false;
    }

    d3d11DevCon->VSSetShader(VS, 0, 0);
    d3d11DevCon->PSSetShader(PS, 0, 0);

    Vertex v[] = {
    Vertex(0.0f, 0.5f, 0.5f),
    Vertex(0.5f, -0.5f, 0.5f),
    Vertex(-0.5f, -0.5f, 0.5f)
    };

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 3;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = v;

    hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &triangleVertBuffer);
    if (FAILED(hr)) {
        MessageBox(NULL, L"An error has occured! Error code: 7", TEXT("Error"), MB_OK);
        return false;
    }

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    d3d11DevCon->IASetVertexBuffers(0, 1, &triangleVertBuffer, &stride, &offset);

    hr = d3d11Device->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
        VS_Buffer->GetBufferSize(), &vertLayout);
    if (FAILED(hr)) {
        MessageBox(NULL, L"An error has ocurred during creating the input layout!", TEXT("Error"), MB_OK);
        return false;
    }

    d3d11DevCon->IASetInputLayout(vertLayout);

    d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(Width);
    viewport.Height = static_cast<float>(Height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    d3d11DevCon->RSSetViewports(1, &viewport);

    return true;
}

//Updates scene
void UpdateScene()
{
    red += colormodr * 0.00005f;
    green += colormodg * 0.00002f;
    blue += colormodb * 0.00001f;

    if (red >= 1.0f || red <= 0.0f)
        colormodr *= -1;
    if (green >= 1.0f || green <= 0.0f)
        colormodg *= -1;
    if (blue >= 1.0f || blue <= 0.0f)
        colormodb *= -1;
}

//Draws scene
void DrawScene()
{
    float bgColor[4] = { red, green, blue, 1.0f };

    d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);

    d3d11DevCon->Draw(3, 0);

    SwapChain->Present(0, 0);
}

int messageloop() {
    MSG msg = { 0 };
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            UpdateScene();  
            DrawScene();    
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