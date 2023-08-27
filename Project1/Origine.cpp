#include <iostream>
#include <cstring>
#include <conncpp.hpp>
#include <curl\curl.h>
#include <unordered_set>
#include "crow.h"
#include <vector>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "DatabaseConfig.h"
#include "HandlerMessage.h"
#include "HMLogin.h"
#include "HMAggiungiUtente.h"
#include "HMCsStats.h"
#include "HMEliminaLobby.h"
#include "HMEliminaUtente.h"
#include "HMGrafico.h"
#include "HMInserisciLobby.h"
#include "HMLolStats.h"
#include "HMPartecipaLobby.h"
#include "HMTftStats.h"
#include "HMVisualizzaLobby.h"
#include "HMVisualizzaLobbyId.h"
#include "HMVisualizzaUtente.h"


using namespace sql;
using namespace rapidjson;


int main()
{

    crow::SimpleApp app;//Creazione di un'istanza di SimpleApp
    
    
    DatabaseConfig dbManager;//Creazione di un'instanza di DatabaseConfig che è un oggetto creato che ha la funzione di gestire il dB
    try {
        dbManager.ReadDatabaseConfig("config.json"); //Lettura dei dati di accesso relativi al dB
        
    }
    catch (const std::exception& e) {
        std::cerr << "Error reading config: " << e.what() << std::endl;
        return 1;
    }
    std::map<std::string, std::unique_ptr<HandlerMessage>> handler;//Creazione di una mappa che associa una chiave ad un'istanza di HandlerMessage dove le HM seguenti sono delle classi derivate da HandlerMessage
    handler["login"] = std::make_unique<HMLogin>();
    handler["visualizza_lobby_id"] = std::make_unique<HMVisualizzaLobbyId>();
    handler["visualizza_utente"] = std::make_unique<HMVisualizzaUtente>();
    handler["partecipa_lobby"] = std::make_unique<HMPartecipaLobby>();
    handler["aggiungi_utente"] = std::make_unique<HMAggiungiUtente>();
    handler["elimina_utente"] = std::make_unique<HMEliminaUtente>();
    handler["elimina_lobby"] = std::make_unique<HMEliminaLobby>();
    handler["inserisci_lobby"] = std::make_unique<HMInserisciLobby>();
    handler["grafico"] = std::make_unique<HMGrafico>();
    handler["csstats"] = std::make_unique<HMCsStats>();
    handler["lolstats"] = std::make_unique<HMLolStats>();
    handler["tftstats"] = std::make_unique<HMTftStats>();
    handler["visualizza_lobby"] = std::make_unique<HMVisualizzaLobby>();
    CROW_ROUTE(app, "/login")//Accesso la Crow Route di login in cui verrano successivamente gestiti i messaggi che arrivano dai client al server tramite websocket
        .websocket()
        .onopen([](crow::websocket::connection& conn) {
        std::cout << "WebSocket opened" << std::endl;
            })
        .onclose([](crow::websocket::connection& conn, const std::string& reason) {
                std::cout << "WebSocket closed: " << reason << std::endl;
            })
        .onmessage([&](crow::websocket::connection& conn, const std::string& message, bool is_binary) {
            std::cout << "WebSocket message received: " << message << std::endl;

            Document document;
            document.Parse(message.c_str());//Conversione del messaggio di arrivo da parte del client ad un tipo Document che si trova nella classe rapidjson

            if (document.HasParseError()) {
                std::cout << "Failed to parse JSON message" << std::endl;
                return "Errore";
            }
            string message_ = document["message"].GetString();
            if (handler.find(message_) != handler.end()) {//viene prelevato la key message che contiene il tipo di operazione che deve effettuare il server
                handler[message_]->Gestisci(document, dbManager,conn);//Metodo che gestisce le varie operazioni ed in base al contenuto di message_ va a selezionare la classe adeguata all'operazione che deve essere eseguita
            }
                    });
                    

       CROW_ROUTE(app, "/registrazione")//Accesso alla Crow Route di registrazione che utilizza il metodo POST per la creazione di un nuovo utente senza l'utilizzo di websocket
            .methods(crow::HTTPMethod::Post)([&dbManager](const crow::request& req) {         
                rapidjson::Document doc;
                doc.Parse(req.body.c_str());
                if (doc.HasParseError()) {
                    cout << "Failed to parse JSON message" <<endl;
                    return "Errore";
                }       
                string nome = doc["Nome"].GetString();       
                string cognome = doc["Cognome"].GetString();       
                string user = doc["Username"].GetString();       
                string pass = doc["Password"].GetString();       
                string email = doc["Email"].GetString();        
                string data_nascita = doc["Data_Nascita"].GetString();
                try {   
                    //Query che viene eseguita, nel caso in cui i dati inseriti sono giò presenti(username,password) nella creazione delle tabelle del Db c'è una legge che non permette un nuovo inserimento
                    ResultSet* res = dbManager.dbQuery("INSERT INTO Utenti (Nome, Cognome, Username, Password, Email, Data_Nascita) VALUES (\"" + nome + "\", \"" + cognome + "\", \"" + user + "\", \"" + pass + "\", \"" + email + "\", \"" + data_nascita + "\")");
                    delete res;
                    dbManager.ClearConnection();//Liberazione della memoria dei vari puntatori
                    return "OK";
                }
                catch (SQLException& e) {
                    cout << "Errore di connessione a MariaDB: " << e.what() << endl;
                    return "Errore Username o Email gia' esistenti";//Caso in cui la query non è andata a buon fine
                }
        
        
                });
            app.port(18080).multithreaded().run();//Run del applicazione si più thread e quindi gestione multi client
}



