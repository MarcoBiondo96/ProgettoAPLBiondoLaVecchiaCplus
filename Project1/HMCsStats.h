#pragma once

#include "HandlerMessage.h"
#include "MyLibCurl.h"
using namespace MyLibCurl;

class HMCsStats : public HandlerMessage {
public:
    void Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) override;
};
