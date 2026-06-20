#pragma once

#ifndef FLX_CLASS_MACRO
#define FLX_CLASS_MACRO

//#pragma execution_character_set("utf-8")

#define FLX_STATIC_VAR inline static
#define FLX_CONSTEXPR inline static constexpr

#define FLX_NODISCARD [[nodiscard]]
#define FLX_FORWARD std::forward<Args>(args)...

#ifdef _DEBUG
inline constexpr bool N_IS_DEBUG = true;
#else
inline constexpr bool N_IS_DEBUG = false;
#endif

#endif