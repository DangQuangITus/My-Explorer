#include "stdafx.h"
#include "CListView.h"
#include <tchar.h>
#include "resource.h"

#include <shlobj.h> //Shell object

//Dùng để sử dụng hàm StrCpy, StrNCat
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


LPWSTR CListView::_GetSize(const WIN32_FIND_DATA &fd) {
	DWORD dwSize = fd.nFileSizeLow;
	return convert(dwSize);
}
LPWSTR CListView::_GetType(const WIN32_FIND_DATA &fd) {
	int nDotPos = StrRStrI(fd.cFileName, NULL, _T(".")) - fd.cFileName;
	int len = wcslen(fd.cFileName);

	if (nDotPos < 0 || nDotPos >= len) //Nếu không tìm thấy
		return _T("Không biết");

	TCHAR *szExtension = new TCHAR[len - nDotPos + 1];
	int i;

	for (i = nDotPos; i < len; ++i)
		szExtension[i - nDotPos] = fd.cFileName[i];
	szExtension[i - nDotPos] = NULL; //Kí tự kết thúc chuỗi

	if (!StrCmpI(szExtension, _T(".htm")) || !StrCmpI(szExtension, _T(".html")))
	{
		return _T("Web page");
	}
	TCHAR pszOut[256];
	HKEY hKey;
	DWORD dwType = REG_SZ;
	DWORD dwSize = 256;

	//Kiếm handle của extension tương ứng trong registry
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szExtension, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return _T("Không biết");
	}

	if (RegQueryValueEx(hKey, NULL, NULL, &dwType, (PBYTE)pszOut, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return _T("Không biết");
	}
	RegCloseKey(hKey);

	//Kiếm mô tả về thông tin của extension thông qua handle của key tương ứng trong registry
	TCHAR *pszPath = new TCHAR[1000];
	dwSize = 1000;
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, pszOut, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return _T("Không biết");
	}

	if (RegQueryValueEx(hKey, NULL, NULL, &dwType, (PBYTE)pszPath, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return _T("Không biết");
	}
	RegCloseKey(hKey);

	return pszPath;
}


CListView::CListView() {
	m_hInst = NULL;
	m_hParent = NULL;
	m_hLVShow = NULL;
	m_nID = 0;
}
CListView::~CListView()
{
	DestroyWindow(m_hLVShow);
}
//Khởi tạo 4 cột tổng quát

void CListView::InitImageList() {
	//load icon cho folder
	HIMAGELIST shiml;  // handle to small image list 
	HIMAGELIST lhiml;   // handle to large image list 
	HBITMAP hbmp;     // handle to bitmap 

					  // Create the image list. 
	lhiml = ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK,
		NUMBER_OF_INIT_ICON, MAX_EXPAND_ICON);
	shiml = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK,
		NUMBER_OF_INIT_ICON, MAX_EXPAND_ICON);

	// Add the open file, closed file, and document bitmaps. 
	hbmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_FOLDER_LARGE));
	lFolderIconIndex = ImageList_Add(lhiml, hbmp, (HBITMAP)NULL); // large folder
	DeleteObject(hbmp);

	hbmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_UNKNOWN_LARGE));
	lUnknown = ImageList_Add(lhiml, hbmp, (HBITMAP)NULL);  // l un
	DeleteObject(hbmp);

	hbmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_DESKTOP));
	lvicon = ImageList_Add(shiml, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_HDD)); // HDD
	lvdriveIconIndexHDD = ImageList_Add(shiml, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_CDROM)); // CD ROM
	lvdriveIconIndexUSB = ImageList_Add(shiml, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_FOLDER_SMALL));
	sFolderIconIndex = ImageList_Add(shiml, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_UNKNOWN_SMALL));
	sUnknown = ImageList_Add(shiml, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	// Associate the image list with the tree-view control. 
	ListView_SetImageList(m_hLVShow, lhiml, LVSIL_NORMAL);
	ListView_SetImageList(m_hLVShow, shiml, LVSIL_SMALL);
}


void CListView::Creat4Column()
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 130;
	lvCol.pszText = (L"Name");
	ListView_InsertColumn(m_hLVShow, 0, &lvCol);

	lvCol.fmt = LVCFMT_LEFT | LVCF_WIDTH;
	lvCol.cx = 100;
	lvCol.pszText = (L"Type");
	ListView_InsertColumn(m_hLVShow, 1, &lvCol);

	lvCol.fmt = LVCFMT_LEFT;
	lvCol.pszText = (L"Size");
	ListView_InsertColumn(m_hLVShow, 2, &lvCol);

	lvCol.pszText = (L"Motify");
	ListView_InsertColumn(m_hLVShow, 3, &lvCol);
}

