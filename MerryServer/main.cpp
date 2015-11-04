//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include <iostream>

#include "ServerEngine.h"
#include "GameWorld.h"

//#include <vld.h>

int main(int argc, char** argv)
{
	if (vgs::ServerEngine::GetInstance()->Startup(new vgs::GameWorld(6)))
	{
		vgs::ServerEngine::GetInstance()->Run();
		vgs::ServerEngine::GetInstance()->Shutdown();
		vgs::ServerEngine::FreeInstance();
	}
	else
	{
		std::cout << "Couldn't start server" << std::endl;
	}

	//_CrtDumpMemoryLeaks();
	return 0;
}