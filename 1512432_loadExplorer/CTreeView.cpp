#pragma once
#include "stdafx.h"
#include "CTreeView.h"
#include "Resource.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

CTreeView::CTreeView() {
	m_hInst = NULL;
	m_hParent = NULL;
	m_hTVShow = NULL;
	m_nID = 0;
}
CTreeView::~CTreeView() {
	DestroyWindow(m_hTVShow);
}

void CTreeView::Create(HWND parentWnd, long ID, HINSTANCE hParentInst, int nWidth, int nHeight,
	long lExtStyle, long lStyle ,int x , int y )
{

	InitCommonControls();
	m_hParent = parentWnd;
	m_hInst = hParentInst;
	m_hTVShow = CreateWindowExW(lExtStyle,
		WC_TREEVIEW, (L"Tree View"),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_SIZEBOX | WS_VSCROLL | WS_TABSTOP | lStyle,
		x,
		y,
		nWidth,
		nHeight,
		parentWnd,
		(HMENU)ID,
		hParentInst, NULL);
	m_nID = ID;

}

HWND CTreeView::GetHandle() {
	return m_hTVShow;
}

int	CTreeView::GetID() {
	return m_nID;
}

LPCWSTR CTreeView::GetPath(HTREEITEM hItem) {
	TVITEMEX tv;
	tv.mask = TVIF_PARAM;
	tv.hItem = hItem;
	TreeView_GetItem(m_hTVShow, &tv);
	return (LPCWSTR)tv.lParam;
}

HTREEITEM CTreeView::GetCurSel() {
	return TreeView_GetNextItem(m_hTVShow, NULL, TVGN_CARET);
}

HTREEITEM CTreeView::GetMyComputer() {
	HTREEITEM myComputer = TreeView_GetRoot(m_hTVShow); //get root
	return TreeView_GetChild(m_hTVShow, myComputer);
}

void CTreeView::GetFocus() {
	SetFocus(m_hTVShow);
}


void CTreeView::LoadChild(HTREEITEM &hParent, LPCWSTR path, BOOL bShowHiddenSystem)
{
	
	///////////////////////////
	TCHAR buffer[MAX_PATH_LEN];
	StrCpy(buffer, path);

	StrCat(buffer, _T("\\*"));

	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvInsert.item.iImage = fdicon;
	tvInsert.item.iSelectedImage = fdicon;

	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);
	BOOL bFound = 1;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	TCHAR * folderPath;
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM)
			&& (StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			tvInsert.item.pszText = fd.cFileName;
			folderPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];

			StrCpy(folderPath, path);
			if (wcslen(path) != 3)
				StrCat(folderPath, _T("\\"));
			StrCat(folderPath, fd.cFileName);

			tvInsert.item.lParam = (LPARAM)folderPath;
			HTREEITEM hItem = TreeView_InsertItem(m_hTVShow, &tvInsert);
			//Preload
			PreLoad(hItem);
		}

		bFound = FindNextFileW(hFile, &fd);
	}//while
}

void CTreeView::ExpandingDrive(HTREEITEM hPrev, HTREEITEM hCurSel) {

	if (hCurSel == GetMyComputer())
		return;

	HTREEITEM curChildNode = TreeView_GetChild(m_hTVShow, hCurSel);
	if (!StrCmp(GetPath(curChildNode), (L"PreLoad")))
	{
		TreeView_DeleteItem(m_hTVShow, curChildNode);
		LoadChild(hCurSel, GetPath(hCurSel), 0);
	}
}
LPCWSTR CTreeView::GetCurPath()
{
	return GetPath(GetCurSel());
}
void CTreeView::Expand(HTREEITEM hItem) {
	TreeView_Expand(m_hTVShow, hItem, TVE_EXPAND);
}
void CTreeView::Size(int cy) {

	RECT main;
	GetWindowRect(m_hParent, &main);
	MoveWindow(m_hTVShow, 0, 0, main.right/5, main.bottom, SWP_SHOWWINDOW);
}

