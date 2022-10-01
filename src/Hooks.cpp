#include "Hooks.h"
#include "Settings.h"

namespace OxygenMeter
{
	struct Update
	{
		static void thunk(RE::HUDChargeMeter* a_this)
		{
			if (auto fillPct = detail::get_player_breath_pct(); fillPct) {
				if (!holding_breath) {
					holding_breath = true;

					alphaValue = detail::set_meter_alpha(100.0);
					if (alphaValue == 100.0) {
						a_this->root.Invoke("FadeOutChargeMeters");
					}
				}

				if (drowning && Settings::GetSingleton()->fadeWhenDrowning) {
					a_this->root.Invoke("FadeOutChargeMeters");
					return;
				}

				const std::array<RE::GFxValue, 4> array{
					*fillPct,
					true,
					Settings::GetSingleton()->useLeftMeter == 1,
					true
				};

				a_this->root.Invoke("SetChargeMeterPercent", nullptr, array);

				if (*fillPct == 0.0) {
					drowning = true;
				}
			} else {
				if (holding_breath || drowning) {
					holding_breath = false;
					drowning = false;

					if (alphaValue == 0.0) {
						detail::set_meter_alpha(alphaValue);
					} else {
						a_this->root.Invoke("FadeOutChargeMeters");
					}
				}

				func(a_this);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline constexpr std::size_t size = 0x1;

	private:
		struct detail
		{
			static double set_meter_alpha(double a_value)
			{
				if (auto movie = RE::UI::GetSingleton()->GetMovieView(RE::HUDMenu::MENU_NAME); movie) {
					auto path = Settings::GetSingleton()->useLeftMeter == 1 ?
					                left_path :
					                right_path;

					auto value = movie->GetVariableDouble(path);
					if (value != a_value) {
						movie->SetVariableDouble(path, a_value);
						return value;
					}
				}

				return 100.0;
			}

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
				const auto player = RE::PlayerCharacter::GetSingleton();

				if (!player->IsPointSubmergedMoreThan(player->GetPosition(), player->GetParentCell(), 0.875f) || player->IsInvulnerable() || player->GetActorValue(RE::ActorValue::kWaterBreathing) > 0.0001f) {
					return std::nullopt;
				}

				const auto totalBreathTime = get_total_breath_time();
				const auto remainingBreath = get_remaining_breath(player->currentProcess);

				return (remainingBreath / totalBreathTime) * 100.0;
			}

		private:
			static inline const char* left_path{ "_root.HUDMovieBaseInstance.BottomLeftLockInstance._alpha" };
			static inline const char* right_path{ "_root.HUDMovieBaseInstance.BottomRightLockInstance._alpha" };
		};

		static inline bool holding_breath{ false };
		static inline bool drowning{ false };

		static inline double alphaValue{ 100.0 };
	};

	void Install()
	{
		Settings::GetSingleton()->Load();

		stl::write_vfunc<RE::HUDChargeMeter, Update>();
	}
}
