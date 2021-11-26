#include <iostream>
#include "SFML/Graphics.hpp"    
#include <Windows.h>
#include <Dwmapi.h>
#include <ShObjIdl.h>
#include <thread>

#pragma comment (lib, "Dwmapi.lib")

#define ID_TRAY_APP_ICON        9999
#define ID_TRAY_EXIT_CONTEXT_MENU_ITEM  3000
#define WM_TRAYICON ( WM_USER + 1 )

UINT WM_TASKBAR = 0;

HMENU Hmenu;

// Animations (rectSource.top)
enum animation_default {
	walking = 0,
	running = 130,
	breeze = 260,
	bicycle = 385,
	wave = 520,
	fallover = 650,
	dancing = 775,
	jumping = 900,
	karate = 1035,
	wave2 = 1160,
	running2 = 1290,
	detach = 1415,
	explosion = 1538,
	dance1 = 1680,
	miku = 1818,
	dance2 = 1945,
	dance3 = 2075,
	dance4 = 2200,
	dance5 = 2330,
	swimming = 2440
};

enum animation_modern {
	gumi = 67,
	gumi_running = 130,
	gumi_breeze = 260,
	gumi_bicycle = 385,
	gumi_wave = 520,
	gumi_fallover = 650,
	cirno_waiting = 1330,
	cirno_stepping = 1530,
	cirno_jumping = 1795,
	cirno_waving = 2100,
	cirno_hula = 2350,
	cirno_windmill = 2560,
	green = 1680,
	pink = 1818,
	red = 1945,
	orange = 2075,
};

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);


