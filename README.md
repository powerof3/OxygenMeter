# Oxygen Meter

SKSE plugin that repurposes the enchantment charge meter for an oxygen meter when underwater

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/) or newer
	* Desktop development with C++
* [CommonLibSSE](https://github.com/powerof3/CommonLibSSE/tree/dev)
	* You need to build from the powerof3/dev branch
	* Add this as as an environment variable `CommonLibSSEPath`

## User Requirements
* [Address Library for SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
	* Needed for SSE/AE

## Building
```
git clone https://github.com/powerof3/OxygenMeter.git
cd OxygenMeter
git submodule update --init --recursive
```
### SSE (1.5.97)
cmake --preset vs2022-se
cmake --build --preset vs2022-se

### AE (1.6.1170+)
cmake --preset vs2022-ae
cmake --build --preset vs2022-ae

## License
[GPL-3.0](LICENSE)
