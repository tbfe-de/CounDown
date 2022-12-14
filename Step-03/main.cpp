/*
 * ===============================================================
 * Applying the "Non Virtual Interface" Idiom (NVI)
 * ===============================================================
 *
 *            +------------------+
 *     +----->|   LimitCounter   |  (base class)
 *     |    1 |------------------|
 *     |      | -value_          |  ...increments value_ AND CALLS
 *     |      | -limit_          |  :  overflowed() WHEN value_ IS
 *     |      |------------------|  :  RESET to 0
 *     |      | +incr()          |..:
 *     |      | -overflowed()    |.....is implemented empty here
 *     |      +---------.--------+
 *     |               /_\
 *     |                |           (derived class)
 *     |      +------------------+
 *     |      |  OverflowCounter |  ...OVERRIDES inherited (empty)
 *     +------|------------------|  :  implementation to increment
 *       next_| -overflowed()    |..:  counter stage connected via
 *            +------------------+     next_
 *
 *   +-----+   +------+   +--------+   +--------+   +---------+
 *   |days_|<--|hours_|<--|minutes_|<--|seconds_|<--|sec_10th_|
 *   +-----+   +------+   +--------+   +--------+   +---------+
 *  \___.___/ \________________________._______________________/
 *      :                              :
 *      :                              :        (as objects)
 *     LimitCounter    OverflowCounter(s)
 *
*/

#include <climits>

class LimitCounter {
public:
    LimitCounter() =default;
    LimitCounter(unsigned limit)
        : limit_{limit}
    {}
    unsigned get_value() const { return value_; }
    unsigned get_limit() const { return limit_; }
    void incr();
private:
    virtual void overflowed() { /*empty*/ }
    unsigned value_ = 0;
    unsigned const limit_ = UINT_MAX;
};

void LimitCounter::incr() {
    if (++value_ == limit_) {
        value_ = 0;
        overflowed();
    }
}

class OverflowCounter : public LimitCounter {
public:
    OverflowCounter(unsigned limit, LimitCounter& next)
        : LimitCounter{limit}, next_{next}
    {}
private:
    void overflowed() override;
    LimitCounter& next_;
};

void OverflowCounter::overflowed() {
    next_.incr();
}

// above: helper classes to built many DIFFERENT kinds of counters
// ---------------------------------------------------------------
// below: a SPECIFIC type of counter built from these classes

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

class OperationHoursMeter {
public:
    OperationHoursMeter();
    std::string to_string() const;
    void incr();
private:
    LimitCounter days_;
    OverflowCounter hours_;
    OverflowCounter minutes_;
    OverflowCounter seconds_;
    OverflowCounter sec_10th_;
};

OperationHoursMeter::OperationHoursMeter()
    : days_{}
    , hours_{24, days_}
    , minutes_{60, hours_}
    , seconds_{60, minutes_}
    , sec_10th_{10, seconds_}
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

void OperationHoursMeter::incr() {
    sec_10th_.incr();
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
