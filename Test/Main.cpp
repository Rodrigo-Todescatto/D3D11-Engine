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
#include "Variables.h"

bool InitializeDirect3dApp(HINSTANCE hInstance);
void CleanUp();
bool InitScene();
void UpdateScene();
void DrawScene();

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
    CleanUp();

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
    hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
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

    D3D11_TEXTURE2D_DESC depthStencilDesc;

    depthStencilDesc.Width = Width;
    depthStencilDesc.Height = Height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
    d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

    d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    return true;
}

void CleanUp()
{
    SwapChain->Release();
    d3d11Device->Release();
    d3d11DevCon->Release();
    renderTargetView->Release();
    squareVertBuffer->Release();
    squareIndexBuffer->Release();
    VS->Release();
    PS->Release();
    VS_Buffer->Release();
    PS_Buffer->Release();
    vertLayout->Release();
    depthStencilView->Release();
    depthStencilBuffer->Release();
    cbPerObjectBuffer->Release();
    WireFrame->Release();
}

bool InitScene()
{
    hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0",
        flags, 0, &vs_blob_ptr, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            OutputDebugStringA((char*)error_blob->GetBufferPointer());
            error_blob->Release();
        }
        if (vs_blob_ptr) { vs_blob_ptr->Release(); }
    }

    hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0",
        flags, 0, &ps_blob_ptr, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            OutputDebugStringA((char*)error_blob->GetBufferPointer());
            error_blob->Release();
        }
        if (ps_blob_ptr) { ps_blob_ptr->Release(); }
    }

    hr = d3d11Device->CreateVertexShader(
        vs_blob_ptr->GetBufferPointer(),
        vs_blob_ptr->GetBufferSize(),
        NULL,
        &VS);

    hr = d3d11Device->CreatePixelShader(
        ps_blob_ptr->GetBufferPointer(),
        ps_blob_ptr->GetBufferSize(),
        NULL,
        &PS);

    d3d11DevCon->VSSetShader(VS, NULL, 0);
    d3d11DevCon->PSSetShader(PS, NULL, 0);

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData;

    iinitData.pSysMem = indices;
    d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);
    d3d11DevCon->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = v;
    hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertBuffer);
    d3d11DevCon->IASetVertexBuffers(0, 1, &squareVertBuffer, &vertex_stride, &vertex_offset);

    hr = d3d11Device->CreateInputLayout(inputElementDesc, numElements, vs_blob_ptr->GetBufferPointer(),
        vs_blob_ptr->GetBufferSize(), &input_layout_ptr);
    d3d11DevCon->IASetInputLayout(vertLayout);

    d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = Width;
    viewport.Height = Height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    d3d11DevCon->RSSetViewports(1, &viewport);

    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbPerObject);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;
    cbbd.MiscFlags = 0;

    hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

    camPosition = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f);
    camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
    camProjection = XMMatrixPerspectiveFovLH(0.4f * 3.14f, (float)Width / Height, 1.0f, 1000.0f);

    D3D11_RASTERIZER_DESC wfdesc;
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    wfdesc.CullMode = D3D11_CULL_NONE;
    hr = d3d11Device->CreateRasterizerState(&wfdesc, &WireFrame);

    d3d11DevCon->RSSetState(WireFrame);

    return true;
}

void UpdateScene()
{
    rot += .0005f;
    if (rot > 6.28f)
        rot = 0.0f;

    cube = XMMatrixIdentity();

    XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    Rotation = XMMatrixRotationAxis(rotaxis, rot);
    Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);

    cube = Translation * Rotation;
}

void DrawScene()
{
    float background_colour[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

    d3d11DevCon->ClearRenderTargetView(renderTargetView, background_colour);

    d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    WVP = cube * camView * camProjection;
    cbPerObj.WVP = XMMatrixTranspose(WVP);
    d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
    d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

    d3d11DevCon->DrawIndexed(index_count, 0, 0);

    SwapChain->Present(0, 0);
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
            InitScene();
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