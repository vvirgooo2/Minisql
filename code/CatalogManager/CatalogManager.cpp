#include "CatalogManager.h"
#include "../SqlDataType.h"
#include "../RecordManager/RecordManager.h"
#include <fstream>

extern RecordManager *rm;

/**
 * Constructor
 */
CatalogManager::CatalogManager() : tables(std::vector<Table>()) {
    LoadFromFile();
}

/**
 * Deconstruct
 */
CatalogManager::~CatalogManager() {
    WriteToFile();
}

/**
 * Write to file
 * 
 * 
 */
void CatalogManager::WriteToFile() const {
    std::ofstream fileOut(metaFileName);
    fileOut << tables.size() << std::endl;

    for (const auto &table: tables) {
        fileOut << table.tablename << std::endl;

       // fileOut << table.recordCnt << std::endl;

        std::ofstream catalogFileOut(table.tablename + ".catalog");
        catalogFileOut << table.attri_names.size() << std::endl;
        int i = 0;
        for (const auto &attri_names: table.attri_names) {
            // attr name
            catalogFileOut << attri_names << std::endl;
            // attr type
            const auto &attr = table.attri_types[i];
            switch (attr.type) {
                case AType::Integer:
                    catalogFileOut << "int" << std::endl;
                    break;
                case AType::Float:
                    catalogFileOut << "float" << std::endl;
                    break;
                case AType::String:
                    catalogFileOut << "char" << std::endl;
                    break;
            }

            // check if is string
            if (attr.type == AType::String) {
                catalogFileOut << attr.char_sz << std::endl;
            } else {
                catalogFileOut << 0 << std::endl;
            }
            // attr primary / unique
            catalogFileOut << (attr.primary ? 1 : 0) << std::endl;
            catalogFileOut << (attr.unique ? 1 : 0) << std::endl;
            auto index = std::find_if(table.index.begin(), table.index.end(),
                                      [&attri_names](const std::pair<std::string, std::string> &p) {
                                          return p.first == attri_names;
                                      });
            if (index != table.index.end()) {
                catalogFileOut << 1 << std::endl << index->second << std::endl;
            } else {
                catalogFileOut << 0 << std::endl << "-" << std::endl;
            }
            ++i;
        }
        catalogFileOut.close();
    }
    fileOut.close();
}


/**
 * Read File
 *
 */
void CatalogManager::LoadFromFile() {
    std::ifstream fileIn(metaFileName);
    if (!fileIn.is_open()) {
        std::ofstream touch(metaFileName); // create file is not exists
        return;
    }

    int tables_count;
    fileIn >> tables_count;


    // read each table from file
    std::string tableName;
    for (auto i = 0; i < tables_count; ++i) {
        fileIn >> tableName;
        auto fileName = tableName + ".catalog";
        std::ifstream attrFileIn(fileName);

        Table table;
        sqlvalue type;        
        std::vector<sqlvalue> indexVector;

        table.tablename = tableName;

        int countsCheck = 0;
        int recordLength = 0;
        int attrCounts;
        attrFileIn >> attrCounts;
        for (auto ci = 0; ci < attrCounts; ++ci) {
            std::string attrName, typeName, indexName;
            int isPrimary, isUnique, isIndex, size;


            attrFileIn >> attrName >> typeName >> size >> isPrimary >> isUnique >> isIndex >> indexName;
            countsCheck++; // check if attr cnt correct
            table.attri_names.push_back(attrName);

            // set type
            if (typeName == "int") {
                type.type.type = AType::Integer ;
            } else if (typeName == "char") {
                type.type.type = AType::String;
            } else if (typeName == "float") {
                type.type.type = AType::Float;
            } else {
                ; // TODO corupted file
            }



            recordLength += type.type.getsize();
            type.type.primary = isPrimary != 0;
            type.type.unique = isUnique != 0;
            type.type.attri_name = attrName;
            table.attri_types.push_back(type.type);

            // if is index
            if (isIndex) {
                auto index = std::make_pair(attrName, indexName);
                table.index.push_back(index);
                indexVector.push_back(type);
            }
        }
        table.attri_count = countsCheck;
        table.row_num = recordLength;
        tables.push_back(table);
    }
}


/**
 * Create table
 * TableName table name string
 * schemaList schemas vector
 * PrimaryKey primary key string
 */
void CatalogManager::CreateTable(const std::string &TableName,
                                 const std::vector<std::pair<std::string, attri_type>> &schemaList,
                                 const std::string &PrimaryKey) {
    Table table;
    table.tablename = TableName;
    table.attri_count = (int) schemaList.size();
    int len = 0;
    char autoIndex = '1';



    // Make attribute
    for (auto &schema: schemaList) {
        //len += schema.second.getsize();
        table.attri_names.push_back(schema.first);
        auto t = schema.second;
        t.attri_name = schema.first;
        table.attri_types.push_back(t);
        if (schema.first == PrimaryKey) {
            (table.attri_types.end() - 1)->primary = true;
            (table.attri_types.end() - 1)->unique = true;
        }
    }
    table.row_num = len;

//    // Setup index
//    for (auto &type: table.attrType) {
//        if (type.unique && !type.primary) {
//            table.index.emplace_back(std::make_pair(type.attrName, std::string("autoIndex_") + (autoIndex++)));
//            rm->createIndex(table, type);
//        }
//    }
    tables.push_back(table);
}

/**
 * Check if table exists
 * TableName table name string
 * return is exists
 */
bool CatalogManager::ExistTable(const std::string &TableName) const {
    return std::find_if(tables.begin(), tables.end(), [&TableName](const Table &table) {
        return (table.tablename == TableName);
    }) != tables.end();
}

/**
 * Get a table
 * TableName table name string
 * return the ref of the table
 */
Table &CatalogManager::GetTable(const std::string &TableName) {
    return *std::find_if(tables.begin(), tables.end(), [&TableName](const Table &table) {
        return (table.tablename == TableName);
    });
}

/**
 * Check if index name exists
 * IndexName index name string
 * return if exists
 */
bool CatalogManager::ExistIndex(const std::string &IndexName) const {
    for (const auto &table: tables) {
        for (auto &index: table.index) {
            if (index.second == IndexName) return true;
        }
    }
    return false;
}

/**
 * Get index
 * return table containing that index
 */
Table &CatalogManager::GetIndex(const std::string &IndexName) {
    for (auto &table: tables) {
        for (const auto &index: table.index) {
            if (index.second == IndexName) return table;
        }
    }
}

/**
 * Remove table
 * table name string
 * return isSuc
 */
bool CatalogManager::RemoveTable(const Table &table) {
    if (std::find_if(tables.begin(), tables.end(),
            [&table](const Table &_table) { return _table.tablename == table.tablename; }) == tables.end())
        return false;
    tables.erase(std::find_if(tables.begin(), tables.end(),
            [&table](const Table &_table) { return _table.tablename == table.tablename; }));
    return true;
}
