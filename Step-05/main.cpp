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
private:
    unsigned value_ = 0;
};

template<unsigned limit_ = UINT_MAX>
class LimitCounter : public I_Incrementable {
public:
    LimitCounter() =default;
    unsigned get_value() const { return value_; }
    unsigned get_limit() const { return limit_; }
    void incr() override;
private:
    unsigned value_ = 0;
};

template<unsigned limit_>
void LimitCounter<limit_>::incr() {
    if (++value_ == limit_) {
        value_ = 0;
    }
}

template<unsigned limit_>
class OverflowCounter : public I_Incrementable {
public:
    OverflowCounter(I_Incrementable& next)
        : LimitCounter<limit_>{}, next_{next}
    {}
private:
    unsigned value_ = 0;
    I_Incrementable& next_;
};

template<unsigned limit_>
void OverflowCounter<limit_>::incr() {
    if (++value == limit_) {
        value = 0;
        next_.incr();
    }
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
