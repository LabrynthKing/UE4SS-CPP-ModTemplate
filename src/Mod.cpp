#include <Mod/CppUserModBase.hpp>

#include <chrono>
#include <optional>

#include "FluxCon.hpp"

namespace MyNamespace
{
    using namespace RC::Unreal;
    using namespace Flux;

    class MyMod : public CppUserModBase
    {
        bool fluxConReady = false;
        bool registered = false;
        bool unregistered = false;
        std::chrono::steady_clock::time_point registrationTime;

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
        // auto on_unreal_init() -> void override {}

        void RegisterMod()
        {
            const ModInfo info = {.name = "MyMod",
                                  .displayName = "My Mod",
                                  .type = ModType::Cpp,
                                  .author = "MyName",
                                  .version = "1.0.0",
                                  .nexusLink = std::nullopt,
                                  .gitHubLink = std::nullopt,
                                  .dependencies = {}};

            FluxConAPI::RegisterMod(info);

            registered = true;
            registrationTime = std::chrono::steady_clock::now();
        }

        auto on_update() -> void override
        {
            if (!fluxConReady)
            {
                if (FluxConAPI::HasInit() && FluxConAPI::GetLoggerState() == LoggerState::Connected)
                {
                    fluxConReady = true;
                    RegisterMod();
                }
            }

            if (registered && !unregistered)
            {
                const auto currentTime = std::chrono::steady_clock::now();
                const auto elapsedSeconds =
                    std::chrono::duration_cast<std::chrono::seconds>(currentTime - registrationTime).count();

                if (elapsedSeconds >= 20)
                {
                    unregistered = true;

                    FluxConAPI::Info("Test");
                    FluxConAPI::UnRegisterMod();
                }
            }
        }
    };
} // namespace MyNamespace

#define MYMOD_API __declspec(dllexport)

extern "C"
{
    MYMOD_API CppUserModBase* start_mod() { return new MyNamespace::MyMod(); }
    MYMOD_API void uninstall_mod(const CppUserModBase* mod) { delete mod; }
}
