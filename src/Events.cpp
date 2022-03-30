#pragma once
#include "Events.h"
#include "oxyMeter.h"
#include "Settings.h"
#include "RE/U/UI.h"
#include "string.h"

MenuOpenCloseEventHandler* MenuOpenCloseEventHandler::GetSingleton()
{
	static MenuOpenCloseEventHandler singleton;
	return std::addressof(singleton);
}

void MenuOpenCloseEventHandler::Register()
{
	auto ui = RE::UI::GetSingleton();
	ui->AddEventSink(GetSingleton());
}

RE::BSEventNotifyControl MenuOpenCloseEventHandler::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
{
		// from ersh TrueHud pretty much verbatim
	if (a_event) {
		if (a_event->menuName == RE::HUDMenu::MENU_NAME) {
			if (a_event->opening) {
				oxygenMenu::Show();
			} else {
				oxygenMenu::Hide();
			}
		} else if (a_event->menuName == RE::RaceSexMenu::MENU_NAME && !a_event->opening) {
			oxygenMenu::Show();
			logger::info("showing menu when racemenu closes");
		} else if (a_event->menuName == RE::LoadingMenu::MENU_NAME && !a_event->opening) {
			oxygenMenu::Show();
		}
		if (a_event->menuName == RE::JournalMenu::MENU_NAME) {
			Settings::GetSingleton()->Load();
		}
	}

	auto controlMap = RE::ControlMap::GetSingleton();
	if (controlMap) {
		auto& priorityStack = controlMap->contextPriorityStack;
		if (priorityStack.empty() ||
			(priorityStack.back() != RE::UserEvents::INPUT_CONTEXT_ID::kGameplay &&
				priorityStack.back() != RE::UserEvents::INPUT_CONTEXT_ID::kFavorites &&
				priorityStack.back() != RE::UserEvents::INPUT_CONTEXT_ID::kConsole)) {
					oxygenMenu::ToggleVisibility(false);
		} else {
			oxygenMenu::ToggleVisibility(true);
		}
	}
	#if false
	auto mName = a_event->menuName;
	auto ui = RE::UI::GetSingleton();

		if (mName == RE::JournalMenu::MENU_NAME ||
			mName == RE::InventoryMenu::MENU_NAME ||
			mName == RE::MapMenu::MENU_NAME ||
			mName == RE::BookMenu::MENU_NAME ||
			mName == RE::MainMenu::MENU_NAME ||
			mName == RE::LoadingMenu::MENU_NAME ||
			mName == RE::LockpickingMenu::MENU_NAME ||
			mName == RE::StatsMenu::MENU_NAME ||
			mName == RE::ContainerMenu::MENU_NAME ||
			mName == RE::DialogueMenu::MENU_NAME ||
			mName == RE::TweenMenu::MENU_NAME ||
			mName == RE::MagicMenu::MENU_NAME ||
			mName == "CustomMenu") {
			if (a_event->opening) {
				oxygenMenu::Hide();
			} else if (!ui->IsMenuOpen(RE::TweenMenu::MENU_NAME)) {
				oxygenMenu::Show();
			}
		}
	#endif
	return RE::BSEventNotifyControl::kContinue;
}
