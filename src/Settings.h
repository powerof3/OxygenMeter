#pragma once

class Settings : public REX::TSingleton<Settings>
{
public:
	void Load()
	{
		const auto store = REX::FIniSettingStore::GetSingleton();
		store->Init(path, "");

		store->Load();
		store->Save();
	}

	// members
	static constexpr auto path = R"(Data\SKSE\Plugins\po3_OxygenMeter.ini)";

	REX::TIniSetting<std::uint32_t> useLeftMeter{ "Settings", "Meter placement", 0 };                // ";Oxygen Meter position. 0 - Right, 1 - Left"
	REX::TIniSetting<bool>          fadeWhenDrowning{ "Settings", "Fade out when drowning", true };  // ";Oxygen meter fades out when you run out of oxygen and your health starts decreasing"
};
