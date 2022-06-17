#pragma once

#include <iostream>
#include <string>

#if defined(__GNUC__)
// Boost 1.78 requires the following workaround.
// See: https://github.com/boostorg/process/issues/235
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#endif
#include <boost/process.hpp>
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

using namespace std;
namespace bp = boost::process;

namespace cpcos {

/**
 * @brief execute system command and get output
 *
 * @param cmd
 * @return string
 */
string SysExec(const string& cmd)
{
  string ret = "";
  // TODO
  int result = bp::system(cmd);
  cout << "result: " << result << endl;
  return ret;
}

}  // namespace cpcos
