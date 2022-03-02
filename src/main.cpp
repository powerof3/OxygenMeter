#include "Settings.h"
#include "SKSE/API.h"
#include "oxyMeter.h"

const SKSE::MessagingInterface* g_messaging = nullptr;
const SKSE::LoadInterface* g_LoadInterface = nullptr;
const SKSE::QueryInterface* g_QueryInterface = nullptr;

namespace OxygenMeter
{
	struct Update
	{
		static void thunk(RE::HUDChargeMeter* a_this)
		{
			RE::GPtr<RE::IMenu> oxyRoot = RE::UI::GetSingleton()->GetMenu("oxygenMeter");

			static bool useLeftMeter{ static_cast<bool>(Settings::GetSingleton()->useLeftMeter) };
			static bool fadeWhenDrowning{ Settings::GetSingleton()->fadeWhenDrowning };
			
			auto fillPct = detail::get_player_breath_pct();
			if (fillPct && oxyRoot) {
				const RE::GFxValue testAmount = *fillPct;

				if (!holding_breath) {
					holding_breath = true;
				}

				if (drowning && fadeWhenDrowning) {
					oxyRoot->uiMovie->Invoke("oxygen.doFadeOut", nullptr, &testAmount, 1);
					return;
				}

				oxyRoot->uiMovie->Invoke("oxygen.doShow", nullptr, nullptr, 0);
				oxyRoot->uiMovie->Invoke("oxygen.updateMeterPercent", nullptr, &testAmount, 1);

				if (*fillPct == 0.0) {
					drowning = true;
				}

			} else if (oxyRoot){
	

				if (holding_breath || drowning) {
					holding_breath = false;
					drowning = false;
					const RE::GFxValue refill = 100;
					oxyRoot->uiMovie->Invoke("oxygen.updateMeterPercent", nullptr, &refill, 1);
					oxyRoot->uiMovie->Invoke("oxygen.doFadeOut", nullptr, nullptr, 0);
				}

				func(a_this);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline constexpr std::size_t size = 0x1;

	private:
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

		static inline bool holding_breath{ false };
		static inline bool drowning{ false };

		static inline double alphaValue{ 100.0 };
	};

	inline void Install()
	{
		stl::write_vfunc<RE::HUDChargeMeter, Update>();
	}
}

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		oxygenMenu::Register();
		break;

	case SKSE::MessagingInterface::kNewGame:
		oxygenMenu::Show();
		break;

	case SKSE::MessagingInterface::kPostLoadGame:
		oxygenMenu::Show();
		break;
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S:%e] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "Oxygen Meter";
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	//while (!IsDebuggerPresent()) {};
	logger::info("loaded plugin");

	SKSE::Init(a_skse);

	Settings::GetSingleton()->Load();

	OxygenMeter::Install();

	g_messaging = reinterpret_cast<SKSE::MessagingInterface*>(a_skse->QueryInterface(SKSE::LoadInterface::kMessaging));
	if (!g_messaging) {
		logger::critical("Failed to load messaging interface! This error is fatal, plugin will not load.");
		return false;
	}

	auto papyrus = reinterpret_cast<SKSE::PapyrusInterface*>(a_skse->QueryInterface(SKSE::LoadInterface::kPapyrus));
	if (!papyrus) {
		logger::critical("Failed to load scripting interface! This error is fatal, plugin will not load.");
		return false;
	}

	g_messaging->RegisterListener("SKSE", SKSEMessageHandler);

	return true;
}
