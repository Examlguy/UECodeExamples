#include "WindowsFunctionLibrary.h"

#if PLATFORM_WINDOWS
#include <Windows.h>
#include <WinUser.h>
#endif

void UWindowsFunctionLibrary::RemoveTopBarOfWindow()
{
#if PLATFORM_WINDOWS
	HWND hParent = GetUEWindowHandle();
	
	LONG_PTR style = GetWindowLongPtr(hParent, GWL_STYLE);

	// Remove the WS_CAPTION (title bar) and WS_THICKFRAME (resizable border) styles
	style &= ~(WS_CAPTION | WS_THICKFRAME);

	// Set the new window style
	SetWindowLongPtr(hParent, GWL_STYLE, style);

	// Apply the changes and redraw the window
	SetWindowPos(hParent, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
#endif
}

void UWindowsFunctionLibrary::ShowSystemMessageBox(FText HeaderText, FText MessageText)
{
#if PLATFORM_WINDOWS
	MessageBoxW(GetUEWindowHandle(), *MessageText.ToString(), *HeaderText.ToString(), MB_OK | MB_ICONINFORMATION);
#endif
}

void UWindowsFunctionLibrary::ShowSystemProgressBar()
{
#if PLATFORM_WINDOWS
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&icex);

	HWND hParent = GetUEWindowHandle();
	if (!hParent)
		return;

	RECT parentRect;
	GetWindowRect(hParent, &parentRect);

	int parentWidth = parentRect.right - parentRect.left;
	int parentHeight = parentRect.bottom - parentRect.top;

	// Desired size of your popup
	int winWidth = 360;
	int winHeight = 80;

	// Compute centered position
	int x = parentRect.left + (parentWidth - winWidth) / 2;
	int y = parentRect.top + (parentHeight - winHeight) / 2;

	HWND hWnd = CreateWindowExW(
		0, L"STATIC", L"Loading...",  // Window title here
		WS_CAPTION | WS_VISIBLE | WS_BORDER,
		x, y, 400, 100,
		hParent, nullptr, GetModuleHandle(nullptr), nullptr
	);

	RECT wndRect;
	GetClientRect(hWnd, &wndRect);

	int wndWidth = wndRect.right - wndRect.left;
	int wndHeight = wndRect.bottom - wndRect.top;
	
	// Compute centered position
	int wndX = wndRect.left + (wndWidth - 150) / 2;
	int wndY = wndRect.top + wndHeight / 2;
	
	// Then create the progress bar as a child of hWnd
	HWND hProgress = CreateWindowExW(
		0, PROGRESS_CLASS, nullptr,
		WS_CHILD | WS_VISIBLE,
		wndX, wndY, 300, 20,
		hWnd, nullptr, GetModuleHandle(nullptr), nullptr
	);

	SendMessageW(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
	SendMessageW(hProgress, PBM_SETSTEP, (WPARAM)1, 0);

	for (int i = 0; i <= 100; i++)
	{
		SendMessageW(hProgress, PBM_SETPOS, i, 0);
		Sleep(10);
	}

	DestroyWindow(hProgress);
	DestroyWindow(hWnd);
#endif
}

HWND UWindowsFunctionLibrary::GetUEWindowHandle()
{
#if PLATFORM_WINDOWS
	TSharedPtr<SWindow> MainWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (!MainWindow.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No active UE window found"));
		return nullptr;
	}

	TSharedPtr<FGenericWindow> NativeWindow = MainWindow->GetNativeWindow();
	if (!NativeWindow.IsValid())
		return nullptr;

	return static_cast<HWND>(NativeWindow->GetOSWindowHandle());
#endif
}
