#include "Game.h"
#include <fstream>
#include <sstream>
#include "StringHelper.h"
#include "MAT_PBRTexture.h"
#include "JobSystem.h"
#include "Raycast.h"
#include "PerlinNoise.h"

// For the DirectX Math library
using namespace DirectX;
using namespace std;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		// The application's handle
		"DirectX Game",	   	// Text for the window's title bar
		1280,			// Width of the window's client area
		720,			// Height of the window's client area
		true)			// Show extra stats (fps) in title bar?
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	//Release singletons
	entityManager->Release();
	lightManager->Release();
	physicsManager->Release();
	resourceManager->Release();
	inputManager->Release();
	renderer->Release();

	//Delete sampler states
	samplerState->Release();
	shadowSampler->Release();

	//Release jobs system
	JobSystem::Release();
}

// --------------------------------------------------------
// Read the config file for data
// --------------------------------------------------------
void Game::ReadConfig(string path)
{
	ifstream config(path, ifstream::in);
	
	if (config.fail())
	{
		printf("Could not find or open config file... Assuming defaults and generating file.");
		return;
	}

	//Loop until EoF
	string key;
	while(!getline(config, key, ':').fail())
	{
		//Read token
		key = trim(key);

		//Read arguments
		string arg;
		getline(config, arg);
		arg = trim(arg);

		//Parse key
		if (key == "FPS")
		{
			//Calculate max FPS from file
			float max = 60;
			try
			{
				stringstream argSS(arg);
				argSS >> max;
			}
			catch(exception e)
			{
				printf("Error parsing 'FPS' from config file");
			}
			//If it's we read in less than 0, then set it to 0
			if (max <= 0)
			{
				maxFrameRate = 0;
			}
			else maxFrameRate = 1 / max;
		}

	}

	config.close();
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	ReadConfig("Assets/rescue+.txt");

	//Initialize perlin noise
	PerlinNoise::Seed(unsigned int(time(NULL)));

	//Initialize job system
	JobSystem::Init();

	//Load all needed assets
	resourceManager = ResourceManager::GetInstance();
	LoadAssets();

	//Initialize singletons
	inputManager = InputManager::GetInstance();
	renderer = Renderer::GetInstance();
	renderer->Init(device, context, width, height);
	entityManager = EntityManager::GetInstance();
	physicsManager = PhysicsManager::GetInstance();

	//Initialize singleton data
	inputManager->Init(hWnd);

	//Setup the scene
	SetupScene();

	//Set new gravity
	physicsManager->SetGravity(-15.0f);

	//Initialize lights
	//Set ambient light
	lightManager = LightManager::GetInstance();
	lightManager->SetAmbientColor(0.789f, 0.886f, 1);

	//Directional lights
	DirectionalLight* dLight = lightManager->CreateDirectionalLight(true, XMFLOAT3(1, 1, 1), 1);
	dLight->gameObject()->SetRotation(60, -45, 0);

	//Clear loading jobs
	JobSystem::DeleteFinishedJobs();
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	camera->SetAspectRatio((float)width / height);
}

// --------------------------------------------------------
// Update your game at a fixed time interval here - physics
// --------------------------------------------------------
void Game::FixedUpdate(float constantStepSize, float totalTime)
{
	if (!inputManager->IsWindowFocused())
		return;

	//Update physics
	physicsManager->Simulate(constantStepSize);

	//FixedUpdate entities
	entityManager->FixedUpdate(constantStepSize);

	//Delete finished jobs
	JobSystem::DeleteFinishedJobs();
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	inputManager->UpdateFocus();
	if (!inputManager->IsWindowFocused())
		return;

	//The only call to UpdateMousePos() for the InputManager
	//Get the current mouse position
	inputManager->UpdateMousePos();
	// --------------------------------------------------------
	//All game code goes below


	// Quit if the escape key is pressed
	if (inputManager->GetKey(Key::Escape))
		Quit();

	//Update all entities
	entityManager->Update(deltaTime);

	if (inputManager->GetKey(Key::G))
	{
		GameObject* box4 = new GameObject("Box4");
		box4->AddComponent<MeshRenderer>(
			resourceManager->GetMesh("Assets\\Models\\Basic\\cube.obj"),
			resourceManager->GetMaterial("white")
			);
		box4->MoveAbsolute(XMFLOAT3(0, 8, 8));
		box4->SetScale(1, 2, 2);
		box4->AddComponent<RigidBody>(1.0f);
		box4->AddComponent<BoxCollider>(box4->GetScale());
	}

	if (inputManager->GetKey(Key::T))
	{
		static float amt = 4;
		amt += 2 * deltaTime;
		crate10C->SetLocalPosition(0, amt, 0);
	}

	if (inputManager->GetKey(Key::R))
	{
		static float amt = 0;
		amt += 10 * deltaTime;
		crate10C->SetLocalRotation(0, amt, 45);
	}

	if (inputManager->GetKeyDown(Key::H))
	{
		RaycastHit hit;
		if (Raycast(camera->gameObject()->GetPosition(), camera->gameObject()->GetForwardAxis(), &hit, 10,
			ShapeDrawType::ForDuration, 30))
		{
			printf("Raycast hit on: %s\n", hit.gameObject->GetName().c_str());
		}
	}

	if (inputManager->GetKeyDown(Key::Y))
	{
		SweepHit hit;
		if (Sweep(crate10C->GetComponent<Collider>(), camera->gameObject()->GetForwardAxis(), &hit, 10,
			CollisionLayers(true),
			ShapeDrawType::ForDuration, 30))
		{
			printf("Sweep hit on: %s\n", hit.gameObject->GetName().c_str());
		}
	}


	//All game code goes above
	// --------------------------------------------------------

	//The only call to Update() for the InputManager
	//Update for next frame
	inputManager->UpdateStates();

	//Delete finished jobs
	JobSystem::DeleteFinishedJobs();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	//Draw all entities in the renderer
	renderer->Draw(context, device, camera, backBufferRTV, depthStencilView, samplerState, width, height, deltaTime);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	inputManager->OnMouseDown(buttonState, x, y);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y, int button)
{
	inputManager->OnMouseUp(buttonState, x, y, button);
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.
// WheelDelta may be positive or negative, depending
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	inputManager->OnMouseWheel(wheelDelta, x, y);
}
#pragma endregion