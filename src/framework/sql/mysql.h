#ifndef DATABASEMYSQL_H
#define DATABASEMYSQL_H

#include "database.h"
#include <framework/global.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#include <mysql/mysql.h>

class DBResult;

class DatabaseMySQL : public Database
{
public:
    DatabaseMySQL();
    ~DatabaseMySQL();

    void connect(const std::string& host, const std::string& user, const std::string& pass,
                 const std::string& db, uint16_t port, const std::string& unix_socket = "");

    bool beginTransaction();
    bool rollback();
    bool commit();

    bool executeQuery(const std::string &query);
    DBResultPtr storeQuery(const std::string &query);

    uint64_t getLastInsertedRowID();

    std::string escapeString(const std::string &s);
    std::string escapeBlob(const char* s, uint32_t length);

    void freeResult(DBResult *res);

protected:
    bool handleError();
    bool internalExecuteQuery(const std::string &query);

    bool m_running;
    MYSQL m_mysqlHandle;
};

class DBResult : public LuaObject
{
public:
    DBResult(MYSQL_RES* res);
     ~DBResult();

    friend class DatabaseMySQL;

    int32_t getDataInt(const std::string &s);
    int64_t getDataLong(const std::string &s);
    std::string getDataString(const std::string &s);
    const char* getDataStream(const std::string &s, unsigned long &size);

    bool next();

private:
    typedef std::map<const std::string, uint32_t> ListNames;
    ListNames m_listNames;

    MYSQL_RES* m_res;
    MYSQL_ROW m_row;
};

class DBTransaction
{
public:
    DBTransaction(DatabaseMySQL* database) {
        m_database = database;
        m_state = STATE_NO_START;
    }

    ~DBTransaction() {
        if(m_state == STATE_START) {
            m_database->rollback();
        }
    }

    bool begin() {
        m_state = STATE_START;
        return m_database->beginTransaction();
    }

    bool commit() {
        if(m_state == STATE_START) {
            m_state = STEATE_COMMIT;
            return m_database->commit();
        } else {
            return false;
        }
    }

private:
    enum TransactionStates_t {
        STATE_NO_START, STATE_START, STEATE_COMMIT
    };

    TransactionStates_t m_state;
    DatabaseMySQL* m_database;
};

#endif
