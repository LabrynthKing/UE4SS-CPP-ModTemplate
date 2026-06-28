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

#include <windows.h>
#include <psapi.h>
/* clang-format on */

namespace Flux
{
    class FluxConAPI
    {
        typedef FluxConAPI*(__cdecl* GetterFunc)();
        static inline FluxConAPI* instance = nullptr;

        static FluxConAPI* Get()
        {
            if (instance != nullptr)
                return instance;

            static std::once_flag flag;
            std::call_once(flag,
                           []()
                           {
                               HMODULE mods[4096];
                               DWORD cb = 0;
                               HANDLE proc = GetCurrentProcess();
                               if (!EnumProcessModules(proc, mods, sizeof(mods), &cb))
                                   return;

                               const DWORD count = cb / sizeof(HMODULE);
                               for (DWORD i = 0; i < count; i++)
                               {
                                   wchar_t path[MAX_PATH] = {};
                                   if (!GetModuleFileNameExW(proc, mods[i], path, MAX_PATH))
                                       continue;
                                   if (std::wstring(path).find(L"Mods\\FluxCon\\dlls\\") == std::wstring::npos)
                                       continue;

                                   const auto func =
                                       reinterpret_cast<GetterFunc>(GetProcAddress(mods[i], "fluxcon_get"));
                                   if (!func)
                                       return;

                                   instance = func();
                                   return;
                               }
                           });

            return instance;
        }

    protected:
        virtual ~FluxConAPI() = default;

        virtual bool IsLoggerInitInternal() = 0;

    public:
        static bool HasInit() { return Get() != nullptr; }

        static bool IsLoggerInit()
        {
            if (!HasInit())
                return false;
            return Get()->IsLoggerInitInternal();
        }
    };
} // namespace Flux
