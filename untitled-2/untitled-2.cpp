#include "framework.h"
#include "untitled-2.h"

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hEdit;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void DoFileOpen(HWND hwnd);
void DoFileSave(HWND hwnd);
void AddMenus(HWND hwnd);
void OpenCppFile(HWND hwnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_UNTITLED2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_UNTITLED2));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UNTITLED2));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_UNTITLED2);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) {
		return FALSE;
	}

	hEdit = CreateWindowEx(0, L"EDIT", NULL,
		WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
		0, 0, 100, 100, hWnd, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	SendMessage(hEdit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(25, 25));

	AddMenus(hWnd);

	return TRUE;
}


void AddMenus(HWND hwnd) {
	HMENU hMenubar = CreateMenu();
	HMENU hMenu = CreateMenu();

	AppendMenu(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
	AppendMenu(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
	AppendMenu(hMenu, MF_STRING, IDM_FILE_SAVE, L"&Save");
	AppendMenu(hMenu, MF_STRING, IDM_FILE_EXIT, L"&Exit");
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&File");

	SetMenu(hwnd, hMenubar);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId) {
		case IDM_FILE_NEW:
			SetWindowText(hEdit, L"");
			break;
		case IDM_FILE_OPEN:
			OpenCppFile(hWnd);  // Add this line
			break;
		case IDM_FILE_SAVE:
			DoFileSave(hWnd);
			break;
		case IDM_FILE_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
				   break;
	case WM_SIZE:
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void DoFileOpen(HWND hwnd) {
	OPENFILENAME ofn;
	wchar_t szFile[100] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
	ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE) {
		HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE) {
			DWORD dwFileSize = GetFileSize(hFile, NULL);

			if (dwFileSize != INVALID_FILE_SIZE) {
				char* pFileContents = (char*)GlobalAlloc(GPTR, dwFileSize + 1);

				if (pFileContents) {
					DWORD dwRead;

					if (ReadFile(hFile, pFileContents, dwFileSize, &dwRead, NULL)) {
						pFileContents[dwRead] = '\0';

						int cchWideChar = MultiByteToWideChar(CP_UTF8, 0, pFileContents, -1, NULL, 0);
						wchar_t* pWideChar = (wchar_t*)GlobalAlloc(GPTR, cchWideChar * sizeof(wchar_t));

						if (pWideChar) {
							MultiByteToWideChar(CP_UTF8, 0, pFileContents, -1, pWideChar, cchWideChar);
							SetWindowText(hEdit, pWideChar);
							GlobalFree(pWideChar);
						}
					}
					GlobalFree(pFileContents);
				}
			}
			CloseHandle(hFile);
		}
	}
}

void OpenCppFile(HWND hwnd) {
	OPENFILENAME ofn;
	wchar_t szFile[MAX_PATH] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
	ofn.lpstrFilter = L"C++ Files\0*.cpp\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE) {
		HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE) {
			DWORD dwFileSize = GetFileSize(hFile, NULL);

			if (dwFileSize != INVALID_FILE_SIZE) {
				char* pFileContents = (char*)GlobalAlloc(GPTR, dwFileSize + 1);

				if (pFileContents) {
					DWORD dwRead;

					if (ReadFile(hFile, pFileContents, dwFileSize, &dwRead, NULL)) {
						pFileContents[dwRead] = '\0';

						// Convert to wide char and replace Unix line endings with Windows line endings
						int cchWideChar = MultiByteToWideChar(CP_UTF8, 0, pFileContents, -1, NULL, 0);
						wchar_t* pWideChar = (wchar_t*)GlobalAlloc(GPTR, cchWideChar * sizeof(wchar_t));

						if (pWideChar) {
							MultiByteToWideChar(CP_UTF8, 0, pFileContents, -1, pWideChar, cchWideChar);

							// Replace \n with \r\n
							std::wstring wideStr(pWideChar);
							size_t pos = 0;
							while ((pos = wideStr.find(L"\n", pos)) != std::wstring::npos) {
								wideStr.replace(pos, 1, L"\r\n");
								pos += 2;
							}

							SetWindowText(hEdit, wideStr.c_str());
							GlobalFree(pWideChar);
						}
					}
					GlobalFree(pFileContents);
				}
			}
			CloseHandle(hFile);
		}
	}
}


void DoFileSave(HWND hwnd) {
	OPENFILENAME ofn;
	wchar_t szFile[100];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetSaveFileName(&ofn) == TRUE) {
		HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE) {
			DWORD dwTextLength = GetWindowTextLength(hEdit);

			if (dwTextLength > 0) {
				wchar_t* pText = (wchar_t*)GlobalAlloc(GPTR, (dwTextLength + 1) * sizeof(wchar_t));

				if (pText) {
					GetWindowText(hEdit, pText, dwTextLength + 1);
					DWORD dwWritten;
					WriteFile(hFile, pText, (dwTextLength + 1) * sizeof(wchar_t), &dwWritten, NULL);
					GlobalFree(pText);
				}
			}
			CloseHandle(hFile);
		}
	}
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));

			return (INT_PTR)TRUE;
		}
		break;
	}

	return (INT_PTR)FALSE;
}