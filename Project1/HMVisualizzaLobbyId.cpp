#include "HMVisualizzaLobbyId.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
void HMVisualizzaLobbyId::Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) {
    string id_lobby = document["lobby_id"].GetString();
    try {
        //Query che restituisce le prenotazioni relative ad una lobby specifica
        ResultSet* res = dbManager.dbQuery("Select utenti.username,Stato_Prenotazione,nota,utenti.id,(account_game.id) AS Account_id from prenotazioni inner join lobby on prenotazioni.ID_lobby=lobby.id inner join account_game on prenotazioni.ID_account=account_game.id inner join utenti on account_game.id_utente=utenti.id where lobby.id=\"" + id_lobby + "\";");

        Document response;
        response.SetObject();
        response.AddMember("type", "stato_utenti_lobby", response.GetAllocator());
        Value utenti_attesa(kArrayType);
        Value utenti_accettati(kArrayType);

        while (res->next()) {
            string stato_prenotazione = res->getString("Stato_Prenotazione").c_str();
            Value utente(kObjectType);
            utente.AddMember("username", Value().SetString(res->getString("username").c_str(), response.GetAllocator()), response.GetAllocator());
            utente.AddMember("Nota", Value().SetString(res->getString("Nota").c_str(), response.GetAllocator()), response.GetAllocator());
            utente.AddMember("id_utente", res->getInt("id"), response.GetAllocator());
            utente.AddMember("id_account", res->getInt("Account_id"), response.GetAllocator());

            //Nel caso in cui l'utente risulta accettato viene inserito in un array json se è in attesa in un'altro array json
            if (stato_prenotazione == "Accettato") {
                utenti_accettati.PushBack(utente, response.GetAllocator());
            }
            else {
                utenti_attesa.PushBack(utente, response.GetAllocator());
            }
        }
        response.AddMember("utenti_accettati", utenti_accettati, response.GetAllocator());
        response.AddMember("utenti_attesa", utenti_attesa, response.GetAllocator());

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
