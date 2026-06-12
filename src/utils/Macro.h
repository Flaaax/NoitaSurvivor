#pragma once

#ifndef N_CLASS_MACRO
#define N_CLASS_MACRO

//#pragma execution_character_set("utf-8")

#define N_STATIC_VAR inline static
#define N_CONSTEXPR inline static constexpr

#define N_NODISCARD [[nodiscard]]

#ifdef _DEBUG
inline constexpr bool N_IS_DEBUG = true;
#else
inline constexpr bool N_IS_DEBUG = false;
#endif

#endif