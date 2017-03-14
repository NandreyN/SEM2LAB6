#undef UNICODE
#include <windows.h>
#include  <math.h>
#include <cmath>
using namespace std;

struct rgb
{
	rgb() {};
	rgb(int rcl, int gcl, int bcl)
	{
		r = rcl; g = gcl; b = bcl;
	}
	unsigned char r, g, b;
};

BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

void FillBackground(HDC& hdc, RECT& area);
void DrawCircle(HDC& hdc, POINT center, int R,rgb color); 

static char appName[] = "Lab6";
static char title[] = "TrafficLight";

HINSTANCE h;
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevHinstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	if (!InitApplication(hinstance))
	{
		MessageBox(NULL, "Unable to Init App", "Error", MB_OK);
		return FALSE;
	}

	if (!InitInstance(hinstance, nCmdShow))
	{
		MessageBox(NULL, "Unable to Init Instance", "Error", MB_OK);
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

BOOL InitApplication(HINSTANCE hinstance)
{
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hinstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = appName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wndclass))
	{
		MessageBox(NULL, "Cannot register class", "Error", MB_OK);
		return FALSE;
	}
	return TRUE;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static int x, y;
	static bool isInProgress, isRed;

	int R = y / 7;
	static rgb red, yellow, green, disabledRed, disabledYellow, disabledGreen;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT clientRect;
	static POINT centres[3];

	switch (message)
	{
	case WM_CREATE:
		isInProgress = false;
		isRed = true;

		red = rgb(225, 20, 20);
		disabledRed = rgb(240,161,146);

		yellow = rgb(218, 225, 20);
		disabledYellow = rgb(233,239,150);

		green = rgb(68, 225, 20);
		disabledGreen = rgb(186,245,169);
		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case WM_CHAR:
	{
		hdc = GetDC(hwnd);
		
		switch(wparam)
		{
		case '+':
			if (isInProgress ||  !isRed) break;

			isInProgress = true;
			Sleep(1000);
			DrawCircle(hdc, centres[1], R, yellow);
			Sleep(1000);
			DrawCircle(hdc, centres[0], R, disabledRed);
			DrawCircle(hdc, centres[1], R, disabledYellow);
			DrawCircle(hdc, centres[2], R, green);
			isInProgress = false;
			isRed = false;

			break;
		case '-':
			if (isInProgress || isRed) break;
			isInProgress = true;

			for (int i = 0; i < 3; i++)
			{
				Sleep(800);
				DrawCircle(hdc, centres[2], R, green);
				Sleep(800);
				DrawCircle(hdc, centres[2], R, disabledGreen);
			}
			DrawCircle(hdc, centres[1], R, yellow);
			Sleep(1000);
			DrawCircle(hdc, centres[1], R, disabledYellow);
			
			DrawCircle(hdc, centres[0], R, red);
			isInProgress = false;
			isRed = true;
			break;
		}
		ReleaseDC(hwnd, hdc);
		ValidateRect(hwnd, NULL);
		break;
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &clientRect);
		FillBackground(hdc, clientRect);

		POINT circleCenter; circleCenter.x = x / 2;
		circleCenter.y = y / 6;
		centres[0] = circleCenter;
		DrawCircle(hdc, circleCenter, R, red);

		circleCenter.y = y / 2;
		centres[1] = circleCenter;
		DrawCircle(hdc, circleCenter, R, disabledYellow);

		circleCenter.y = y - y / 6;
		centres[2] = circleCenter;
		DrawCircle(hdc, circleCenter, R, disabledGreen);

		EndPaint(hwnd, &ps);
	}
		break;
	
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return FALSE;
}

BOOL InitInstance(HINSTANCE hinstance, int nCmdShow)
{
	HWND hwnd;
	hwnd = CreateWindow(
		appName,         
		title,           
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		CW_USEDEFAULT,      
		0,       
		350,
		700,       
		NULL,    
		NULL,    
		hinstance,
		NULL);


	if (!hwnd)
		return FALSE;
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return TRUE;
}
void FillBackground(HDC& hdc, RECT& area)
{
	HBRUSH old, newBrush = CreateSolidBrush(RGB(198, 198, 198)); // Gray
	old = (HBRUSH)SelectObject(hdc, newBrush);

	FillRect(hdc, &area, newBrush);

	SelectObject(hdc, old);
	DeleteObject(newBrush);
}

void DrawCircle(HDC& hdc, POINT center, int R, rgb color)
{
	HBRUSH old, newBrush = CreateSolidBrush(RGB(color.r, color.g, color.b));
	old = (HBRUSH)SelectObject(hdc, newBrush);

	Ellipse(hdc, center.x - R, center.y - R, center.x + R, center.y + R);

	SelectObject(hdc, old);
	DeleteObject(newBrush);
}