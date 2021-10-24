#include "Settings.h"

namespace OxygenMeter
{
	struct Update
	{
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
				if (player->GetSubmergedWaterLevel(player->GetPositionZ(), player->GetParentCell()) < 0.875f || player->IsInvulnerable() || player->GetActorValue(RE::ActorValue::kWaterBreathing) > 0.0001f) {
					return std::nullopt;
				}

				float totalBreathTime = detail::get_total_breath_time();
				float remainingBreath = detail::get_remaining_breath(player->currentProcess);

				return (remainingBreath / totalBreathTime) * 100.0;
			}
		};

		static void thunk(RE::HUDChargeMeter* a_this)
		{
			static bool holding_breath = false;
			static bool drowning = false;

			auto fillPct = detail::get_player_breath_pct();
			if (fillPct) {
				static bool fadeWhenDrowning = Settings::GetSingleton()->fadeWhenDrowning;
				if (drowning && fadeWhenDrowning) {
					a_this->root.Invoke("FadeOutChargeMeters");
					return;
				}
				
				if (!holding_breath) {
					a_this->root.Invoke("FadeOutChargeMeters");
					holding_breath = true;
				}

				static bool useLeftMeter = static_cast<bool>(Settings::GetSingleton()->useLeftMeter);
				std::array<RE::GFxValue, 4> array{ *fillPct, true, useLeftMeter, true };
				a_this->root.Invoke("SetChargeMeterPercent", nullptr, array);

				if (*fillPct == 0.0) {
					drowning = true;
				}
			} else {
				if (holding_breath || drowning) {
					a_this->root.Invoke("FadeOutChargeMeters");
					holding_breath = false;
					drowning = false;
				}

				func(a_this);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline constexpr std::size_t size = 0x1;
	};

	inline void Install()
	{
		stl::write_vfunc<RE::HUDChargeMeter, Update>();
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
	logger::info("loaded plugin");

	SKSE::Init(a_skse);

	Settings::GetSingleton()->Load();

	OxygenMeter::Install();

	return true;
}
