#pragma once

class Settings
{
public:
	[[nodiscard]] static Settings* GetSingleton()
	{
		static Settings singleton;
		return std::addressof(singleton);
	}

	void Load()
	{
		constexpr auto path = L"Data/SKSE/Plugins/po3_OxygenMeter.ini";

		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(path);

		detail::get_value(ini, useLeftMeter, "Settings", "Meter placement", ";Oxygen Meter position. 0 - Right, 1 - Left");
		detail::get_value(ini, fadeWhenDrowning, "Settings", "Fade out when drowning", ";Oxygen meter fades out when you run out of oxygen and your health starts decreasing");

		ini.SaveFile(path);
	}

	std::uint32_t useLeftMeter{ 0 };
	bool fadeWhenDrowning{ true };

private:
	struct detail
	{
		static void get_value(CSimpleIniA& a_ini, std::uint32_t& a_value, const char* a_section, const char* a_key, const char* a_comment)
		{
			try {
				a_value = string::lexical_cast<std::uint32_t>(a_ini.GetValue(a_section, a_key, "0"));
				a_ini.SetValue(a_section, a_key, std::to_string(a_value).c_str(), a_comment);
			} catch (...) {
			}
		}

		static void get_value(CSimpleIniA& a_ini, float& a_value, const char* a_section, const char* a_key, const char* a_comment)
		{
			a_value = static_cast<float>(a_ini.GetDoubleValue(a_section, a_key, a_value));
			a_ini.SetDoubleValue(a_section, a_key, a_value, a_comment);
		}

		static void get_value(CSimpleIniA& a_ini, bool& a_value, const char* a_section, const char* a_key, const char* a_comment)
		{
			a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
			a_ini.SetBoolValue(a_section, a_key, a_value, a_comment);
		};

		static void get_value(CSimpleIniA& a_ini, std::string& a_value, const char* a_section, const char* a_key, const char* a_comment)
		{
			a_value = a_ini.GetValue(a_section, a_key, a_value.c_str());
			a_ini.SetValue(a_section, a_key, a_value.c_str(), a_comment);
		};
	};
};
