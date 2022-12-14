/*
 * ===============================================================
 * Implemented as a sequence of a single type of class
 * ===============================================================
 *
 *                +------------------+
 *         +----->| ChainableCounter |
 *         | 0..1 |------------------|      (as class)
 *         |      | value_           |
 *         +------| limit_           |
 *          next_ +------------------+
 *                                          (as objects)
 *      +-----+   +------+   +--------+   +--------+   +---------+
 *  0<--|days_|<--|hours_|<--|minutes_|<--|seconds_|<--|sec_10th_|
 *      +-----+   +------+   +--------+   +--------+   +---------+
*/

#include <climits>

// shows a countdown in
// - days,
// - hours,
// - minutes,
// - seconds, and
// - and tenth of a second

class ChainableCounter {
public:
    ChainableCounter() =default;
    ChainableCounter(unsigned limit, ChainableCounter* next)
        : limit_{limit}, next_{next}
    {}
    unsigned get_value() const { return value_; }
    unsigned get_limit() const { return limit_; }
    void incr();
private:
    unsigned value_ = 0;
    unsigned const limit_ = UINT_MAX;
    ChainableCounter* const next_ = nullptr;
};

void ChainableCounter::incr() {
    if (++value_ == limit_) {
        value_ = 0;
        if (next_) next_->incr();
    }
}

// above: helper class to built many DIFFERENT kinds of counters
// ---------------------------------------------------------------
// below: a SPECIFIC type of counter built from that class

#include <iomanip>
#include <sstream>
#include <string>

class OperationHoursMeter {
public:
    OperationHoursMeter();
    std::string to_string() const;
    void incr() { sec_10th_.incr();  }
private:
    ChainableCounter days_;
    ChainableCounter hours_;
    ChainableCounter minutes_;
    ChainableCounter seconds_;
    ChainableCounter sec_10th_;
};

OperationHoursMeter::OperationHoursMeter()
    : days_{}
    , hours_{24, &days_}
    , minutes_{60, &hours_}
    , seconds_{60, &minutes_}
    , sec_10th_{10, &seconds_}
{}

std::string OperationHoursMeter::to_string() const {
    std::ostringstream os{};
    os.fill('0');
    os << days_.get_value()
       << 'd'
       << std::setw(2) << hours_.get_value()
       << ':'
       << std::setw(2) << minutes_.get_value()
       << ':'
       << std::setw(2) << seconds_.get_value()
       << '.'
       << std::setw(1) << sec_10th_.get_value();
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
