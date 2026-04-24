/**
 * @file dynamic_httplib.cpp
 * @brief Wrapper dinámico para httplib.h
 * @version 0.1
 */

#define DLL_EXPORT
#include "dynamic_httplib.h"
#include "httplib.h"

// Exponer las clases principales de httplib
namespace httplib {

// Re-exportar Server
class ServerDLL : public Server {
public:
    using Server::Server;
    using Server::Get;
    using Server::Post;
    using Server::set_mount_point;
    using Server::listen;
    using Server::is_running;
    using Server::stop;
};

// Funciones de fábrica para crear instancias
Server* CreateServer() {
    return new Server();
}

void DestroyServer(Server* srv) {
    delete srv;
}

} // namespace httplib