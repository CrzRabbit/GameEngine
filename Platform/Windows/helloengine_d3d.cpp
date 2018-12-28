#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdint.h>

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

const uint32_t SCREEN_WIDTH = 960;
const uint32_t SCREEN_HEIGHT = 480;

// global decarations
IDXGISwapChain* g_pSwapchain = nullptr;		//swap chain interface
ID3D11Device* g_pDev = nullptr;				// Direct3D device interface
ID3D11DeviceContext* g_pDevcon = nullptr;	// Direct3D device context

ID3D11RenderTargetView* g_pRTView = nullptr;

ID3D11InputLayout* g_pLayout = nullptr;		// input layout
ID3D11VertexShader* g_pVS = nullptr;		// vertex shader
ID3D11PixelShader* g_pPS = nullptr;			// pixel shader

ID3D11Buffer * g_pVBuffer = nullptr;		// vertex buffer

// vertex buffer structer
struct VERTEX {
	XMFLOAT3 Position;
	XMFLOAT4 Color;
};

template <class T>
inline void SafeRelease(T** ppInterfaceToRelease)
{
	if ((*ppInterfaceToRelease) != nullptr)
	{
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = nullptr;
	}
}

void CreateRenderTarget()
{
	HRESULT hr;
	ID3D11Texture2D* pBackBuffer;

	g_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBuffer);

	g_pDev->CreateRenderTargetView(pBackBuffer, NULL, &g_pRTView);
	pBackBuffer->Release();

	g_pDevcon->OMSetRenderTargets(1, &g_pRTView, NULL);
}

void SetViewPort()
{
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	g_pDevcon->RSSetViewports(1, &viewport);
}

// loads and prepares the shaders
void InitPipiline()
{
	// load and compile the two shader
	ID3DBlob *VS, *PS;

	D3DReadFileToBlob(L"copy.vso", &VS);
	D3DReadFileToBlob(L"copy.pso", &PS);

	// encapsulate both shaders into shader objects
	g_pDev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVS);
	g_pDev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPS);

	// set shader objects
	g_pDevcon->VSSetShader(g_pVS, 0, 0);
	g_pDevcon->PSSetShader(g_pPS, 0, 0);

	// create input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	g_pDev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &g_pLayout);
	g_pDevcon->IASetInputLayout(g_pLayout);

	VS->Release();
	PS->Release();
}

// crate the shape to render
void InitGraphics()
{
	// create triangle
	VERTEX OurVertex[] = {
		{ XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(0.45f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(-0.45f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
	};

	// create vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;		// write access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * 3;	//
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// a vertex buffer
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE; // CPU can write buffer

	g_pDev->CreateBuffer(&bd, NULL, &g_pVBuffer);

	// copy vertex to buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	g_pDevcon->Map(g_pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, OurVertex, sizeof(VERTEX) * 3);
	g_pDevcon->Unmap(g_pVBuffer, NULL);
}

HRESULT CreateGraphicsResources(HWND hWnd)
{
	HRESULT hr = S_OK;
	if (g_pSwapchain == nullptr)
	{
		// info of swap chain
		DXGI_SWAP_CHAIN_DESC scd;
		
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

		// fill swap chain
		scd.BufferCount = 1;
		scd.BufferDesc.Width = SCREEN_WIDTH;
		scd.BufferDesc.Height = SCREEN_HEIGHT;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 32-bit color
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// how to use swap chain
		scd.OutputWindow = hWnd;
		scd.SampleDesc.Count = 4;							// how many multisamples
		scd.Windowed = TRUE;								// windowed/full-screen mode
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// allow full-screen

		const D3D_FEATURE_LEVEL FeatureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		D3D_FEATURE_LEVEL FeatureLevelSupported;

		HRESULT hr = S_OK;
		
		// create device��device contex and swap chain with info in scd
		hr = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			0,
			FeatureLevels,
			_countof(FeatureLevels),
			D3D11_SDK_VERSION,
			&scd,
			&g_pSwapchain,
			&g_pDev,
			&FeatureLevelSupported,
			&g_pDevcon
			);
		if (hr == E_INVALIDARG)
		{
			hr = D3D11CreateDeviceAndSwapChain(NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				0,
				&FeatureLevelSupported,
				1,
				D3D11_SDK_VERSION,
				&scd,
				&g_pSwapchain,
				&g_pDev,
				NULL,
				&g_pDevcon
				);
		}

		if (hr == S_OK)
		{
			CreateRenderTarget();
			SetViewPort();
			InitPipiline();
			InitGraphics();
		}
	}
	return hr;
}

void DiscardGraphicsResources()
{
	SafeRelease(&g_pLayout);
	SafeRelease(&g_pVS);
	SafeRelease(&g_pPS);
	SafeRelease(&g_pVBuffer);
	SafeRelease(&g_pSwapchain);
	SafeRelease(&g_pRTView);
	SafeRelease(&g_pDev);
	SafeRelease(&g_pDevcon);
}
// rendre single frame
void RenderFrame()
{
	const FLOAT clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
	g_pDevcon->ClearRenderTargetView(g_pRTView, clearColor);

	// rendering on the back buffer
	{
		// select vertex to display
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		g_pDevcon->IASetVertexBuffers(0, 1, &g_pVBuffer, &stride, &offset);
		
		// select primtive type
		g_pDevcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// draw the vertex buffer to the back buffer
		g_pDevcon->Draw(3, 0);
	}
	// swap the back buffer and the front buffer
	g_pSwapchain->Present(0, 0);
}

LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine,
	int nCmdShow)
{
	// handle of the window
	HWND hWnd;

	// struct of window class information
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = _T("WindowClass1");

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(0,
		_T("WindowClass1"),				// name
		_T("Hello, engine[Direct 3D]!"),// title
		WS_OVERLAPPEDWINDOW,			// style
		100,							// x-position
		100,							// y-position
		SCREEN_WIDTH,					// width
		SCREEN_HEIGHT,					// height
		NULL,							// parent window
		NULL,							// menus
		hInstance,						// application handle
		NULL);							// mutiple windows

	ShowWindow(hWnd, nCmdShow);

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);

		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	bool wasHandled = false;
	switch (message)
	{
	case WM_CREATE:
	wasHandled = true;
	break;
	case WM_PAINT:
	{
		result = CreateGraphicsResources(hWnd);
		RenderFrame();
	}
	wasHandled = true;
	break;
	case WM_SIZE:
	{
		if (g_pSwapchain != nullptr)
		{
			DiscardGraphicsResources();
		}
	}
	wasHandled = true;
	break;
	case WM_DESTROY:
	{
		DiscardGraphicsResources();
		PostQuitMessage(0);
	}
	wasHandled = true;
	break;
	case WM_DISPLAYCHANGE:
	{
		InvalidateRect(hWnd, nullptr, false);
	}
	wasHandled = true;
	break;
	}

	if (!wasHandled)
	{
		result = DefWindowProc(hWnd, message, wParam, lParam);
	}
	return result;
}
