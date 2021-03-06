#undef UNICODE
#define TIMERRED 1
#define TIMERGEEN 2
#define TIMERYELLOW 3
#include <windows.h>
#include  <math.h>
#include <cmath>
#include <string>
#include <fstream>

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
bool PerformCircleChanging(Circle& circle, int delta, bool state);

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
	static int x, y, R, delta;
	static bool beat[3];
	static bool currentStates[3];
	static ofstream out;

	static rgb red, yellow, green;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT clientRect;
	static Circle circles[3];

	switch (message)
	{
	case WM_CREATE:
		for (int i = 0; i < 3; i++) beat[i] = false;
		POINT center;
		red = rgb(225, 20, 20);
		yellow = rgb(218, 225, 20);
		green = rgb(68, 225, 20);
		RECT re;

		GetClientRect(hwnd, &re);
		x = re.right;
		y = re.bottom;
		R = (x <= y) ? x / 5 : y / 5;
		delta = R *0.2;
		center.x = 1.2 * R; center.y = y / 4;
		circles[0] = Circle(center, R);

		center.x = x - 1.2*R; center.y = y / 4;
		circles[1] = Circle(center, R);

		center.x = x / 2; center.y = y - y / 4;
		circles[2] = Circle(center, R);

		out.open("log.txt");
		break;
	case WM_SIZE:
		GetClientRect(hwnd, &clientRect);
		out << "CLIENT CHANGED :" << endl << " ___________________________________________" << endl;
		x = clientRect.right;
		y = clientRect.bottom;

		R = (x <= y) ? x / 5 : y / 5;
		delta = R *0.2;
		out << "x = " << x << ", y = " << y << "New R = " << R << ", delta = " << delta<< endl << "_______________________________ " << endl;
		break;

	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);

		DrawCircle(hdc, circles[0], red);
		//out << "Red :" << circles[0].R << endl;
		DrawCircle(hdc, circles[1], green);
		//out << "Green :" << circles[1].R << endl;
		DrawCircle(hdc, circles[2], yellow);
		//out << "Yellow :" << circles[2].R << endl;

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
		switch (wparam)
		{
		case 0: // red
			currentStates[0] = PerformCircleChanging(circles[0], delta, currentStates[0]);
			break;
		case 1:
			currentStates[1] = PerformCircleChanging(circles[1], delta, currentStates[1]);
			break; // green
		case 2:
			currentStates[2] = PerformCircleChanging(circles[2], delta, currentStates[2]);
			break; // yellow
		}
		InvalidateRect(hwnd, NULL, true);
		break;
	}

	case WM_CLOSE:
		DestroyWindow(hwnd);
		out.close();
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
bool PerformCircleChanging(Circle& circle, int delta, bool state)
{
	if (delta == 0) return state;

	if (state) // big
	{
		circle.R -= delta;
		return false;
	}
	circle.R += delta;
	return true;
}