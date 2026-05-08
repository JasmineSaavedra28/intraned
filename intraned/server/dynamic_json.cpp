/**
 * @file dynamic_json.cpp
 * @brief Wrapper dinámico para json.hpp
 * @version 0.1
 */

#define DLL_EXPORT
#include "dynamic_json.h"
#include "json.hpp"

namespace nlohmann {

// Re-exportar json como biblioteca dinámica
// La biblioteca json.hpp es header-only, pero necesitamos
// crear una interfaz de carga dinámica

// Instancias de json factory
extern "C" DLL_API void* CreateJson() {
    return new json();
}

extern "C" DLL_API void DestroyJson(void* j) {
    delete static_cast<json*>(j);
}

// Serialización
extern "C" DLL_API const char* JsonToString(void* j) {
    static std::string s = static_cast<json*>(j)->dump();
    return s.c_str();
}

extern "C" DLL_API void* ParseJson(const char* str) {
    try {
        return new json(json::parse(str));
    } catch (...) {
        return nullptr;
    }
}

} // namespace nlohmann