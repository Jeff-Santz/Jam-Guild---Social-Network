cmake_minimum_required(VERSION 3.10)
project(SocialNetwork)

# Configura o padrão C++ (17)
set(CMAKE_CXX_STANDARD 17)

# Inclui a pasta de headers
include_directories(include)

# Procura todos os arquivos .cpp na pasta src
file(GLOB SOURCES "src/*.cpp")

# ADICIONE ESTA LINHA: Adiciona o sqlite3.c na lista de compilacao
list(APPEND SOURCES "src/sqlite3.c") 

# Cria o executável com tudo junto
add_executable(SocialNetworkApp ${SOURCES})

# Linka a biblioteca de threads (necessário pro SQLite em alguns sistemas)
if(UNIX)
    target_link_libraries(SocialNetworkApp pthread dl)
endif()