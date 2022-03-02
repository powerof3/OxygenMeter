#pragma once
#include "oxyMeter.h"

oxygenMenu::oxygenMenu()
{
	auto scaleformManager = RE::BSScaleformManager::GetSingleton();

	inputContext = Context::kNone;
	depthPriority = 127;

	menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);
	menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);
	menuFlags.set(RE::UI_MENU_FLAGS::kCustomRendering);

	scaleformManager->LoadMovieEx(this, MENU_PATH, [](RE::GFxMovieDef* a_def) -> void {
		a_def->SetState(RE::GFxState::StateType::kLog,
			RE::make_gptr<Logger>().get());
	});
}

void oxygenMenu::Register()
{
	auto ui = RE::UI::GetSingleton();
	if (ui) {
		ui->Register(MENU_NAME, Creator);

		oxygenMenu::Show();
	}
}

void oxygenMenu::Show()
{
	auto msgQ = RE::UIMessageQueue::GetSingleton();
	if (msgQ) {
		msgQ->AddMessage(oxygenMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
	}
}

void oxygenMenu::Hide()
{
	auto msgQ = RE::UIMessageQueue::GetSingleton();
	if (msgQ) {
		msgQ->AddMessage(oxygenMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
	}
}

void oxygenMenu::Update()
{
	RE::DebugNotification("1");
	RE::GPtr<RE::IMenu> oxygenMenu = RE::UI::GetSingleton()->GetMenu(oxygenMenu::MENU_NAME);
	if (!oxygenMenu || !oxygenMenu->uiMovie)
		return;

}

void oxygenMenu::AdvanceMovie(float a_interval, std::uint32_t a_currentTime)
{
	RE::DebugNotification("AdvanceMovie");
	oxygenMenu::Update();

	RE::IMenu::AdvanceMovie(a_interval, a_currentTime);
}
