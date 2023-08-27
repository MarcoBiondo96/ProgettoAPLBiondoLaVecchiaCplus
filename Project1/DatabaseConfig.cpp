#include "DatabaseConfig.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
using namespace std;
using namespace sql;
using namespace rapidjson;
DatabaseConfig::DatabaseConfig() : driver(nullptr), con(nullptr), stmt(nullptr) {}//Costruttore vuoto che inizializza i puntatori da utilizzare a null

DatabaseConfig::~DatabaseConfig() {//Distruttore che effettua l'operazione di ClearConnection
    ClearConnection();
}

void DatabaseConfig::ReadDatabaseConfig(const std::string& configFile) {//Funzione che permette la lettura del file di config creando una connessione con il dB
    FILE* file;
    if (fopen_s(&file, configFile.c_str(), "r") != 0) {
        throw std::runtime_error("Failed to open config file");
    }

    char readBuffer[65536];
    FileReadStream inputStream(file, readBuffer, sizeof(readBuffer));
    Document document;
    document.ParseStream(inputStream);//Creazione di un oggetto document rapidjson che viene effettuato il parsing del contenuto letto da file (il file ha un formato json)

    fclose(file);

    if (document.HasParseError() || !document.HasMember("database")) {
        throw std::runtime_error("Failed to parse config file");
    }

    const Value& dbConfig = document["database"];
    this->host = dbConfig["host"].GetString();
    this->username = dbConfig["username"].GetString();
    this->password = dbConfig["password"].GetString();
    this->dbname = dbConfig["dbname"].GetString();
    driver = mariadb::get_driver_instance();//Restituisce un puntatore a un'istanza del driver MySQL/MariaDB. Questo driver è utilizzato per stabilire la connessione al database, eseguire query e altre operazioni relative al database
    con = nullptr;
    stmt = nullptr;
}

ResultSet* DatabaseConfig::dbQuery(const std::string& query) {//Funzione che esegue la query che viene passata come parametro 
    if (!con) {
            con = driver->connect(host, username, password);//Il driver con il connect restituisce un puntatore di Connection utile ad eseguire la connessione dal dB e nel eseguire le varie query
            con->setSchema(dbname);//Connessione al dB
        }
        if (!stmt) {
            stmt = con->createStatement();//Statement è un oggetto che permette di eseguire le query
        }
        ResultSet* res = stmt->executeQuery(query);//esecuzione query
        return res;//Invio risultato query
}

void DatabaseConfig::Commit() {//Funzione che permette di effettuare delle operazioni di commit in alcuni casi particolari all'interno del programma
    if (!con) {
        con = driver->connect(host, username, password);
        con->setSchema(dbname);
        con->setAutoCommit(false);
    }
    else {
        con->commit();
    }
}

void DatabaseConfig::RollBack() {//Funzione che permette di effettuare un rollback(utiliazzato in concomitanza al commit nel caso di esito negativo)
    if (con) {
        con->rollback();
        delete con;
        con = nullptr;
    }
}

void DatabaseConfig::ClearConnection(int i) {//Funzione che permette la Liberazione della memoria dei puntatori utilizzati precedentemente(stmt,con)
    delete stmt;
    stmt = nullptr;
    if (i == 0) {
        delete con;
        con = nullptr;
    }
}
