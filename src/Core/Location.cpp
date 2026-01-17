#include "Core/Location.h"
#include <algorithm>
#include <cctype>

namespace Core {

    // MAPA GIGANTE: ESTADO -> LISTA DE CIDADES
    static const std::map<std::string, std::vector<std::string>> GEO_DB = {
        
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
        {"RR", {"Boa Vista", "Rorainopolis"}},

        // --- USA ---
        {"CA", {"San Francisco", "Los Angeles", "San Diego", "San Jose", "Sacramento", "Palo Alto"}}, // California
        {"NY", {"New York", "Buffalo", "Rochester", "Albany"}}, // New York
        {"TX", {"Austin", "Houston", "Dallas", "San Antonio"}}, // Texas
        {"FL", {"Miami", "Orlando", "Tampa", "Jacksonville"}},  // Florida
        {"WA", {"Seattle", "Redmond", "Olympia"}},              // Washington
        {"MA", {"Boston", "Cambridge", "Worcester"}}            // Massachusetts
    };

    std::string Location::normalize(const std::string& str) {
        std::string upper = str;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        return upper;
    }

    std::vector<std::string> Location::getStates() {
        std::vector<std::string> states;
        for (const auto& pair : GEO_DB) {
            states.push_back(pair.first);
        }
        return states;
    }

    std::vector<std::string> Location::getCities(const std::string& state) {
        std::string s = normalize(state);
        
        for (const auto& pair : GEO_DB) {
            if (normalize(pair.first) == s) {
                return pair.second;
            }
        }
        return {}; 
    }

    bool Location::isValid(const std::string& inputCity, const std::string& inputState) {
        std::string s = normalize(inputState);
        std::string c = normalize(inputCity);

        const std::vector<std::string>* citiesPtr = nullptr;
        for (const auto& pair : GEO_DB) {
            if (normalize(pair.first) == s) {
                citiesPtr = &pair.second;
                break;
            }
        }

        if (!citiesPtr) return false; 

        for (const auto& dbCity : *citiesPtr) {
            if (normalize(dbCity) == c) return true;
        }

        return false;
    }
}