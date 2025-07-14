////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"
#ifdef ENABLE_QRHI
#include <QGuiApplication>
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
#ifdef ENABLE_QRHI
	// 构造QGuiApplication，因为QWindow::fromWinId(WId id) >> QGuiApplicationPrivate::platformIntegration()
	int argc = __argc;
	char** argv = __argv;
	QGuiApplication app(argc, argv);
#endif
	SystemClass* System;
	bool result;


	// Create the system object.
	System = new SystemClass;

	// Initialize and run the system object.
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}