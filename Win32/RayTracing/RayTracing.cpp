// RayTracing.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "RayTracing.h"
#include "RayTracer.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
Scene scene;
RayTracer* tracer = nullptr;


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  在此放置代码。
	SetDefaultScene(&scene);
	tracer = new RayTracer(&scene);
	HBITMAP bmp = CreateBitmap(800, 600, 0, 32, 0);

	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_RAYTRACING, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RAYTRACING));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RAYTRACING));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW - WS_MAXIMIZEBOX - WS_THICKFRAME,
      CW_USEDEFAULT, 0, 816, 638, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HDC mhdc = NULL;
HBITMAP bitmap = NULL;
LPVOID buffer = NULL;
float height = 4;
int n = 0;
int start = 0;
//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_UP:
			height += 0.1f;
			break;
		case VK_DOWN:
			height -= 0.1f;
			break;
		}
		break;
	case WM_ERASEBKGND:
		return 1;
	case WM_PAINT:
		{
			auto tick = GetTickCount();
			n++;
			if (tick - start > 1000)
			{
				wchar_t str[100];
				wsprintf(str, L"FPS=%d\0", n);
				SetWindowText(hWnd, str);
				n = 0;
				start = tick;
			}

			Vector pos;
			pos.X = (6 * cos(tick / 2000.0f));
			pos.Y = height;
			pos.Z = (6 * sin(tick / 2000.0f));
			scene.Camera = Camera(pos, Vector());

			hdc = BeginPaint(hWnd, &ps);

			if (bitmap == NULL)
			{
				bitmap = CreateCompatibleBitmap(hdc, 800, 600);
				mhdc = CreateCompatibleDC(hdc);
				SelectObject(mhdc, bitmap);
			}

			BITMAP bm;
			GetObject(bitmap, sizeof(bm), &bm);
			if (buffer == NULL)
				buffer = malloc(bm.bmWidthBytes * bm.bmHeight);

			tracer->Render(buffer, bm.bmWidth, bm.bmHeight, bm.bmWidthBytes, 5);

			SetBitmapBits(bitmap, bm.bmWidthBytes * bm.bmHeight, buffer);

			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, mhdc, 0, 0, SRCCOPY);

			EndPaint(hWnd, &ps);
		}
		InvalidateRect(hWnd, NULL, false);
		break;
	case WM_DESTROY:
		delete tracer;
		free(buffer);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}