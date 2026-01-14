cmake_minimum_required(VERSION 3.10)
project(SocialEngine VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Configurar Bibliotecas (SQLite, Crow, Asio)
# Adiciona a pasta libs/sqlite como biblioteca
add_library(sqlite_lib STATIC libs/sqlite/sqlite3.c)
target_include_directories(sqlite_lib PUBLIC libs/sqlite)

# Adiciona pasta de headers globais
include_directories(include)

# 2. Encontrar todos os arquivos .cpp novos (Modular)
file(GLOB_RECURSE SOURCES 
    "src/Core/*.cpp"
    "src/Auth/*.cpp"
    "src/Social/*.cpp"
    "src/Content/*.cpp"
    "src/Governance/*.cpp"
    "src/API/*.cpp"
    "src/main.cpp"
)

# 3. Criar o Executável
add_executable(SocialEngine ${SOURCES})

# 4. Linkar Bibliotecas
# Linka SQLite e Threads/Network (Necessário pro Crow)
if(WIN32)
    target_link_libraries(SocialEngine PRIVATE sqlite_lib ws2_32 mswsock)
else()
    target_link_libraries(SocialEngine PRIVATE sqlite_lib pthread)
endif()

# Mensagem de sucesso
message(STATUS ">> Social Engine build configured successfully! <<")
