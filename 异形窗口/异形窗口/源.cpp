// 对话框位图背景  - WM_CTLCOLORDLG中使用StretchBlt贴图
//By MoreWindows-(http://blog.csdn.net/MoreWindows)
#include <windows.h>
#include "resource.h"

const char szDlgTitle[] = "位图背景 使用StretchBlt贴图";

// 对话框消息处理函数
BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	return 0;
}


BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT       rcDialog;
	HBITMAP    hBitmap;
	static BITMAP s_bm;
	static HDC    s_hdcMem;

	switch (message)
	{
	case WM_INITDIALOG:
		// 设置对话框标题
		SetWindowText(hDlg,szDlgTitle);
		// 设置对话框大小可调节
		SetWindowLong(hDlg, GWL_STYLE, GetWindowLong(hDlg, GWL_STYLE) | WS_SIZEBOX);

		// 加载背景图片
		hBitmap = (HBITMAP)LoadImage(NULL, "005.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		if (hBitmap == NULL)
		{
			MessageBox(hDlg, "LoadImage failed", "Error", MB_ICONERROR);
			exit(0);
		}
		else
		{
			// 将背影图片放入HDC - s_hdcMem
			HDC        hdc;
			hdc = GetDC(hDlg);
			s_hdcMem = CreateCompatibleDC(hdc);
			SelectObject(s_hdcMem, hBitmap);
			ReleaseDC(hDlg, hdc);

			// 得到位图信息
			GetObject(hBitmap, sizeof(s_bm), &s_bm);
		}

		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			DeleteDC(s_hdcMem);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;


	case WM_SIZE:
		InvalidateRect(hDlg, NULL, TRUE);
		return TRUE;

	case WM_CTLCOLORDLG:
		GetClientRect(hDlg, &rcDialog);
		//通过SetStretchBltMode的设置能使StretchBlt在缩放图像更加清晰
		SetStretchBltMode((HDC)wParam, COLORONCOLOR);
		StretchBlt((HDC)wParam, 0, 0, rcDialog.right, rcDialog.bottom, s_hdcMem, 0, 0, s_bm.bmWidth, s_bm.bmHeight, SRCCOPY);
		return (BOOL)((HBRUSH)GetStockObject(NULL_BRUSH));
	}
	return FALSE;
}
