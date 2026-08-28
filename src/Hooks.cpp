#include "Hooks.h"
#include "Settings.h"

namespace OxygenMeter
{
	struct Update
	{
		static void thunk(RE::HUDChargeMeter* a_this)
		{
			const auto settings = Settings::GetSingleton();
			const bool useLeftMeter = settings->useLeftMeter == 1;

			if (auto fillPct = detail::get_player_breath_pct(); fillPct) {
				if (!holding_breath) {
					holding_breath = true;

					alphaValue = detail::set_meter_alpha(100.0, useLeftMeter);
					if (alphaValue == 100.0) {
						a_this->root.Invoke("FadeOutChargeMeters");
					}
				}

				if (drowning && settings->fadeWhenDrowning) {
					a_this->root.Invoke("FadeOutChargeMeters");
					return;
				}

				const std::array<RE::GFxValue, 4> array{
					*fillPct,
					true,
					useLeftMeter,
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
						detail::set_meter_alpha(alphaValue, useLeftMeter);
					} else {
						a_this->root.Invoke("FadeOutChargeMeters");
					}
				}

				func(a_this);
			}
		}

		static inline REL::Relocation<decltype(thunk)> func;
		static inline constexpr std::size_t            idx = 0x1;

	private:
		struct detail
		{
			static double set_meter_alpha(double a_value, bool a_useLeftMeter)
			{
				if (auto movie = RE::UI::GetSingleton()->GetMovieView(RE::HUDMenu::MENU_NAME); movie) {
					auto path = a_useLeftMeter ?
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
				return (50.0f * *"fActorSwimBreathMult"_gs) + *"fActorSwimBreathBase"_gs;
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
			static constexpr const char* left_path{ "_root.HUDMovieBaseInstance.BottomLeftLockInstance._alpha" };
			static constexpr const char* right_path{ "_root.HUDMovieBaseInstance.BottomRightLockInstance._alpha" };
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
