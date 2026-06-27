#!/bin/bash

# IMPORTANT: Set NINJA_PATH To Your Own, I Just Use CLion's Built-In Ninja
NINJA_PATH="$HOME/.local/share/JetBrains/Toolbox/apps/clion/bin/ninja/linux/x64/ninja"

cmake -G Ninja -DCMAKE_BUILD_TYPE=Game__Shipping__Win64 -DCMAKE_MAKE_PROGRAM="$NINJA_PATH" -S . -B ./cmake_shipping
