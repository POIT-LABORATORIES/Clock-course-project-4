#pragma comment(lib, "d2d1")
#pragma warning(disable: 4996)

// Clock.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Clock.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
D2D1_COLOR_F const COLOR_PINK = { 0.78f, 0.15f, 0.517f, 1.0f };
D2D1_COLOR_F const COLOR_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
HWND hMainWnd;
ID2D1Factory* pD2D1Factory{};
ID2D1HwndRenderTarget* pRT{};
ID2D1SolidColorBrush* m_brush;
ID2D1StrokeStyle* m_style;
int winSizeX = 500, winSizeY = 500;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                CreateDeviceIndependentResources();
void                CreateDeviceResources();
void                SafeRelease();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &pD2D1Factory);
    if (!SUCCEEDED(hr))
        return EXIT_FAILURE;

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLOCK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLOCK));

    MSG msg;

    // Main message loop:
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            DispatchMessage(&msg);
        }
        else
        {
            msg.message = WM_PAINT;
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
        {
            break;
        }
    }

    SafeRelease();

    return (int)msg.wParam;
}

void DrawClock()
{
    auto size = pRT->GetSize();

    auto radius = std::max(200.0f,
        std::min(size.width, size.height)) / 2.0f - 50.0f;

    auto const offset = D2D1::SizeF(2.0f, 2.0f);

    auto translation = D2D1::Matrix3x2F::Translation(
        size.width / offset.width,
        size.height / offset.height);

    pRT->SetTransform(translation);

    pRT->DrawEllipse(
        D2D1::Ellipse(D2D1::Point2F(), radius, radius),
        m_brush,
        radius / 20.f);

    // Get local time.
    SYSTEMTIME time;
    GetLocalTime(&time);

    auto secondAngle = (time.wSecond + (time.wMilliseconds / 1000.0f)) * 6.0f;
    auto minuteAngle = time.wMinute * 6.0f + secondAngle / 60.0f;
    auto hourAngle = time.wHour % 12 * 30.0f + minuteAngle / 12.0f;

    pRT->SetTransform(D2D1::Matrix3x2F::Rotation(secondAngle) * translation);

    pRT->DrawLine(
        D2D1::Point2F(),
        D2D1::Point2F(0.0f, -(radius * 0.75f)),
        m_brush,
        radius / 25.0f,
        m_style);

    pRT->SetTransform(D2D1::Matrix3x2F::Rotation(minuteAngle) * translation);

    pRT->DrawLine(
        D2D1::Point2F(),
        D2D1::Point2F(0.0f, -(radius * 0.75f)),
        m_brush,
        radius / 15.0f,
        m_style);

    pRT->SetTransform(D2D1::Matrix3x2F::Rotation(hourAngle) * translation);

    pRT->DrawLine(
        D2D1::Point2F(),
        D2D1::Point2F(0.0f, -(radius * 0.5f)),
        m_brush,
        radius / 10.0f,
        m_style);
}

void Draw()
{
    pRT->BeginDraw();

    pRT->Clear(COLOR_WHITE);

    DrawClock();

    pRT->EndDraw();
}

// Release Direct2D objects.
void SafeRelease()
{
    if (m_brush != nullptr)
        m_brush->Release();

    if (m_style != nullptr)
        m_style->Release();

    if (pD2D1Factory != nullptr)
        pD2D1Factory->Release();

    if (pRT != nullptr)
        pRT->Release();
}

HRESULT CreateRenderTarget()
{
    return pD2D1Factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hMainWnd, D2D1::SizeU(winSizeX, winSizeY)),
        &pRT);
}

void CreateDeviceIndependentResources()
{
    D2D1_STROKE_STYLE_PROPERTIES style = {};
    style.startCap = D2D1_CAP_STYLE_ROUND;
    style.endCap = D2D1_CAP_STYLE_TRIANGLE;

    pD2D1Factory->CreateStrokeStyle(
        style,      // Stroke style props.
        nullptr,    // Dashes.
        0,          // Dashes count.
        &m_style    // Stroke style.
    );
}

void CreateDeviceResources()
{
    pRT->CreateSolidColorBrush(
        COLOR_PINK,
        D2D1::BrushProperties(0.8f),
        &m_brush);
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLOCK));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    hMainWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, winSizeX, winSizeY, nullptr, nullptr, hInstance, nullptr);

    if (!hMainWnd)
    {
        return FALSE;
    }


    // Defining Direct2D objects.
    if (!SUCCEEDED(CreateRenderTarget()))
        return EXIT_FAILURE;

    // Create resources.
    CreateDeviceResources();
    CreateDeviceIndependentResources();

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SIZE:
    {
        pRT->Resize(D2D1::SizeU(LOWORD(lParam), HIWORD(lParam)));
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        Draw();

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}