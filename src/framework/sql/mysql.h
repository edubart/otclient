/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef DATABASEMYSQL_H
#define DATABASEMYSQL_H

#include "database.h"
#include <framework/global.h>
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
