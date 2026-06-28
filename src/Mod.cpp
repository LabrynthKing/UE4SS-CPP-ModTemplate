#include <Mod/CppUserModBase.hpp>

#include <DynamicOutput/DynamicOutput.hpp>

#include "FluxCon.hpp"

namespace MyNamespace
{
    using namespace RC::Unreal;
    using namespace Flux;

    class MyMod : public CppUserModBase
    {
        bool fluxConReady = false;

    public:
        MyMod()
        {
            ModName = STR("MyMod");
            ModDescription = STR("MyMod Description");
            ModVersion = STR("1.0.0");
            ModAuthors = STR("MyName");
        }

        ~MyMod() override = default;

        // Fires On Program Start
        // auto on_program_start() -> void override;

        // Fires When All C++ Mods Have Been Loaded
        // auto on_cpp_mods_loaded() -> void override;

        // Fires For Each Lua Mod Start
        // auto on_lua_start(StringViewType mod_name, LuaMadeSimple::Lua& lua, LuaMadeSimple::Lua& main_lua,
        // LuaMadeSimple::Lua& async_lua, LuaMadeSimple::Lua* hook_lua) -> void override;

        // You Can Use Unreal Namespace After This Function Fires
        auto on_unreal_init() -> void override { Output::send<LogLevel::Normal>(STR("MyMod Loaded!!\n")); }

        auto on_cpp_mods_loaded() -> void override
        {
            if (!FluxConAPI::HasInit())
            {
                Output::send<LogLevel::Warning>(STR("FluxCon Not Found!\n"));
            }
        }

        auto on_update() -> void override
        {
            if (fluxConReady)
                return;
            if (!FluxConAPI::HasInit())
                return;
            if (!FluxConAPI::IsLoggerInit())
                return;

            fluxConReady = true;
            Output::send<LogLevel::Normal>(STR("FluxCon ready!\n"));
        }
    };
} // namespace MyNamespace

#define MYMOD_API __declspec(dllexport)

extern "C"
{
    MYMOD_API CppUserModBase* start_mod() { return new MyNamespace::MyMod(); }
    MYMOD_API void uninstall_mod(const CppUserModBase* mod) { delete mod; }
}
