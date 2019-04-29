// 可变透明度窗体 先加上WS_EX_LAYERED属性再SetLayeredWindowAttributes设置透明度
//By MoreWindows-(http://blog.csdn.net/MoreWindows)
#include <windows.h>
#include <commctrl.h>
#include "resource.h"
const char szDlgTitle[] = "可变透明度窗体";
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
	const int INIT_TRANSPARENT = 200; //窗体初始透明度
	static HBRUSH    s_hBitmapBrush; //位图画刷

	switch (message)
	{
	case WM_INITDIALOG:
		// 设置对话框标题
		SetWindowText(hDlg, szDlgTitle);

		// 加载背影图片
		HBITMAP hBitmap;
		hBitmap = (HBITMAP)LoadImage(NULL, "005.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		if (hBitmap == NULL)
		{
			MessageBox(hDlg, "LoadImage failed", "Error", MB_ICONERROR);
			exit(0);
		}

		// 创建位图画刷
		s_hBitmapBrush = CreatePatternBrush(hBitmap);

		// 设置分层属性
		SetWindowLong(hDlg, GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_LAYERED);
		// 设置透明度 0 - completely transparent   255 - opaque
		SetLayeredWindowAttributes(hDlg, 0, INIT_TRANSPARENT, LWA_ALPHA);

		// 设置滑动条变化范围
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER_TRANSPARENT), TBM_SETRANGE, (WPARAM)FALSE, MAKELONG(0, 255));
		// 设置滑块初始位置
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER_TRANSPARENT), TBM_SETPOS, (WPARAM)TRUE, INIT_TRANSPARENT);
		return 0;


	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			DeleteObject(s_hBitmapBrush);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;


	case WM_HSCROLL: // slider control 滑块位置有变化时的响应函数
	{
		// 获取当前滑块位置
		int nTransparent = SendMessage(GetDlgItem(hDlg, IDC_SLIDER_TRANSPARENT), TBM_GETPOS, 0, 0);
		// 设置新透明度
		SetLayeredWindowAttributes(hDlg, 0, nTransparent, LWA_ALPHA);
	}
	break;


	case WM_CTLCOLORDLG:  //对话框背影
		return (BOOL)s_hBitmapBrush;
	}
	return FALSE;
}
