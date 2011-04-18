#ifndef TRACE_EXCEPTION_HPP_
#define TRACE_EXCEPTION_HPP_

#include <string>

namespace VerifyTAPN
{
	class trace_exception : public std::runtime_error
	{
	public:
		trace_exception(const std::string& msg):
            std::runtime_error(msg)
        {}
	};
}

#endif /* TRACE_EXCEPTION_HPP_ */
