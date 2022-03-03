#pragma once
#include "oxyMeter.h"
#include "Settings.h"

oxygenMenu::oxygenMenu()
{
	auto scaleformManager = RE::BSScaleformManager::GetSingleton();

	inputContext = Context::kNone;
	depthPriority = 1;

	menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);
	menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);
	menuFlags.set(RE::UI_MENU_FLAGS::kCustomRendering);

	if (uiMovie) {
		uiMovie->SetMouseCursorCount(0);  // disable input
	}

	scaleformManager->LoadMovieEx(this, MENU_PATH, [](RE::GFxMovieDef* a_def) -> void {
		a_def->SetState(RE::GFxState::StateType::kLog,
			RE::make_gptr<Logger>().get());
	});
}

struct detail
{
	static float get_total_breath_time()
	{
		const auto gamesetting = RE::GameSettingCollection::GetSingleton();
		return (50.0f * gamesetting->GetSetting("fActorSwimBreathMult")->GetFloat()) + gamesetting->GetSetting("fActorSwimBreathBase")->GetFloat();
	}

	static float get_remaining_breath(RE::AIProcess* a_process)
	{
		auto high = a_process ? a_process->high : nullptr;
		return high ? high->breathTimer : 20.0f;
	}

	static std::optional<double> get_player_breath_pct()
	{
		auto player = RE::PlayerCharacter::GetSingleton();
		if (player->IsPointDeepUnderWater(player->GetPositionZ(), player->GetParentCell()) < 0.875f || player->IsInvulnerable() || player->GetActorValue(RE::ActorValue::kWaterBreathing) > 0.0001f) {
			return std::nullopt;
		}

		float totalBreathTime = detail::get_total_breath_time();
		float remainingBreath = detail::get_remaining_breath(player->currentProcess);

		return (remainingBreath / totalBreathTime) * 100.0;
	}
};

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

// pass the breath meter percents to the scaleform menu using invokes, and tell the menu when to show or hide itself (as in within the scaleform, not the IMenu kHide flag)
void oxygenMenu::Update()
{
	RE::GPtr<RE::IMenu> oxygenMeter = RE::UI::GetSingleton()->GetMenu(oxygenMenu::MENU_NAME);
	if (!oxygenMeter || !oxygenMeter->uiMovie)
		return;

	static bool fadeWhenDrowning{ Settings::GetSingleton()->fadeWhenDrowning };
	auto fillPct = detail::get_player_breath_pct();

	if (fillPct) {
		applyLayout();

		const RE::GFxValue testAmount = *fillPct;

		if (!holding_breath) {
			holding_breath = true;
		}

		if (drowning && fadeWhenDrowning) {
			oxygenMeter->uiMovie->Invoke("oxygen.doFadeOut", nullptr, &testAmount, 1);
			return;
		}

		oxygenMeter->uiMovie->Invoke("oxygen.doShow", nullptr, nullptr, 0);
		oxygenMeter->uiMovie->Invoke("oxygen.setMeterPercent", nullptr, &testAmount, 1);

		if (*fillPct == 0.0) {
			drowning = true;
		}

	} else {
		if (holding_breath || drowning) {
			holding_breath = false;
			drowning = false;
			const RE::GFxValue refill = 100;
			oxygenMeter->uiMovie->Invoke("oxygen.updateMeterPercent", nullptr, &refill, 1);
			oxygenMeter->uiMovie->Invoke("oxygen.doFadeOut", nullptr, nullptr, 0);
		}
	}

}

// apply location, rotations and scale settings to menu.
void oxygenMenu::applyLayout()
{
	RE::GPtr<RE::IMenu> oxygenMeter = RE::UI::GetSingleton()->GetMenu(oxygenMenu::MENU_NAME);
	if (!oxygenMeter || !oxygenMeter->uiMovie)
		return;

	const RE::GFxValue widget_xpos = Settings::GetSingleton()->widget_xpos;
	const RE::GFxValue widget_ypos = Settings::GetSingleton()->widget_ypos;
	const RE::GFxValue widget_rotation = Settings::GetSingleton()->widget_rotation;
	const RE::GFxValue widget_xscale = Settings::GetSingleton()->widget_xscale;
	const RE::GFxValue widget_yscale = Settings::GetSingleton()->widget_yscale;
	RE::GFxValue posArray[5]{ widget_xpos, widget_ypos, widget_rotation, widget_xscale, widget_yscale };
	oxygenMeter->uiMovie->Invoke("oxygen.setLocation", nullptr, posArray, 5);

	oxygenMeter->uiMovie->SetVariableDouble("oxygen.setup", 1);
}

// Every time a new frame of the menu is rendered call the update function.
void oxygenMenu::AdvanceMovie(float a_interval, std::uint32_t a_currentTime)
{
	oxygenMenu::Update();
	RE::IMenu::AdvanceMovie(a_interval, a_currentTime);
}
