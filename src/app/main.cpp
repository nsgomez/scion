#include <Windows.h>
#include <cGZFramework.h>
#include <cRZCmdLine.h>

static int AppStart(cRZCmdLine const& cmdLine, bool unknown)
{
	cGZFramework* framework = new cGZFramework();
	// TODO: set up app

	framework->sSetFramework(framework);
	// framework->sSetApplication(NULL); // TODO

	int returnCode = cGZFramework::Main(cmdLine, unknown);
	// TODO

	return returnCode;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE unused, PSTR cmdLine, int cmdShow)
{
	AppStart(cRZCmdLine() /* TODO */, false);
	return 0;
}