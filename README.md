# UE4SS-CPP-ModTemplate

A Simple Mod Template For UE4SS C++ Mods

***

# Usage:-

## Requirements:-

### **Universal Requirements:-**
- [Rust](https://rust-lang.org/tools/install) With Target `x86_64-pc-windows-msvc`
- [CMake](https://cmake.org/download)
- [Ninja](https://ninja-build.org) Or [MSVC](https://visualstudio.microsoft.com/vs/features/cplusplus) (Windows Only)
- Access To [UE Repo](https://www.unrealengine.com/ue-on-github) On Github

### **Windows Requirements:-**
- [Visual Studio](https://visualstudio.microsoft.com/downloads)

### **Linux Requirements:-**
- [Clang](https://github.com/llvm/llvm-project)
- [XWin](https://github.com/Jake-Shadle/xwin)

## **Building:-**

1. Clone The Repository
```shell
git clone git@github.com:LabrynthKing/UE4SS-CPP-ModTemplate.git
cd UE4SS-CPP-ModTemplate
```
2. Update Submodules
```shell
git submodule update --init --recursive
```
3. **IF ON LINUX** Make Sure Xwin Is Setup **And Set `XWIN_DIR` Environment Variable As The `~/.xwin-cache` Directory**
```shell
xwin --accept-license splat --output ~/.xwin-cache
```
4. Depending On Your Platform, Generate CMake Build Files
```shell
# Windows
cmake -G "Visual Studio 19 2026" -DCMAKE_BUILD_TYPE=Game__Shipping__Win64 -S . -B ./cmake_shipping

# Linux
cmake -G Ninja -DCMAKE_BUILD_TYPE=Game__Shipping__Win64 -DCMAKE_MAKE_PROGRAM="$NINJA_PATH" -S . -B ./cmake_shipping
```
5. Build Using CMake
```shell
# Windows (Adjust Processor Cores According To Your Choice)
cmake --build ./cmake_shipping --config Game__Shipping__Win64 -j 8

# Linux
cmake --build ./cmake_shipping -j (nproc)
```
6. Your Mod File Should Be Named `main.dll` And Available In The `cmake_shipping` Directory

## **Adding Your Mod In-Game:-**

1. Make Sure UE4SS Is Installed
2. Your File Tree Should Look Like This:-
```
ue4ss/
  |_Mods/
      |_MyMod/
          |_enabled.txt (optional)
          |_dlls/
             |_main.dll
```
3. You Can Either Add This Line To `ue4ss/Mods/mods.txt`
```
MyMod : 1
```
4. Or You Can Use `enabled.txt` If Your Mod Doesn't Care About Load Order

***

# Credits:-
- [UE4SS Development Team](https://github.com/UE4SS-RE/RE-UE4SS)
