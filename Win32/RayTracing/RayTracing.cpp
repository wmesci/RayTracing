// RayTracing.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "RayTracing.h"
#include "RayTracer.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������: 
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

 	// TODO:  �ڴ˷��ô��롣
	SetDefaultScene(&scene);
	tracer = new RayTracer(&scene);
	HBITMAP bmp = CreateBitmap(800, 600, 0, 32, 0);

	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_RAYTRACING, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RAYTRACING));

	// ����Ϣѭ��: 
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
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
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
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
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