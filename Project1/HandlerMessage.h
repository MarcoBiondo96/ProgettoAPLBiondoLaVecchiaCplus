#pragma once
#include "crow.h"
#include "rapidjson/document.h"
#include "DatabaseConfig.h"
using namespace std;
using namespace sql;
using namespace rapidjson;
class HandlerMessage
{
public:
    virtual void Gestisci(const Document& messaggio,DatabaseConfig& dbManager, crow::websocket::connection& conn) = 0;//Funzione in cui viene fatto un Override nelle classi figlie
};

