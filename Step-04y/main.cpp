/*
 * ===============================================================
 * Connecting subsequent counter stages via an Interface
 * ===============================================================
 *
 *                   <<interface>>
 *               +------------------+
 *               | I_Incrementable  |
 *               |------------------|  ALL counter classes below
 *               | incr()           |  IMPLEMENT this interface
 *               +------------------+  directly[*] but are NOT
 *                     ^               in an inheritance chain
 *                     |
 *                     +----------------+
 *                                      |
 *  +--------------+  +--------------+  |     +-----------------+
 *  | BasicCounter |  | LimitCounter |  |     | OverflowCounter |
 *  |--------------|  |--------------|  |     |-----------------|
 *  | +incr()...   |  | +incr()...   |  +-----| +incr()...      |
 *  +----------:---+  +----------:---+  next_ +----------:------+
 *              :                :                       :
 *              :                :                       :
 *  just increment    increment and                    increment,
 *                   eventually reset           eventually reset,
 *                                                  and then also
 *   *: any of the three counter classes     increment subsequent
 *    above can serve a subsequent stage            counter stage
*/
#include <climits>

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

class LimitCounter : public I_Incrementable {
public:
    LimitCounter() =default;
    unsigned get_value() const { return value_; }
    LimitCounter(unsigned limit)
        : limit_{limit}
    {}
    unsigned get_limit() const { return limit_; }
    void incr() override;
private:
    unsigned value_ = 0;
    unsigned const limit_ = UINT_MAX;
};

void LimitCounter::incr() {
    if (++value_ == limit_) {
        value_ = 0;
    }
}

class OverflowCounter : public I_Incrementable {
public:
    OverflowCounter(unsigned limit, I_Incrementable& next)
        : limit_{limit}, next_{next}
    {}
    unsigned get_value() const { return value_; }
    virtual void incr();
private:
    unsigned value_ = 0;
    unsigned const limit_ = UINT_MAX;
    I_Incrementable& next_;
};

void OverflowCounter::incr() {
    if (++value_ == limit_) {
        value_ = 0;
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
