#pragma once

#include "HandlerMessage.h"

class HMVisualizzaUtente : public HandlerMessage {
public:
    void Gestisci(const rapidjson::Document& document, DatabaseConfig& dbManager, crow::websocket::connection& conn) override;
};
