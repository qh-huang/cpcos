#include <gtest/gtest.h>

#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <iostream>

using namespace std;

namespace bp = boost::process;

TEST(BoostTest, SyncProcess)
{
  bp::ipstream is;  // reading pipe-stream
  // error, following gets 'unknown file' failure
  // bp::system("/sbin/ifconfig", "-a", bp::std_out > is, bp::std_err > is, bp::std_in.close());
  // TODO: remove 'search_path', which add dependency to boost::filesystem (not header only)
  bp::child c(bp::search_path("uname"), "-a", bp::std_out > is);
  std::string line;

  while (c.running() && std::getline(is, line) && !line.empty())
    cout << line << endl;

  EXPECT_TRUE(true);
}