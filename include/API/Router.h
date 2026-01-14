#ifndef ROUTER_H
#define ROUTER_H

#include "crow_all.h" 

namespace API {

    class Router {
    public:
        static void setupRoutes(crow::SimpleApp& app);
    };

}

#endif