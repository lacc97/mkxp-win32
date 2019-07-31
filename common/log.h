#ifndef LOG_H
#define LOG_H

#define BOOST_ALL_DYN_LINK
#include <boost/log/trivial.hpp>

#ifndef NDEBUG
#   ifdef TRACE
#       define DO_TRACE true
#   else
#       define DO_TRACE false
#   endif
#   define DO_DEBUG true
#else
#   define DO_TRACE false
#   define DO_DEBUG false
#endif


#define trace() if constexpr(DO_TRACE) BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << ": "
#define debug() if constexpr(DO_DEBUG) BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ": "
#define info() BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << ": "
#define warn() BOOST_LOG_TRIVIAL(warning) << __FUNCTION__ << ": "
#define error() BOOST_LOG_TRIVIAL(error) << __FUNCTION__ << ": "
#define fatal() BOOST_LOG_TRIVIAL(fatal) << __FUNCTION__ << ": "


#endif
