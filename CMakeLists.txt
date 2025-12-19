# CMakeList.txt : CMake project for Lab1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Lab1")

# Add source to this project's executable.
add_executable (Lab1 "Lab1.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Lab1 PROPERTY CXX_STANDARD 20)
endif()

if (MSVC)
    target_compile_options(Lab1 PRIVATE /utf-8)
else()
    target_compile_options(Lab1 PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# TODO: Add tests and install targets if needed.
