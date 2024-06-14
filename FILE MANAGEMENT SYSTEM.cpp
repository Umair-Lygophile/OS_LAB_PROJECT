#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <windows.h>

using namespace std;

const int BLOCK_SIZE = 1024;
const int NUM_BLOCKS = 1000;

struct File {
    string name;
    int size;
    vector<int> blocks;
};

struct Directory {
    map<string, File> files;
};

class FileSystem {
private:
    vector<bool> allocated_blocks;
    Directory root_directory;

public:
    FileSystem() {
        allocated_blocks.resize(NUM_BLOCKS, false);
    }

    void createFile(const string& filename, int size) {
        if (root_directory.files.find(filename) != root_directory.files.end()) {
            MessageBox(NULL, L"File already exists", L"Error", MB_OK);
            return;
        }

        File new_file;
        new_file.name = filename;
        new_file.size = size;
        int num_blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for (int i = 0; i < num_blocks_needed; ++i) {
            int block_index = findFreeBlock();
            if (block_index == -1) {
                MessageBox(NULL, L"Disk space full", L"Error", MB_OK);
                return;
            }
            new_file.blocks.push_back(block_index);
            allocated_blocks[block_index] = true;
        }

        root_directory.files[filename] = new_file;
        MessageBox(NULL, L"File created successfully", L"Success", MB_OK);
    }

    void deleteFile(const string& filename) {
        if (root_directory.files.find(filename) == root_directory.files.end()) {
            MessageBox(NULL, L"File not found", L"Error", MB_OK);
            return;
        }

        File& file = root_directory.files[filename];
        for (int block : file.blocks) {
            allocated_blocks[block] = false;
        }

        root_directory.files.erase(filename);
        MessageBox(NULL, L"File deleted successfully", L"Success", MB_OK);
    }

    void readFile(const string& filename) {
        if (root_directory.files.find(filename) == root_directory.files.end()) {
            MessageBox(NULL, L"File not found", L"Error", MB_OK);
            return;
        }

        File& file = root_directory.files[filename];
        wstring msg = L"Reading data from file: " + wstring(filename.begin(), filename.end()) + L"\n";
        for (int block : file.blocks) {
            msg += L"Reading data from block " + to_wstring(block) + L"\n";
        }
        MessageBox(NULL, msg.c_str(), L"Read File", MB_OK);
    }

    void writeFile(const string& filename, const string& data) {
        if (root_directory.files.find(filename) == root_directory.files.end()) {
            MessageBox(NULL, L"File not found", L"Error", MB_OK);
            return;
        }

        File& file = root_directory.files[filename];
        int data_index = 0;
        wstring msg = L"Writing data to file: " + wstring(filename.begin(), filename.end()) + L"\n";
        for (int block : file.blocks) {
            msg += L"Writing data to block " + to_wstring(block) + L"\n";
        }
        MessageBox(NULL, msg.c_str(), L"Write File", MB_OK);
    }

    void viewFileContent(const string& filename) {
        if (root_directory.files.find(filename) == root_directory.files.end()) {
            MessageBox(NULL, L"File not found", L"Error", MB_OK);
            return;
        }

        File& file = root_directory.files[filename];
        wstring msg = L"Content of file " + wstring(filename.begin(), filename.end()) + L":\n";
        for (int block : file.blocks) {
            msg += L"Block " + to_wstring(block) + L": <content goes here>\n";
        }
        MessageBox(NULL, msg.c_str(), L"View File Content", MB_OK);
    }

private:
    int findFreeBlock() {
        for (int i = 0; i < NUM_BLOCKS; ++i) {
            if (!allocated_blocks[i]) {
                return i;
            }
        }
        return -1;
    }
};

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static FileSystem fs;
    static HWND hFileName, hFileSize, hFileData;
    switch (message) {
    case WM_CREATE: {
        CreateWindowW(L"Static", L"Filename:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);
        hFileName = CreateWindowW(L"Edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 20, 150, 20, hwnd, NULL, NULL, NULL);

        CreateWindowW(L"Static", L"File Size:", WS_VISIBLE | WS_CHILD, 20, 50, 100, 20, hwnd, NULL, NULL, NULL);
        hFileSize = CreateWindowW(L"Edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 50, 150, 20, hwnd, NULL, NULL, NULL);

        CreateWindowW(L"Button", L"Create File", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 20, 80, 100, 30, hwnd, (HMENU)1, NULL, NULL);
        CreateWindowW(L"Button", L"Delete File", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 130, 80, 100, 30, hwnd, (HMENU)2, NULL, NULL);
        CreateWindowW(L"Button", L"Read File", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 240, 80, 100, 30, hwnd, (HMENU)3, NULL, NULL);

        CreateWindowW(L"Static", L"File Data:", WS_VISIBLE | WS_CHILD, 20, 120, 100, 20, hwnd, NULL, NULL, NULL);
        hFileData = CreateWindowW(L"Edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 120, 150, 20, hwnd, NULL, NULL, NULL);

        CreateWindowW(L"Button", L"Write File", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 20, 150, 100, 30, hwnd, (HMENU)4, NULL, NULL);
        CreateWindowW(L"Button", L"View Content", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 130, 150, 100, 30, hwnd, (HMENU)5, NULL, NULL);
        break;
    }
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        WCHAR fileName[256];
        GetWindowTextW(hFileName, fileName, 256);
        string filename(fileName, fileName + wcslen(fileName));

        switch (wmId) {
        case 1: {
            WCHAR fileSize[256];
            GetWindowTextW(hFileSize, fileSize, 256);
            int size = _wtoi(fileSize);
            fs.createFile(filename, size);
            break;
        }
        case 2: {
            fs.deleteFile(filename);
            break;
        }
        case 3: {
            fs.readFile(filename);
            break;
        }
        case 4: {
            WCHAR fileData[256];
            GetWindowTextW(hFileData, fileData, 256);
            string data(fileData, fileData + wcslen(fileData));
            fs.writeFile(filename, data);
            break;
        }
        case 5: {
            fs.viewFileContent(filename);
            break;
        }
        }
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    return 0;
}

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSW wc = {0};
    wc.lpszClassName = L"MyWindowClass";
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowW(L"MyWindowClass", L"File Management System", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 400, 300, NULL, NULL, hInstance, NULL);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}
