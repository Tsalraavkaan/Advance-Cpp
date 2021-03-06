#ifndef TASK2_LOGGER_H
#define TASK2_LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

namespace Task2 {
enum class Level {
    BASELVL,
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class BaseLogger {
private:
    void log(const std::string &, const Level);
    Level level_;
    std::ostream *stream_;

protected:
    BaseLogger(const Level, std::ostream *);

public:
    void flush();
    virtual ~BaseLogger() = default;
    void debug(const std::string &);
    void info(const std::string &);
    void warn(const std::string &);
    void error(const std::string &);
    void set_level(const Level);
};

class FileLogger : public BaseLogger {
private:
    std::ofstream file_;

public:
    FileLogger(const std::string &, const Level = Level::BASELVL);
    ~FileLogger() override;
};

class StdoutLogger : public BaseLogger {
public:
    StdoutLogger(const Level = Level::BASELVL);
};

class StderrLogger : public BaseLogger {
public:
    StderrLogger(const Level = Level::BASELVL);
};

}//namespace Task2


#endif