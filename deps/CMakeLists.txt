add_subdirectory(RE-UE4SS EXCLUDE_FROM_ALL)

add_library(MyModDeps INTERFACE)

target_link_libraries(MyModDeps INTERFACE UE4SS)
