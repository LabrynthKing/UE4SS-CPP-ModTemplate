// FluxCon => A Diagnostic Logger For Subnautica 2
// Copyright (C) 2026 LabrynthKing
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

/* clang-format off */
#include <string>
#include <mutex>
#include <optional>
#include <vector>

#include <windows.h>
#include <psapi.h>
/* clang-format on */

namespace Flux
{
    enum class ModType : uint32_t
    {
        None = 0,
        Lua = 1 << 0,
        Cpp = 1 << 1,
        Blueprint = 1 << 2,
        Pak = 1 << 3,

        // Hybrids
        LuaCpp = Lua | Cpp,
        LuaBlueprint = Lua | Blueprint,
        CppBlueprint = Cpp | Blueprint,
        LuaCppBlueprint = Lua | Cpp | Blueprint,
    };

    enum class LoggerState : uint8_t
    {
        NotStarted = 0,
        Connecting = 1,
        Connected = 2,
        Reconnecting = 3,
        ShuttingDown = 4,
    };

    struct ModInfo
    {
        std::string name;
        std::string displayName;
        ModType type;
        std::string author;
        std::string version;
        std::optional<std::string> nexusLink;
        std::optional<std::string> gitHubLink;
        std::vector<std::string> dependencies;
    };

    class FluxConAPI
    {
        typedef FluxConAPI*(__cdecl* GetterFunc)();
        static inline std::atomic<FluxConAPI*> instance{nullptr};

        static inline std::uint32_t modId = 0;

        static FluxConAPI* Get()
        {
            if (FluxConAPI* cached = instance.load(std::memory_order_acquire))
                return cached;

            static std::mutex lookupMutex;
            std::lock_guard lock(lookupMutex);

            // Re-Check
            if (FluxConAPI* cached = instance.load(std::memory_order_acquire))
                return cached;

            HMODULE mods[4096];
            DWORD cb = 0;
            HANDLE proc = GetCurrentProcess();
            if (!EnumProcessModules(proc, mods, sizeof(mods), &cb))
                return nullptr;

            const DWORD count =
                (std::min)(static_cast<DWORD>(cb / sizeof(HMODULE)), static_cast<DWORD>(std::size(mods)));

            for (DWORD i = 0; i < count; i++)
            {
                wchar_t path[MAX_PATH] = {};
                if (!GetModuleFileNameExW(proc, mods[i], path, MAX_PATH))
                    continue;
                if (std::wstring(path).find(L"Mods\\FluxCon\\dlls\\") == std::wstring::npos)
                    continue;

                if (const auto func = reinterpret_cast<GetterFunc>(GetProcAddress(mods[i], "fluxcon_get")))
                {
                    FluxConAPI* found = func();
                    instance.store(found, std::memory_order_release);
                    return found;
                }
            }

            return nullptr;
        }

    protected:
        virtual ~FluxConAPI() = default;

        virtual LoggerState GetLoggerStateInternal() = 0;
        virtual void RegisterModInternal(const ModInfo& info) = 0;
        virtual void UnRegisterModInternal(uint32_t modId) = 0;

    public:
        // Hash Func For ModID
        static uint32_t GetFNV1aHash(const std::string_view input) noexcept
        {
            uint32_t hash = 2166136261U; // FNV Offset Basis

            for (const char c : input)
            {
                hash ^= static_cast<uint8_t>(c);
                hash *= 16777619U; // FNV Prime
            }

            return hash;
        }

        static bool HasInit() { return Get() != nullptr; }

        static LoggerState GetLoggerState()
        {
            if (!HasInit())
                return LoggerState::NotStarted;
            return Get()->GetLoggerStateInternal();
        }

        static void RegisterMod(const ModInfo& info)
        {
            if (HasInit())
            {
                modId = GetFNV1aHash(info.name);
                Get()->RegisterModInternal(info);
            }
        }

        static void UnRegisterMod()
        {
            if (HasInit() && modId != 0)
            {
                Get()->UnRegisterModInternal(modId);
            }
        }
    };
} // namespace Flux