int main(int argc, char** argv) {
	//configuration options
	int animation = cirno_stepping;
	int sprite_width = 220;
	int sprite_length = 256;
	int rate = 105;
	int frames = 8;
	int sheet_width = 880;
	int sheet_width2x = sheet_width * 2;
	int rectangle_left = sheet_width2x / frames;
	char stylesheet[] = "images/sheet2.png";

	HWND sysTrayHwnd;

	// CONSOLE
/*
	FILE* pFile;
	AllocConsole();
	SetConsoleTitle(L"WaifuDance Console");
	freopen_s(&pFile, "conin$", "r", stdin);
	freopen_s(&pFile, "conout$", "w", stdout);
	freopen_s(&pFile, "conout$", "w", stderr);
*/
	// ANIMATION WINDOW
	sf::RenderWindow renderWindow(sf::VideoMode(220, 256), "Waifu Dance", sf::Style::None);
	HWND hwnd = renderWindow.getSystemHandle();

	// Transparent Window
	MARGINS margins;
	margins.cxLeftWidth = -1;

	SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	DwmExtendFrameIntoClientArea(hwnd, &margins);

	// Keep on top
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	// Remove icon from taskbar
	ITaskbarList* pTaskList = NULL;
	HRESULT initRet = CoInitialize(NULL);
	HRESULT createRet = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList, (LPVOID*)&pTaskList);

	if (createRet == S_OK)
	{
		pTaskList->DeleteTab(hwnd);
		pTaskList->Release();
	}

	CoUninitialize();

	// SYSTEM TRAY

	// Create window specifically for systray icon and pop up menu 
	HICON appIcon = (HICON)LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	HINSTANCE hInst = GetModuleHandle(NULL);
	LPCTSTR szClassName = L"WaifuDance Menu";
	MSG message;
	WNDCLASSEX wincl;
	ZeroMemory(&wincl, sizeof(WNDCLASSEX));

	WM_TASKBAR = RegisterWindowMessageA("TaskbarCreated");

	wincl.hInstance = hInst;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WinProc;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof(WNDCLASSEX);

	wincl.hIcon = appIcon;
	wincl.hIconSm = appIcon;
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;                 
	wincl.cbClsExtra = 0;                      /* No extra bytes after the window class ZeroMemory should make this redundant */
	wincl.cbWndExtra = 0;                      
	wincl.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;

	if (!RegisterClassEx(&wincl)) {
		std::cout << GetLastError() << std::endl;
		return 0;
	}
	

	sysTrayHwnd = CreateWindowEx(
		0,                   /* Extended possibilites for variation */
		szClassName,         /* Classname */
		szClassName,	     /* Title Text */
		WS_OVERLAPPEDWINDOW, /* default window */
		CW_USEDEFAULT,       /* Windows decides the position */
		CW_USEDEFAULT,       /* where the window ends up on the screen */
		544,                 /* The programs width */
		375,                 /* and height in pixels */
		HWND_DESKTOP,        /* The window is a child-window to desktop */
		NULL,                /* No menu */
		hInst,				 /* Program Instance handler */
		NULL                 /* No Window Creation data */
	);
	
	std::cout << GetLastError() << std::endl;
	ShowWindow(sysTrayHwnd, SW_SHOWDEFAULT);

	NOTIFYICONDATA niData;
	ZeroMemory(&niData, sizeof(NOTIFYICONDATA));


	niData.cbSize = sizeof(NOTIFYICONDATA);
	niData.hWnd = sysTrayHwnd;
	niData.uID = ID_TRAY_APP_ICON;
	niData.hIcon = appIcon;
	niData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	niData.uCallbackMessage = WM_TRAYICON;

	Shell_NotifyIcon(NIM_ADD, &niData);

	// RENDER LOOP 
	sf::Event event;
	sf::Texture texture;
	sf::IntRect rectSource(0, animation, sprite_width, sprite_length);
	texture.loadFromFile(stylesheet);

	sf::Sprite sprite(texture, rectSource);
	sf::Clock clock;

	bool grabbed = false;
	sf::Vector2i grabbedOffset;

	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	
	while (renderWindow.isOpen()) {
		while (renderWindow.pollEvent(event)) {
			if (event.type == sf::Event::EventType::Closed) {
				renderWindow.close();
			}
			else if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					grabbed = true;
					grabbedOffset = renderWindow.getPosition() - sf::Mouse::getPosition();
					std::cout << "[!] grabbed" << std::endl;
				}
			}
			else if (event.type == sf::Event::MouseButtonReleased) {
				if (event.mouseButton.button == sf::Mouse::Left)
					grabbed = false;
				std::cout << "[!] released" << std::endl;
			}
			else if (event.type == sf::Event::MouseMoved && grabbed) {
				renderWindow.setPosition(sf::Mouse::getPosition() + grabbedOffset);
			}
		}

		if (clock.getElapsedTime().asMilliseconds() >= rate) {
			sprite.setTextureRect(rectSource);
			rectSource.left += 220;

			if (rectSource.left == 1760) {
				rectSource.left = 0;
			}

			clock.restart();
		}

		renderWindow.clear(sf::Color::Transparent);
		renderWindow.draw(sprite);
		renderWindow.display();
	}
	
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_TASKBAR && !IsWindowVisible(hWnd))
	{
		return 0;
	}

	switch (message)                  /* handle the messages */
	{
	case WM_ACTIVATE:
		break;
	case WM_CREATE:
		
		ShowWindow(hWnd, SW_HIDE);
		Hmenu = CreatePopupMenu();
		AppendMenu(Hmenu, MF_STRING, 100, TEXT("Exit The Demo"));

		break;

	case WM_SYSCOMMAND:
		/*In WM_SYSCOMMAND messages, the four low-order bits of the wParam parameter
		are used internally by the system. To obtain the correct result when testing the value of wParam,
		an application must combine the value 0xFFF0 with the wParam value by using the bitwise AND operator.*/

		switch (wParam & 0xFFF0)
		{
		case SC_MINIMIZE:
		case SC_CLOSE:
			return 0;
			break;
		}
		break;


		// Our user defined WM_SYSICON message.
	case WM_TRAYICON:
	{

		switch (wParam)
		{
		case ID_TRAY_APP_ICON:
			SetForegroundWindow(hWnd);

			break;
		}


		if (lParam == WM_LBUTTONUP)
		{
			break;
		}
		else if (lParam == WM_RBUTTONDOWN)
		{
			// Get current mouse position.
			POINT curPoint;
			GetCursorPos(&curPoint);
			SetForegroundWindow(hWnd);

			// TrackPopupMenu blocks the app until TrackPopupMenu returns

			UINT clicked = TrackPopupMenu(Hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, hWnd, NULL);



			SendMessage(hWnd, WM_NULL, 0, 0); // send benign message to window to make sure the menu goes away.
			if (clicked == 100)
			{
				// quit the application.
				PostQuitMessage(0);
			}
		}
	}
	break;

	// intercept the hittest message..
	case WM_NCHITTEST:
	{
		UINT uHitTest = DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam);
		if (uHitTest == HTCLIENT)
			return HTCAPTION;
		else
			return uHitTest;
	}

	case WM_CLOSE:
		return 0;
		break;

	case WM_DESTROY:

		PostQuitMessage(0);
		break;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

