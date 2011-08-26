#include "logger.h"
#include "eventdispatcher.h"

Logger g_logger;

Logger::Logger() : m_terminated(false)
{

}

void Logger::log(LogLevel level, std::string message)
{
    const static std::string logPrefixes[] = { "", "", "WARNING: ", "ERROR: ", "FATAL ERROR: " };

    if(!m_terminated) {
        message.insert(0, logPrefixes[level]);
        std::cout << message << std::endl;

        std::size_t now = std::time(NULL);
        m_logMessages.push_back(LogMessage(level, message, now));

        if(m_onLog)
            m_onLog(level, message, now);
    }

    if(level == LogFatal) {
        m_terminated = true;
        exit(-1);
    }
}

void Logger::logFunc(LogLevel level, const std::string& message, std::string prettyFunction)
{
    std::stringstream ss;
    prettyFunction = prettyFunction.substr(0, prettyFunction.find_first_of('('));
    if(prettyFunction.find_last_of(' ') != std::string::npos)
        prettyFunction = prettyFunction.substr(prettyFunction.find_last_of(' ') + 1);

    if(!prettyFunction.empty())
        ss << "[" << prettyFunction << "] ";

    ss << message;
    log(level, ss.str());
}

void Logger::fireOldMessages()
{
    if(m_onLog) {
        auto backup = m_logMessages;
        for(const LogMessage& logMessage : backup)
            m_onLog(logMessage.level, logMessage.message, logMessage.when);
    }
}
