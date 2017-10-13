#pragma once
#include "stdafx.h"
#include "CDrive.h"
#include <tchar.h>
#include <Shlwapi.h>
#include "Resource.h"
#pragma comment(lib, "shlwapi.lib")

#include <shellapi.h>
#define BUFFER_LEN 200 //Để chữa chuỗi tối đa 26 x 4 + 1 kí tự (Ví dụ "A:\")

#define KB 1
#define MB 2
#define GB 3
#define TB 4
#define RADIX 10

 LPWSTR convert(__int64 nSize) {
	int nType = 0; //Bytes

	while (nSize >= 1048576) //
	{
		nSize /= 1024;
		++nType;
	}

	__int64 nRight; // phần sau dấu '.'

	if (nSize >= 1024){
		//Lấy một chữ số sau thập phân của nSize chứa trong nRight
		nRight = nSize % 1024;

		while (nRight > 99)
			nRight /= 10;

		nSize /= 1024;
		++nType;
	}
	else
		nRight = 0;

	WCHAR *buffer = new WCHAR[11];
	_itow_s(nSize, buffer, 11, RADIX);

	if (nRight != 0 && nType > KB)
	{
		StrCat(buffer, L".");
		WCHAR *right = new WCHAR[3];
		_itow_s(nRight, right, 3, RADIX);
		StrCat(buffer, right);
	}

	switch (nType)
	{
	case 0://Bytes
		StrCat(buffer, (L" bytes"));
		break;
	case KB:
		StrCat(buffer, (L" KB"));
		break;
	case MB:
		StrCat(buffer, (L" MB"));
		break;
	case GB:
		StrCat(buffer, (L" GB"));
		break;
	case TB:
		StrCat(buffer, (L" TB"));
		break;
	}

	return buffer;
}


CDrive::CDrive() {
	m_DriveName = NULL;
	m_VolName = NULL;
	m_DisplayName = NULL;
	m_CounOfDrive = 0;
	m_Type = NULL;
}

CDrive::~CDrive() {
	for (int i = 0; i < m_CounOfDrive; ++i)
	{
		delete[] m_DriveName[i];
		delete[] m_VolName[i];
		delete[] m_DisplayName[i];
	}

	delete[] m_DriveName;
	delete[] m_VolName;
	delete[] m_DisplayName;

	m_CounOfDrive = 0;
}

WCHAR* CDrive::GetDriveName(int i) {
	return m_DriveName[i];
}
WCHAR* CDrive::GetVolName(const int i) {
	return m_VolName[i];
}
WCHAR* CDrive::GetDisplayName(int i) {
	return m_DisplayName[i];
}
int CDrive::GetCount() {
	return m_CounOfDrive;
}


LPWSTR CDrive::getTypeToStr(int i) {
	int l_type = m_Type[i];
	switch (l_type)
	{
	case IDI_FLOPPY:
		return (L"3½ Floppy");
		break;
	case IDI_CD:
		return (L"CD Rom");
		break;
	case IDI_HDD:
		return (L"Local Disk");
		break;
	}

	//Còn lại chắc chắn là USB
	return (L"Removable Disk");
}

