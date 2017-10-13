// 1512432_loadExplorer.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "1512432_loadExplorer.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE g_hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY1512432_LOADEXPLORER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY1512432_LOADEXPLORER));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_SMALL));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY1512432_LOADEXPLORER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON_LARGE));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_SIZEBOX | WS_MAXIMIZE,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}
	g_hWnd = hWnd;

	ShowWindow(hWnd, SW_MAXIMIZE);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_NOTIFY, OnNotify);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void OnPaint(HWND hWnd) {
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = BeginPaint(hWnd, &ps);
	// TODO: Add any drawing code here...
	EndPaint(hWnd, &ps);
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}


BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	//Lấy các kích thước của cửa sổ hiện hành
	GetWindowRect(hwnd, &g_mainWin);

	g_Drive = new CDrive;
	g_Drive->GetSystemDrives();

	int parentWidth = g_mainWin.right - g_mainWin.left;
	int parentHeight = g_mainWin.bottom - g_mainWin.top;

	g_TreeView = new CTreeView;
	g_TreeView->Create(hwnd, IDC_TREEVIEW, g_hInst, parentWidth / 3, parentHeight);
	g_TreeView->InitImageList();
	g_TreeView->LoadMyComputer(g_Drive);
	g_TreeView->GetFocus();

	g_ListView = new CListView;
	g_ListView->Create(hwnd, IDC_LISTVIEW, g_hInst, parentWidth * 2 / 3, parentHeight, parentWidth + 1);
	g_ListView->InitImageList();
	g_ListView->LoadMyComputer(g_Drive);

	//Đã khởi tạo xong ứng dụng
	g_start = TRUE;

	return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDM_EXIT:
		DestroyWindow(g_hWnd);
		break;

	case IDM_ABOUT:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), g_hWnd, About);
		break;

	case ID_HELP_HELP:
		MessageBox(NULL, (L"Try MyExplorer yourself! ^_^"), _T("Help"), MB_OK);
		break;
	}
}

LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm)
{
	int nCurSelIndex;

	if (g_start) //Để tránh vòng lặp lẩn quẩn T_T
	{
		LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW)pnm;

		switch (pnm->code)
		{
		case TVN_ITEMEXPANDING:
			g_TreeView->ExpandingDrive(lpnmtv->itemOld.hItem, lpnmtv->itemNew.hItem);
			break;
			//------------------------------------------------------------------------------
		case TVN_SELCHANGED:
			g_TreeView->Expand(g_TreeView->GetCurSel()); // 
			g_ListView->DeleteAll(); //Xóa sạch List View để nạp cái mới
			g_ListView->LoadChild(g_TreeView->GetCurPath(), g_Drive);
			break;
		case NM_DBLCLK:
			if (pnm->hwndFrom == g_ListView->GetHandle())
				g_ListView->LoadCurSel();
			break;

		default:
			break;
			//------------------------------------------------------------------------------
		}//switch
	}//if

	return 0;
}

void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	g_TreeView->Size(cy);
	GetWindowRect(g_TreeView->GetHandle(), &g_TVRect); //Cap nhat lai cho TreeView

	g_ListView->Size(g_TreeView->GetHandle());

}

//void InitImageList() {
//	HIMAGELIST himl;  // handle to image list 
//	HBITMAP hbmp;     // handle to bitmap 
//
//					  // Create the image list. 
//	himl = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK,
//		NUMBER_OF_INIT_ICON, MAX_EXPAND_ICON);
//
//	// Add the open file, closed file, and document bitmaps. 
//	hbmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
//	desktopIconIndex = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
//	DeleteObject(hbmp);
//
//	hbmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
//	myComputerIconIndex = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
//	DeleteObject(hbmp);
//
//	/*hbmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP7));
//	fdicon = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
//	DeleteObject(hbmp);*/
//
//	hbmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
//	driveIconIndexHDD = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
//	DeleteObject(hbmp);
//
//	hbmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
//	driveIconIndexUSB = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
//	DeleteObject(hbmp);
//
//	// Add the open file, closed file, and document bitmaps. 
//	
//
//	// Associate the image list with the tree-view control. 
//	HWND tv = g_TreeView->GetHandle();
//	TreeView_SetImageList(tv, himl, TVSIL_NORMAL);
//}



//void CTreeView::LoadMyComputer(CDrive *drive) {
//	TV_INSERTSTRUCT myTV;
//	myTV.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
//
//	HWND tv = g_TreeView->GetHandle();
//	// Load desktop
//
//	myTV.hParent = NULL;
//	myTV.hInsertAfter = TVI_ROOT;
//	myTV.item.iImage = desktopIconIndex;
//	myTV.item.iSelectedImage = desktopIconIndex;
//	myTV.item.pszText = L"Desktop";
//	myTV.item.lParam = (LPARAM)L"Desktop";
//	HTREEITEM hDesktop = TreeView_InsertItem(tv, &myTV);
//
//	//load mycomputer into treeview
//
//	myTV.hParent = hDesktop;
//	myTV.hInsertAfter = TVI_LAST;
//	myTV.item.iImage = myComputerIconIndex;
//	myTV.item.iSelectedImage = myComputerIconIndex;
//	myTV.item.pszText = L"MyComputer";
//	myTV.item.lParam = (LPARAM)L"MyComputer";
//	HTREEITEM hMyComputer = TreeView_InsertItem(tv, &myTV);
//
//	// Load drive in my computer into treeview
//	int i;
//	int n = drive->GetCount();
//	for (i = 0; i < n; ++i) {
//		myTV.hParent = hMyComputer;
//		myTV.item.pszText = drive->GetDisplayName(i);
//		if (drive->m_Type[i] == IDI_HDD) {
//			myTV.item.iImage = driveIconIndexHDD;
//			myTV.item.iSelectedImage = driveIconIndexHDD;
//		}
//		else if(drive->m_Type[i] == IDI_USB || drive->m_Type[i] == IDI_CD) {
//			myTV.item.iImage = driveIconIndexUSB;
//			myTV.item.iSelectedImage = driveIconIndexUSB;
//		}
//		else {
//			myTV.item.iImage = fdicon;
//			myTV.item.iSelectedImage = fdicon;
//		}
//		myTV.item.lParam = (LPARAM)drive->GetDriveName(i);
//		HTREEITEM hDrive = TreeView_InsertItem(tv, &myTV);
//
//		//Preload
//		if (drive->m_Type[i] == IDI_HDD || drive->m_Type[i] == IDI_USB)
//		{
//			myTV.hParent = hDrive; //Them
//			myTV.item.pszText = _T("PreLoad"); //Them
//			myTV.item.lParam = (LPARAM)_T("PreLoad");
//			TreeView_InsertItem(tv, &myTV);
//		}
//	}
//
//	TreeView_Expand(tv, hMyComputer, TVE_EXPAND);
//	TreeView_SelectItem(tv, hMyComputer);
//}


