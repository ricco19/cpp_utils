#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <string>

namespace utils {

class stopwatch {
    using timepoint_t = std::chrono::high_resolution_clock::time_point;
    using ns_t = std::chrono::nanoseconds;

  private:
    timepoint_t start_ = timepoint_t();
    timepoint_t stop_ = timepoint_t();
    ns_t delta_ = std::chrono::nanoseconds(0);
    bool was_stopped_ = false;

  public:
    inline void start() noexcept {
        was_stopped_ = false;
        start_ = std::chrono::high_resolution_clock::now();
    }
    inline void stop() noexcept {
        stop_ = std::chrono::high_resolution_clock::now();
        was_stopped_ = true;
        delta_ = std::chrono::duration_cast<ns_t>(stop_ - start_);
    }
    inline ns_t delta() const noexcept {
        if (!was_stopped_) {
            return std::chrono::nanoseconds(0);
        }
        return delta_;
    }
};

class clock {
    using clk_t = std::chrono::high_resolution_clock;
    using timepoint_t = std::chrono::high_resolution_clock::time_point;
    using ns_t = std::chrono::nanoseconds;
    using us_t = std::chrono::microseconds;
    using ms_t = std::chrono::milliseconds;
    using sec_t = std::chrono::seconds;
    using min_t = std::chrono::minutes;
    using hour_t = std::chrono::hours;

  private:
    const timepoint_t start_ = clk_t::now();

  public:
    inline ns_t nanos() const noexcept {
        return std::chrono::duration_cast<ns_t>(clk_t::now() - start_);
    }
    inline us_t micros() const noexcept {
        return std::chrono::duration_cast<us_t>(clk_t::now() - start_);
    }
    inline ms_t millis() const noexcept {
        return std::chrono::duration_cast<ms_t>(clk_t::now() - start_);
    }
    inline sec_t seconds() const noexcept {
        return std::chrono::duration_cast<sec_t>(clk_t::now() - start_);
    }
    inline min_t minutes() const noexcept {
        return std::chrono::duration_cast<min_t>(clk_t::now() - start_);
    }
    inline hour_t hours() const noexcept {
        return std::chrono::duration_cast<hour_t>(clk_t::now() - start_);
    }

    std::string elapsed() const {
        using namespace std::literals::chrono_literals;
        const auto ns = nanos();
        if (ns < 1us) {
            return std::string(std::to_string(ns.count()) + "ns");
        }
        if (ns < 1ms) {
            const auto d = std::chrono::duration_cast<us_t>(ns);
            return std::string(std::to_string(d.count()) + "us");
        }
        if (ns < 1s) {
            const auto d = std::chrono::duration_cast<ms_t>(ns);
            return std::string(std::to_string(d.count()) + "ms");
        }
        if (ns < 1min) {
            const auto d = std::chrono::duration_cast<sec_t>(ns);
            return std::string(std::to_string(d.count()) + "s");
        }
        if (ns < 1h) {
            const auto m = std::chrono::duration_cast<min_t>(ns);
            const auto s = std::chrono::duration_cast<sec_t>(ns % 1min);
            return std::string(std::to_string(m.count()) + "m " +
                               std::to_string(s.count()) + "s");
        }
        const auto h = std::chrono::duration_cast<hour_t>(ns);
        const auto m = std::chrono::duration_cast<min_t>(ns % 1h);
        const auto s = std::chrono::duration_cast<sec_t>(ns % 1min);
        return std::string(std::to_string(h.count()) + "h " +
                           std::to_string(m.count()) + "m " +
                           std::to_string(s.count()) + "s");
    }
};

inline uint_fast32_t the_time() {
    using ms_t = std::chrono::duration<uint_fast32_t, std::milli>;
    using clk_t = std::chrono::high_resolution_clock;
    return ms_t(clk_t::now().time_since_epoch().count()).count();
}

} // namespace utils

#endif