void CDrive::GetSystemDrives() {
	WCHAR buffer[BUFFER_LEN]; // chứa info các ổ đĩa quét được
	int i;

	GetLogicalDriveStrings(BUFFER_LEN, buffer); // lấy danh sách các ổ đĩa trên máy vào buffer

												// chia tên ổ đĩa tìm được
	for (i = 0; !((buffer[i] == 0) && (buffer[i + 1] == 0)); ++i)
		if (buffer[i] == 0)
			++m_CounOfDrive;
	++m_CounOfDrive;

	m_DriveName = new WCHAR*[m_CounOfDrive]; //các cột chứa tên ổ đĩa
	m_VolName = new WCHAR*[m_CounOfDrive]; // loại ổ đĩa
	m_DisplayName = new WCHAR*[m_CounOfDrive]; //kết hợp
	m_Type = new int[m_CounOfDrive];

	for (i = 0; i < m_CounOfDrive; ++i)
	{
		m_DriveName[i] = new WCHAR[4];// tên ổ đĩa (C,D,E..)
		m_VolName[i] = new WCHAR[30]; //loại ổ đĩa ( local disk,..)
		m_DisplayName[i] = new WCHAR[35]; // kết hợp (local disk (C:)..)
	}

	int j, k;
	i = 0;

	//Lấy từng kí tự ổ đĩa
	for (j = 0; j < m_CounOfDrive; ++j)
	{
		k = 0;
		while (buffer[i] != 0) // lấy tên ổ đĩa đầu tiên
			m_DriveName[j][k++] = buffer[i++];
		m_DriveName[j][k] = 0; //Kết thúc chuỗi
		++i;
	}

	int nType;

	for (i = 0; i < m_CounOfDrive; ++i)
	{
		nType = GetDriveType(m_DriveName[i]); // lấy kiểu ổ đĩa
		StrCpyW(buffer, (L"")); // khởi tạo buffer rỗng

		if ((nType == DRIVE_FIXED) || ((i > 1) && (nType == DRIVE_REMOVABLE)))//Nếu là ổ cứng hay USB
		{
			if (nType == DRIVE_FIXED)
				m_Type[i] = IDI_HDD;
			else
				m_Type[i] = IDI_USB;

			GetVolumeInformation(m_DriveName[i], buffer, BUFFER_LEN, NULL, NULL, NULL, NULL, 0);//lấy tên ổ đĩa của user vào buffer
			StrCpy(m_VolName[i], buffer); //chép tên vào volname tương ứng
		}
		else {
			if (nType == DRIVE_CDROM) //ổ CD
			{
				GetVolumeInformation(m_DriveName[i], buffer, BUFFER_LEN, NULL, NULL, NULL, NULL, 0);
				if (wcslen(buffer) == 0)
					StrCpy(m_VolName[i], (L"CD Rom"));
				else
					StrCpy(m_VolName[i], buffer);
				m_Type[i] = IDI_CD;
			}
			else {
				if (((i == 0) || (i == 1)) && (nType == DRIVE_REMOVABLE)) //Nếu là ổ mềm
				{
					StrCpy(m_VolName[i], L"3½ Floppy");
					m_Type[i] = IDI_USB; // lấy icon của USB luôn cho tiện 
				}
			}
		}
		if (wcslen(m_VolName[i]) == 0)  // tên theo hệ thống (người dùng kh đặt tên)
			StrCpy(m_DisplayName[i], (L"Local Disk"));
		else
			StrCpy(m_DisplayName[i], m_VolName[i]); //

		//Thêm vào phần sau hai dấu mở ngoặc. Ví dụ: (C:) như ta thường thấy WinXp (C:)
		StrCat(m_DisplayName[i], (L" ("));
		StrNCat(m_DisplayName[i], m_DriveName[i], 3);
		StrCat(m_DisplayName[i], (L")"));
	}
}


__int64 CDrive::GetSize(const int &i) {
	__int64 Size;
	SHGetDiskFreeSpaceEx(GetDriveName(i), NULL, (PULARGE_INTEGER)&Size, NULL);

	return Size;
}
LPWSTR CDrive::GetSizeStr(const int &i) {
	__int64 size = GetSize(i);
	return convert(size);
}

__int64 CDrive::GetFreeSize(const int &i) {
	__int64 nFreeSpace;
	GetDiskFreeSpaceEx(GetDriveName(i), NULL, NULL, (PULARGE_INTEGER)&nFreeSpace);

	return nFreeSpace;
}
LPWSTR CDrive::GetFreeSizeStr(const int &i) {
	__int64 nFreeSize = GetFreeSize(i);
	return convert(nFreeSize);
}