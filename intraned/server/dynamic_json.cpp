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
json* CreateJson() {
    return new json();
}

void DestroyJson(json* j) {
    delete j;
}

// Serialización
std::string JsonToString(const json& j) {
    return j.dump();
}

json* ParseJson(const std::string& str) {
    try {
        return new json(json::parse(str));
    } catch (...) {
        return nullptr;
    }
}

} // namespace nlohmann