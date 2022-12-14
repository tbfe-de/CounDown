/*
 * ===============================================================
 * Connecting subsequent stages via `std::function` and lambdas
 * ===============================================================
 * This version is basically the same as a prior one except
 * holds the limit as template argument, not in a data member.
*/
#include <climits>
#include <functional>

class BasicCounter {
public:
    unsigned get_value() const { return value_; }
    void incr() { ++value_; }
    void reset() { value_ = 0;}
private:
    unsigned value_ = 0;
};

template<unsigned limit_ = UINT_MAX>
class LimitCounter : public BasicCounter {
public:
    LimitCounter() =default;
    static constexpr unsigned get_limit() { return limit_; }
    void incr();
private:
    virtual void overflowed() { /*empty*/ }
};

template<unsigned limit_>
void LimitCounter<limit_>::incr() {
    BasicCounter::incr();
    if (get_value() == limit_) { // BasicCounter::get_value() ...
        reset();                 // BasicCounter::reset()
        overflowed();     // may be LimitCounter::overflowed()
                          // -OR-   OverflowCounter::overflowed()
    }
}

template<unsigned limit_>
class OverflowCounter : public LimitCounter<limit_> {
public:
    OverflowCounter(std::function<void()> next)
        : next_{next}
    {}
private:
    void overflowed() override;
    std::function<void()> next_;
};

template<unsigned limit_>
void OverflowCounter<limit_>::overflowed() {
    if (next_) next_();
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
    BasicCounter days_;
    OverflowCounter<24> hours_;
    OverflowCounter<60> minutes_;
    OverflowCounter<60> seconds_;
    OverflowCounter<10> sec_10th_;
};

OperationHoursMeter::OperationHoursMeter()
    : days_{}
    , hours_{[this]{ days_.incr(); }}
    , minutes_{[this]{ hours_.incr(); }}
    , seconds_{[this]{ minutes_.incr(); }}
    , sec_10th_{[this]{ seconds_.incr(); }}
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
    for (int i = 0; i < 2222222; ++i) {
        test.incr();
        std::cout << '\r' << test.to_string() << std::flush;
    }
    std::cout << std::endl;
}
