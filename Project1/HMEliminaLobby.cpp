#include "HMEliminaLobby.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
void HMEliminaLobby::Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) {
    string id_lobby = document["lobby_id"].GetString();
    try {
        //Eliminazione di una lobby da parte del creatore della lobby in base alle regole del nostro dB vengone cancellate in cascata le relative prenotazioni per tale lobby
        ResultSet* res = dbManager.dbQuery("Delete from Lobby where lobby.id=\"" + id_lobby + "\";");
        rapidjson::Document response;
        response.SetObject();
        response.AddMember("type", "eliminazione_lobby", response.GetAllocator());

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
