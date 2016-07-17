//
// Created by spooky on 2016-07-10.
//

#ifndef PROJECT_MAYBE_H
#define PROJECT_MAYBE_H

#include <algorithm>

namespace sp {

    template<typename T>
    class Maybe {
    private:
        const bool m_present;
        T m_data;
    public:
        explicit Maybe(const T &p_data) :
                m_present{true},
                m_data{p_data} {
        }

        explicit Maybe(T &&p_data) :
                m_present(true),
                m_data{std::forward<T>(p_data)} {

        }

        explicit Maybe() :
                m_present{false},
                m_data{1l} {//TODO

        }

        bool is_present() const {
            return m_present;
        }

        T &get() {
            return m_data;
        }

        const T &get_or(const T &def) const {
            if (m_present) {
                return m_data;
            }
            return def;
        }

        T &get_or(T &def) {
            if (m_present) {
                return m_data;
            }
            return def;
        }
    };
}
#endif //PROJECT_MAYBE_H
