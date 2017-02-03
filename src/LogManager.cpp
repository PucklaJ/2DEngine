#include "LogManager.h"
#include <cstdio>
#include <sstream>
#include <iostream>

namespace SDL
{
    const char* LogManager::m_file = "log.txt";
    time_t LogManager::m_timer;
    struct tm* LogManager::m_tm = NULL;

    LogManager::LogManager()
    {
        //ctor
    }

    LogManager::~LogManager()
    {
        //dtor
    }

    const std::string setTime(int i)
    {
        std::string text = "";
        std::stringstream str;

        str << i;
        str >> text;

        if(text.length() == 1)
        {
            text = "0"+text;
        }

        return text;
    }

    const std::string LogManager::getTime()
    {
        std::string text = "";

        time(&m_timer);
        m_tm = localtime(&m_timer);

        std::stringstream str;

        str << "[" << setTime(m_tm->tm_hour) << ":" << setTime(m_tm->tm_min) << ":" << setTime(m_tm->tm_sec) << "]";

        str >> text;
        str.clear();

        return text;
    }

    const std::string LogManager::getDay()
    {
        std::string text = ""; // Hallo

        time(&m_timer);
        m_tm = localtime(&m_timer);

        std::stringstream str;

        str << "[" << setTime(m_tm->tm_mday) << ":" << setTime(m_tm->tm_mon+1) << ":" << m_tm->tm_year+1900 << "]";

        str >> text;
        str.clear();

        return text;
    }

    void LogManager::log(const std::string& text)
    {
        std::string msg = getDay() + " " + getTime();

        msg += " " + text + "\n";

        std::cout << msg;

        FILE* file = fopen(m_file,"a");
        fprintf(file,msg.c_str(),"");
        fclose(file);
    }
}
