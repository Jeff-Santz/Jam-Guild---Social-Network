#include "Core/Location.h"
#include "Core/Database.h"
#include <algorithm>
#include <cctype>
#include <iostream>

namespace Core {

    void Location::seed() {
        auto* db = Database::getInstance();
        
        // 1. Verifica se já tem dados. Se tiver, sai.
        bool hasData = false;
        db->query("SELECT count(*) FROM states", [&](int, char** argv, char**){
            if (argv[0] && std::stoi(argv[0]) > 0) hasData = true;
            return 0;
        });

        if (hasData) {
            std::cout << ">> Geo Database already populated. Skipping." << std::endl;
            return;
        }

        std::cout << ">> Seeding Geo Database (First Run)..." << std::endl;

        // 2. O Mapa Gigante (Mora aqui agora, temporariamente)
        std::map<std::string, std::vector<std::string>> br_data = {
            // --- REGIÃO SUDESTE ---
            {"SP", {"Sao Paulo", "Guarulhos", "Campinas", "Sao Bernardo do Campo", "Santo Andre", "Osasco", "Sorocaba", "Ribeirao Preto", "Sao Jose dos Campos", "Santos", "Sao Carlos", "Bauru", "Piracicaba", "Jundiai", "Franca", "Barueri", "Taubate", "Mogi das Cruzes"}},
            {"RJ", {"Rio de Janeiro", "Sao Goncalo", "Duque de Caxias", "Nova Iguacu", "Niteroi", "Belford Roxo", "Campos dos Goytacazes", "Petropolis", "Volta Redonda", "Macae", "Angra dos Reis", "Paraty", "Cabo Frio"}},
            {"MG", {"Belo Horizonte", "Uberlandia", "Contagem", "Juiz de Fora", "Betim", "Montes Claros", "Ribeirao das Neves", "Uberaba", "Governador Valadares", "Ipatinga", "Sete Lagoas", "Divinopolis", "Ouro Preto"}},
            {"ES", {"Vitoria", "Vila Velha", "Serra", "Cariacica", "Cachoeiro de Itapemirim", "Linhares", "Colatina", "Guarapari"}},

            // --- REGIÃO SUL ---
            {"PR", {"Curitiba", "Londrina", "Maringa", "Ponta Grossa", "Cascavel", "Sao Jose dos Pinhais", "Foz do Iguacu", "Colombo", "Guarapuava", "Paranagua"}},
            {"SC", {"Florianopolis", "Joinville", "Blumenau", "Sao Jose", "Chapeco", "Itajai", "Criciuma", "Jaragua do Sul", "Palhoca", "Lages", "Balneario Camboriu"}},
            {"RS", {"Porto Alegre", "Caxias do Sul", "Canoas", "Pelotas", "Santa Maria", "Gravatai", "Novo Hamburgo", "Sao Leopoldo", "Rio Grande", "Passo Fundo", "Gramado", "Canela"}},

            // --- REGIÃO CENTRO-OESTE ---
            {"DF", {"Brasilia", "Taguatinga", "Ceilandia", "Gama", "Aguas Claras"}},
            {"GO", {"Goiania", "Aparecida de Goiania", "Anapolis", "Rio Verde", "Luziania", "Aguas Lindas de Goias", "Valparaiso de Goias", "Trindade"}},
            {"MT", {"Cuiaba", "Varzea Grande", "Rondonopolis", "Sinop", "Tangara da Serra", "Caceres", "Sorriso"}},
            {"MS", {"Campo Grande", "Dourados", "Tres Lagoas", "Corumba", "Ponta Pora"}},

            // --- REGIÃO NORDESTE ---
            {"BA", {"Salvador", "Feira de Santana", "Vitoria da Conquista", "Camacari", "Itabuna", "Juazeiro", "Ilheus", "Lauro de Freitas", "Jequie", "Porto Seguro"}},
            {"PE", {"Recife", "Jaboatao dos Guararapes", "Olinda", "Caruaru", "Petrolina", "Paulista", "Cabo de Santo Agostinho", "Camaragibe", "Garanhuns"}},
            {"CE", {"Fortaleza", "Caucaia", "Juazeiro do Norte", "Maracanau", "Sobral", "Crato", "Itapipoca", "Maranguape"}},
            {"MA", {"Sao Luis", "Imperatriz", "Sao Jose de Ribamar", "Timon", "Caxias", "Paco do Lumiar"}},
            {"PB", {"Joao Pessoa", "Campina Grande", "Santa Rita", "Patos", "Bayeux"}},
            {"RN", {"Natal", "Mossoro", "Parnamirim", "Sao Goncalo do Amarante", "Ceara-Mirim"}},
            {"AL", {"Maceio", "Arapiraca", "Rio Largo", "Palmeira dos Indios"}},
            {"SE", {"Aracaju", "Nossa Senhora do Socorro", "Lagarto", "Itabaiana"}},
            {"PI", {"Teresina", "Parnaiba", "Picos", "Piripiri"}},

            // --- REGIÃO NORTE ---
            {"AM", {"Manaus", "Parintins", "Itacoatiara", "Manacapuru", "Coari"}},
            {"PA", {"Belem", "Ananindeua", "Santarem", "Maraba", "Parauapebas", "Castanhal", "Abaetetuba"}},
            {"RO", {"Porto Velho", "Ji-Parana", "Ariquemes", "Vilhena", "Cacoal"}},
            {"TO", {"Palmas", "Araguaina", "Gurupi", "Porto Nacional"}},
            {"AC", {"Rio Branco", "Cruzeiro do Sul", "Sena Madureira"}},
            {"AP", {"Macapa", "Santana"}},
            {"RR", {"Boa Vista", "Rorainopolis"}}
        };

        std::map<std::string, std::vector<std::string>> us_data = {
            {"CA", {"San Francisco", "Los Angeles", "San Diego", "San Jose", "Sacramento", "Palo Alto"}}, // California
            {"NY", {"New York", "Buffalo", "Rochester", "Albany"}}, // New York
            {"TX", {"Austin", "Houston", "Dallas", "San Antonio"}}, // Texas
            {"FL", {"Miami", "Orlando", "Tampa", "Jacksonville"}},  // Florida
            {"WA", {"Seattle", "Redmond", "Olympia"}},              // Washington
            {"MA", {"Boston", "Cambridge", "Worcester"}}            // Massachusetts
        };

        // 3. Inserção no Banco (Transação para ser rápido)
        db->execute("BEGIN TRANSACTION;");

        // Insere BRASIL
        for (const auto& pair : br_data) {
            std::string state = pair.first;
            db->execute("INSERT INTO states (code, name, country) VALUES ('" + state + "', '" + state + "', 'BR');");
            
            for (const auto& city : pair.second) {
                std::string safeCity = Database::escape(city);
                db->execute("INSERT INTO cities (name, state_code) VALUES ('" + safeCity + "', '" + state + "');");
            }
        }

        // Insere EUA
        for (const auto& pair : us_data) {
            std::string state = pair.first;
            db->execute("INSERT INTO states (code, name, country) VALUES ('" + state + "', '" + state + "', 'US');");
            
            for (const auto& city : pair.second) {
                std::string safeCity = Database::escape(city);
                db->execute("INSERT INTO cities (name, state_code) VALUES ('" + safeCity + "', '" + state + "');");
            }
        }

        db->execute("COMMIT;");
        std::cout << ">> Geo Database Populated Successfully!" << std::endl;
    }