void CListView::LoadMyComputer(CDrive *drive)
{
	//SetDlgItemText(GetDlgItem(m_hLVShow, IDC_ADDRESS), IDC_ADDRESS_EDIT, (L"My Computer"));
	InitDriveColumn();
	DeleteAll();

	int n = drive->GetCount();
	for (int i = 0; i < n; ++i)
	{
		LV_ITEM lv;

		//Nạp cột đầu tiên cũng là thông tin chính
		lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM; // các thuộc tính

		lv.iItem = i; // row
		if (drive->getType(i) == IDI_HDD)
			lv.iImage = lvdriveIconIndexHDD; // icon
		else if (drive->getType(i) == IDI_USB || drive->getType(i) == IDI_CD)
			lv.iImage = lvdriveIconIndexUSB; // icon
		else
			lv.iImage = sFolderIconIndex; // icon

		lv.iSubItem = 0; // index subitem
		lv.pszText = drive->GetDisplayName(i);
		lv.lParam = (LPARAM)drive->GetDriveName(i);
		ListView_InsertItem(m_hLVShow, &lv);

		//Nạp các cột còn lại (Type, Size, Free Space)
		lv.mask = LVIF_TEXT;

		//Cột đầu tiên là Type
		lv.iSubItem = 1;
		lv.pszText = drive->getTypeToStr(i);
		ListView_SetItem(m_hLVShow, &lv);

		//Cột tiếp theo là Size
		lv.iSubItem = 2;

		if (drive->getType(i) != IDI_FLOPPY && drive->getType(i) != IDI_CD)
			lv.pszText = drive->GetSizeStr(i);
		else
			lv.pszText = NULL;

		ListView_SetItem(m_hLVShow, &lv);

		//Cột cuối cùng là Free Space
		lv.iSubItem = 3;

		if (drive->getType(i) != IDI_FLOPPY && drive->getType(i) != IDI_CD)
			lv.pszText = drive->GetFreeSizeStr(i);
		else
			lv.pszText = NULL;

		ListView_SetItem(m_hLVShow, &lv);
	}
}

void CListView::LoadDesktop()
{
	DeleteAll();
	InitFolColumn();

	LV_ITEM lv;
	lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lv.iItem = 0;
	lv.iSubItem = 0;
	lv.pszText = (L"My Computer");
	lv.iImage = lvicon;
	lv.lParam = (LPARAM)(L"MyComputer");
	ListView_InsertItem(m_hLVShow, &lv);
}

void CListView::LoadChild(LPCWSTR path, CDrive *drive)
{
	if (path == NULL)
		return;

	if (!StrCmp(path, (L"Desktop")))
		LoadDesktop();
	else
		if (!StrCmp(path, (L"MyComputer")))
			LoadMyComputer(drive);
		else
			LoadFileAndFolder(path);
}

void CListView::InitFolColumn()
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_WIDTH;
	lvCol.cx = 180;
	ListView_SetColumn(m_hLVShow, 0, &lvCol);

	lvCol.mask = LVCF_TEXT | LVCF_FMT;
	lvCol.fmt = LVCFMT_RIGHT;
	lvCol.pszText = (L"Size");
	ListView_SetColumn(m_hLVShow, 1, &lvCol);

	lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 130;
	lvCol.pszText = (L"type");
	ListView_SetColumn(m_hLVShow, 2, &lvCol);

	lvCol.pszText = (L"Updated Time");
	ListView_SetColumn(m_hLVShow, 3, &lvCol);
}

HWND CListView::GetHandle()
{
	return m_hLVShow;
}

LPCWSTR CListView::GetPath(int iItem)
{
	LVITEM lv;
	lv.mask = LVIF_PARAM;
	lv.iItem = iItem;
	lv.iSubItem = 0;
	ListView_GetItem(m_hLVShow, &lv);
	return (LPCWSTR)lv.lParam;
}

void CListView::LoadCurSel()
{
	LPCWSTR path = GetPath(ListView_GetSelectionMark(m_hLVShow));

	WIN32_FIND_DATA fd;
	GetFileAttributesEx(path, GetFileExInfoStandard, &fd);

	//Nếu là thư mục
	if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		ListView_DeleteAllItems(m_hLVShow);
		LoadFileAndFolder(path);
	}
	else //Nếu là tập tin thì chạy nó
		ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOWNORMAL);
}

