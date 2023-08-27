#pragma once

#include "HandlerMessage.h"

class HMEliminaLobby : public HandlerMessage {
public:
    void Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) override;
};
