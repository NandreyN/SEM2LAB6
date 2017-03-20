#undef UNICODE
#define CARTIMERFORWARD 0
#define CARTIMERREVERSE 1
#define TIMERDELAY 20
double amplitude = 0.0;

#include <windows.h>
#include  <math.h>
#include <cmath>
#include <string>
#include <vector>

using namespace std;

BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
void DrawCar(HDC& hdc, RECT& area, bool isDirect);
bool Move(HWND hwnd, HDC& hdc, int& x0, int& y0, double& angleDeg, bool isDirect);
static char appName[] = "Lab6";
static char title[] = "Car";
static double SPEED = 0.5;
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
	h = hinstance;
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
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
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
	XFORM xForm;
	static HBITMAP bmp;
	static BITMAP bitmap;
	static bool isForward, flag1, flag2;
	static int x, y, x0, y0, dX, dY, aX, bY;
	static double angle, dAng, cAng;
	static HDC hdc;
	PAINTSTRUCT ps;
	static RECT prev, area;
	static int counter;

	switch (message)
	{
	case WM_CREATE:
		SetTimer(hwnd, CARTIMERFORWARD, TIMERDELAY, NULL);
		angle = x0 = y0 = 0;
		counter = 0;
		isForward = true;
		angle = 0;
		SetRect(&area, 0, 0, 150, 75);
		dX = aX = 4;
		dY = bY = 1;
		dAng = cAng = (double)bY / 2;
		flag1 = flag2 = false;
		break;
	case WM_SIZE:
		RECT r;
		GetClientRect(hwnd, &r);
		x = r.right;
		y = r.bottom;
		x0 = 100;
		y0 = 50;
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		HBRUSH b = NULL, prevB = NULL;
		if (abs(x - x0) <= x / 5 || x0 <= x/5)
		{
			b = CreateSolidBrush(RGB(200, 50, 70));
			prevB = (HBRUSH)SelectObject(hdc, b);
		}
		Move(hwnd, hdc, x0, y0, angle, isForward);
		if (b && prevB)
		{
			SelectObject(hdc, prevB);
			DeleteObject(b);
		}
		EndPaint(hwnd, &ps);
	}
	break;
	case WM_TIMER:
		switch (wparam)
		{
		case CARTIMERFORWARD:
			if (x0 <= x / 8)
			{
				dX = aX;
				dY = 0;
				dAng = 0;
			}

			if (x0 > x / 8 && 2 * x / 8)
			{
				dX = aX;
				dY = bY;
				dAng = cAng;
			}
			if (x0 >= 2 * x / 8 && x0 <= 6 * x / 8)
			{
				dY = bY * 1.8;
				dX = aX * 1.2;
				dAng = 0;
			}
			if (x0 >= 6 * x / 8 && x0 <= 7 * x / 8)
			{
				dX = aX;
				dY = bY;
				dAng = -cAng;
			}
			if (x0 > 7 * x / 8)
			{
				dX = aX * 0.7;
				dY = 0;
				dAng = 0;
			}
			x0 += (isForward) ? dX : -dX;
			y0 += (isForward) ? dY : -dY;
			angle += (isForward) ? dAng : -dAng;

			if (x0 >= x || x0 + 200 <= 0)
			{
				isForward = !isForward;
			}

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
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		0,
		0,
		1400,
		600,
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

void DrawCar(HDC& hdc, RECT& area, bool isDirect)
{
	static int angleDeg = 0;

	int x = abs(area.left - area.right);
	int y = abs(area.top - area.bottom);
	int wheelR = y / 6;
	POINT wheel1, wheel2;
	wheel1.x = area.left + x / 3; wheel1.y = area.top + y - wheelR;
	wheel2.x = area.left + x - x / 3; wheel2.y = area.top + y - wheelR;

	Ellipse(hdc, wheel1.x - wheelR, wheel1.y - wheelR, wheel1.x + wheelR, wheel1.y + wheelR);
	Ellipse(hdc, wheel2.x - wheelR, wheel2.y - wheelR, wheel2.x + wheelR, wheel2.y + wheelR);

	RECT body;
	SetRect(&body, area.left + 0.2*x, area.top + 0.5*y, area.right - 0.15*x, area.bottom - wheelR);
	Rectangle(hdc, body.left, body.top, body.right, body.bottom);

	MoveToEx(hdc, body.left, body.top, NULL);
	LineTo(hdc, body.left, body.top - abs(body.bottom - body.top) / 2);
	LineTo(hdc, body.right, body.bottom - abs(body.bottom - body.top));
	SetPixel(hdc, body.right, body.bottom, RGB(0, 0, 0));

	// Draw lines at wheels
	double a = angleDeg % 360;
	if (a / 180 >= 1 || a < 0) { angleDeg += (isDirect) ? 10 : -10; return; }
	double x0, y0; x0 = wheelR * cos(3.1415 * a / 180); y0 = wheelR * sin(3.1415 * a / 180);
	MoveToEx(hdc, wheel1.x, wheel1.y, NULL);
	LineTo(hdc, wheel1.x + x0, wheel1.y + y0);

	MoveToEx(hdc, wheel2.x, wheel2.y, NULL);
	LineTo(hdc, wheel2.x + x0, wheel2.y + y0);
	angleDeg += (isDirect) ? 10 : -10;
	//Rectangle(hdc, area.left, area.top, area.right, area.bottom);
}

bool Move(HWND hwnd, HDC& hdc, int& x0, int& y0, double& angleDeg, bool isDirect)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	POINT centerPt;
	centerPt.x = x0;
	centerPt.y = y0;

	//
	// rotate the DC
	//
	SetGraphicsMode(hdc, GM_ADVANCED);
	XFORM xform;
	if (angleDeg != 0)
	{
		double fangle = (double)angleDeg / 180. * 3.1415926;
		xform.eM11 = (float)cos(fangle);
		xform.eM12 = (float)sin(fangle);
		xform.eM21 = (float)-sin(fangle);
		xform.eM22 = (float)cos(fangle);
		xform.eDx = (float)(centerPt.x - cos(fangle)*centerPt.x + sin(fangle)*centerPt.y);
		xform.eDy = (float)(centerPt.y - cos(fangle)*centerPt.y - sin(fangle)*centerPt.x);

		SetWorldTransform(hdc, &xform);
	}

	// draw a rectangle or ellipse
	RECT car;
	SetRect(&car, x0 - 100, y0 - 50, x0 + 100, y0 + 50);
	DrawCar(hdc, car, isDirect);
	// draw the rotated coordinate

	//InvalidateRect(hwnd, NULL, true);
	return isDirect;
}