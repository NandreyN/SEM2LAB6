#undef UNICODE
#define TIMERRED 1
#define TIMERGEEN 2
#define TIMERYELLOW 3
#include <windows.h>
#include  <math.h>
#include <cmath>
#include <string>

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
struct Circle
{
	Circle() {};
	Circle(int x, int y, int r)
	{
		center.x = x; center.y = y;
		R = r;
	};
	Circle(POINT p, int r)
	{
		center = p;
		R = r;
	}
	POINT center;
	int R;
};

BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

void DrawCircle(HDC& hdc, Circle& circle, rgb color);
void ClearCircle(HDC& hdc, Circle& circle);
bool isInCircle(Circle& circle, int x, int y);
bool PerformCircleChanging(HDC& hdc, Circle& circle, int delta, bool state, rgb& color);

static char appName[] = "Lab6";
static char title[] = "Circles";

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
	static int x, y, R;
	static bool beat[3];
	static bool currentStates[3];

	static rgb red, yellow, green;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT clientRect;
	static Circle circles[3];

	switch (message)
	{
	case WM_CREATE:
		for (int i = 0; i < 3; i++) beat[i] = false;

		red = rgb(225, 20, 20);
		yellow = rgb(218, 225, 20);
		green = rgb(68, 225, 20);
		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		R = (x <= y) ? x / 5 : y / 5;
		break;

	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		POINT center;

		center.x = 1.2 * R; center.y = y / 4;
		circles[0] = Circle(center, R);
		DrawCircle(hdc, circles[0], red);

		center.x = x - 1.2*R; center.y = y / 4;
		circles[1] = Circle(center, R);
		DrawCircle(hdc, circles[1], green);

		center.x = x / 2; center.y = y - y / 4;
		circles[2] = Circle(center, R);
		DrawCircle(hdc, circles[2], yellow);

		GetClientRect(hwnd, &clientRect);
		EndPaint(hwnd, &ps);
	}
	break;

	case WM_LBUTTONDOWN:
	{
		int cX = LOWORD(lparam), cY = HIWORD(lparam);
		for (int i = 0; i < 3; i++)
			if (isInCircle(circles[i], cX, cY))
			{
				beat[i] = true;
				SetTimer(hwnd, i, 500, NULL);
				break;
			}
		break;
	}

	case WM_RBUTTONUP:
	{
		int cX = LOWORD(lparam), cY = HIWORD(lparam);
		for (int i = 0; i < 3; i++)
			if (isInCircle(circles[i], cX, cY))
			{
				beat[i] = false;
				KillTimer(hwnd, i);
				break;
			}
		break;
	}

	case WM_TIMER:
	{
		hdc = GetDC(hwnd);
		int delta = R *0.2;

		switch (wparam)
		{
		case 0: // red
			currentStates[0] = PerformCircleChanging(hdc, circles[0], delta, currentStates[0], red);
			break;
		case 1:
			currentStates[1] = PerformCircleChanging(hdc, circles[1], delta, currentStates[1], green);
			break; // green
		case 2:
			currentStates[2] = PerformCircleChanging(hdc, circles[2], delta, currentStates[2], yellow);
			break; // yellow
		}
		ReleaseDC(hwnd, hdc);
		break;
	}

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
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
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

void DrawCircle(HDC& hdc, Circle& circle, rgb color)
{
	HPEN oldPen, newPen = CreatePen(PS_DOT, 1, RGB(255, 255, 255));
	HBRUSH old, newBrush = CreateSolidBrush(RGB(color.r, color.g, color.b));
	oldPen = (HPEN)SelectObject(hdc, newPen);
	old = (HBRUSH)SelectObject(hdc, newBrush);

	Ellipse(hdc, circle.center.x - circle.R, circle.center.y - circle.R, circle.center.x + circle.R, circle.center.y + circle.R);

	SelectObject(hdc, old);
	DeleteObject(newBrush);
	SelectObject(hdc, oldPen);
	DeleteObject(newPen);
}

void ClearCircle(HDC& hdc, Circle& circle)
{
	rgb white(255, 255, 255);
	DrawCircle(hdc, circle, white);
}

bool isInCircle(Circle& circle, int x, int y)
{
	return pow(circle.center.x - x, 2) + pow(circle.center.y - y, 2) <= pow(circle.R, 2);
}
bool PerformCircleChanging(HDC& hdc, Circle& circle, int delta, bool state, rgb& color)
{
	if (state) // big
	{
		ClearCircle(hdc, circle);
		circle.R -= delta;
		DrawCircle(hdc, circle, color);
		return false;
	}
	circle.R += delta;
	DrawCircle(hdc, circle, color);
	return true;
}