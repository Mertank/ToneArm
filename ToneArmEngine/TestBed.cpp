//#include <vld.h>

#include "Engine.h"
#include "../Merrymen/MainMenuScene.h"

int main( int argc, const char* argv[] ) {
	vgs::Engine::Initialize();

	return vgs::Engine::RunScene( merrymen::MainMenuScene::Create() );
}