#include "HMCsStats.h"

void HMCsStats::Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) {
    string nome_ev = document["nome_ev"].GetString();
    string url = "http://localhost:5000/processlogin/" + nome_ev;//Dichiarazione del url che rappresenta l'indirizzo url del server python 
    replaceSpaces(url);//Formattazione del url
    string response = gamesreturnValue(url);//Funzione che restituisce il risultato della chiamata da parte del server python
    

    if (document.HasMember("check")) {//Chiave che viene utilizzata per verificare se l'utente nel client è in possesso di un account nel suo stato attuale(non ha registrato nessun account per il gioco cs_go)
        if (response != "Errore_account_non_esistente" && response != "errore") {//Viene verificato sia se l'account è presente sui server ufficiali tramite api(gestita da python) e inoltre nel caso in cui il server python sia offline non prosegue con l'operazione
            try {
                string gioco = "Counter Strike Go";
                string utente = document["Username"].GetString();
                //query che restituisce se il nickname del gioco è stato già utilizzato da un altro utente della nostra piattaforma
                ResultSet* res = dbManager.dbQuery("Select Count(Nickname) from Utenti Inner Join Account_Game on Utenti.ID=Account_Game.ID_utente INNer Join Giochi On Account_Game.ID_gioco=Giochi.ID where Giochi.Nome=\"" + gioco + "\" and Nickname=\"" + nome_ev + "\";");
                int count = 0;
                while (res->next()) {
                    count = res->getInt("Count(Nickname)");
                    
                }
                delete res;
                dbManager.ClearConnection();
                if (count == 0) {//Caso in cui il nickname di gioco non è associato a nessun altro account
                    ResultSet* res = dbManager.dbQuery("INSERT INTO Account_Game (ID_utente, ID_gioco, Nickname) SELECT Utenti.ID, Giochi.ID,\"" + nome_ev + "\" FROM Utenti, Giochi WHERE Utenti.Username = \"" + utente + "\" AND Giochi.Nome =\"" + gioco + "\";");
                    delete res;
                    dbManager.ClearConnection();
                    string msg = "OK";
                    conn.send_binary(msg.data());
                }
                else {
                    string msg = "Errore_Account_esistente";
                    conn.send_binary(msg.data());
                }
            }
            catch (SQLException& e) {
                cout << "Errore di connessione a MariaDB: " << e.what() << endl;
            }
        }
        else
            conn.send_binary(response.data());//Viene inviato al client un messagio di errore nel caso in cui il server python è offilne o nel caso di account non esistente
    }
    else
        conn.send_binary(response.data());//Invio delle statistiche relative all'account di gioco al client
}
