/**
 * @file dynamic_httplib.h
 * @brief Header para carga dinámica de httplib
 * @version 0.1
 */

#pragma once

#include <string>
#include <functional>
#include <map>
#include <memory>
#include <vector>

#ifdef _WIN32
    #ifdef DLL_EXPORT
        #define DLL_API __declspec(dllexport)
    #else
        #define DLL_API __declspec(dllimport)
    #endif
#else
    #define DLL_API __attribute__((visibility("default")))
#endif

// Forward declarations de las clases de httplib
namespace httplib {
    
struct Request {
    std::string method;
    std::string path;
    std::string body;
    std::string remote_addr;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> params;
    struct {
        std::multimap<std::string, std::string> files;
        bool has_file(const std::string& name) const;
        std::string get_file(const std::string& name) const;
    } form;
    
    std::string get_header_value(const std::string& name) const;
};

struct Response {
    int status = 200;
    std::string body;
    std::map<std::string, std::string> headers;
    
    void set_content(const std::string& content, const std::string& content_type);
};

class Server {
public:
    Server();
    ~Server();
    
    Server& Get(const std::string& path, std::function<void(const Request&, Response&)> handler);
    Server& Post(const std::string& path, std::function<void(const Request&, Response&)> handler);
    Server& set_mount_point(const std::string& mount_point, const std::string& dir);
    
    bool listen(const char* host, int port);
    void stop();
    bool is_running() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

// Funciones de fábrica
extern "C" DLL_API void* CreateServer();
extern "C" DLL_API void DestroyServer(void* srv);

} // namespace httplib