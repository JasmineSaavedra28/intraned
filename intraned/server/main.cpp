/**
 * @file main.cpp
 * @author Gabriel Nicolás González Ferreira
 * @brief Servidor principal - Intraned
 * @version 0.4
 */

#include "include/dynamic_httplib.h"
#include "include/dynamic_json.h"
#include "include/dynamic_loader.h"
#include "utils/fileHelper.cpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

using json = nlohmann::json;

// Función auxiliar simple para asegurar que el directorio existe
void asegurar_directorio(const std::string& path) {
    struct stat info;
    // Si stat falla, el directorio probablemente no existe
    if (stat(path.c_str(), &info) != 0) {
        // Creamos el directorio con permisos 0777 (rwxrwxrwx)
        #ifdef _WIN32
            mkdir(path.c_str());
        #else
            mkdir(path.c_str(), 0777);
        #endif
    }
}

const std::string ADMIN_TOKEN = "educacion_libre_2026"; 

int main() 
{
    const std::string upload_dir = "./server/uploads";
    asegurar_directorio(upload_dir);

    HttplibLoader httplibLoader;
    if (!httplibLoader.load() || !httplibLoader.loadFunctions()) {
        std::cerr << "Error loading httplib.dll" << std::endl;
        return 1;
    }

    JsonLoader jsonLoader;
    if (!jsonLoader.load() || !jsonLoader.loadFunctions()) {
        std::cerr << "Error loading json.dll" << std::endl;
        return 1;
    }

    httplib::Server* svr = (httplib::Server*)httplibLoader.createServer();

    svr->set_mount_point("/", "./public");
    svr->set_mount_point("/recursos", upload_dir);

    // Ruta de Login
    svr->Post("/api/login", [&jsonLoader](const httplib::Request &req, httplib::Response &res) {
        try {
            void* j_input = jsonLoader.parseJson(req.body.c_str());
            if (!j_input) {
                res.status = 400;
                res.set_content("{\"error\": \"JSON invalido\"}", "application/json");
                return;
            }
            json* j = static_cast<json*>(j_input);
            if ((*j)["password"] == "admin123") {
                res.set_content("{\"token\": \"" + ADMIN_TOKEN + "\"}", "application/json");
            } else {
                res.status = 401;
                res.set_content("{\"error\": \"No autorizado\"}", "application/json");
            }
            jsonLoader.destroyJson(j_input);
        } catch (...) {
            res.status = 400;
            res.set_content("{\"error\": \"JSON invalido\"}", "application/json");
        }
    });

    // Obtener contenidos
    svr->Get("/api/contenidos", [&jsonLoader, &upload_dir](const httplib::Request &, httplib::Response &res) {
        std::ifstream file(upload_dir + "/metadata.json");
        if (file.is_open()) {
            void* db = jsonLoader.createJson();
            json* j_db = static_cast<json*>(db);
            file >> *j_db;
            res.set_content(jsonLoader.jsonToString(db), "application/json");
            jsonLoader.destroyJson(db);
        } else {
            res.set_content("{\"contenidos\": []}", "application/json");
        }
    });

    // Ruta de Upload (Usando req.form para httplib 0.38.0)
    svr->Post("/api/upload", [&jsonLoader, &upload_dir](const httplib::Request &req, httplib::Response &res) {
        if (req.get_header_value("Authorization") != ADMIN_TOKEN) {
            res.status = 403;
            res.set_content("{\"error\": \"Token invalido\"}", "application/json");
            return;
        }

        if (!req.form.has_file("archivo")) {
            res.status = 400;
            res.set_content("{\"error\": \"Falta el archivo\"}", "application/json");
            return;
        }

        const auto &file_part = req.form.get_file("archivo");
        std::string titulo = req.form.get_field("titulo");
        std::string autor = req.form.get_field("autor");
        std::string tema = req.form.get_field("tema");
        std::string filename = file_part.filename;

        std::string path = upload_dir + "/" + filename;
        std::ofstream ofs(path, std::ios::binary);
        
        if (ofs.is_open()) {
            ofs << file_part.content;
            ofs.close();

            void* nuevo = jsonLoader.createJson();
            json* j_nuevo = static_cast<json*>(nuevo);
            (*j_nuevo)["titulo"] = titulo;
            (*j_nuevo)["autor"] = autor;
            (*j_nuevo)["tema"] = tema;
            (*j_nuevo)["file"] = filename;
            
            if (FileHelper::registrarArchivo(*j_nuevo)) {
                res.set_content("{\"status\": \"success\"}", "application/json");
            } else {
                res.status = 500;
                res.set_content("{\"error\": \"Error en metadata\"}", "application/json");
            }
            jsonLoader.destroyJson(nuevo);
        } else {
            res.status = 500;
            res.set_content("{\"error\": \"Error de E/S en disco\"}", "application/json");
        }
    });

    std::cout << "Servidor iniciado en http://localhost:8080" << std::endl;
    svr->listen("0.0.0.0", 8080);
    
    httplibLoader.destroyServer(svr);
    return 0;
}