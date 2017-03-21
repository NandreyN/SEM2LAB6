#undef UNICODE

#define EXP(x) exp(x)
#define LN(x) log(x)
#define SQRT(x) sqrt(x)
#define XSINX(x) (double)x*sin(x)
#define CUSTOM(x) (double)1/x

#define EXP_ID 0
#define LN_ID 1
#define SQRT_ID 2
#define XSINX_ID 3
#define CUSTOM_ID 4

#include <windows.h>
#include  <math.h>
#include <cmath>
#include <string>
using namespace std;

struct Graph
{
	int a;
	int b;
	int c;
};
struct DrawAreaInfo
{
	int xPoints, yPoints, divValueX, divValueY, newX, newY, a, b;
};
BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
DrawAreaInfo GetAreaInfo(int x, int y, int a, int b);
DrawAreaInfo Draw(HDC& hdc, int x, int y, Graph& graph , int funcID, int a, int b);
POINT ConvertCoordinates(int x, int y, int widthOld, int heightOld);
double GetDistance(int x1, int y1, int x2, int y2);
double CALLFUNC(int id, double x);

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
	wndclass.lpszClassName = "Graph";
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
	static int x, y,a,b;
	static HDC hdc, buffer;
	static DrawAreaInfo dAInfo;
	HBITMAP hbmp;

	PAINTSTRUCT ps;
	static int funcID;

	RECT clientRect;
	static Graph gr;
	switch (message)
	{
	case WM_CREATE:
		gr.a = 1; gr.b = 1; gr.c = 1;
		funcID = XSINX_ID;
		a = -50; b = 30;

		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case WM_PAINT:
	{
		RECT Rect; GetClientRect(hwnd, &Rect);

		hdc = BeginPaint(hwnd, &ps);
		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, x, y, NULL);
		SetViewportExtEx(hdc, x, -y, NULL);
		SetViewportOrgEx(hdc, x / 2, y / 2, NULL);

		{/*buffer = CreateCompatibleDC(hdc);
		hbmp = CreateCompatibleBitmap(hdc, Rect.right - Rect.left, Rect.bottom - Rect.top);
		SelectObject(buffer, hbmp);


		LOGBRUSH br;
		br.lbStyle = BS_SOLID;
		br.lbColor = RGB(255,255,255);
		HBRUSH brush;
		brush = CreateBrushIndirect(&br);
		FillRect(buffer, &Rect, brush);
		DeleteObject(brush);

		dAInfo = Draw(buffer, x, y, gr, funcID);
		BitBlt(hdc, -x/2, y/2,x*2, y*2, buffer, -x/2, y/2, SRCCOPY);

		EndPaint(hwnd, &ps);

		DeleteObject(buffer);
		DeleteObject(hbmp);

		//ReleaseDC(hwnd, buffer);*/}
		dAInfo = Draw(hdc, x, y, gr, funcID, a ,b);
		EndPaint(hwnd, &ps);
		break;
	}

	case WM_LBUTTONDOWN:
	{
		DrawAreaInfo di = GetAreaInfo(x, y, a,b);
		int cx = LOWORD(lparam);
		int cy = HIWORD(lparam);
		POINT clickedCoord = ConvertCoordinates(cx, cy, x, y);

		double fx = CALLFUNC(funcID, (double)clickedCoord.x / di.divValueX);
		
		double distance = abs(fx - ((double)clickedCoord.y) / di.divValueY);
		if (distance <= 0.3)
		{
			string text;
			double t = (double)clickedCoord.x / di.divValueX;
			text += "X: " + to_string(t); text += ", Y : " + to_string(fx);
			hdc = GetDC(hwnd);
			TextOut(hdc, 0, 0, text.c_str(), text.size());
			ReleaseDC(hwnd, hdc);
		}
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
		"Graph",
		"Graph",
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
POINT ConvertCoordinates(int x, int y, int widthOld, int heightOld)
{
	// center is (x/2, y/2)
	POINT pt;
	pt.x = x - widthOld / 2;
	pt.y = heightOld / 2 - y;
	return pt;
}

DrawAreaInfo GetAreaInfo(int x, int y, int a, int b)
{
	int xPoints, yPoints, divValueX, divValueY; // Кол-во делений

	xPoints = abs(b - a); yPoints = xPoints / 2;
	divValueX = x / (xPoints * 2);
	divValueY = y / (yPoints * 2);
	int newX, newY; newX = x / 2; newY = y / 2;
	DrawAreaInfo di;


	di.divValueX = divValueX; di.divValueY = divValueY; di.newX = newX; di.newY = newY; di.xPoints = xPoints; di.yPoints = yPoints;
	return di;
}
DrawAreaInfo Draw(HDC& hdc, int x, int y, Graph& gr, int ID, int a , int b)
{
	DrawAreaInfo dai = GetAreaInfo(x, y, a,b);

	HPEN newPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hdc, newPen);

	MoveToEx(hdc, -dai.newX, 0, NULL);
	LineTo(hdc, dai.newX, 0);
	MoveToEx(hdc, 0, dai.newY, NULL);
	LineTo(hdc, 0, -dai.newY);


	DeleteObject(newPen);
	SelectObject(hdc, oldPen);

	for (int xx = 0; xx <= dai.newX; xx += dai.divValueX)
	{
		MoveToEx(hdc, xx, 5, NULL);
		LineTo(hdc, xx, -5);
		MoveToEx(hdc, -xx, 5, NULL);
		LineTo(hdc, -xx, -5);
	}

	for (int yy = 0; yy <= dai.newY; yy += dai.divValueY)
	{
		MoveToEx(hdc, 5, yy, NULL);
		LineTo(hdc, -5, yy);
		MoveToEx(hdc, 5, -yy, NULL);
		LineTo(hdc, -5, -yy);
	}
	MoveToEx(hdc, dai.a, 0, NULL);

	double fx = CALLFUNC(ID, a * dai.divValueX);
	
	MoveToEx(hdc, a * dai.divValueX, fx * dai.divValueY, NULL);

	for (double xArg = a * dai.divValueX; xArg <= b * dai.divValueX; xArg += 0.05)
	{
		fx = CALLFUNC(ID, xArg / dai.divValueX);
		if (fx == fx)
			SetPixel(hdc, xArg, fx * dai.divValueY, RGB(0, 0, 0));
	}

	newPen = CreatePen(PS_DOT, 1, BLACK_PEN);
	oldPen = (HPEN)SelectObject(hdc, newPen);
	MoveToEx(hdc, a * dai.divValueX, dai.newY, NULL);
	LineTo(hdc, a * dai.divValueX, -dai.newY);

	MoveToEx(hdc, b * dai.divValueX, dai.newY, NULL);
	LineTo(hdc, b * dai.divValueX, -dai.newY);
	SelectObject(hdc, oldPen);
	DeleteObject(newPen);

	return dai;
}
double GetDistance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}

double CALLFUNC(int id , double x)
{
	double val;
	switch (id)
	{
	case EXP_ID:
		return EXP(x);
	case LN_ID:
		return LN(x);
	case SQRT_ID:
		return SQRT(x);
	case XSINX_ID:
		return XSINX(x);
	case CUSTOM_ID:
		return CUSTOM(x);
	default: return 0.0;
	}
}