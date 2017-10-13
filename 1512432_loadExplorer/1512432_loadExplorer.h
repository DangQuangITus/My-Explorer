#pragma once

#include "resource.h"
#include <windowsx.h>
#include <commdlg.h>
#include <WinUser.h>
#include "CDrive.h"
#include "CTreeView.h"
#include "CListView.h"

#include <Dbt.h> //Chứa các hằng số sự kiện của thông điệp WM_DEVICECHANGE

/*int myComputerIconIndex, desktopIconIndex;*/ // icon của các ổ đĩa/desktop tương ứng
//int driveIconIndexHDD, driveIconIndexUSB;


HWND g_hWnd; // hWnd toàn cục
CDrive *g_Drive; // con trỏ đến các ổ đĩa
CTreeView *g_TreeView; // 
CListView *g_ListView;
RECT g_TVRect;
RECT g_mainWin;


void OnDestroy(HWND hwnd);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm);
void OnPaint(HWND hWnd);
void OnSize(HWND hwnd, UINT state, int cx, int cy);


BOOL g_start = FALSE;

