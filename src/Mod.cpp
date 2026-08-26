// Windows Macro Thingy Fix For XWin
#include <windows.h>

#undef InterlockedCompareExchangePointer
#undef MemoryBarrier

#include <Mod/CppUserModBase.hpp>

#include "TranslationHelperAPI.h"

#include <chrono>
#include <thread>

#include <DynamicOutput/DynamicOutput.hpp>

namespace MyNamespace
{
    using namespace RC::Unreal;

    class MyMod : public CppUserModBase
    {
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
        auto on_unreal_init() -> void override
        {
            Output::send<LogLevel::Normal>(STR("MyMod Loaded!!"));

            // Delayed Cuz It Was Loading Too Quickly Lol
            std::thread(
                [this]()
                {
                    std::this_thread::sleep_for(std::chrono::seconds(60));

                    TH::RegisterMod("THelperCPPExample");

                    const char* someVar = TH::Translate("greeting", "Hi I Am LabrynthKing From CPP");

                    Output::send(TH::TranslateW("greeting", "Hi I Am LabrynthKing From CPP"));
                    Output::send(TH::TranslateW("wazzup", "What's Up?"));
                    Output::send(TH::TranslateW("daKey", "DA VALUEEEEEEEEEE"));
                })
                .detach();
        }

        // Fires On Each Unreal Engine Update Tick
        // auto on_update() -> void override;
    };
} // namespace MyNamespace

#define MYMOD_API __declspec(dllexport)

extern "C"
{
    MYMOD_API CppUserModBase* start_mod() { return new MyNamespace::MyMod(); }
    MYMOD_API void uninstall_mod(const CppUserModBase* mod) { delete mod; }
}
