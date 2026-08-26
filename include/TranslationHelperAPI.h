// Translation Helper - A Simple Translation Utility For Subnautica 2 Mods
// Copyright (C) 2026  Labrynth King
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

/**
 * @file TranslationHelperAPI.h
 * @brief API For Translation Helper
 * @author LabrynthKing
 * @date 2026
 */

#pragma once

#include <atomic>
#include <mutex>
#include <string>

#include <windows.h>
// ReSharper disable once CppWrongIncludesOrder
#include <psapi.h>

#include "Helpers/String.hpp"

/// @brief Small Helper For STR
#define BRUH(x) STR(x)

/// @brief Current Version Of This API Header
#define THVersion "1.0.0"

class TH
{
    /// @brief Function Pointer Signature For Fetching The Core API Instance.
    typedef TH*(__cdecl*GetterFunc)();

    /// @brief Atomic Cache Storing The Located TranslationHelper API Implementation Instance.
    static inline std::atomic<TH*> instance{nullptr};

    /// @brief Unique Mod Name For The Current Mod
    static inline std::string modName{};

    /**
     * @brief Dynamically Locates And Returns The Active TranslationHelper API Instance
     * @return TH* Pointer To The Resolved Instance, or `nullptr` If Skill Issue.
     */
    // Hoping This Doesn't Break Bruh
    static TH* Get()
    {
        if (TH* cached = instance.load(std::memory_order_acquire))
            return cached;

        static std::mutex lookupMutex;
        std::lock_guard lock(lookupMutex);

        if (TH* cached = instance.load(std::memory_order_acquire))
            return cached;

        HMODULE mods[4096];
        DWORD cb = 0;
        HANDLE proc = GetCurrentProcess();
        if (!EnumProcessModules(proc, mods, sizeof(mods), &cb))
            return nullptr;

        const DWORD count = (std::min)(static_cast<DWORD>(cb / sizeof(HMODULE)), static_cast<DWORD>(std::size(mods)));

        for (DWORD i = 0; i < count; i++)
        {
            wchar_t path[MAX_PATH] = {};
            if (!GetModuleFileNameExW(proc, mods[i], path, MAX_PATH))
                continue;
            if (std::wstring(path).find(L"Mods\\TranslationHelper\\dlls\\") == std::wstring::npos)
                continue;

            if (const auto func = reinterpret_cast<GetterFunc>(GetProcAddress(mods[i], "th_get")))
            {
                TH* found = func();
                instance.store(found, std::memory_order_release);
                return found;
            }
        }

        return nullptr;
    }

protected:
    /// @brief Virtual Destructor For Proper Cleanup.
    virtual ~TH() = default;

    /**
     * @brief Internal Implementation For Mod Registering So JSONs Can Be Loaded Before Everything
     */
    virtual void RegisterModInternal(const char* mod_name) = 0;

    /**
     * @brief Internal Implementation For Translating A String
     * @param modName The Mod's Unique Name
     * @param key The Key Of The Translation String
     * @param fallback The Default String If Translation Is Not Found
     * @return The Translated String Or Default
     */
    virtual const char* TranslateInternal(const char* modName, const char* key, const char* fallback) = 0;

public:
    /**
     * @brief Checks If TranslationHelper Instance Is Available
     * @return True If Found, False Otherwise
     */
    static bool HasInit()
    {
        const TH* instance_ptr = Get();
        return !instance_ptr;
    }

    /**
     * @brief Registers A Mod To The API (Required For Translation To Work)
     * @param mod_name The Mod's Unique Name
     */
    static void RegisterMod(const char* mod_name)
    {
        TH* instance_ptr = Get();
        if (!instance_ptr || !mod_name)
            return;

        modName = mod_name;
        instance_ptr->RegisterModInternal(modName.c_str());
    }

    /**
     * Translates A String Using Its Unique Key
     * @param key The Key Of The Translation String
     * @param fallback The Default String If Translation Is Not Found
     * @return The Translated String Or Default
     */
    static const char* Translate(const char* key, const char* fallback)
    {
        TH* instance_ptr = Get();
        if (!instance_ptr || modName.empty())
        {
            return fallback;
        }

        return instance_ptr->TranslateInternal(modName.c_str(), key, fallback);
    }

    /**
     * Translates A String Using Its Unique Key
     * @param key The Key Of The Translation String
     * @param fallback The Default String If Translation Is Not Found
     * @return The Translated String Or Default As A WString
     */
    static std::wstring TranslateW(const char* key, const char* fallback)
    {
        const char* str = Translate(key, fallback);

        if (!str || *str == '\0') return L"";

        return RC::to_wstring(str);
    }
};
