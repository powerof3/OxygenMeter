#include "PCH.h"
#include "RE/G/GFxValue.h"
#include "RE/G/GPtr.h"

class oxygenMenu : RE::IMenu
{
public:
    static constexpr const char* MENU_PATH = "oxygenMeter";
	static constexpr const char* MENU_NAME = "oxygenMeter";

    oxygenMenu();

    static void Register();
	static void Show();
	static void Hide();

    static RE::stl::owner<RE::IMenu*> Creator() { return new oxygenMenu(); }

private:
	class Logger : public RE::GFxLog
	{
	public:
		void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_argList) override
		{
			std::string fmt(a_fmt ? a_fmt : "");
			while (!fmt.empty() && fmt.back() == '\n') {
				fmt.pop_back();
			}

			std::va_list args;
			va_copy(args, a_argList);
			std::vector<char> buf(static_cast<std::size_t>(std::vsnprintf(0, 0, fmt.c_str(), a_argList) + 1));
			std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
			va_end(args);

			logger::info("{}"sv, buf.data());
		}
	};
};
