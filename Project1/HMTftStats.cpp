#include "HMTftStats.h"

void HMTftStats::Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) {//Vedi HMCsStats.cpp per i commenti funzioni simili ma adattate per giochi differenti suddivise per avere una scalabilità del codice maggiore
    string nome_ev = document["nome_ev"].GetString();
    string url = "http://localhost:5000/tft/" + nome_ev;
    replaceSpaces(url);
    string response = gamesreturnValue(url);

    if (document.HasMember("check")) {
        if (response != "Errore_account_non_esistente" && response != "errore") {

            try {
                string gioco = "Teamfight Tactics";
                string utente = document["Username"].GetString();

                ResultSet* res = dbManager.dbQuery("Select Count(Nickname) from Utenti Inner Join Account_Game on Utenti.ID=Account_Game.ID_utente INNer Join Giochi On Account_Game.ID_gioco=Giochi.ID where Giochi.Nome=\"" + gioco + "\" and Nickname=\"" + nome_ev + "\";");
                int count = 0;
                while (res->next()) {
                    count = res->getInt("Count(Nickname)");
                }
                delete res;
                dbManager.ClearConnection();
                if (count == 0) {
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
        else {
            conn.send_binary(response.data());
        }
    }
    else {
        conn.send_binary(response.data());
    }
}
