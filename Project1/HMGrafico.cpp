#include "HMGrafico.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "MyLibCurl.h"
void HMGrafico::Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) {
	string gioco = document["gioco"].GetString();
	string tipologia = document["tipologia"].GetString();
	string giorno = document["giorno"].GetString();
	string mese = document["mese"].GetString();
	string anno= document["anno"].GetString();
	Document response;
	response.SetObject();
	Value value(tipologia.c_str(), response.GetAllocator());//Creazione di un oggetto value necessario per l'inserimento di una variabile stringa all'interno di response
	response.AddMember("tipologia", value, response.GetAllocator());
	Value value1(gioco.c_str(), response.GetAllocator());
	response.AddMember("gioco", value1, response.GetAllocator());
	Value orari_lobby(kArrayType);//Creazione di un array di tipo json 
	try {
		ResultSet* res=nullptr;
		if (tipologia == "dailyuser") {//In base al valore ricevuto dal client che si trova nella key tipologia esegue una query differente che restituisce dei dati necessari per calcolare differenti grafici
			res = dbManager.dbQuery("SELECT Orario FROM lobby INNER JOIN Prenotazioni ON lobby.id = prenotazioni.id_lobby INNER JOIN account_game ON prenotazioni.id_account = account_game.id INNER JOIN Giochi ON account_game.id_gioco = giochi.id WHERE giochi.Nome=\"" + gioco + "\" and YEAR(lobby.orario) =\"" + anno + "\" and MONTH(lobby.orario) =\"" + mese + "\" and  DAY(lobby.orario) =\"" + giorno + "\";");
		}
		else if (tipologia == "monthuser") {
			res = dbManager.dbQuery("SELECT Orario FROM lobby INNER JOIN Prenotazioni ON lobby.id = prenotazioni.id_lobby INNER JOIN account_game ON prenotazioni.id_account = account_game.id INNER JOIN Giochi ON account_game.id_gioco = giochi.id WHERE giochi.Nome=\"" + gioco + "\" and YEAR(lobby.orario) =\"" + anno + "\" and MONTH(lobby.orario) =\"" + mese + "\";");
		}
		else if (tipologia == "yearuser") {
			res = dbManager.dbQuery("SELECT Orario FROM lobby INNER JOIN Prenotazioni ON lobby.id = prenotazioni.id_lobby INNER JOIN account_game ON prenotazioni.id_account = account_game.id INNER JOIN Giochi ON account_game.id_gioco = giochi.id WHERE giochi.Nome=\"" + gioco + "\" and YEAR(lobby.orario) =\"" + anno + "\";");
		}
		while (res->next()) {
			orari_lobby.PushBack(Value().SetString(res->getString("Orario").c_str(), response.GetAllocator()), response.GetAllocator());//Inserisce il risultato della row della query all'interno del array 
		}
		response.AddMember("orari_lobby", orari_lobby, response.GetAllocator());//Associazione chiave valore con valore l'array di tipo json
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		response.Accept(writer);
		string msg = buffer.GetString();
		dbManager.ClearConnection();
		delete res;
		
		SendPythonRequest(conn, msg);//Funzione che permette di effettuare una chiamata al server python che restituisce un immagine al client la invia tramite conn
	}
	catch (SQLException& e) {
		cout << "Errore  " << e.what() << endl;
	}
	
	
}