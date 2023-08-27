#include "HMLogin.h"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/writer.h>
void HMLogin::Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) {
    std::string username = document["username"].GetString();
    std::string password1 = document["password"].GetString();
    int count = 0;
    try {
        //Query che restituisce in base all'username e password se l'utente è presente sulla piattaforma
        ResultSet* res = dbManager.dbQuery("SELECT *,count(ID) FROM utenti where Username=\"" + username + "\" and Password=\"" + password1 + "\";");
        Document response;
        response.SetObject();
        Value name;
        res->next();
        count = res->getInt("Count(ID)");
        response.AddMember("ID", res->getInt("ID"), response.GetAllocator());
        response.AddMember("Presente", res->getInt("Count(ID)"), response.GetAllocator());
        response.AddMember("Nome", name.SetString(res->getString("Nome").c_str(), response.GetAllocator()), response.GetAllocator());
        response.AddMember("Cognome", name.SetString(res->getString("Cognome").c_str(), response.GetAllocator()), response.GetAllocator());
        response.AddMember("Username", name.SetString(res->getString("Username").c_str(), response.GetAllocator()), response.GetAllocator());
        response.AddMember("Email", name.SetString(res->getString("Email").c_str(), response.GetAllocator()), response.GetAllocator());
        response.AddMember("Data_Nascita", name.SetString(res->getString("Data_Nascita").c_str(), response.GetAllocator()), response.GetAllocator());
        delete res;
        dbManager.ClearConnection();
        if (count == 1) {//Se l'utente è presente vengono prelevati i vari dati relativi all'utente

            ResultSet* res = dbManager.dbQuery("Select Username,Giochi.Nome,Account_Game.Nickname,Account_Game.ID from Utenti Inner Join Account_Game on Utenti.ID=Account_Game.ID_utente INNer Join Giochi On Account_Game.ID_gioco=Giochi.ID where Username=\"" + username + "\";");
            while (res->next()) {

                if (res->getString("Nome") == "League of Legends") {

                    response.AddMember("User_lol", name.SetString(res->getString("Nickname").c_str(), response.GetAllocator()), response.GetAllocator());
                }
                else if (res->getString("Nome") == "Teamfight Tactics") {

                    response.AddMember("User_tft", name.SetString(res->getString("Nickname").c_str(), response.GetAllocator()), response.GetAllocator());
                }
                else if (res->getString("Nome") == "Counter Strike Go") {
                    response.AddMember("User_csgo", name.SetString(res->getString("Nickname").c_str(), response.GetAllocator()), response.GetAllocator());
                }
            }
            delete res;
            dbManager.ClearConnection();
        }

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        response.Accept(writer);
        std::string message = buffer.GetString();
        conn.send_binary(message.data());


    }
    catch (SQLException& e) {
        cout << "Errore di connessione a MariaDB: " << e.what() << endl;
    }
}
