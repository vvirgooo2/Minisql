#ifndef MINISQL_CATALOGMANAGER_H
#define MINISQL_CATALOGMANAGER_H
#include "../SqlDataType.h"
#include <string>
#include <vector>
#include <map>

class CatalogManager {
public:
    CatalogManager();

    ~CatalogManager();

    void CreateTable(const std::string &TableName,
                     const std::vector<std::pair<std::string, attri_type>> &schemaList,
                     const std::string &PrimaryKey);

    bool ExistTable(const std::string &table) const;

    Table &GetTable(const std::string &table);

    bool ExistIndex(const std::string &IndexName) const;

    Table &GetIndex(const std::string &IndexName);

    bool RemoveTable(const Table &_table);

    void WriteToFile() const;

private:
    void LoadFromFile();

private:
    std::vector<Table> tables;

    static constexpr auto metaFileName = "tables.meta";
};


#endif //MINISQL_CATALOGMANAGER_H
