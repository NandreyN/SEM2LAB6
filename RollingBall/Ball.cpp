#undef UNICODE
#include <windows.h>
#include  <math.h>
#define BALL_TIMER 0
#define SPEED 5

using namespace std;

struct Circle
{
	Circle() { R = -1; };
	int R;
	POINT center;
};

BOOL InitWnd1(HINSTANCE hinstance);
BOOL InitInstance1(HINSTANCE hinstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
void DeleteCircle(HDC& hdc, Circle& circle);
void DrawCircle(HDC& hdc, Circle& circle, int angleDeg);
bool HandleTimer(HWND& hwnd, HDC& hdc, int x, int y, Circle& circle, bool isDirect, int speed);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevHinstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	if (!InitWnd1(hinstance))
	{
		MessageBox(NULL, "Unable to Init App", "Error", MB_OK);
		return FALSE;
	}

	if (!InitInstance1(hinstance, nCmdShow))
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

BOOL InitWnd1(HINSTANCE hinstance)
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
	wndclass.lpszClassName = "Ball";
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
	static HDC hdc;
	static Circle circle;
	static bool isDirect, moves;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		SetTimer(hwnd, BALL_TIMER, 20, (TIMERPROC)NULL);
		circle.center.x = circle.center.y = 0;
		circle.R = 30;
		isDirect = true;
		moves = true;
		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		HandleTimer(hwnd, hdc, x, y, circle, isDirect, SPEED);
		EndPaint(hwnd, &ps);
		break;
	case WM_TIMER:
		hdc = GetDC(hwnd);
		switch (wparam)
		{
		case BALL_TIMER:
			if (!moves) return FALSE;
			isDirect = HandleTimer(hwnd, hdc, x, y, circle, isDirect, SPEED);
			ValidateRect(hwnd, NULL);
			break;
		default:break;
		}
		ReleaseDC(hwnd, hdc);
		break;
	case WM_CLOSE:
		KillTimer(hwnd, BALL_TIMER);
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_RBUTTONUP:
		moves = !moves;
		break;
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return FALSE;
}

void DeleteCircle(HDC& hdc, Circle& circle)
{
	SetROP2(hdc, R2_WHITE);
	DrawCircle(hdc, circle, 0);
	SetROP2(hdc, R2_COPYPEN);
}

void DrawCircle(HDC& hdc, Circle& circle, int angleDeg = 0)
{

	HBRUSH newBrush, oldBrush;
	newBrush = CreateSolidBrush(RGB(255, 255, 255));
	oldBrush = (HBRUSH)SelectObject(hdc, newBrush);

	int x = circle.center.x, y = circle.center.y, r = circle.R;
	Ellipse(hdc, x - r, y - r, x + r, y + r);
	int x0, y0;
	x0 = r * cos((angleDeg * 3.1415) / 180);
	y0 = r * sin((angleDeg * 3.1415) / 180);
	MoveToEx(hdc, x, y, NULL);
	LineTo(hdc, x + x0, y + y0);

	SelectObject(hdc, oldBrush);
	DeleteObject(newBrush);
}

bool HandleTimer(HWND& hwnd, HDC& hdc, int x, int y, Circle& circle, bool isDirect, int speed)
{
	static double angle = 0;
	DeleteCircle(hdc, circle);

	int perX = x / 6.28, perY = y / 2;
	circle.center.x += (isDirect) ? speed : -speed;

	double val = sin((double)circle.center.x / perX)  * perY;
	circle.center.y = y / 2 + val;
	DrawCircle(hdc, circle, angle);
	angle += (isDirect) ? speed * 2 : -speed * 2;

	if ((x - speed / 2 <= circle.center.x && circle.center.x <= x + speed / 2) || (circle.center.x >= -speed / 2 && circle.center.x <= speed / 2))
		return !isDirect;
	return isDirect;
}

BOOL InitInstance1(HINSTANCE hinstance, int nCmdShow)
{
	HWND hwnd;
	hwnd = CreateWindow(
		"Ball",
		"Ball",
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