cmake_minimum_required(VERSION 3.10)
project(SocialNetworkApp)

set(CMAKE_CXX_STANDARD 17)

# 1. Configura a pasta include globalmente
include_directories(include)

# 2. Motor do Sistema (Ajustado o nome de ContactsHandler.cpp)
set(CORE_SOURCES
    src/SocialNetwork.cpp
    src/User.cpp
    src/Profile.cpp
    src/Post.cpp
    src/Page.cpp
    src/Notification.cpp
    src/Utils.cpp
    src/ProfileNotFound.cpp
    src/Comment.cpp
    src/ImagePost.cpp
    src/NetworkStorage.cpp
    src/ContactHandlers.cpp 
    src/UIHandlers.cpp
    src/sqlite3.c
)

# --- EXECUTÁVEIS ---

# 1. Versão Terminal
add_executable(JamGuildCLI src/main.cpp ${CORE_SOURCES})

# 2. Versão Servidor Web
add_executable(JamGuildServer src/Server.cpp ${CORE_SOURCES})

# Link da biblioteca de rede do Windows (Obrigatório para o Crow)
if(WIN32)
    target_link_libraries(JamGuildServer ws2_32 mswsock)
endif()

# 3. Versão Testes
add_executable(JamGuildTester src/teste_api.cpp ${CORE_SOURCES})