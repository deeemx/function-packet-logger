#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <ctime>
#include <map>

class Logger {
public:
    enum class Level {
        INFO,
        WARNING,
        ERROR,
        DEBUG
    };

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void setLogFile(const std::string& filename) {
        std::lock_guard<std::mutex> guard(logMutex);
        if (logFile.is_open()) {
            logFile.close();
        }
        logFile.open(filename, std::ios::app);
        if (!logFile) {
            std::cerr << "Could not open log file: " << filename << std::endl;
        }
    }

    void log(Level level, const std::string& message) {
        std::lock_guard<std::mutex> guard(logMutex);
        if (level < currentLevel) return;

        std::string formattedMessage = format(level, message);
        if (logFile.is_open()) {
            logFile << formattedMessage << "\n";
        } else {
            std::cerr << formattedMessage << std::endl;
        }
    }

    void setLogLevel(Level level) {
        std::lock_guard<std::mutex> guard(logMutex);
        currentLevel = level;
    }

private:
    std::ofstream logFile;
    Level currentLevel = Level::INFO;
    std::mutex logMutex;

    Logger() = default;
    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string format(Level level, const std::string& message) {
        const static std::map<Level, std::string> levelNames = {
            {Level::INFO, "INFO"},
            {Level::WARNING, "WARNING"},
            {Level::ERROR, "ERROR"},
            {Level::DEBUG, "DEBUG"}
        };

        time_t now = std::time(nullptr);
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

        return "[" + std::string(timestamp) + "] [" + levelNames.at(level) + "] " + message;
    }
};