    std::string Location::normalize(const std::string& str) {
        std::string upper = str;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        return upper;
    }

    std::vector<std::string> Location::getStates(bool isEnglish) {
        auto* db = Database::getInstance();
        std::vector<std::string> list;
        std::string country = isEnglish ? "US" : "BR";

        // Busca no banco ordenado
        std::string sql = "SELECT code FROM states WHERE country = '" + country + "' ORDER BY code ASC;";
        
        db->query(sql, [&](int, char** argv, char**){
            list.push_back(argv[0]);
            return 0;
        });
        
        return list;
    }

    std::vector<std::string> Location::getCities(const std::string& state) {
        auto* db = Database::getInstance();
        std::vector<std::string> list;
        std::string safeState = Database::escape(state);
        std::string sql = "SELECT name FROM cities WHERE state_code = '" + safeState + "' ORDER BY name ASC;";

        db->query(sql, [&](int, char** argv, char**){
            list.push_back(argv[0]);
            return 0;
        });

        return list;
    }

    bool Location::isValid(const std::string& city, const std::string& state) {
        auto* db = Database::getInstance();
        bool found = false;
        
        std::string safeCity = Database::escape(city);
        std::string safeState = Database::escape(state);
        std::string sql = "SELECT count(*) FROM cities WHERE name = '" + safeCity + 
                          "' AND state_code = '" + safeState + "' COLLATE NOCASE;";

        db->query(sql, [&](int, char** argv, char**){
            if (argv[0] && std::stoi(argv[0]) > 0) found = true;
            return 0;
        });

        return found;
    }
}