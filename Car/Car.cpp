#undef UNICODE
#define CARTIMERFORWARD 0
#define CARTIMERREVERSE 1


#include <windows.h>
#include  <math.h>
#include <cmath>
#include <string>

using namespace std;

BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
void DrawCar(HDC& hdc, RECT& area, int angleDeg);
void Move(HDC& hdc, int x, int y, RECT& area);

static char appName[] = "Lab6";
static char title[] = "TrafficLight";
static double SPEED  = 0.5;
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
	static int x, y,x0,y0, angle;
	static HDC hdc;
	PAINTSTRUCT ps;
	static RECT prev, area;
	static int counter;
	switch (message)
	{
	case WM_CREATE:
		SetTimer(hwnd, CARTIMERFORWARD, 100, NULL);
		prev = area;
		angle = x0 = y0 = 0;
		counter = 0;
		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;

	case WM_PAINT:
	{

		hdc = BeginPaint(hwnd, &ps);
		SetRect(&area, x0 ,y/2, x0 +  200, y);

		HBRUSH newBrush, oldBrush;
		if(abs (x - area.right) <= 100 || area.left <= 100 || abs(x - area.left) <= 100)
			counter = 180;

		newBrush = CreateSolidBrush(RGB(counter, counter, counter));
		oldBrush = (HBRUSH)SelectObject(hdc, newBrush);

		Move(hdc, x, y, area);
		

		SelectObject(hdc, oldBrush);
		DeleteObject(newBrush);

		EndPaint(hwnd, &ps);	
		
	}
	break;
	case WM_TIMER:
		switch(wparam)
		{
		case CARTIMERFORWARD:
			x0 += SPEED;
			if (area.left >= x || area.right <= 0)
			{
				SPEED = -SPEED;
			}
			SPEED += (x0 <= x / 2) ? 0.5 : -0.5;
			counter = 255;
			InvalidateRect(hwnd, NULL, true);
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		KillTimer(hwnd, CARTIMERFORWARD);
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
		0,
		0,
		1400,
		200,
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

void DrawCar(HDC& hdc, RECT& area, int angleDeg)
{
	int x = abs(area.left - area.right);
	int y = abs(area.top - area.bottom);
	int wheelR = y / 6;
	POINT wheel1, wheel2;
	wheel1.x = area.left + x / 3; wheel1.y = area.top + y - wheelR;
	wheel2.x =area.left+ x - x / 3; wheel2.y = area.top + y - wheelR;

	Ellipse(hdc, wheel1.x - wheelR, wheel1.y - wheelR, wheel1.x + wheelR, wheel1.y + wheelR);
	Ellipse(hdc, wheel2.x - wheelR, wheel2.y - wheelR, wheel2.x + wheelR, wheel2.y + wheelR);
	
	RECT body;
	SetRect(&body, area.left + 0.2*x, area.top + 0.5*y, area.right - 0.15*x, area.bottom - wheelR);
	Rectangle(hdc, body.left, body.top, body.right, body.bottom);
	
	MoveToEx(hdc, body.left, body.top, NULL);
	LineTo(hdc, body.left, body.top - abs(body.bottom - body.top )/ 2);
	LineTo(hdc, body.right, body.bottom - abs(body.bottom - body.top));
	SetPixel(hdc, body.right, body.bottom, RGB(0, 0, 0));

	// Draw lines at wheels
	angleDeg %= 360;
	if (angleDeg / 180 >= 1 || angleDeg < 0) return;
	double x0, y0; x0 = wheelR * cos(3.1415 * angleDeg / 180); y0 = wheelR * sin(3.1415 * angleDeg / 180);
	MoveToEx(hdc, wheel1.x, wheel1.y, NULL);
	LineTo(hdc, wheel1.x + x0, wheel1.y + y0);

	MoveToEx(hdc, wheel2.x, wheel2.y, NULL);
	LineTo(hdc, wheel2.x + x0, wheel2.y + y0);
}

void Move(HDC& hdc, int x, int y, RECT& area)
{
	static int prevAngle;
	prevAngle += SPEED * 2;
	DrawCar(hdc, area, prevAngle);
	area.left += SPEED; area.right += SPEED;
}