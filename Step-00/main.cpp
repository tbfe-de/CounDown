// a counter to count
// - days,
// - hours,
// - minutes,
// - seconds, and
// - and tenth of a second

#include <string>

class OperationHoursMeter {
public:
    OperationHoursMeter() =default;
    std::string to_string() const;
    void incr() { ++value_; }
private:
    unsigned long long value_{};
};

#include <iomanip>
#include <sstream>

std::string OperationHoursMeter::to_string() const {
    std::ostringstream os{};
    os.fill('0');
    os << value_ / (24*60*60*10)
       << 'd'
       << std::setw(2) << (value_ % (24*60*60*10) / (60*60*10))
       << ':'
       << std::setw(2) << (value_ % (60*60*10) / (60*10))
       << ':'
       << std::setw(2) << (value_ % (60*10) / 10)
       << '.'
       << std::setw(1) << (value_ % 10);
    return os.str();
}

#include <iostream>

int main() {
    OperationHoursMeter test{};
    for (int i = 0; i < 2222222; ++i) {
        test.incr();
        std::cout << '\r' << test.to_string() << std::flush;
    }
    std::cout << std::endl;
}
