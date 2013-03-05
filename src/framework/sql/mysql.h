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

#ifdef WINDOWS
    #include <winsock2.h>
#endif

#include <mysql/mysql.h>

class DatabaseMySQL : public Database
{
public:
    DatabaseMySQL();
    virtual ~DatabaseMySQL();

    virtual void connect(const std::string& host, const std::string& user, const std::string& pass,
                 const std::string& db, uint16 port, const std::string& unix_socket = "");

    virtual bool beginTransaction();
    virtual bool rollback();
    virtual bool commit();

    virtual bool executeQuery(const std::string& query);
    virtual DBResultPtr storeQuery(const std::string& query);

    virtual std::string escapeString(const std::string &s);
    virtual std::string escapeBlob(const char* s, uint32 length);

    virtual uint64 getLastInsertedRowID();
    virtual Fw::DatabaseEngine getDatabaseEngine() {return Fw::DatabaseMySQL;}

protected:
    bool handleError();
    bool internalExecuteQuery(const std::string &query);

    MYSQL* m_handle;
};

class MySQLResult : public DBResult
{

friend class DatabaseMySQL;

public:
    MySQLResult(MYSQL_RES* result);
    virtual ~MySQLResult();

    virtual int32 getDataInt(const std::string& s);
    virtual int64 getDataLong(const std::string& s);
    virtual std::string getDataString(const std::string& s);
    virtual const char* getDataStream(const std::string& s, uint64& size);

    virtual void free();
    virtual bool next();
    virtual int getRowCount() { return mysql_num_rows(m_result); }

protected:
    typedef std::map<const std::string, uint32> RowNames_t;
    RowNames_t m_names;

    MYSQL_RES* m_result;
    MYSQL_ROW m_row;
};

#endif
