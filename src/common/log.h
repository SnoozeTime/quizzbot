//
// Created by benoit on 18/09/09.
//

#ifndef QUIZZBOT_LOG_H
#define QUIZZBOT_LOG_H

#include <boost/log/trivial.hpp>

#define LOG_INFO \
    BOOST_LOG_STREAM_WITH_PARAMS(::boost::log::trivial::logger::get(),\
        (::boost::log::keywords::severity = ::boost::log::trivial::info))


#define LOG_DEBUG \
    BOOST_LOG_STREAM_WITH_PARAMS(::boost::log::trivial::logger::get(),\
        (::boost::log::keywords::severity = ::boost::log::trivial::debug))
#endif //QUIZZBOT_LOG_H