void CListView::LoadFileAndFolder(LPCWSTR path)
{
	InitFolColumn(); // khởi tạo columns
	DeleteAll(); // xóa để tải lại từ đầu

	WCHAR buffer[10240];
	StrCpy(buffer, path);

	if (wcslen(path) == 3) //Nếu quét các ổ đĩa
		StrCat(buffer, (L"*"));
	else
		StrCat(buffer, (L"\\*"));

	//Bắt đầu tìm các file và folder trong thư mục
	WIN32_FIND_DATA fd;
	HANDLE hFile;
	BOOL bFound = true;
	LV_ITEM lv;

	WCHAR * folderPath;
	int nItemCount = 0;

	//Chạy lần thứ nhất lấy các thư mục
	hFile = FindFirstFileW(buffer, &fd);
	bFound = TRUE;

	if (hFile == INVALID_HANDLE_VALUE) {
		bFound = FALSE;
	}

	while (bFound)
	{
		if (
			(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			&& (StrCmp(fd.cFileName, (L".")) != 0)
			&& (StrCmp(fd.cFileName, (L"..")) != 0)

			)
		{
			folderPath = new WCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(folderPath, path);

			if (wcslen(path) != 3)
				StrCat(folderPath, (L"\\"));

			StrCat(folderPath, fd.cFileName);
			//cột tên file
			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = nItemCount;
			lv.iImage = sFolderIconIndex;
			lv.iSubItem = 0;
			lv.pszText = fd.cFileName;
			lv.lParam = (LPARAM)folderPath;
			ListView_InsertItem(m_hLVShow, &lv);

			//size
			ListView_SetItemText(m_hLVShow, nItemCount, 1, _GetSize(fd));
			//của thư mục ở thanh status bar thôi

			//Cột thứ ba là cột Type
			ListView_SetItemText(m_hLVShow, nItemCount, 2, _GetType(fd));

			//Cột thứ tư là cột Date modified
			ListView_SetItemText(m_hLVShow, nItemCount, 3, _GetDateModified(fd.ftLastWriteTime));
			++nItemCount;
		}//if

		bFound = FindNextFileW(hFile, &fd);
	}//while

	DWORD folderCount = nItemCount;
	/*************************************************************************************/
	//load
	TCHAR *filePath;
	DWORD fileSizeCount = 0;
	DWORD fileCount = 0;

	hFile = FindFirstFileW(buffer, &fd);
	bFound = TRUE;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	while (bFound)
	{
		if (
			((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) && 
			((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			)
		{
			filePath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(filePath, path);

			if (wcslen(path) != 3)
				StrCat(filePath, (L"\\"));

			StrCat(filePath, fd.cFileName);

			//Cột thứ nhất là tên hiển thị của tập tin
			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = nItemCount;
			lv.iSubItem = 0;
			lv.iImage = sUnknown;
			lv.pszText = fd.cFileName;
			lv.lParam = (LPARAM)filePath;
			ListView_InsertItem(m_hLVShow, &lv);

			//Cột thứ hai là Size
			ListView_SetItemText(m_hLVShow, nItemCount, 1, _GetSize(fd));
			fileSizeCount += fd.nFileSizeLow;

			//Cột thứ ba là Type
			ListView_SetItemText(m_hLVShow, nItemCount, 2, _GetType(fd));

			//Cột thứ tư là Date Modified	
			ListView_SetItemText(m_hLVShow, nItemCount, 3, _GetDateModified(fd.ftLastWriteTime));

			++nItemCount;
			++fileCount;
		}//if

		bFound = FindNextFileW(hFile, &fd);
	}//while

	TVITEMEX tv;
	WCHAR *folder = new WCHAR[512];
	WCHAR *info = new WCHAR[256];

	tv.mask = TVIF_TEXT;
	tv.hItem = TreeView_GetNextItem(GetDlgItem(m_hParent, IDC_TREEVIEW), NULL, TVGN_CARET);
	tv.pszText = folder;
	tv.cchTextMax = 256;
	TreeView_GetItem(GetDlgItem(m_hParent, IDC_TREEVIEW), &tv);

}

void CListView::Create(HWND parentWnd, long ID, HINSTANCE hParentInst, int nWidth,
	int nHeight, int x, int y, long lExtStyle, long lStyle) {

	InitCommonControls();
	m_hInst = hParentInst;
	m_hParent = parentWnd;

	m_hLVShow = CreateWindowEx(lExtStyle, WC_LISTVIEW, (L"List View"),
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | lStyle,
		x, y, nWidth, nHeight, m_hParent, (HMENU)ID, m_hInst, NULL);

	ListView_SetExtendedListViewStyle(m_hLVShow, LVS_EX_FULLROWSELECT);
	m_nID = ID;

	Creat4Column();
}

void CListView::InitDriveColumn()
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_TEXT | LVCF_FMT;

	lvCol.fmt = LVCFMT_LEFT | LVCF_WIDTH;
	lvCol.cx = 100;
	lvCol.pszText = (L"Type");
	ListView_SetColumn(m_hLVShow, 1, &lvCol);

	lvCol.fmt = LVCFMT_RIGHT | LVCF_WIDTH;
	lvCol.cx = 80;
	lvCol.pszText = (L"Total Size");
	ListView_SetColumn(m_hLVShow, 2, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = (L"Free Space Size");
	ListView_SetColumn(m_hLVShow, 3, &lvCol);

}

void CListView::DeleteAll()
{
	ListView_DeleteAllItems(m_hLVShow);
}

void CListView::Size(HWND treeview)
{
	RECT tree;
	GetWindowRect(treeview, &tree);

	RECT main;
	GetWindowRect(m_hParent, &main);

	MoveWindow(m_hLVShow, tree.right - tree.left, 0, main.right - tree.right, tree.bottom - tree.top, TRUE);
	ListView_Arrange(m_hLVShow, LVA_ALIGNTOP);
}