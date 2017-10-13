#pragma once
#include <windows.h>
#include <tchar.h>
#include "CDrive.h"

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")


static int myComputerIconIndex, desktopIconIndex;
static int driveIconIndexHDD, driveIconIndexUSB;

static int fdicon = 0;
static int uk = 0;

class CTreeView
{
private:
	HINSTANCE	m_hInst;
	HWND		m_hParent;
	HWND		m_hTVShow;
	int			m_nID;

public:
	CTreeView();
	~CTreeView();

	void Create(HWND parentWnd, long ID, HINSTANCE hParentInst, int nWidth, int nHeight,
		long lExtStyle = 0,
		long lStyle = TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EX_AUTOHSCROLL,
		int x = CW_USEDEFAULT, int y = 0);

	HWND		GetHandle();
	LPCWSTR		GetPath(HTREEITEM hItem);
	HTREEITEM	GetCurSel();
	HTREEITEM	GetMyComputer();
	LPCWSTR		GetCurPath();
	int			GetID();
	void		GetFocus();
	void	InitImageList();
	void LoadMyComputer(CDrive *drive);
	void LoadChild(HTREEITEM &hParent, LPCWSTR path, BOOL bShowHiddenSystem);
	void ExpandingDrive(HTREEITEM hPrev, HTREEITEM hCurSel);
	void Expand(HTREEITEM hItem);
	void Size(int cy);
	void PreLoad(HTREEITEM hItem);
};