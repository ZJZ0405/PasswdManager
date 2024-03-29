﻿# CMakeList.txt: PasswdManager 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("PasswdManager")
aux_source_directory(src SRC_LIST)

# 寻找第三方库
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(log4cplus CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

# 将源代码添加到此项目的可执行文件。
add_executable (PasswdManager ${SRC_LIST})


# 动态链接库
target_link_libraries(PasswdManager PRIVATE unofficial::sqlite3::sqlite3)
target_link_libraries(PasswdManager PRIVATE log4cplus::log4cplus)
target_link_libraries(PasswdManager PRIVATE OpenSSL::SSL OpenSSL::Crypto)



if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PasswdManager PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
