#include <iostream>
#include <string>
#include <windows.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <commctrl.h>
#include <tchar.h>

#pragma comment(lib, "winmm.lib")


std::wstring selectFile() {
    LPCWSTR title = L"DSoundModule";
    SetConsoleTitle(title);
    OPENFILENAMEW ofn;
    wchar_t filename[MAX_PATH] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"File .mp3\0*.mp3\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"Select your .mp3 file";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameW(&ofn) != TRUE) {
        throw std::runtime_error("Error selecting file");
    }

    return std::wstring(filename);
}

int main() {
    std::cout << "a module that plays MPEG files via DirectSound" << std::endl;

    try {
        std::wstring selectedFile = selectFile();

        MCI_OPEN_PARMSW mciOpenParms = { 0 };
        mciOpenParms.lpstrDeviceType = L"mpegvideo";
        mciOpenParms.lpstrElementName = selectedFile.c_str();
        DWORD dwFlags = MCI_OPEN_ELEMENT | MCI_WAIT;
        std::wcout << L"Playing file: " << selectedFile << std::endl;

        if (mciSendCommandW(NULL, MCI_OPEN, dwFlags, reinterpret_cast<DWORD_PTR>(&mciOpenParms)) != 0) {
            throw std::runtime_error("Error opening file");
        }

        DWORD dwDeviceID = mciOpenParms.wDeviceID;

        MCI_PLAY_PARMS mciPlayParms = { 0 };
        dwFlags = MCI_NOTIFY | MCI_WAIT;
        if (mciSendCommand(dwDeviceID, MCI_PLAY, dwFlags, reinterpret_cast<DWORD_PTR>(&mciPlayParms)) != 0) {
            throw std::runtime_error("Error playing file");
        }

        INPUT_RECORD irInBuf[128];
        DWORD cNumRead;

        if (mciSendCommand(dwDeviceID, MCI_CLOSE, 0, NULL) != 0) {
            throw std::runtime_error("Error closing file");
        }

        std::cout << "Playing stopped. Program closed." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}