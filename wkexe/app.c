
#include "app.h"
#include "cmdline.h"
#include "path.h"

#include <wke.h>
#include <stdio.h>
#include <stdlib.h>

#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


BOOL FixupHtmlFileUrl(LPCWSTR pathOption, LPWSTR urlBuffer, size_t bufferSize)
{
    WCHAR htmlPath[MAX_PATH + 1] = { 0 };

    if (pathOption[0] == 0)
    {
        do
        {
            GetWorkingPath(htmlPath, MAX_PATH, L"index.html");
            if (PathFileExistsW(htmlPath))
                break;

            GetWorkingPath(htmlPath, MAX_PATH, L"main.html");
            if (PathFileExistsW(htmlPath))
                break;

            GetWorkingPath(htmlPath, MAX_PATH, L"wkexe.html");
            if (PathFileExistsW(htmlPath))
                break;    

            GetProgramPath(htmlPath, MAX_PATH, L"index.html");
            if (PathFileExistsW(htmlPath))
                break;    

            GetProgramPath(htmlPath, MAX_PATH, L"main.html");
            if (PathFileExistsW(htmlPath))
                break;

            GetProgramPath(htmlPath, MAX_PATH, L"wkexe.html");
            if (PathFileExistsW(htmlPath))
                break;

            return FALSE;
        }
        while (0);

        _snwprintf(urlBuffer, bufferSize, L"file:///%s", htmlPath);
        return TRUE;
    }

    else//if (!wcsstr(pathOption, L"://"))
    {
        do
        {
            GetWorkingPath(htmlPath, MAX_PATH, pathOption);
            if (PathFileExistsW(htmlPath))
                break;

            GetProgramPath(htmlPath, MAX_PATH, pathOption);
            if (PathFileExistsW(htmlPath))
                break;

            return FALSE;
        }
        while (0);

        _snwprintf(urlBuffer, bufferSize, L"file:///%s", htmlPath);
        return TRUE;
    }

    return FALSE;
}

BOOL FixupHtmlUrl(Application* app)
{
    LPWSTR htmlOption = app->options.htmlFile;
    WCHAR htmlUrl[MAX_PATH + 1] = { 0 };

    // ���� :// ˵����������URL
    if (wcsstr(htmlOption, L"://"))
    {
        wcsncpy(app->url, htmlOption, MAX_PATH);
        return TRUE;
    }

    // ����������URL����ȫ֮
    if (FixupHtmlFileUrl(htmlOption, htmlUrl, MAX_PATH))
    {
        wcsncpy(app->url, htmlUrl, MAX_PATH);
        return TRUE;
    }

    // �޷����������URL������
    return FALSE;
}

BOOL ProcessOptions(Application* app)
{
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    ParseOptions(argc, argv, &app->options);
    LocalFree(argv);

    return TRUE;
}

// �ص�������˹رա����� true �����ٴ��ڣ����� false ʲô��������
bool HandleWindowClosing(wkeWebView webWindow, void* param)
{
    Application* app = (Application*)param;
    return IDYES == MessageBoxW(NULL, L"ȷ��Ҫ�˳�������", L"wkexe", MB_YESNO|MB_ICONQUESTION);
}

// �ص�������������
void HandleWindowDestroy(wkeWebView webWindow, void* param)
{
    Application* app = (Application*)param;
    app->window = NULL;
    PostQuitMessage(0);
}

// �ص����ĵ����سɹ�
void HandleDocumentReady(wkeWebView webWindow, void* param, const wkeDocumentReadyInfo* info)
{
    //��ҳ����سɹ�(��iframe)
    if (info->frameJSState == info->mainFrameJSState)
        wkeShowWindow(webWindow, TRUE);
}

// �ص���ҳ�����ı�
void HandleTitleChanged(wkeWebView webWindow, void* param, const wkeString title)
{
    wkeSetWindowTitleW(webWindow, wkeGetStringW(title));
}

// �ص��������µ�ҳ�棬����˵������ window.open ���ߵ���� <a target="_blank" .../>
wkeWebView HandleCreateView(wkeWebView webWindow, void* param, const wkeNewViewInfo* info)
{
    wkeWebView newWindow = wkeCreateWebWindow(WKE_WINDOW_TYPE_POPUP, NULL, info->x, info->y, info->width, info->height);
    wkeShowWindow(newWindow, SW_SHOW);
    return newWindow;
}

// ������ҳ�洰��
BOOL CreateWebWindow(Application* app)
{
    if (app->options.transparent)
        app->window = wkeCreateWebWindow(WKE_WINDOW_TYPE_TRANSPARENT, NULL, 0, 0, 640, 480);
    else
        app->window = wkeCreateWebWindow(WKE_WINDOW_TYPE_POPUP, NULL, 0, 0, 640, 480);

    if (!app->window)
        return FALSE;

    wkeOnWindowClosing(app->window, HandleWindowClosing, app);
    wkeOnWindowDestroy(app->window, HandleWindowDestroy, app);
    wkeOnDocumentReady(app->window, HandleDocumentReady, app);
    wkeOnTitleChanged(app->window, HandleTitleChanged, app);
    wkeOnCreateView(app->window, HandleCreateView, app);

    wkeMoveToCenter(app->window);
    wkeLoadURLW(app->window, app->url);

    return TRUE;
}

void PrintHelpAndQuit(Application* app)
{
    PrintHelp();
    PostQuitMessage(0);
}

void RunMessageLoop(Application* app)
{
    MSG msg = { 0 };
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void RunApplication(Application* app)
{
    memset(app, 0, sizeof(Application));

    if (!ProcessOptions(app))
    {
        PrintHelpAndQuit(app);
        return;
    }

    if (!FixupHtmlUrl(app))
    {
        PrintHelpAndQuit(app);
        return;
    }

    if (!CreateWebWindow(app))
    {
        PrintHelpAndQuit(app);
        return;
    }

    RunMessageLoop(app);
}

void QuitApplication(Application* app)
{
    if (app->window)
    {
        wkeDestroyWebWindow(app->window);
        app->window = NULL;
    }
}
