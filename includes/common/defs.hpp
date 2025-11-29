#pragma once

// ===BUILD INFO===

#ifndef PROJECT_NAME
#define PROJECT_NAME "unknown"
#endif

#ifndef PROJECT_VERSION
#define PROJECT_VERSION "0.0.0"
#endif

#ifndef COMPILER_INFO
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#if defined(__clang__)
#define COMPILER_INFO "Clang " STR(__clang_major__) "." STR(__clang_minor__)
#elif defined(__GNUC__)
#define COMPILER_INFO "GCC " STR(__GNUC__) "." STR(__GNUC_MINOR__)
#elif defined(_MSC_VER)
#define COMPILER_INFO "MSVC " STR(_MSC_VER)
#else
#define COMPILER_INFO "Unknown"
#endif
#endif

#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__
#define BUILD_MODE (#ifdef NDEBUG "Release" #else "Debug" #endif)

// ===OS===

// check for unix-style os
#if !defined(_WIN32) &&                                                                            \
	(defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
// posix compliant
#define REACTOR_UNIX
#define REACTOR_POSIX

#if defined(__linux) || defined(__linux__)
#define REACTOR_LINUX
#endif //  defined(__linux) || defined(__linux__)

#if defined(__APPLE__) && defined(__MACH__) && defined(TARGET_OS_MAC)
#define REACTOR_APPLE
#endif

#endif

#if defined(__CYGWIN__) && !defined(_WIN32)
// Cygwin POSIX under Microsoft Windows.
#define REACTOR_CYGWIN
#endif

#if defined(_WIN64)
// Microsoft Windows (64-bit)
#define REACTOR_WINDOWS64
#define REACTOR_WINDOWS
#elif defined(_WIN32)
// Microsoft Windows (32-bit)
#define REACTOR_WINDOWS32
#define REACTOR_WINDOWS
#endif

#ifndef REACTOR_DEFAULT_CONFIG_PATH

#if defined(REACTOR_LINUX) || defined(REACTOR_UNIX) || defined(REACTOR_POSIX) ||                   \
	defined(REACTOR_CYGWIN)
// ~/.config/reactor/config.toml
#define REACTOR_DEFAULT_CONFIG_PATH "~/.config/reactor/config.toml"

#elif defined(REACTOR_APPLE)
// ~/Library/Application Support/reactor/config.toml
#define REACTOR_DEFAULT_CONFIG_PATH "~/Library/Application Support/reactor/config.toml"

#elif defined(REACTOR_WINDOWS)
// %APPDATA%\reactor\config.toml
// Заметь: ПОКА ЧТО это строка, не разворачивающая %APPDATA% — разворачивать нужно в рантайме.
#define REACTOR_DEFAULT_CONFIG_PATH "%APPDATA%\\reactor\\config.toml"

#else
// Fallback — POSIX-like
#define REACTOR_DEFAULT_CONFIG_PATH "~/.config/reactor/config.toml"
#endif

#endif // REACTOR_DEFAULT_CONFIG_PATH
