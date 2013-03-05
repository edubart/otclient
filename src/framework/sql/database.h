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

#ifndef DATABASE_H
#define DATABASE_H
#include "declarations.h"

#include <framework/luaengine/luaobject.h>

#include <boost/thread.hpp>

class Database : public LuaObject
{
public:
    friend class DBTransaction;

    Database(): m_connected(false) {}
    virtual ~Database() { m_connected = false; }

    /**
    * Database connector.
    *
    * Connects the database to the source host.
    */
    virtual void connect(const std::string& host, const std::string& user, const std::string& pass,
                 const std::string& db, uint16 port, const std::string& unix_socket = "") {}

    /**
    * Transaction related methods.
    *
    * Methods for starting, commiting and rolling back transaction. Each of the returns boolean value.
    *
    * @return true on success, false on error
    * @note#include <ctime>
    *    If your database system doesn't support transactions you should return true - it's not feature test, code should work without transaction, just will lack integrity.
    */

    virtual bool beginTransaction() { return false; }
    virtual bool rollback() { return false; }
    virtual bool commit() { return false; }

    /**
    * Executes command.
    *
    * Executes query which doesn't generates results (eg. INSERT, UPDATE, DELETE...).
    *
    * @param std::string query command
    * @return true on success, false on error
    */
    virtual bool executeQuery(const std::string& query) { return false; }

    /**
    * Queries database.
    *
    * Executes query which generates results (mostly SELECT).
    *
    * @param std::string query
    * @return results object (null on error)
    */
    virtual DBResultPtr storeQuery(const std::string& query) { return nullptr; }

    /**
    * Escapes string for query.
    *
    * Prepares string to fit SQL queries including quoting it.
    *
    * @param std::string string to be escaped
    * @return quoted string
    */
    virtual std::string escapeString(const std::string&) { return "''"; }

    /**
    * Escapes binary stream for query.
    *
    * Prepares binary stream to fit SQL queries.
    *
    * @param char* binary stream
    * @param long stream length
    * @return quoted string
    */
    virtual std::string escapeBlob(const char*, uint32) { return "''"; }

    /**
     * Retrieve id of last inserted row
     *
     * @return id on success, 0 if last query did not result on any rows with auto_increment keys
     */
    virtual uint64 getLastInsertedRowID() { return 0; }

    /**
    * Get case insensitive string comparison operator
    *
    * @return the case insensitive operator
    */
    virtual std::string getStringComparer() { return "= "; }
    virtual std::string getUpdateLimiter() { return " LIMIT 1;"; }

    /**
    * Get database engine
    *
    * @return the database engine type
    */
    virtual Fw::DatabaseEngine getDatabaseEngine() { return Fw::DatabaseNone; }

    /**
    * Database connected.
    *
    * Returns whether or not the database is connected.
    *
    * @return whether or not the database is connected.
    */
    bool isConnected() { return m_connected; }

protected:
    /**
    * Database set connected.
    *
    * Sets the database to know that it is connected.
    */
    void setConnected(bool connected) { m_connected = connected; }

    virtual bool handleError() { return false; }
    virtual bool internalExecuteQuery(const std::string &query) { return false; }

    DBResultPtr verifyResult(DBResultPtr result);

    ticks_t m_use;
    bool m_connected;

private:
    static DatabasePtr m_instance;
};

class DBResult : public LuaObject
{
public:
    DBResult() {}
    virtual ~DBResult() {}

    /** Get the Integer value of a field in database
    *\returns The Integer value of the selected field and row
    *\param s The name of the field
    */
    virtual int32 getDataInt(const std::string&) { return 0; }

    /** Get the Long value of a field in database
    *\returns The Long value of the selected field and row
    *\param s The name of the field
    */
    virtual int64 getDataLong(const std::string&) { return 0; }

    /** Get the String of a field in database
    *\returns The String of the selected field and row
    *\param s The name of the field
    */
    virtual std::string getDataString(const std::string&) { return ""; }

    /** Get the blob of a field in database
    *\returns a PropStream that is initiated with the blob data field, if not exist it returns NULL.
    *\param s The name of the field
    */
    virtual const char* getDataStream(const std::string&, uint64&) { return ""; }

    /** Result freeing
    */
    virtual void free() {}

    /** Moves to next result in set
    *\returns true if moved, false if there are no more results.
    */
    virtual bool next() { return false; }

    /** Returned the number of rows from result
    *\returns integer value of row amount, 0 if result is empty.
    */
    virtual int getRowCount() { return 0; }
};

/**
 * Thread locking hack.
 *
 * By using this class for your queries you lock and unlock database for threads.
*/
class DBQuery : public std::stringstream, public LuaObject
{
friend class Database;
public:
    DBQuery() { databaseLock.lock(); }
    ~DBQuery() { databaseLock.unlock(); }

    void set(std::string& query) { str(query); }
    void append(char query) { putback(query); }

protected:
    static boost::recursive_mutex databaseLock;
};

/**
 * INSERT statement.
 *
 * Gives possibility to optimize multiple INSERTs on databases that support multiline INSERTs.
 */
class DBInsert
{
public:
    /**
    * Associates with given database handler.
    *
    * @param DatabasePtr database wrapper
    */
    DBInsert(const DatabasePtr& db): m_db(db), m_rows(0) {}
    ~DBInsert() {}

    /**
    * Sets query prototype.
    *
    * @param std::string& INSERT query
    */
    void setQuery(const std::string& query);

    /**
    * Adds new row to INSERT statement.
    *
    * On databases that doesn't support multiline INSERTs it simply execute INSERT for each row.
    *
    * @param std::string& row data
    */
    bool addRow(const std::string& row);

    /**
    * Allows to use addRow() with stringstream as parameter.
    */
    bool addRow(std::stringstream& row);

    /**
    * Executes current buffer.
    */
    bool execute();

protected:
    DatabasePtr m_db;

    uint32 m_rows;
    std::string m_query;
    std::string m_buf;
};

class DBTransaction
{
public:
    DBTransaction(DatabasePtr database)
    {
        m_db = database;
        m_state = STATE_FRESH;
    }

    ~DBTransaction()
    {
        if(m_state == STATE_READY)
            m_db->rollback();
    }

    bool begin()
    {
        m_state = STATE_READY;
        return m_db->beginTransaction();
    }

    bool commit()
    {
        if(m_state != STATE_READY)
            return false;

        m_state = STATE_DONE;
        return m_db->commit();
    }

private:
    DatabasePtr m_db;
    enum TransactionStates_t
    {
        STATE_FRESH,
        STATE_READY,
        STATE_DONE
    } m_state;
};

#endif
