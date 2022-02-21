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
