#pragma once

#include "HandlerMessage.h"
#include <rapidjson/document.h>
#include <crow/websocket.h>

class HMLogin : public HandlerMessage {
public:
    void Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) override;
};