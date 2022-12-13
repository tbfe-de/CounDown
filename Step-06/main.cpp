#include <climits>

// shows a countdown in
// - days,
// - hours,
// - minutes,
// - seconds, and
// - and tenth of a second

class I_Incrementable {
public:
    virtual ~I_Incrementable() =default;
    virtual void incr() =0;
};

class BasicCounter : public I_Incrementable {
public:
    unsigned get_value() const { return value_; }
    void incr() override { ++value_; }
protected:
    unsigned value_ = 0;
};

class LimitCounter : public BasicCounter {
public:
    LimitCounter() =default;
    LimitCounter(unsigned limit)
        : limit_{limit}
    {}
    unsigned get_limit() const { return limit_; }
    void incr() override;
private:
    virtual void overflowed() { /*empty*/ }
    unsigned const limit_ = UINT_MAX;
};

void LimitCounter::incr() {
    BasicCounter::incr();
    if (get_value() >= limit_) {
        value_ = 0;
        overflowed();
    }
}

class OverflowCounter : public LimitCounter {

public:
    OverflowCounter(unsigned limit, I_Incrementable& next)
        : LimitCounter{limit}, next_{next}
    {}
private:
    void overflowed() override;
    I_Incrementable& next_;
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
    BasicCounter days_;
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
    for (int i = 0; i < 2222222; ++i) {
        test.incr();
        std::cout << '\r' << test.to_string() << std::flush;
    }
    std::cout << std::endl;
}
