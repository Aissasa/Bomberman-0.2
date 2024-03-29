﻿#pragma once

#include "StepTimer.h"
#include "DeviceResources.h"
#include <vector>
#include <memory>

namespace DX
{
	class GameComponent;
	class MouseComponent;
	class KeyboardComponent;
	class GamePadComponent;
}

// Renders Direct2D and 3D content on the screen.
namespace DirectXGame
{
	class GameMain : public DX::IDeviceNotify
	{
	public:
		GameMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~GameMain();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		void AddComponent(const std::shared_ptr<DX::GameComponent>& component);
		void RemoveComponent(const DX::GameComponent& component);

		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		void IntializeResources();

		void AddNewComponents();
		void RemoveComponents();

		std::shared_ptr<DX::DeviceResources> mDeviceResources;
		std::vector<std::shared_ptr<DX::GameComponent>> mComponents;
		DX::StepTimer mTimer;
		std::shared_ptr<DX::KeyboardComponent> mKeyboard;
		std::shared_ptr<DX::MouseComponent> mMouse;
		std::shared_ptr<DX::GamePadComponent> mGamePad;

		std::vector<std::shared_ptr<DX::GameComponent>> mComponentsToAdd;
		std::vector<const DX::GameComponent*> mComponentsToDelete;
	};
}