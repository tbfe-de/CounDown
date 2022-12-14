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

template<unsigned N>
class FlexCounter {
public:
    static const unsigned MAX = N;
    FlexCounter(std::function<bool()> next)
        : next_{next}
    {}
    unsigned get_value() const { return value_; }
    bool incr();
private:
    unsigned value_ = 0;
    std::function<bool()> next_;
};

template<unsigned N>
bool FlexCounter<N>::incr() {
    if (++value_ < MAX)
        return true;
    if (next_ && next_()) {
        value_ = 0;
        return true;
    }
    return false;
}

// ---------------------------------------------------------------
// below: a SPECIFIC type of counter built from these classes

#include <iostream>

void test_counter_chain(int n) {
    FlexCounter<3> upper{[]{ return true; }};
    FlexCounter<8> lower{[&upper]{ return upper.incr(); }};
    for (int i = 0; i < n; ++i) {
        std::cout << upper.get_value() << '/'
                  << lower.get_value() << ' '
                  << std::flush;
        lower.incr();
    }
    std::cout << std::endl;
}

void test_sticky_counter(int n) {
    FlexCounter<3> sticky{[]{ return false; }};
    for (int i = 0; i < n; ++i) {
        std::cout << sticky.get_value() << ' ' << std::flush;
        sticky.incr();
    }
    std::cout << std::endl;
}

void test_throwing_counter(int n) {
    FlexCounter<3> throwing{[]()-> bool { throw 42; }};
    for (int i = 0; i < n; ++i) {
        try {
            throwing.incr();
            std::cout << throwing.get_value()
                      << ' ' << std::flush;
        }
        catch(int ex) {
            std::cout << "--- exception caught: "
                      << ex << std::endl;
        }
    }
}

int main() {
    test_counter_chain(17);
    test_sticky_counter(5);
    test_throwing_counter(4);
}
