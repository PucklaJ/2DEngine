#ifndef LOGMANAGER_H
#define LOGMANAGER_H
#include <ctime>
#include <string>

namespace SDL

{
    /** \brief
     *
     * Class for Logging
     *
     *
     *
     */
    class LogManager
    {
        public:
            LogManager();
            virtual ~LogManager();

            static void setFile(const char* file){m_file=file;}
            /** \brief
             *
             *  Logs some text to the console and a file
             *
             *
             *
             */

            static void log(const std::string& text);

        protected:

        private:
            static const std::string getTime();
            static const std::string getDay();

            static const char* m_file;
            static time_t m_timer;
            static struct tm* m_tm;
    };
}



#endif // LOGMANAGER_H
