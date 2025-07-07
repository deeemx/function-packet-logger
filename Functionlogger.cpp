#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <ctime>
#include <map>

class L {
public:
    enum class LV { I, W, E, D };

    static L& G() {
        static L x;
        return x;
    }

    void F(const std::string& f) {
        std::lock_guard<std::mutex> g(m);
        if (s.is_open()) s.close();
        s.open(f, std::ios::app);
        if (!s) std::cerr << "Can't open: " << f << std::endl;
    }

    void Lg(LV l, const std::string& msg) {
        std::lock_guard<std::mutex> g(m);
        if (l < c) return;
        std::string z = T(l, msg);
        if (s.is_open()) s << z << "\n";
        else std::cerr << z << std::endl;
    }

    void S(LV l) {
        std::lock_guard<std::mutex> g(m);
        c = l;
    }

private:
    std::ofstream s;
    LV c = LV::I;
    std::mutex m;

    L() = default;
    ~L() { if (s.is_open()) s.close(); }
    L(const L&) = delete;
    L& operator=(const L&) = delete;

    std::string T(LV l, const std::string& msg) {
        static const std::map<LV, std::string> n = {
            {LV::I, "INFO"}, {LV::W, "WARNING"},
            {LV::E, "ERROR"}, {LV::D, "DEBUG"}
        };
        time_t t = std::time(nullptr);
        char ts[20];
        std::strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        return "[" + std::string(ts) + "] [" + n.at(l) + "] " + msg;
    }
};
