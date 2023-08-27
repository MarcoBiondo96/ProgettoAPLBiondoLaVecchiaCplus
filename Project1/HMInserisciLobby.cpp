#include "HMInserisciLobby.h"

void HMInserisciLobby::Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) {
    std::string gioco = document["gioco"].GetString();
    std::string nome = document["nome"].GetString();
    std::string id_utente = document["utente_id"].GetString();
    std::string orario = document["orario"].GetString();
    std::string nota = document["nota"].GetString();

    dbManager.Commit();//Richiamo della funzione commit che permette di effettuare un operazione atomica
    try {
        //Creazione di una lobby
        ResultSet* res = dbManager.dbQuery("INSERT INTO Lobby (Orario, Nome, ID_utente) VALUES (\"" + orario + "\", \"" + nome + "\", \"" + id_utente + "\");");
        dbManager.ClearConnection(1);//Il parametro ad 1 permette di cancellare unicamente lo statement mantenendo la con attiva in quanto stamo eseguendo delle operazioni atomiche
        res = dbManager.dbQuery("SELECT LAST_INSERT_ID()");
        res->next();
        int id_lobby = res->getInt(1);
        dbManager.ClearConnection(1);
        //Restituzione del id_account del utente che ha creato la lobby
        res = dbManager.dbQuery("Select Account_Game.id from Lobby inner join Utenti on Lobby.ID_utente=Utenti.ID Inner Join Account_Game on Utenti.ID=Account_Game.ID_utente INNer Join Giochi On Account_Game.ID_gioco=Giochi.ID where giochi.nome=\"" + gioco + "\" and utenti.id=\"" + id_utente + "\";");
        res->next();
        int id_acc = res->getInt("id");
        dbManager.ClearConnection(1);
        //Creazione della prenotazione da parte del creatore della lobby nella sua lobby appena creata
        res = dbManager.dbQuery("INSERT INTO Prenotazioni (ID_account, ID_lobby, Nota, Stato_Prenotazione) VALUES (" + std::to_string(id_acc) + ", " + std::to_string(id_lobby) + ", \"" + nota + "\", 'Accettato')");

        // Conferma la transazione
        dbManager.Commit();

        std::cout << "Nuova lobby e prenotazione create con successo!" << std::endl;
        dbManager.ClearConnection();
        delete res;

        string msg = "Lobby Inserita";
        conn.send_binary(msg.data());
    }
    catch (SQLException& e) {
        //Caso in cui si verifichino degli errori viene eseguito un rollback garantendo l'atomicità delle operazioni precedenti
        dbManager.RollBack();
        cout << "Errore  MariaDB: " << e.what() << endl;
        string msg = "Errore";
        conn.send_binary(msg.data());
    }
}
