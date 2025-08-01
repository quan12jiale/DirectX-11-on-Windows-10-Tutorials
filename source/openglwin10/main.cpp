////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"
#include <QGuiApplication>

std::wstring getExecutablePath() {
	wchar_t path[MAX_PATH];
	const DWORD size = GetModuleFileNameW(NULL, path, MAX_PATH); // 获取可执行文件路径
	if (size < MAX_PATH)
		return std::wstring(path, size);
	return std::wstring();
}

std::wstring getExecutableDirectory() {
	std::wstring fullPath = getExecutablePath();
	size_t pos = fullPath.find_last_of(L"\\/");
	return (pos != std::wstring::npos) ? fullPath.substr(0, pos) : L"";
}

std::wstring getCurrentDirectory() {
	wchar_t currentName[MAX_PATH];
	const DWORD size = ::GetCurrentDirectoryW(MAX_PATH, currentName);
	if (size != 0) {
		if (size <= MAX_PATH) {
			return std::wstring(currentName, size);
		}
	}
	return std::wstring();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	// 构造QGuiApplication，因为QWindow::fromWinId(WId id) >> QGuiApplicationPrivate::platformIntegration()
	int argc = __argc;
	char** argv = __argv;
	QGuiApplication app(argc, argv);

	bool result;
	
	const std::wstring exeDir = getExecutableDirectory();
	const std::wstring currDir = getCurrentDirectory();
	if (!exeDir.empty() && exeDir != currDir) {
		// 用于ColorShaderClass::InitializeShader函数调用D3DCompileFromFile可以正确获取到相对路径
		result = ::SetCurrentDirectoryW(reinterpret_cast<const wchar_t*>(exeDir.data()));
	}
	
	// Create the system object.
	SystemClass* System = new SystemClass;

	// Initialize and run the system object.
	result = System->Initialize();
	if(result)
	{
		System->Run();
	}

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}