#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "common/database/i_database.hpp"

class DatabaseManager
{
public:
    static DatabaseManager& get_instance();
    std::shared_ptr<IDatabase> get_database(const std::string& database_name);
    void add_database(const std::string& database_name, const std::string& database_type);
    void add_creator(const std::string& database_type, std::shared_ptr<IDatabaseCreator> database_creator);
private:
    DatabaseManager();
    ~DatabaseManager();
private:
    std::unordered_map<std::string, std::shared_ptr<IDatabase>> m_databases;
    std::unordered_map<std::string, std::shared_ptr<IDatabaseCreator>> m_database_creators;
};