#include "pch.h"
#include "GameMain.h"
#include "MapRenderable.h"
#include "Player.h"
#include "CollisionManager.h"
#include "LevelManager.h"
#include "Bomb.h"

using namespace DX;
using namespace std;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Concurrency;

namespace DirectXGame
{
	// Loads and initializes application assets when the application is loaded.
	GameMain::GameMain(const shared_ptr<DX::DeviceResources>& deviceResources) :
		mDeviceResources(deviceResources)
	{
		// Register to be notified if the Device is lost or recreated
		mDeviceResources->RegisterDeviceNotify(this);

		LevelManager::GetInstance().SetGameMain(*this);

		auto camera = make_shared<OrthographicCamera>(mDeviceResources);
		mComponents.push_back(camera);
		camera->SetPosition(0, 0, 1);

		CoreWindow^ window = CoreWindow::GetForCurrentThread();
		mKeyboard = make_shared<KeyboardComponent>(mDeviceResources);		
		mKeyboard->Keyboard()->SetWindow(window);
		mComponents.push_back(mKeyboard);

		mMouse = make_shared<MouseComponent>(mDeviceResources);		
		mMouse->Mouse()->SetWindow(window);
		mComponents.push_back(mMouse);

		mGamePad = make_shared<GamePadComponent>(mDeviceResources);
		mComponents.push_back(mGamePad);

		auto fpsTextRenderer = make_shared<FpsTextRenderer>(mDeviceResources);
		mComponents.push_back(fpsTextRenderer);

		auto map = make_shared<MapRenderable>(mDeviceResources, camera);
		mComponents.push_back(map);

		auto player = make_shared<Player>(mDeviceResources, camera, mKeyboard, mGamePad, map->GetMap());
		CollisionManager::GetInstance().SetMap(map);

		mComponents.push_back(player);

		mTimer.SetFixedTimeStep(true);
		mTimer.SetTargetElapsedSeconds(1.0 / 60);

		IntializeResources();
	}

	GameMain::~GameMain()
	{
		mDeviceResources->RegisterDeviceNotify(nullptr);
	}

	// Updates application state when the window size changes (e.g. device orientation change)
	void GameMain::CreateWindowSizeDependentResources()
	{
		for (auto& component : mComponents)
		{
			component->CreateWindowSizeDependentResources();
		}
	}

	// Updates the application state once per frame.
	void GameMain::Update()
	{
		// Update scene objects.
		mTimer.Tick([&]()
		{
			for (auto& component : mComponents)
			{
				component->Update(mTimer);
			}

			if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape) ||
				mMouse->WasButtonPressedThisFrame(MouseButtons::Middle) ||
				mGamePad->WasButtonPressedThisFrame(GamePadButtons::Back))
			{
				CoreApplication::Exit();
			}
		});

		RemoveComponents();
		AddNewComponents();
	}

	// Renders the current frame according to the current application state.
	// Returns true if the frame was rendered and is ready to be displayed.
	bool GameMain::Render()
	{
		// Don't try to render anything before the first Update.
		if (mTimer.GetFrameCount() == 0)
		{
			return false;
		}

		auto context = mDeviceResources->GetD3DDeviceContext();

		// Reset the viewport to target the whole screen.
		auto viewport = mDeviceResources->GetScreenViewport();
		context->RSSetViewports(1, &viewport);

		// Reset render targets to the screen.
		ID3D11RenderTargetView *const targets[1] = { mDeviceResources->GetBackBufferRenderTargetView() };
		context->OMSetRenderTargets(1, targets, nullptr);

		// Clear the back buffer and depth stencil view.
		context->ClearRenderTargetView(mDeviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::Black);
		context->ClearDepthStencilView(mDeviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		for (auto& component : mComponents)
		{
			auto drawableComponent = dynamic_pointer_cast<DrawableGameComponent>(component);
			if (drawableComponent != nullptr && drawableComponent->Visible())
			{
				drawableComponent->Render(mTimer);
			}
		}

		return true;
	}

	void GameMain::AddComponent(const shared_ptr<DX::GameComponent>& component)
	{
		mComponentsToAdd.push_back(component);
	}

	void GameMain::RemoveComponent(const DX::GameComponent& component)
	{
		mComponentsToDelete.push_back(&component);
	}

	// Notifies renderers that device resources need to be released.
	void GameMain::OnDeviceLost()
	{
		for (auto& component : mComponents)
		{
			component->ReleaseDeviceDependentResources();
		}
	}

	// Notifies renderers that device resources may now be recreated.
	void GameMain::OnDeviceRestored()
	{
		IntializeResources();
	}

	void GameMain::IntializeResources()
	{
		for (auto& component : mComponents)
		{
			component->CreateDeviceDependentResources();
		}

		CreateWindowSizeDependentResources();
	}

	void GameMain::AddNewComponents()
	{
		if (mComponentsToAdd.size() == 0)
		{
			return;
		}

		auto player = mComponents.back();
		mComponents.pop_back();

		for (auto& componentToAdd : mComponentsToAdd)
		{
			mComponents.push_back(componentToAdd);
		}

		mComponents.push_back(player);
		mComponentsToAdd.clear();
	}

	void GameMain::RemoveComponents()
	{
		if (mComponentsToDelete.size() == 0)
		{
			return;
		}

		for (auto& componentToRemove : mComponentsToDelete)
		{
			for (auto it = mComponents.begin(); it != mComponents.end(); ++it)
			{
				if ((*it).get() == componentToRemove)
				{
					mComponents.erase(it);
					break;
				}
			}
		}

		mComponentsToDelete.clear();
	}
}