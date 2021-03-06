#include <iostream>
#include <ctime>
#include <cstdlib>

#include "StageState.h"
#include "Game.h"
#include "InputManager.h"
#include "Camera.h"
#include "DialogWindow.h"
#include "SaveComponent.h"
#include "menu/InGameQuests.h"

GameObject* StageState::player = nullptr;
std::vector<GameObject*> StageState::objectArray = std::vector<GameObject*>();

StageState::StageState(std::string mode_, int sizeX, int sizeY):
	State(),
	mode(mode_),
	tileSet(nullptr),
	paused(false),
	tileMap(nullptr),
	windowArray(),
	roomArray(nullptr),
	healthBar(nullptr),
	energyBar(nullptr),
	inGameMenu(nullptr)
{
	int random;
	bool endRandom;
	srand(time(NULL));
	SaveComponent _("teste.txt");

	roomArray = new int*[sizeX];
	for(int i = 0; i < sizeX; i++){
		roomArray[i] = new int[sizeY];
	}

	for(int i = 0; i < sizeX; i++){
		for(int j = 0; j < sizeY; j++){
			roomArray[i][j] = -1;
		}
	}

	tileSet = new TileSet(64, 64, "Tileset3D.png", 9, 9);
	tileMap = new TileMap("resources/map/tileMap.txt", tileSet);
	//Camera::GetInstance().maxPos = Vec2(tileMap->GetWidth()*tileMap->GetTileWidth(),
	//									tileMap->GetHeight()*tileMap->GetTileHeight());
	//Camera::GetInstance().maxPos = Vec2(tileMap.m_nodes[0]->m_data.tileMap->GetWidth()*tileMap.m_nodes[0]->m_data.tileMap->GetTileWidth(),
	//									tileMap.m_nodes[0]->m_data.tileMap->GetHeight()*tileMap.m_nodes[0]->m_data.tileMap->GetTileHeight());
	if(mode == "Lancelot")
		player = new Lancelot(200, 1000);
	else if(mode == "Gallahad")
		player = new Gallahad(200, 1000);
	Camera::GetInstance().Follow(player);
	AddObject(player);
	AddObject(new Notfredo(800, 1280));

	CreateBars(player->name);
}

StageState::~StageState()
{
	delete[] roomArray;
	music.Stop();
	player = nullptr;
	delete tileMap;
	for(unsigned i = 0; i < objectArray.size(); i++)
	{
		delete objectArray[i];
	}
	objectArray.clear();
	windowArray.clear();
}

GameObject* StageState::GetPlayer()
{
	return player;
}

void StageState::CreateBars(std::string playerName)
{
	healthBar = new LoadingBar("lifebar_"+playerName+".png",11);
	energyBar = new LoadingBar("energybar_"+playerName+".png",174,28,5);
}

void StageState::AddObject(GameObject* ptr)
{
	objectArray.emplace_back(ptr);
}

void StageState::AddObjectAsFirst(GameObject* ptr)
{
	objectArray.emplace(objectArray.begin(), ptr);
}

void StageState::RemoveObject(GameObject* ptr)
{
	for(unsigned i = 0; i < objectArray.size(); i++)
	{
		if(ptr == objectArray[i])
		{
			delete objectArray[i];
			objectArray.erase(objectArray.begin()+i);
			break;
		}
	}
}

void StageState::RemoveWindow(Window* ptr)
{
	windowArray.emplace_back(ptr);
}

void StageState::Pause()
{
	paused = true;
	inGameMenu = new InGameQuests();
	inGameMenu->LoadAssets();
}

void StageState::Resume()
{
	paused = false;
	if (inGameMenu!=nullptr)
	{
		delete inGameMenu;
		inGameMenu = nullptr;
	}
}

void StageState::LoadAssets()
{
	music.Open("stageState.ogg");
	bg.Open("LancelotIdle.png");
	bg.Open("LancelotRunning.png");
	bg.Open("Melee.png");
	bg.Open("NotfredoRunning.png");
	bg.Open("NotfredoIdle.png");
	bg.Open("Tileset3D.png");
	music.Play(-1);
}

void StageState::Update()
{
	if(!paused)
	{
		HandleInput();
		UpdateGame();
	}
	else
	{
		if (inGameMenu!=nullptr)
		{
			inGameMenu->Update();
			if (inGameMenu->QuitRequested()==true)
				Resume();
		}
		else
			std::cerr << "ERRO: Menu in-game nao existe" <<std::endl;
	}
}

void StageState::UpdateGame()
{
	for(unsigned i = 0; i < objectArray.size(); i++)
	{
		objectArray[i]->Update(tileMap, Game::GetInstance().GetDeltaTime());
	}
	for(unsigned i = 0; i < objectArray.size(); i++)
	{
		for(unsigned j = i+1; j < objectArray.size(); j++)
		{
			if(objectArray[i]->Is("Animation") || objectArray[j]->Is("Animation"))
			{

			}
			else if(objectArray[i]->box.OverlapsWith(objectArray[j]->box))
			{
				objectArray[i]->NotifyObjectCollision(objectArray[j]);
				objectArray[j]->NotifyObjectCollision(objectArray[i]);
			}
		}
	}
	for(unsigned i = 0; i < objectArray.size(); i++)
	{
		if(objectArray[i]->IsDead())
		{
			if(objectArray[i] == Camera::GetInstance().GetFocus())
				Camera::GetInstance().Unfollow();
			if(objectArray[i] == player)
				player = nullptr;
			delete objectArray[i];
			objectArray.erase(objectArray.begin()+i);
		}
	}

	if(player)
	{
		if(mode == "Lancelot")
		{
			Lancelot* p = (Lancelot*) player;
			healthBar->SetPercentage(p->GetHealth()/10.0);
		}
		else if(mode == "Gallahad")
		{
			Gallahad* p = (Gallahad*) player;
			healthBar->SetPercentage(p->GetHealth()/10.0);
		}
	}
	else
	{
		healthBar->SetPercentage(0);
	}

	Camera::GetInstance().Update(Game::GetInstance().GetDeltaTime());
}

void StageState::HandleInput()
{
	if(InputManager::GetInstance().KeyPress(SDLK_ESCAPE))
	{
		quitRequested = true;
	}

	if(InputManager::GetInstance().KeyPress(SDLK_RETURN))
	{
		if(paused)
			Resume();
		else
			Pause();
	}

	//Teste do sistema de janelas
	if(InputManager::GetInstance().KeyPress(SDLK_j))
	{
		RemoveWindow(new DialogWindow(0, 512, 512, 640, "uhaeuh aeuheauh aeuheauh euhuhe huaheuhua ehueah uea hueah eueuhaauhea ehuauehe uhaeuhuha euhea"));
	}
}

void StageState::Render() {
	tileMap->RenderLayer(0, Camera::GetInstance().pos.x, Camera::GetInstance().pos.y);
	for(unsigned int i = 0; i < objectArray.size(); i++)
		objectArray[i]->Render();
	tileMap->RenderLayer(1, Camera::GetInstance().pos.x, Camera::GetInstance().pos.y);
	for(unsigned int i = 0; i < windowArray.size(); i++)
		windowArray.at(i)->Render();
	healthBar->Render(10,10);
	energyBar->Render(10,10);

	if (inGameMenu!=nullptr)
		inGameMenu->Render();
}

bool StageState::QuitRequested()
{
	return quitRequested;
}

bool StageState::Is(std::string type)
{
	return (type == "Stage");
}

StageState* StageState::get()
{
	return this;
}