void CTreeView::PreLoad(HTREEITEM hItem) {
	TCHAR buffer[MAX_PATH_LEN];
	StrCpy(buffer, GetPath(hItem));

	if (wcslen(buffer) == 3) // gặp ổ đĩa 
	{
		if (StrStr(buffer, _T("A:")) || StrStr(buffer, _T("B:"))) //đĩa trống
			return; // thoát
	}
	else
		StrCat(buffer, _T("\\"));

	StrCat(buffer, _T("*"));

	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);

	if (hFile == INVALID_HANDLE_VALUE)
		return;

	BOOL bFound = true;

	//Trong khi còn tìm thấy file hay thư mục
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& (StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			TV_INSERTSTRUCT tvInsert;
			tvInsert.hParent = hItem;
			tvInsert.hInsertAfter = TVI_LAST;
			tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
			tvInsert.item.pszText = NULL;
			tvInsert.item.lParam = (LPARAM)_T("PreLoad");
			TreeView_InsertItem(m_hTVShow, &tvInsert);
			bFound = FALSE;
		}
		else
			bFound = FindNextFileW(hFile, &fd);
	}//while
}

void CTreeView::InitImageList() {
	HIMAGELIST himl;  // handle to image list 
	HBITMAP hbmp;     // handle to bitmap 

					  // Create the image list. 
	himl = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK,
		NUMBER_OF_INIT_ICON, MAX_EXPAND_ICON);

	// Add the open file, closed file, and document bitmaps. 
	hbmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_DESKTOP));
	desktopIconIndex = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_PC));
	myComputerIconIndex = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_ITEM_TVFOLDER));
	fdicon = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_HDD));
	driveIconIndexHDD = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_CDROM));
	driveIconIndexUSB = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	// Add the open file, closed file, and document bitmaps. 


	// Associate the image list with the tree-view control. 
	//HWND tv = g_TreeView->GetHandle();
	TreeView_SetImageList(m_hTVShow, himl, TVSIL_NORMAL);
}

void CTreeView::LoadMyComputer(CDrive *drive) {
	TV_INSERTSTRUCT myTV;
	myTV.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	// Load desktop

	myTV.hParent = NULL;
	myTV.hInsertAfter = TVI_ROOT;
	myTV.item.iImage = desktopIconIndex;
	myTV.item.iSelectedImage = desktopIconIndex;
	myTV.item.pszText = L"Desktop";
	myTV.item.lParam = (LPARAM)L"Desktop";
	HTREEITEM hDesktop = TreeView_InsertItem(m_hTVShow, &myTV);

	//load mycomputer into treeview
	
	myTV.hParent = hDesktop;
	myTV.hInsertAfter = TVI_LAST;
	myTV.item.iImage = myComputerIconIndex;
	myTV.item.iSelectedImage = myComputerIconIndex;
	myTV.item.pszText = L"This PC";
	myTV.item.lParam = (LPARAM)L"This PC";
	HTREEITEM hMyComputer = TreeView_InsertItem(m_hTVShow, &myTV);

	// Load drive in my computer into treeview
	

	int i;
	int n = drive->GetCount(); // số lượng ổ đĩa

	for (i = 0; i < n; ++i) {
		myTV.hParent = hMyComputer;
		myTV.item.pszText = drive->GetDisplayName(i);
		if (drive->getType(i) == IDI_HDD) {
			myTV.item.iImage = driveIconIndexHDD;
			myTV.item.iSelectedImage = driveIconIndexHDD;
		}
		else if (drive->getType(i) == IDI_USB || drive->getType(i) == IDI_CD) {
			myTV.item.iImage = driveIconIndexUSB;
			myTV.item.iSelectedImage = driveIconIndexUSB;
		}
		else {
			myTV.item.iImage = fdicon;
			myTV.item.iSelectedImage = fdicon;
		}
		myTV.item.lParam = (LPARAM)drive->GetDriveName(i);
		HTREEITEM hDrive = TreeView_InsertItem(m_hTVShow, &myTV);

		//Preload
		if (drive->getType(i) == IDI_HDD || drive->getType(i) == IDI_USB)
		{
			myTV.hParent = hDrive; //Them
			myTV.item.pszText = (L"PreLoad"); //Them
			myTV.item.lParam = (LPARAM)(L"PreLoad");
			TreeView_InsertItem(m_hTVShow, &myTV);
		}
	}

	TreeView_Expand(m_hTVShow, hMyComputer, TVE_EXPAND);
	TreeView_SelectItem(m_hTVShow, hMyComputer);

}
