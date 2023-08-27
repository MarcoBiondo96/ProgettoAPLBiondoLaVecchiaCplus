#include "HMVisualizzaLobby.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
void HMVisualizzaLobby::Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) {
    std::string gioco = document["gioco"].GetString();
    std::string user = document["user_id"].GetString();
    try {
        Document response;
        response.SetObject();
        response.AddMember("type", "visualizza_lobby", response.GetAllocator());

        Value lobby_possedute(kArrayType);
        //Query che restituisce le lobby possedute dal utente
        ResultSet* res = dbManager.dbQuery("Select Lobby.nome,Lobby.id,Orario,count(Account_Game.id) from Lobby INNER JOIN Prenotazioni On Lobby.ID=Prenotazioni.ID_lobby INNER JOIN Account_Game ON Prenotazioni.ID_account=Account_Game.ID INNER JOIN Giochi ON Account_Game.ID_gioco=Giochi.ID where giochi.nome=\"" + gioco + "\" and lobby.id_utente=\"" + user + "\" and Stato_Prenotazione=\"Accettato\" GROUP BY Lobby.id ;");
        while (res->next()) {
            Value lobby(kObjectType);
            lobby.AddMember("nomelobby", Value().SetString(res->getString("nome").c_str(), response.GetAllocator()), response.GetAllocator());
            lobby.AddMember("id_lobby", res->getInt("id"), response.GetAllocator());
            lobby.AddMember("Orario", Value().SetString(res->getString("Orario").c_str(), response.GetAllocator()), response.GetAllocator());
            lobby.AddMember("Accettati", res->getInt("count(Account_Game.id)"), response.GetAllocator());
            lobby_possedute.PushBack(lobby, response.GetAllocator());
        }
        response.AddMember("lobby_possedute", lobby_possedute, response.GetAllocator());

        Value lobby_presente(kArrayType);
        //Query che restituisce le lobby dove l'utente è stato accettato
        res = dbManager.dbQuery("Select Lobby.nome,Lobby.id,Orario,count(Account_Game.id) from Lobby INNER JOIN Prenotazioni On Lobby.ID=Prenotazioni.ID_lobby INNER JOIN Account_Game ON Prenotazioni.ID_account=Account_Game.ID INNER JOIN Giochi ON Account_Game.ID_gioco=Giochi.ID where giochi.nome=\"" + gioco + "\" and Stato_Prenotazione=\"Accettato\" and lobby.id_utente!=\"" + user + "\"  AND Lobby.id  IN(SELECT Lobby.id FROM Lobby INNER JOIN Prenotazioni ON Lobby.ID = Prenotazioni.ID_lobby INNER JOIN Account_game on Account_game.id = Prenotazioni.id_account where Account_game.id_utente= \"" + user + "\" and Stato_Prenotazione=\"Accettato\" ) GROUP BY Lobby.id; ");

        while (res->next()) {
            Value lobby(kObjectType);
            lobby.AddMember("nomelobby", Value().SetString(res->getString("nome").c_str(), response.GetAllocator()), response.GetAllocator());
            lobby.AddMember("id_lobby", res->getInt("id"), response.GetAllocator());
            lobby.AddMember("Orario", Value().SetString(res->getString("Orario").c_str(), response.GetAllocator()), response.GetAllocator());
            lobby.AddMember("Accettati", res->getInt("count(Account_Game.id)"), response.GetAllocator());
            lobby_presente.PushBack(lobby, response.GetAllocator());
        }
        response.AddMember("lobby_presente", lobby_presente, response.GetAllocator());
        Value lobby_disponibili(kArrayType);
        //Query che restituisce le lobby dove l'utente può prenotarsi 
        res = dbManager.dbQuery("Select Lobby.nome,Lobby.id,Orario,count(Account_Game.id) from Lobby INNER JOIN Prenotazioni On Lobby.ID=Prenotazioni.ID_lobby INNER JOIN Account_Game ON Prenotazioni.ID_account=Account_Game.ID INNER JOIN Giochi ON Account_Game.ID_gioco=Giochi.ID where giochi.nome=\"" + gioco + "\" and Stato_Prenotazione=\"Accettato\" and lobby.id_utente!=\"" + user + "\"  AND Lobby.id NOT IN(SELECT Lobby.id FROM Lobby INNER JOIN Prenotazioni ON Lobby.ID = Prenotazioni.ID_lobby INNER JOIN Account_game on Account_game.id = Prenotazioni.id_account where Account_game.id_utente= \"" + user + "\") GROUP BY Lobby.id; ");
        while (res->next()) {
            Value lobby(kObjectType);
            lobby.AddMember("nomelobby", Value().SetString(res->getString("nome").c_str(), response.GetAllocator()), response.GetAllocator());
            lobby.AddMember("id_lobby", res->getInt("id"), response.GetAllocator());
            lobby.AddMember("Orario", Value().SetString(res->getString("Orario").c_str(), response.GetAllocator()), response.GetAllocator());
            lobby.AddMember("Accettati", res->getInt("count(Account_Game.id)"), response.GetAllocator());
            lobby_disponibili.PushBack(lobby, response.GetAllocator());
        }
        response.AddMember("lobby_disponibili", lobby_disponibili, response.GetAllocator());
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        response.Accept(writer);
        string msg = buffer.GetString();
        dbManager.ClearConnection();
        delete res;
        conn.send_binary(msg.data());

    }
    catch (SQLException& e) {
        cout << "Errore  " << e.what() << endl;
    }
}
