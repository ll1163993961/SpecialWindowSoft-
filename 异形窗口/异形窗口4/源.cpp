//   异形窗口2  在WM_ERASEBKGND消息中自贴图
//By MoreWindows-(http://blog.csdn.net/MoreWindows)
#include <windows.h>
const char szAppName[] = "异形窗口4";

/*
* 函数名称: GetWindowSize
* 函数功能: 得到窗口的宽高
* hwnd      窗口句柄
* pnWidth   窗口宽
* pnHeight  窗口高
*/
void GetWindowSize(HWND hwnd, int *pnWidth, int *pnHeight);


/*
* 函数名称: InitBitmapWindow
* 函数功能: 位图窗口初始化
* hinstance 进程实例
* nWidth    窗口宽
* nHeight   窗口高
* nCmdshow  显示方式-与ShowWindow函数的第二个参数相同
*/
BOOL InitBitmapWindow(HINSTANCE hinstance, int nWidth, int nHeight, int nCmdshow);

// 位图窗口消息处理函数
LRESULT CALLBACK BitmapWindowWndPrco(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParm);


HBITMAP  g_hBitmap;
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	//先创建一个无背影画刷窗口，
	//然后在WM_CREATE中并指定透明颜色, 缩放位图后加载至s_hdcMem中.
	//最后在WM_ERASEBKGND中用s_hdcMem贴图即可
	g_hBitmap = (HBITMAP)LoadImage(NULL, "005.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (g_hBitmap == NULL)
	{
		MessageBox(NULL, "位图加载失败", "Error", MB_ICONERROR);
		return 0;
	}

	// 设置异形窗口大小
	BITMAP bm;
	GetObject(g_hBitmap, sizeof(bm), &bm);
	int nWindowWidth = bm.bmWidth;
	int nWindowHeight = bm.bmHeight + 100; //拉高100高度

	if (!InitBitmapWindow(hInstance, nWindowWidth, nWindowHeight, nCmdShow))
		return 0;

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(g_hBitmap);

	return msg.wParam;
}


BOOL InitBitmapWindow(HINSTANCE hinstance, int nWidth, int nHeight, int nCmdshow)
{
	HWND hwnd;
	WNDCLASS wndclass;

	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	wndclass.lpfnWndProc = BitmapWindowWndPrco;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hinstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);//窗口背影画刷为空
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, "Program Need Windows NT!", "Error", MB_ICONERROR);
		return FALSE;
	}

	hwnd = CreateWindowEx(WS_EX_TOPMOST,
		szAppName,
		szAppName,
		WS_POPUP,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		nWidth,
		nHeight,
		NULL,
		NULL,
		hinstance,
		NULL);
	if (hwnd == NULL)
		return FALSE;

	ShowWindow(hwnd, nCmdshow);
	UpdateWindow(hwnd);

	return TRUE;
}

LRESULT CALLBACK BitmapWindowWndPrco(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParm)
{
	static HDC s_hdcMem; //放置缩放后的位图

	switch (message)
	{
	case WM_CREATE:
	{
		// 设置分层属性
		SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		// 设置透明色
		COLORREF clTransparent = RGB(0, 0, 0);
		SetLayeredWindowAttributes(hwnd, clTransparent, 0, LWA_COLORKEY);

		//   缩放位图
		// 加载位图到hdcTemp中
		HDC hdc = GetDC(hwnd);
		HDC hdcTemp = CreateCompatibleDC(hdc);
		SelectObject(hdcTemp, g_hBitmap);

		// 得到窗口大小
		int nWidth, nHeight;
		GetWindowSize(hwnd, &nWidth, &nHeight);

		// 创建与窗口大小相等且能容纳位图的HDC - s_hdcMem
		s_hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmp = CreateCompatibleBitmap(hdc, nWidth, nHeight);
		SelectObject(s_hdcMem, hbmp);

		// 将原位图缩放到窗口大小
		BITMAP bm;
		GetObject(g_hBitmap, sizeof(bm), &bm);
		StretchBlt(s_hdcMem, 0, 0, nWidth, nHeight, hdcTemp, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

		// 释放资源
		DeleteDC(hdcTemp);
		ReleaseDC(hwnd, hdc);
	}
	return 0;


	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE: //按下Esc键时退出
			SendMessage(hwnd, WM_DESTROY, 0, 0);
			return TRUE;
		}
		break;


	case WM_LBUTTONDOWN: //当鼠标左键点击时可以拖曳窗口
		PostMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
		return TRUE;

	case WM_ERASEBKGND: //在窗口背景中直接贴图
	{
		HDC hdc = (HDC)wParam;
		int nWidth, nHeight;
		GetWindowSize(hwnd, &nWidth, &nHeight);
		BitBlt(hdc, 0, 0, nWidth, nHeight, s_hdcMem, 0, 0, SRCCOPY);
		return TRUE;
	}

	case WM_DESTROY:
		DeleteDC(s_hdcMem);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParm);
}


void GetWindowSize(HWND hwnd, int *pnWidth, int *pnHeight)
{
	RECT rc;
	GetWindowRect(hwnd, &rc);
	*pnWidth = rc.right - rc.left;
	*pnHeight = rc.bottom - rc.top;
}
