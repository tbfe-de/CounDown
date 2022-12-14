/*
 * ===============================================================
 * Connecting subsequent counter stages via an Interface
 * ===============================================================
 * This version is basically the same as a prior one except
 * holds the limit as template argument, not in a data member.
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
        : next_{next}
    {}
    unsigned get_value() const { return value_; }
    static constexpr unsigned get_limit() { return limit_; }
    void incr() override;
private:
    unsigned value_ = 0;
    I_Incrementable& next_;
};

template<unsigned limit_>
void OverflowCounter<limit_>::incr() {
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
    OverflowCounter<24> hours_;
    OverflowCounter<60> minutes_;
    OverflowCounter<60> seconds_;
    OverflowCounter<10> sec_10th_;
};

OperationHoursMeter::OperationHoursMeter()
    : days_{}
    , hours_{days_}
    , minutes_{hours_}
    , seconds_{minutes_}
    , sec_10th_{seconds_}
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
