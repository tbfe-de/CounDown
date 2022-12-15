/*
 * ===============================================================
i* Solution FlexCounter -- DOCUMENTATION NEEDS UPDATE
 * ===============================================================
 * This version is basically the same as a prior one except
 * holds the limit as template argument, not in a data member.
 *
 * !!!!!!!!!!!!!!! DOCUMENTATION IS NOT-UP-TO-DATE !!!!!!!!!!!!!!!
 * !!!!!!!!!!!!!!!!! DISREGARD THE PICTURE BELOW !!!!!!!!!!!!!!!!!
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
#include <limits>
#include <type_traits>

template<typename T, T N = std::numeric_limits<T>::max()>
class FlexCounter {
public:
    using value_type = T;
    static const value_type MAX = N;
    FlexCounter(std::function<bool()> next)
        : next_{next}
    {}
    value_type get_value() const { return value_; }
    bool incr();
private:
    value_type value_ = value_type{};
    std::function<bool()> next_;
};

template<typename T, T N>
bool FlexCounter<T, N>::incr() {
    auto const lv = value_ + 1;
    if (lv < MAX) {
        value_ = lv;
        return true;
    }
    if (next_ && next_()) {
        value_ = value_type{};
        return true;
    }
    return false;
}

// above: helper classes to built many DIFFERENT kinds of counters
// ---------------------------------------------------------------
// below: a SPECIFIC type of counter built from these classes

#include <iostream>
#include <thread>

void test_counter_chain(int n) {
    std::cout << "== " << __func__ << " ==" << std::endl;
    FlexCounter<int, 3> upper{[]{ return true; }};
    FlexCounter<int, 7> lower{[&upper]{ return upper.incr(); }};
    for (int i = 0; i < n; ++i) {
        auto const lower_not_at_limit =
            (lower.get_value()+1 != lower.MAX);
        auto const space_or_nl = lower_not_at_limit ? ' ' : '\n';
        std::cout << upper.get_value() << '/'
                  << lower.get_value() << space_or_nl
                  << std::flush;
        lower.incr();
    }
    std::cout << std::endl;
}

void test_throwing_counter(int n) {
    std::cout << "== " << __func__ << " ==" << std::endl;
    FlexCounter<int, 3> throwing{[]()-> bool { throw 42; }};
    for (int i = 0; i < n; ++i) {
        try {
            throwing.incr();
            std::cout << throwing.get_value()
                      << ' ' << std::flush;
        }
        catch(int ex) {
            std::cout << "---exception caught: "
                      << ex << std::endl;
        }
    }
}

class HhmmssChain {
public:
    HhmmssChain(bool true_or_false)
        : hh{[=]{return true_or_false; }}
    {}
    void incr() { ss.incr(); }
    std::string to_string() const;
private:
    FlexCounter<int, 24> hh;
    FlexCounter<int, 60> mm{[this]{ return hh.incr(); }};
    FlexCounter<int, 60> ss{[this]{ return mm.incr(); }};
};

std::string HhmmssChain::to_string() const {
    std::string result;
    if (hh.get_value() < 10) result += "0";
    result += std::to_string(hh.get_value());
    result += ":";
    if (mm.get_value() < 10) result += "0";
    result += std::to_string(mm.get_value());
    result += ":";
    if (ss.get_value() < 10) result += "0";
    result += std::to_string(ss.get_value());
    return result;
}

void test_hhmmss_chain(int n1, int n2) {
    std::cout << "==== " << __func__ << " ====" << std::endl;
    HhmmssChain resetting_hhmmss{true};
    HhmmssChain sticky_hhmmss{false};
    auto const old_fill = std::cout.fill('0');
    for (int i = 0; i < n1-n2; ++i) {
        resetting_hhmmss.incr();
        sticky_hhmmss.incr();
    }
    for (int i = 0; i < 2*n2; ++i) {
        resetting_hhmmss.incr();
        sticky_hhmmss.incr();
        std::cout << '\r'
                  << resetting_hhmmss.to_string()
                  << " <-------> "
                  << sticky_hhmmss.to_string()
                  << std::flush;
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(17ms);
    }
    std::cout.fill(old_fill);
    std::cout << std::endl;
}

int main() {
    test_counter_chain(25);
    test_throwing_counter(4);
    test_hhmmss_chain(24*60*60, 333);
}
