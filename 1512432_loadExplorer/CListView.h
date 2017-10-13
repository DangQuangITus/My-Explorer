#pragma once
#include <windows.h>

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include "CDrive.h"
#include <shellapi.h>

static int lFolderIconIndex, sFolderIconIndex, lUnknown, sUnknown;
static int lvdriveIconIndexHDD, lvdriveIconIndexUSB;

static int lvicon=0;
static int lvicon2 = 0;

class CListView {
private:
	HINSTANCE	m_hInst;
	HWND		m_hLVShow;
	int			m_nID;

	LPWSTR _GetSize(const WIN32_FIND_DATA &fd);

	LPWSTR _GetType(const WIN32_FIND_DATA &fd);

	LPWSTR _GetDateModified(const FILETIME &ftLastWrite)
	{
		//Chuyển đổi sang local time
		SYSTEMTIME stUTC, stLocal;
		FileTimeToSystemTime(&ftLastWrite, &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

		WCHAR *buffer = new WCHAR[50];
		swprintf(buffer,50, (L"%02d/%02d/%04d %02d:%02d %s"),
			stLocal.wDay, stLocal.wMonth, stLocal.wYear,
			(stLocal.wHour > 12) ? (stLocal.wHour / 12) : (stLocal.wHour),
			stLocal.wMinute,
			(stLocal.wHour > 12) ? ((L"Chiều")) : ((L"Sáng")));

		return buffer;
	}
public:
	HWND	m_hParent;
	CListView();
	~CListView();
	void InitImageList();
	//Khởi tạo 4 cột tổng quát
	void Creat4Column();
	void LoadMyComputer(CDrive *drive);
	void LoadDesktop();
	void LoadChild(LPCWSTR path, CDrive *drive);
	void InitFolColumn();
	void InitDriveColumn();
	HWND		GetHandle();
	LPCWSTR		GetPath(int iItem);

	void LoadCurSel();
	void LoadFileAndFolder(LPCWSTR path);
	void Create(HWND parentWnd, long ID, HINSTANCE hParentInst, int nWidth,
		int nHeight, int x = CW_USEDEFAULT, int y = 0,
		long lExtStyle = WS_EX_CLIENTEDGE,
		long lStyle = LVS_ICON | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS );

	void DeleteAll();
	void Size(HWND);

};