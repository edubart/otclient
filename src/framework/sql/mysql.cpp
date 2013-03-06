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

#include "mysql.h"

#include <mysql/errmsg.h>

#include <framework/core/logger.h>

DatabaseMySQL::DatabaseMySQL()
{
    m_handle = new MYSQL();
    if(!mysql_init(m_handle)) {
        g_logger.fatal("Failed to initialize MySQL connection handle.");
    }

    my_bool reconnect = true;
    mysql_options(m_handle, MYSQL_OPT_RECONNECT, &reconnect);
}

DatabaseMySQL::~DatabaseMySQL()
{
    mysql_close(m_handle);
    delete m_handle;
}

void DatabaseMySQL::connect(const std::string& host, const std::string& user, const std::string& pass,
             const std::string& db, uint16 port, const std::string& unix_socket)
{
    if(!mysql_real_connect(m_handle,
                           host.c_str(),
                           user.c_str(),
                           pass.c_str(),
                           db.c_str(),
                           port,
                           unix_socket.empty() ? NULL : unix_socket.c_str(), 0)) {
        g_logger.error(stdext::format("Failed to connect to database. MYSQL ERROR: %s", mysql_error(m_handle)));
    }
    else {
        setConnected(true);
    }
}

bool DatabaseMySQL::handleError()
{
    unsigned int error = mysql_errno(m_handle);
    g_logger.error(stdext::format("MYSQL error code = %d, message: %s", error, mysql_error(m_handle)));

    if(error == CR_SOCKET_CREATE_ERROR ||
       error == CR_CONNECTION_ERROR ||
       error == CR_CONN_HOST_ERROR ||
       error == CR_IPSOCK_ERROR ||
       error == CR_UNKNOWN_HOST ||
       error == CR_SERVER_GONE_ERROR ||
       error == CR_SERVER_LOST ||
       error == CR_SERVER_HANDSHAKE_ERR) {
        g_logger.error("MYSQL connection lost, trying to reconnect...");
        setConnected(false);

        ticks_t startTime = stdext::millis();
        while(true) {
            bool connected = (mysql_ping(m_handle) == 0);
            ticks_t diffTime = (stdext::millis() - startTime);
            if(connected) {
                g_logger.info(stdext::format("MySQL reconneted in %d ms", diffTime));
                setConnected(true);
                return true;
            }
            stdext::millisleep(100);
        }
    }

    return false;
}

bool DatabaseMySQL::beginTransaction()
{
    return executeQuery("BEGIN");
}

bool DatabaseMySQL::rollback()
{
    if(mysql_rollback(m_handle)) {
        g_logger.error(stdext::format("[DatabaseMySQL::rollback] ERROR: %s (%s)", mysql_error(m_handle), mysql_errno(m_handle)));
        return false;
    }

    return true;
}

bool DatabaseMySQL::commit()
{
    if(mysql_commit(m_handle)) {
        g_logger.error(stdext::format("[DatabaseMySQL::commit] ERROR: %s (%s)", mysql_error(m_handle), mysql_errno(m_handle)));
        return false;
    }

    return true;
}

bool DatabaseMySQL::internalExecuteQuery(const std::string &query)
{
    while(mysql_real_query(m_handle, query.c_str(), query.length()) != 0) {
        if(!handleError()) {
            return false;
        }
    }

    return true;
}

bool DatabaseMySQL::executeQuery(const std::string &query)
{
    if(internalExecuteQuery(query)) {
        MYSQL_RES* res = mysql_store_result(m_handle);

        if(res) {
            mysql_free_result(res);
        } else if(mysql_errno(m_handle) != 0) {
            handleError();
        }

        return true;
    }

    return false;
}

DBResultPtr DatabaseMySQL::storeQuery(const std::string& query)
{
    while(internalExecuteQuery(query)) {
        MYSQL_RES* res = mysql_store_result(m_handle);

        if(res) {
            DBResultPtr result = (DBResultPtr)MySQLResultPtr(new MySQLResult(res));
            if(!verifyResult(result))
                break;

            return result;
        }
        else if(mysql_errno(m_handle) != 0) {
            if(!handleError())
                break;
        }

        stdext::millisleep(10);
    }

    return nullptr;
}

uint64 DatabaseMySQL::getLastInsertedRowID()
{
    return (uint64)mysql_insert_id(m_handle);
}

std::string DatabaseMySQL::escapeString(const std::string &s)
{
    return escapeBlob( s.c_str(), s.length() );
}

std::string DatabaseMySQL::escapeBlob(const char* s, uint32 length)
{
    if(!s) {
        return std::string();
    }

    char* output = new char[length * 2 + 1];
    mysql_real_escape_string(m_handle, output, s, length);

    std::string res = "'";
    res += output;
    res += "'";

    delete[] output;
    return res;
}

int32 MySQLResult::getDataInt(const std::string& s)
{
    RowNames_t::iterator it = m_names.find(s);
    if(it != m_names.end())
        return m_row[it->second] ? atoi(m_row[it->second]) : 0;

    g_logger.error(stdext::format("[MySQLResult::getDataInt] Error: %d", s));
    return 0;
}

int64 MySQLResult::getDataLong(const std::string& s)
{
    RowNames_t::iterator it = m_names.find(s);
    if(it != m_names.end())
        return m_row[it->second] ? atoll(m_row[it->second]) : 0;

    g_logger.error(stdext::format("[MySQLResult::getDataLong] Error: %d", s));
    return 0;
}

std::string MySQLResult::getDataString(const std::string& s)
{
    RowNames_t::iterator it = m_names.find(s);
    if(it != m_names.end())
        return m_row[it->second] ? std::string(m_row[it->second]) : std::string();

    g_logger.error(stdext::format("[MySQLResult::getDataString] Error: %d", s));
    return std::string();
}

const char* MySQLResult::getDataStream(const std::string& s, uint64& size)
{
    size = 0;
    RowNames_t::iterator it = m_names.find(s);
    if(it == m_names.end()) {
        g_logger.error(stdext::format("[MySQLResult::getDataStream] Error: %d", s));
        return NULL;
    }

    if(!m_row[it->second])
        return NULL;

    size = mysql_fetch_lengths(m_result)[it->second];
    return m_row[it->second];
}

void MySQLResult::free()
{
    if(!m_result) {
        g_logger.fatal("[MySQLResult::free] Error: trying to free already freed result");
        return;
    }

    mysql_free_result(m_result);
    m_result = NULL;
}

bool MySQLResult::next()
{
    m_row = mysql_fetch_row(m_result);
    return (m_row != NULL);
}

MySQLResult::~MySQLResult()
{
    if(m_result)
        mysql_free_result(m_result);
}

MySQLResult::MySQLResult(MYSQL_RES* result)
{
    m_result = result;
    m_names.clear();

    MYSQL_FIELD* field;
    int32 i = 0;
    while((field = mysql_fetch_field(m_result))) {
        m_names[field->name] = i++;
    }
}
