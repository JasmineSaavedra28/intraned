#!/bin/bash

# Colores para la terminal
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${BLUE}Iniciando compilación de Intraned...${NC}"

# 1. Crear carpetas necesarias si no existen
mkdir -p build
mkdir -p uploads

# 2. Compilación del servidor
# -lws2_32: Librería de sockets de Windows
# -static: Incluir todas las librerías estáticamente
echo -e "${YELLOW}Compilando servidor...${NC}"
g++ -O3 -s server/main.cpp -o build/intraned.exe -I server/include -pthread -lws2_32 -static-libgcc -static-libstdc++

# 4. Verificar si la compilación fue exitosa
if [ $? -eq 0 ]; then
    echo -e "${GREEN}=========================================${NC}"
    echo -e "${GREEN} Compilación exitosa en: build/intraned ${NC}"
    echo -e "${GREEN}=========================================${NC}"
    echo -e "Bibliotecas dinámicas creadas:"
    echo -e "  - build/httplib.dll (Windows) / build/libhttplib.so (Linux)"
    echo -e "  - build/json.dll (Windows) / build/libjson.so (Linux)"
    echo -e "Para iniciar el servidor, ejecuta: ./build/intraned"
else
    echo -e "${RED}Error en la compilación. Revisa las dependencias en server/include/${NC}"
fi