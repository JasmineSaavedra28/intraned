/**
 * @file dynamic_loader.h
 * @brief Sistema de carga dinámica para bibliotecas
 * @version 0.1
 */

#pragma once

#include <string>
#include <memory>
#include <functional>

#ifdef _WIN32
    #include <windows.h>
    #define LOAD_LIBRARY(name) LoadLibraryA(name)
    #define GET_PROC_ADDRESS(lib, name) GetProcAddress(lib, name)
    #define FREE_LIBRARY(lib) FreeLibrary(lib)
    typedef HMODULE LibraryHandle;
#else
    #include <dlfcn.h>
    #define LOAD_LIBRARY(name) dlopen(name, RTLD_NOW)
    #define GET_PROC_ADDRESS(lib, name) dlsym(lib, name)
    #define FREE_LIBRARY(lib) dlclose(lib)
    typedef void* LibraryHandle;
#endif

/**
 * @brief Clase base para carga dinámica de bibliotecas
 */
class DynamicLoader {
public:
    DynamicLoader(const std::string& libName);
    ~DynamicLoader();
    
    bool load();
    void unload();
    bool isLoaded() const;
    
    void* getFunction(const std::string& funcName);
    
protected:
    std::string libName;
    LibraryHandle handle;
    bool loaded;
};

/**
 * @brief Loader específico para httplib
 */
class HttplibLoader : public DynamicLoader {
public:
    HttplibLoader();
    
    // Funciones de fábrica para Server
    using ServerFactory = void*(*)();
    using ServerDestroyer = void(*)(void*);
    
    ServerFactory createServer;
    ServerDestroyer destroyServer;
    
    bool loadFunctions();
};

/**
 * @brief Loader específico para json
 */
class JsonLoader : public DynamicLoader {
public:
    JsonLoader();
    
    // Funciones de fábrica para json
    using JsonFactory = void*(*)();
    using JsonDestroyer = void(*)(void*);
    using JsonToString = const char*(*)(const void*);
    using JsonParse = void*(*)(const char*);
    
    JsonFactory createJson;
    JsonDestroyer destroyJson;
    JsonToString jsonToString;
    JsonParse parseJson;
    
    bool loadFunctions();
};

// Implementaciones inline

inline DynamicLoader::DynamicLoader(const std::string& name)
    : libName(name), handle(nullptr), loaded(false) {}

inline DynamicLoader::~DynamicLoader() {
    if (loaded) {
        unload();
    }
}

inline bool DynamicLoader::load() {
    if (loaded) return true;
    
    handle = LOAD_LIBRARY(libName.c_str());
    if (!handle) {
        return false;
    }
    loaded = true;
    return true;
}

inline void DynamicLoader::unload() {
    if (handle && loaded) {
        FREE_LIBRARY(handle);
        handle = nullptr;
        loaded = false;
    }
}

inline bool DynamicLoader::isLoaded() const {
    return loaded;
}

inline void* DynamicLoader::getFunction(const std::string& funcName) {
    if (!loaded || !handle) return nullptr;
    return GET_PROC_ADDRESS(handle, funcName.c_str());
}

inline HttplibLoader::HttplibLoader() 
    : DynamicLoader(
#ifdef _WIN32
        "httplib.dll"
#else
        "libhttplib.so"
#endif
      ),
      createServer(nullptr), destroyServer(nullptr) {}

inline bool HttplibLoader::loadFunctions() {
    if (!load()) return false;
    
    createServer = (ServerFactory)getFunction("CreateServer");
    destroyServer = (ServerDestroyer)getFunction("DestroyServer");
    
    return createServer && destroyServer;
}

inline JsonLoader::JsonLoader()
    : DynamicLoader(
#ifdef _WIN32
        "json.dll"
#else
        "libjson.so"
#endif
      ),
      createJson(nullptr), destroyJson(nullptr), 
      jsonToString(nullptr), parseJson(nullptr) {}

inline bool JsonLoader::loadFunctions() {
    if (!load()) return false;
    
    createJson = (JsonFactory)getFunction("CreateJson");
    destroyJson = (JsonDestroyer)getFunction("DestroyJson");
    jsonToString = (JsonToString)getFunction("JsonToString");
    parseJson = (JsonParse)getFunction("ParseJson");
    
    return createJson && destroyJson && jsonToString && parseJson;
}