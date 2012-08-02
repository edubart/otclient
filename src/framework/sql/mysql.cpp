#include "mysql.h"
#include <mysql/errmsg.h>
#include <framework/core/clock.h>
#include <framework/core/logger.h>

DatabaseMySQL::DatabaseMySQL() : m_running(false)
{
    if(!mysql_init(&m_mysqlHandle))
        g_logger.fatal("Failed to initialize MySQL connection handle.");

    my_bool reconnect = true;
    mysql_options(&m_mysqlHandle, MYSQL_OPT_RECONNECT, &reconnect);
}

DatabaseMySQL::~DatabaseMySQL()
{
    mysql_close(&m_mysqlHandle);
}

void DatabaseMySQL::connect(const std::string& host, const std::string& user, const std::string& pass,
             const std::string& db, uint16_t port, const std::string& unix_socket)
{
    if(!mysql_real_connect(&m_mysqlHandle,
                           host.c_str(),
                           user.c_str(),
                           pass.c_str(),
                           db.c_str(),
                           port,
                           unix_socket.empty() ? NULL : unix_socket.c_str(), 0)) {
        g_logger.error(stdext::format("Failed to connect to database. MYSQL ERROR: %s", mysql_error(&m_mysqlHandle)));
    }
}

bool DatabaseMySQL::handleError()
{
    unsigned int error = mysql_errno(&m_mysqlHandle);
    g_logger.error(stdext::format("MYSQL error code = %d, message: %s", error, mysql_error(&m_mysqlHandle)));

    if(error == CR_SOCKET_CREATE_ERROR ||
       error == CR_CONNECTION_ERROR ||
       error == CR_CONN_HOST_ERROR ||
       error == CR_IPSOCK_ERROR ||
       error == CR_UNKNOWN_HOST ||
       error == CR_SERVER_GONE_ERROR ||
       error == CR_SERVER_LOST ||
       error == CR_SERVER_HANDSHAKE_ERR) {
        g_logger.error("MYSQL connection lost, trying to reconnect...");

        //int64_t startTime = g_clock.millis();

        /*while(true) {
            bool connected = (mysql_ping(&m_mysqlHandle) == 0);
            uint32_t diffTime = (mTime() - startTime);
            if(connected) {
                g_logger.info(stdext::format("MySQL reconneted in %d ms", diffTime));
                return true;
            }
            mSleep(100);
        }*/
    }

    return false;
}

bool DatabaseMySQL::beginTransaction()
{
    return executeQuery("BEGIN");
}

bool DatabaseMySQL::rollback()
{
    if(mysql_rollback(&m_mysqlHandle) != 0) {
        g_logger.error(mysql_error(&m_mysqlHandle));
        return false;
    }

    return true;
}

bool DatabaseMySQL::commit()
{
    if(mysql_commit(&m_mysqlHandle) != 0) {
        g_logger.error(mysql_error(&m_mysqlHandle));
        return false;
    }

    return true;
}

bool DatabaseMySQL::internalExecuteQuery(const std::string &query)
{
    while(mysql_real_query(&m_mysqlHandle, query.c_str(), query.length()) != 0) {
        if(!handleError()) {
            return false;
        }
    }

    return true;
}

bool DatabaseMySQL::executeQuery(const std::string &query)
{
    //LOG_ONDELAY(500);

    if(internalExecuteQuery(query)) {
        MYSQL_RES *m_res = mysql_store_result(&m_mysqlHandle);

        if(m_res) {
            mysql_free_result(m_res);
        } else if(mysql_errno(&m_mysqlHandle) != 0) {
            handleError();
        }

        return true;
    }

    return false;
}

DBResult* DatabaseMySQL::storeQuery(const std::string &query)
{
    //LOG_ONDELAY(500);

    while(internalExecuteQuery(query)) {
        MYSQL_RES *m_res = mysql_store_result(&m_mysqlHandle);

        if(m_res) {
            DBResult* res = new DBResult(m_res);
            if(res->next()) {
                return res;
            } else {
                delete res;
                break;
            }
        } else if(mysql_errno(&m_mysqlHandle) != 0) {
            if(!handleError()) {
                break;
            }
        }

        //mSleep(10);
    }

    return NULL;
}

uint64_t DatabaseMySQL::getLastInsertedRowID()
{
    return (uint64_t)mysql_insert_id(&m_mysqlHandle);
}

std::string DatabaseMySQL::escapeString(const std::string &s)
{
    return escapeBlob( s.c_str(), s.length() );
}

std::string DatabaseMySQL::escapeBlob(const char* s, uint32_t length)
{
    if(!s)
        return std::string("''");

    char* output = new char[length * 2 + 1];

    mysql_real_escape_string(&m_mysqlHandle, output, s, length);
    std::string r = "'";
    r += output;
    r += "'";
    delete[] output;
    return r;
}

void DatabaseMySQL::freeResult(DBResult* res)
{
    delete res;
}

DBResult::DBResult(MYSQL_RES* res)
{
    m_res = res;
    m_listNames.clear();

    MYSQL_FIELD* field;
    int32_t i = 0;
    while((field = mysql_fetch_field(m_res))) {
        m_listNames[field->name] = i;
        i++;
    }
}

DBResult::~DBResult()
{
    mysql_free_result(m_res);
}

int32_t DBResult::getDataInt(const std::string &s)
{
    ListNames::iterator it = m_listNames.find(s);
    if(it != m_listNames.end() ) {
        if(m_row[it->second] == NULL) {
            return 0;
        }
        else {
            return atoi(m_row[it->second]);
        }
    }

    g_logger.error(stdext::format("error during getDataInt(%s).", s));
    return 0;
}

int64_t DBResult::getDataLong(const std::string &s)
{
    ListNames::iterator it = m_listNames.find(s);
    if(it != m_listNames.end()) {
        if(m_row[it->second] == NULL) {
            return 0;
        }
        else {
            return atoll(m_row[it->second]);
        }
    }

    g_logger.error(stdext::format("error during getDataLong(%s).", s));
    return 0;
}

std::string DBResult::getDataString(const std::string &s)
{
    ListNames::iterator it = m_listNames.find(s);
    if(it != m_listNames.end() ) {
        if(m_row[it->second] == NULL)
        return std::string("");
        else
        return std::string(m_row[it->second]);
    }

    g_logger.error(stdext::format("error during getDataString(%s).", s));
    return std::string("");
}

const char* DBResult::getDataStream(const std::string &s, unsigned long &size)
{
    ListNames::iterator it = m_listNames.find(s);
    if(it != m_listNames.end() ) {
        if(m_row[it->second] == NULL) {
            size = 0;
            return NULL;
        }
        else {
            size = mysql_fetch_lengths(m_res)[it->second];
            return m_row[it->second];
        }
    }

    g_logger.error(stdext::format("error during getDataStream(%s).", s));
    size = 0;
    return NULL;
}

bool DBResult::next()
{
    m_row = mysql_fetch_row(m_res);
    return m_row != NULL;
}
