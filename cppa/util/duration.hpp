#ifndef DURATION_HPP
#define DURATION_HPP

#include <chrono>
#include <cstdint>

#include <boost/thread/thread_time.hpp>

namespace cppa { namespace util {

enum class time_unit : std::uint32_t
{
    none = 0,
    seconds = 1,
    milliseconds = 1000,
    microseconds = 1000000
};

template<typename Period>
constexpr time_unit get_time_unit_from_period()
{
    return (Period::num != 1
            ? time_unit::none
            : (Period::den == 1
               ? time_unit::seconds
               : (Period::den == 1000
                  ? time_unit::milliseconds
                  : (Period::den == 1000000
                     ? time_unit::microseconds
                     : time_unit::none))));
}

class duration
{

 public:

    constexpr duration() : unit(time_unit::none), count(0)
    {
    }

    constexpr duration(time_unit un, std::uint32_t val) : unit(un), count(val)
    {
    }

    template <class Rep, class Period>
    constexpr duration(std::chrono::duration<Rep, Period> d)
        : unit(get_time_unit_from_period<Period>()), count(d.count())
    {
        static_assert(get_time_unit_from_period<Period>() != time_unit::none,
                      "only seconds, milliseconds or microseconds allowed");
    }

    time_unit unit;

    std::uint32_t count;

};

bool operator==(const duration& lhs, const duration& rhs);

inline bool operator!=(const duration& lhs, const duration& rhs)
{
    return !(lhs == rhs);
}

} } // namespace cppa::util

namespace boost {

system_time& operator+=(system_time& lhs, const cppa::util::duration& d);

} // namespace boost

#endif // DURATION_HPP
