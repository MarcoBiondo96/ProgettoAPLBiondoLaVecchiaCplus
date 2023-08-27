#include "HMVisualizzaUtente.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
void HMVisualizzaUtente::Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) {
    string id_utente = document["username"].GetString();
    try {
        //Restituisce i dati relativi ad un utente con i relativi nickname nei diversi giochi
        ResultSet* res = dbManager.dbQuery("select utenti.id,Nickname,Giochi.Nome from utenti inner join account_game on utenti.id=account_game.id_utente inner join Giochi on account_game.id_gioco=giochi.id where utenti.id=\"" + id_utente + "\";");

        Document response;
        response.SetObject();
        response.AddMember("type", "utente_visualizzato", response.GetAllocator());
        
        Value value(id_utente.c_str(), response.GetAllocator());
        response.AddMember("id_utente", value, response.GetAllocator());

        while (res->next()) {
            if (res->getString("Nome") == "League of Legends") {
                response.AddMember("User_lol", Value().SetString(res->getString("Nickname").c_str(), response.GetAllocator()), response.GetAllocator());
            }
            else if (res->getString("Nome") == "Teamfight Tactics") {
                response.AddMember("User_tft", Value().SetString(res->getString("Nickname").c_str(), response.GetAllocator()), response.GetAllocator());
            }
            else if (res->getString("Nome") == "Counter Strike Go") {
                response.AddMember("User_csgo", Value().SetString(res->getString("Nickname").c_str(), response.GetAllocator()), response.GetAllocator());
            }
        }
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        response.Accept(writer);
        string msg = buffer.GetString();
        dbManager.ClearConnection();
        delete res;
        conn.send_binary(msg.data());
    }
    catch (SQLException& e) {
        cout << "Errore di connessione a MariaDB: " << e.what() << endl;
    }
}
