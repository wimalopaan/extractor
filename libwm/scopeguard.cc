/*
 * ++C - C++ introduction
 * Copyright (C) 2013, 2014, 2015, 2016 Wilhelm Meier <wilhelm.meier@hs-kl.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "libwm.h"

#include <vector>
#include <memory>

#define NO_TRACE
#include "tracer.h"

#include "folly/ScopeGuard.h"

#ifdef USE_BOOSTTEST
# define BOOST_TEST_DYN_LINK
# ifdef __GNUC__
// Vermeiden der boost::unit_test::ut__detail::nil_t Warnung
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Weffc++"
# endif
# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4668)
#  pragma warning(disable: 4625)
#  pragma warning(disable: 4626)
#  pragma warning(disable: 4265)
#  pragma warning(disable: 4640)
#  pragma warning(disable: 4571)
# endif
# include <boost/test/unit_test.hpp>
#endif

#ifdef USE_GTEST
# include <gtest/gtest.h>
#endif

#ifdef USE_SIMPLETEST
# include "simpletest.h"
#endif

#ifdef USE_BOOSTTEST

BOOST_AUTO_TEST_SUITE(ScopedGuardTestSuite)

BOOST_AUTO_TEST_CASE(ScopeGuard0)
{
    int i = 0;
    {
        SCOPE_EXIT{
            ++i;
        };
        BOOST_CHECK(i == 0);
    }
    BOOST_CHECK(i ==1);
}

BOOST_AUTO_TEST_CASE(ScopeGuard1)
{
    std::vector<int> v1;
    {
        SCOPE_EXIT {
            v1.pop_back();
        };
        v1.push_back(1);
    }
    BOOST_CHECK_EQUAL(v1.size(), 0);
}

BOOST_AUTO_TEST_CASE(ScopeGuard2)
{
    int i = 0;
    {
        SCOPE_SUCCESS {
            ++i;
        };
        BOOST_CHECK(i == 0);
    }
    BOOST_CHECK(i == 1);
}

BOOST_AUTO_TEST_CASE(ScopeGuard3)
{
    int i = 0;
    int j = 0;
    {
        SCOPE_SUCCESS {
            ++i;
        };
        SCOPE_FAIL {
            ++j;
        };
        BOOST_CHECK(i == 0);
        BOOST_CHECK(j == 0);
    }
    BOOST_CHECK(i == 1);
    BOOST_CHECK(j == 0);
}

BOOST_AUTO_TEST_CASE(ScopeGuard4)
{
    int i = 0;
    int j = 0;
    {
        SCOPE_SUCCESS {
            ++i;
        };
        SCOPE_FAIL {
            ++j;
        };
        SCOPE_EXIT {
            ++i;
        };
        BOOST_CHECK(i == 0);
        BOOST_CHECK(j == 0);
    }
    BOOST_CHECK(i == 2);
    BOOST_CHECK(j == 0);
}

class Mock {
public:
    Mock() {
        TRACE;
    }
    void open() {
        TRACE;
        mData.push_back("opened");
    }
    void insert1(const std::string& s) {
        TRACE;
        mData.push_back(s);
    }
    void insert2(const std::string& s) {
        TRACE;
        throw(42);
        mData.push_back(s);
    }
    void undo() {
        TRACE;
        mData.pop_back();
    }
    void close() {
        TRACE;
        mData.push_back("closed");
    }
    std::string result() const {
        std::string r;
        for(const auto& s: mData) {
            r += s;
        }
        return r;
    }
    ~Mock() {
        TRACE;
    }
    std::vector<std::string> mData;
};

BOOST_AUTO_TEST_CASE(ScopeGuard5)
{
    auto m = std::make_unique<Mock>();
    bool exeptionHappened = false;
    {
        m->open();
        try {
            m->insert1("A");
            m->insert2("B");
        }
        catch(...) {
            m->undo();
            exeptionHappened = true;
        }
        m->close();
    }
    if (exeptionHappened) {
        BOOST_CHECK_EQUAL(m->result(), "openedclosed");
    }
    else {
        BOOST_CHECK_EQUAL(m->result(), "openedABclosed");
    }
}

BOOST_AUTO_TEST_CASE(ScopeGuard6)
{
    int i = 0;
    int j = 0;
    try {
        SCOPE_FAIL {
            ++i;
        };
        SCOPE_EXIT {
            ++j;
        };
        throw 42;
    } catch(...) {}
    BOOST_CHECK(i == 1);
    BOOST_CHECK(j == 1);
}

BOOST_AUTO_TEST_CASE(ScopeGuard7)
{
    auto m = std::make_unique<Mock>();
    bool exeptionHappened = false;
    try {
        SCOPE_EXIT {
            m->close();
        };
        SCOPE_FAIL {
            m->undo();
            exeptionHappened = true;
        };

        m->open();
        m->insert1("A");
        m->insert2("B");
    } catch(...){}

    if (exeptionHappened) {
        BOOST_CHECK_EQUAL(m->result(), "openedclosed");
    }
    else {
        BOOST_CHECK_EQUAL(m->result(), "openedABclosed");
    }
}

BOOST_AUTO_TEST_CASE(ScopeGuard8)
{
    auto m = std::make_unique<Mock>();
    bool exeptionHappened = false;
    try {
        auto onExit = folly::detail::ScopeGuardOnExit() + [&](){m->close();};
        auto onFail = folly::detail::ScopeGuardOnFail() + [&](){
            m->undo();
            exeptionHappened = true;
        };
        m->open();
        m->insert1("A");
        m->insert2("B");
    } catch(...){}

    if (exeptionHappened) {
        BOOST_CHECK_EQUAL(m->result(), "openedclosed");
    }
    else {
        BOOST_CHECK_EQUAL(m->result(), "openedABclosed");
    }
}

template<typename F>
auto makeExitGuard(F&& f) {
    return folly::detail::ScopeGuardOnExit() + f;
}

template<typename F>
auto makeFailGuard(F&& f) {
    return folly::detail::ScopeGuardOnFail() + f;
}

BOOST_AUTO_TEST_CASE(ScopeGuard9)
{
    auto m = std::make_unique<Mock>();
    bool exeptionHappened = false;
    try {
        auto onExit = makeExitGuard([&](){m->close();});
        auto onFail = makeFailGuard([&](){
            m->undo();
            exeptionHappened = true;
        });
        m->open();
        m->insert1("A");
        m->insert2("B");
    } catch(...){}

    if (exeptionHappened) {
        BOOST_CHECK_EQUAL(m->result(), "openedclosed");
    }
    else {
        BOOST_CHECK_EQUAL(m->result(), "openedABclosed");
    }
}

BOOST_AUTO_TEST_SUITE_END()

#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
#ifdef _MSC_VER
# pragma warning(pop)
#endif
#endif

