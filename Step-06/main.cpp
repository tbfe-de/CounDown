/*
 * ===============================================================
 * Connecting subsequent stages via `std::function` and lambdas
 * ===============================================================
 *
 * +-----------------------+  none of three classes below needs to
 * | std::function<void()> |  implement any particular interface;
 * +-----------------------+  they don't even need to participate
 *   ^                        in the same inheritance chain (ie.
 *   |                        the decision whether or not they do
 *   |                        by guided by other considerations)
 * : | : : : : : : : : : : : : : : : : : : : : : : : : : : : : : :
 *   |      +--------------+
 *   |      | BasicCounter |
 *   |      |--------------|      (difference to prior version is
 *   |      | +incr()...   |       now all three counters are in
 *   |      +--:----.------+       single chain of inheritance)
 *   |         :   /_\
 *   |         :    |      +--------------+
 *   |       just   +------| LimitCounter |  ...increment and
 *   |  increment          |--------------|  :  eventually reset
 *   |                     | +incr()      |..:
 *   |                     +------.-------+
 *   |                           /_\
 *   |                            |       +-----------------+
 *   |                            + ------| OverflowCounter |
 *   |                                    |-----------------|
 *   +------------------------------------| -overflowed()   |
 *                                  next_ +------:----------+
 *                                    :            :
 *                                    :            :
 *   *: any of the three counter classes    increment subsequent
 *    above can serve a subsequent stage        counter stage
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

class LimitCounter : public BasicCounter {
public:
    LimitCounter();
    LimitCounter(unsigned limit)
        : limit_{limit}
    {}
    unsigned get_limit() const { return limit_; }
    void incr();
private:
    unsigned const limit_ = UINT_MAX;
    virtual void overflowed() { /*empty*/ }
};

void LimitCounter::incr() {
    BasicCounter::incr();
    if (get_value() == limit_) { // BasicCounter::get_value() ...
        reset();                 // BasicCounter::reset()
        overflowed();     // may be LimitCounter::overflowed()
                          // -OR-   OverflowCounter::overflowed()
    }
}

class OverflowCounter : public LimitCounter {
public:
    OverflowCounter(unsigned limit, std::function<void()> next)
        : LimitCounter{limit}, next_{next}
    {}
private:
    void overflowed() override;
    std::function<void()> next_;
};

void OverflowCounter::overflowed() {
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
    OverflowCounter hours_;
    OverflowCounter minutes_;
    OverflowCounter seconds_;
    OverflowCounter sec_10th_;
};

OperationHoursMeter::OperationHoursMeter()
    : days_{}
    , hours_{24, [this]{ days_.incr(); }}
    , minutes_{60, [this]{ hours_.incr(); }}
    , seconds_{60, [this]{ minutes_.incr(); }}
    , sec_10th_{10, [this]{ seconds_.incr(); }}
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
