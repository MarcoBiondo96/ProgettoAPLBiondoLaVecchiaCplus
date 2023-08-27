#pragma once
#include <conncpp.hpp>

class DatabaseConfig
{
private:
    std::string host, username, password, dbname;
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;

public:
    DatabaseConfig();
    ~DatabaseConfig();

    void ReadDatabaseConfig(const std::string& configFile);
    sql::ResultSet* dbQuery(const std::string& query);
    void Commit();
    void RollBack();
    void ClearConnection(int i = 0);
};
