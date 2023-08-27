#include "HMPartecipaLobby.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
void HMPartecipaLobby::Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) {
    string id_lobby = document["lobby_id"].GetString();
    string id_utente = document["id_utente"].GetString();
    string gioco = document["gioco"].GetString();
    string nota = document["nota"].GetString();

    try {
        ResultSet* res = dbManager.dbQuery("select account_game.id from utenti inner join account_game on utenti.id=account_game.id_utente inner join giochi on account_game.id_gioco=giochi.id where utenti.id=\"" + id_utente + "\" and giochi.nome=\"" + gioco + "\";");
        int id_account;
        res->next();
        id_account = res->getInt("id");
        //Nella query viene prelevato id_account associato ad un gioco e in quella successiva viene effettuata la partecipazione ad una lobby
        dbManager.ClearConnection();
        res = dbManager.dbQuery("INSERT INTO Prenotazioni (ID_account, ID_lobby, Nota, Stato_Prenotazione) VALUES (\"" + to_string(id_account) + "\", \"" + id_lobby + "\", \"" + nota + "\", 'Attesa')");
        Document document;
        document.SetObject();
        document.AddMember("type", "partecipazione_inviata", document.GetAllocator());
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        document.Accept(writer);
        string msg = buffer.GetString();
        dbManager.ClearConnection();
        delete res;
        conn.send_binary(msg.data());
    }
    catch (SQLException& e) {
        cout << "Errore di connessione a MariaDB: " << e.what() << endl;
    }
}
