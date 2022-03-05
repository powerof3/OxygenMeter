#pragma once
#include "Events.h"
#include "oxyMeter.h"
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
	auto mName = a_event->menuName;
	auto ui = RE::UI::GetSingleton();

	if (mName == RE::JournalMenu::MENU_NAME ||
		mName == RE::InventoryMenu::MENU_NAME ||
		mName == RE::MapMenu::MENU_NAME ||
		mName == RE::BookMenu::MENU_NAME ||
		mName == RE::LockpickingMenu::MENU_NAME ||
		mName == RE::MagicMenu::MENU_NAME ||
		mName == RE::RaceSexMenu::MENU_NAME ||
		mName == RE::CraftingMenu::MENU_NAME ||
		mName == RE::SleepWaitMenu::MENU_NAME ||
		mName == RE::TrainingMenu::MENU_NAME ||
		mName == RE::BarterMenu::MENU_NAME ||
		mName == RE::FavoritesMenu::MENU_NAME ||
		mName == RE::GiftMenu::MENU_NAME ||
		mName == RE::StatsMenu::MENU_NAME ||
		mName == RE::ContainerMenu::MENU_NAME ||
		mName == RE::DialogueMenu::MENU_NAME ||
		mName == RE::MessageBoxMenu::MENU_NAME ||
		mName == RE::TweenMenu::MENU_NAME ||
		mName == RE::MainMenu::MENU_NAME ||
		mName == "CustomMenu") 
		{
			if (a_event->opening) {
				oxygenMenu::toggleVisibility(false);
			} else if (!ui->IsMenuOpen(RE::TweenMenu::MENU_NAME)) {
				oxygenMenu::toggleVisibility(true);
				oxygenMenu::Show();
			}
		}
	return RE::BSEventNotifyControl::kContinue;
}
