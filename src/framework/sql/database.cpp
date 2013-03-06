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

#include "database.h"

boost::recursive_mutex DBQuery::databaseLock;

DBResultPtr Database::verifyResult(DBResultPtr result)
{
    if(result->next())
        return result;

    result->free();
    return nullptr;
}

void DBInsert::setQuery(const std::string& query)
{
    m_query = query;
    m_buf = "";
    m_rows = 0;
}

bool DBInsert::addRow(const std::string& row)
{
    ++m_rows;
    if(m_buf.empty()) {
        m_buf = "(" + row + ")";
    }
    else if(m_buf.length() > 8192)
    {
        if(!execute())
            return false;

        m_buf = "(" + row + ")";
    }
    else
        m_buf += ",(" + row + ")";

    return true;
}

bool DBInsert::addRow(std::stringstream& row)
{
    bool ret = addRow(row.str());
    row.str("");
    return ret;
}

bool DBInsert::execute()
{
    if(m_buf.empty() || !m_rows)
        return true;

    m_rows = 0;
    bool ret = m_db->executeQuery(m_query + m_buf);
    m_buf = "";
    return ret;
}
