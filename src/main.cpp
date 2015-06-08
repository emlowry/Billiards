#include "PoolTable.h"
#include "Engine.h"

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// create a poolTable
	PoolTable* poolTable = new PoolTable();

	// set up the engine
	Engine::SetWindowTitle("Billiards");
	Engine::Action start = [&](){ poolTable->Start(); };
	Engine::RegisterStartAction(start);
	Engine::Action update = [&](){ poolTable->Update(); };
	Engine::RegisterUpdateAction(update);
	Engine::Action draw = [&](){ poolTable->Draw(); };
	Engine::RegisterDrawAction(draw);
	Engine::Action stop = [&](){ poolTable->Stop(); };
	Engine::RegisterStopAction(stop);

	// Play the game!
	Engine::Run();

	// clean up
	delete poolTable;
	return 0;
}