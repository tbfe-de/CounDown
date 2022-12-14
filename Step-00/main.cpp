/*
 * ===============================================================
 * Straight Forward Direct Implementation
 * ===============================================================
 * The `OperarionHoursMeter` is expected to be ticked every 100ms
 * and accumulates the number of ticks as:
 *  - basically unlimited full days
 *  - 0..23 as hours within a single day
 *  - 0..59 minutes within a single hour
 *  - 0..59 seconds within a single minute
 *  - 0..9 fractional digits
 * The member functions `to_string` returns this in a human
 * readable format.
*/

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
    for (int i = 0; i < 2'222'222; ++i) {
        test.incr();
        std::cout << '\r' << test.to_string() << std::flush;
    }
    std::cout << std::endl;
}
