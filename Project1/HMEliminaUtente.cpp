#include "HMEliminaUtente.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
void HMEliminaUtente::Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) {
    string id_lobby = document["lobby_id"].GetString();
    string id_account = document["username"].GetString();
    try {
        //Eliminazione  della prenotazione di un utente prenotato ad una lobby da parte del creatore di suddetta lobby
        ResultSet* res = dbManager.dbQuery("Delete from prenotazioni where id_account=\"" + id_account + "\" and id_lobby=\"" + id_lobby + "\";");
        rapidjson::Document response;
        response.SetObject();
        response.AddMember("type", "utente_eliminato", response.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
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
