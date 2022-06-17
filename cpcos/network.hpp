#pragma once

#include <string>

#ifdef __APPLE__
#endif  // __APPLE__

#ifdef __linux__
#include <sdbus-c++/sdbus-c++.h>
#endif  // __linux__

using namespace std;

namespace cpcos {
namespace network {

/**
 * @brief Get the WiFi Intrface Name
 *
 * @return string
 */
string GetWifiIntrfaceName()
{
  string ret = "";
  // TODO
  return ret;
}

}  // namespace network
}  // namespace cpcos
