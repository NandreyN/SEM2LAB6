#undef UNICODE
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
DrawAreaInfo GetAreaInfo(int x, int y);
DrawAreaInfo Draw(HDC& hdc, int x, int y, Graph& graph);
POINT ConvertCoordinates(int x, int y, int widthOld, int heightOld);
double GetDistance(int x1, int y1, int x2, int y2);

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
	static int x, y;
	static HDC hdc;
	static DrawAreaInfo dAInfo;
	PAINTSTRUCT ps;

	RECT clientRect;
	static Graph gr;
	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);
		gr.a = 1; gr.b = 1; gr.c = 1;

		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case WM_PAINT:
	{
		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, x, y, NULL);
		SetViewportExtEx(hdc, x, -y, NULL);
		SetViewportOrgEx(hdc, x / 2, y / 2, NULL);
		dAInfo = Draw(hdc, x, y, gr);
		break;
	}

	case WM_LBUTTONDOWN:
	{
		DrawAreaInfo di = GetAreaInfo(x, y);
		int cx = LOWORD(lparam);
		int cy = HIWORD(lparam);
		POINT clickedCoord = ConvertCoordinates(cx, cy, x, y);

		//double fx = log((double)clickedCoord.x / di.divValueX );
		// double fx = 1.0/((double)clickedCoord.x / di.divValueX );
		double fx = sqrt((double)clickedCoord.x / di.divValueX);
		//double fx = (gr.a*((double)clickedCoord.x / di.divValueX)*((double)clickedCoord.x / di.divValueX) + gr.b*((double)clickedCoord.x / di.divValueX) + gr.c);

		//double newY = (double)clickedCoord.y / di.divValueY;
		double distance = abs(fx - ((double)clickedCoord.y) / di.divValueY);
		if (distance <= 0.8)
		{
			string text;
			double t = (double)clickedCoord.x / di.divValueX;
			text += "X: " + to_string(t); text += ", Y : " + to_string(fx);
			TextOut(hdc, 0, 0, text.c_str(), text.size());
		}
		break;
	}

	case WM_CLOSE:
		ReleaseDC(hwnd, hdc);
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

DrawAreaInfo GetAreaInfo(int x, int y)
{

	int xPoints, yPoints, divValueX, divValueY, a, b; // Кол-во делений
	a = -20; b = 20;

	xPoints = abs(a - b); yPoints = 14;
	divValueX = x / (xPoints * 2);
	divValueY = y / (yPoints * 2);
	int newX, newY; newX = x / 2; newY = y / 2;
	DrawAreaInfo di;

	
	di.divValueX = divValueX; di.divValueY = divValueY; di.newX = newX; di.newY = newY; di.xPoints = xPoints; di.yPoints = yPoints;
	di.a = a; di.b = b;
	return di;
}
DrawAreaInfo Draw(HDC& hdc, int x, int y, Graph& gr)
{
	DrawAreaInfo dai = GetAreaInfo(x, y);

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
	//MoveToEx(hdc, -dai.newX, 0, NULL);

	//double fx = log(dai.a / dai.divValueX);
	//double fx = 1.0/(dai.a / dai.divValueX);
	//double fx = 0;
	//double fx = sqrt(dai.a * dai.divValueX);
	double fx = exp(dai.a * dai.divValueX);
	//if (fx == fx && !isinf(fx))
		MoveToEx(hdc, dai.a * dai.divValueX, fx * dai.divValueY, NULL);
	//LineTo(hdc, xArg, fx * dai.divValueY);
	// Drawing
	for (double xArg = dai.a * dai.divValueX; xArg <= dai.b * dai.divValueX; xArg += 0.1)
	{
		fx = exp(xArg / dai.divValueX);
		//fx = sqrt(xArg / dai.divValueX);
		//fx = log(xArg / dai.divValueX);
		//fx = 1.0/(xArg / dai.divValueX);
	   //fx = gr.a * ((xArg / dai.divValueX) * (xArg / dai.divValueX)) + gr.b * (xArg / dai.divValueX) + gr.c;
		if (fx == fx)
			//LineTo(hdc, xArg, fx * dai.divValueY);
		SetPixel(hdc, xArg, fx * dai.divValueY, RGB(0,0,0));
	}

	newPen = CreatePen(PS_DOT, 1, BLACK_PEN);
	oldPen = (HPEN)SelectObject(hdc, newPen);
	MoveToEx(hdc, dai.a * dai.divValueX, dai.newY, NULL);
	LineTo(hdc, dai.a * dai.divValueX, -dai.newY);

	MoveToEx(hdc, dai.b * dai.divValueX, dai.newY, NULL);
	LineTo(hdc, dai.b * dai.divValueX, -dai.newY);
	SelectObject(hdc, oldPen);
	DeleteObject(newPen);

	return dai;
}
double GetDistance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}
