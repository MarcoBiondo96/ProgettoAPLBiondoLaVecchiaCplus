#include "HMAggiungiUtente.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
using namespace rapidjson;
void HMAggiungiUtente::Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) {
    string id_lobby = document["lobby_id"].GetString();
    string id_utente = document["username"].GetString();
    try {
        //Query che permette di aggiornare lo stato di prenotazione di una lobby di un utente da in attesa ad accettato(da parte del creatore della lobby)
        ResultSet* res = dbManager.dbQuery("Update prenotazioni set Stato_Prenotazione='Accettato' where id_account=\"" + id_utente + "\" and id_lobby=\"" + id_lobby + "\";");
        rapidjson::Document response;
        response.SetObject();//Assegnazione di del contenuto di response come un oggetto json vuoto
        response.AddMember("type", "utente_aggiunto", response.GetAllocator());// Viene aggiunto un nuova coppia chiave valore response con chiave type e valore utente_aggiunto mentre GetAllocator() viene utilizzato per ottenere l'allocazione della memoria nell'oggetto response per memorizzare il valore. L'allocazione della memoria è gestita internamente da RapidJSON.

        rapidjson::StringBuffer buffer;//Questa variabile verrà utilizzata per accumulare il testo della rappresentazione JSON.
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);//Writer viene utilizzato per serializzare oggetti JSON in testo JSON formattato.
        response.Accept(writer);// response che è l'oggetto JSON verrà serializzato e scritto nel buffer writer

        std::string msg = buffer.GetString();
        dbManager.ClearConnection();
        delete res;
        
        conn.send_binary(msg.data());//Invio del messaggio di risposta che risulta essere un Document rapidjson convertito a stringa al client per la conferma dell'opeazione svolta
    }
    catch (SQLException& e) {
        cout << "Errore di connessione a MariaDB: " << e.what() << endl;
    }
}