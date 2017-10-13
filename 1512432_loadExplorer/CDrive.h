#pragma once
#include <tchar.h>
#include <Windows.h>

#define MAX_PATH_LEN 10240
#define NUMBER_OF_INIT_ICON 8
#define MAX_EXPAND_ICON 3


LPWSTR convert(__int64 nSize);

class CDrive {
private:
	WCHAR **m_DriveName; // chứa tên của từng ổ đĩa duyệt được trên máy người dùng 
	WCHAR **m_VolName; // tên thể hiện trên app
	WCHAR **m_DisplayName; //Kết hợp giữa pszDrive và pszVolName
	int m_CounOfDrive;
	int *m_Type;
public:
	
	CDrive();
	~CDrive();
	int getType(int i) {
		return m_Type[i];
	}
	WCHAR* GetDriveName(int i);
	WCHAR* GetVolName(const int i);
	WCHAR* GetDisplayName(int i);
	int GetCount();
	LPWSTR getTypeToStr(int i);

	void GetSystemDrives();

	//LPWSTR GetType(const int &i) {

	//}
	__int64 GetSize(const int &i);
	LPWSTR GetSizeStr(const int &i);

	__int64 GetFreeSize(const int &i);
	LPWSTR GetFreeSizeStr(const int &i);
};