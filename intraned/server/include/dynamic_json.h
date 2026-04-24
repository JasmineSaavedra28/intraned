/**
 * @file dynamic_json.h
 * @brief Header para carga dinámica de json.hpp
 * @version 0.1
 */

#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#ifdef _WIN32
    #ifdef DLL_EXPORT
        #define DLL_API __declspec(dllexport)
    #else
        #define DLL_API __declspec(dllimport)
    #endif
#else
    #define DLL_API __attribute__((visibility("default")))
#endif

namespace nlohmann {

// json es una clase template, necesitamos una implementación completa
// Incluimos el header original y re-exportamos
#include "json.hpp"

typedef json json_t;

// Funciones de fábrica
extern "C" DLL_API void* CreateJson();
extern "C" DLL_API void DestroyJson(void* j);
extern "C" DLL_API const char* JsonToString(const json& j);
extern "C" DLL_API void* ParseJson(const char* str);

} // namespace nlohmann