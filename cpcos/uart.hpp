#pragma once

#include <serial/serial.h>

#include <iostream>
#include <memory>
#include <mutex>

using namespace std;

namespace cpcos {

using serial::Serial;

class Uart
{
public:
  Uart() : serial_(nullptr) {}
  Uart(const std::string& port, uint32_t baudrate, uint32_t timeout_ms = 500)
  {
    Open(port, baudrate, timeout_ms);
  }
  // non-copiable
  Uart(const Uart&) = delete;
  Uart& operator=(const Uart&) = delete;

  virtual ~Uart()
  {
    if (serial_)
    {
      lock_guard<recursive_mutex> lock_r(mtx_read_);
      lock_guard<recursive_mutex> lock_w(mtx_write_);
      serial_->close();
      serial_.reset();
    }
  }

  bool Open(const std::string& port, uint32_t baudrate, uint32_t timeout_ms = 500)
  {
    lock_guard<recursive_mutex> lock_r(mtx_read_);
    lock_guard<recursive_mutex> lock_w(mtx_write_);
    if (serial_ && serial_->isOpen())
    {
      serial_->close();
      serial_.reset();
    }
    serial_ = make_shared<Serial>(port, baudrate);
    if (!serial_)
    {
      cerr << "failed to open serial: serial_ is null" << endl;
      return false;
    }
    if (!serial_->isOpen())
    {
      cerr << "failed to open serial: serial_ is not open" << endl;
      return false;
    }
    if (!SetTimeout(timeout_ms))
    {
      cerr << "failed to set timeout" << endl;
      return false;
    }
    return true;
  }

  bool SetTimeout(uint32_t timeout_ms)
  {
    lock_guard<recursive_mutex> lock_r(mtx_read_);
    lock_guard<recursive_mutex> lock_w(mtx_write_);
    if (!serial_ || !serial_->isOpen())
    {
      cerr << "failed to set timeout: serial_ is null or not open" << endl;
      return false;
    }
    serial_->setTimeout(0,           // inter_byte_timeout
                        timeout_ms,  // read_timeout_constant
                        0,           // read_timeout_multiplier,
                        timeout_ms,  // write_timeout_constant,
                        0);          // write_timeout_multiplier)
    return true;
  }

  bool IsOpen() const
  {
    lock_guard<recursive_mutex> lock_r(mtx_read_);
    lock_guard<recursive_mutex> lock_w(mtx_write_);
    if (!serial_)
    {
      cerr << "failed to check serial status: serial_ is null" << endl;
      return false;
    }
    return serial_->isOpen();
  }

  uint32_t Read(uint8_t* buf, uint32_t size)
  {
    lock_guard<recursive_mutex> lock_r(mtx_read_);
    if (!serial_ || !serial_->isOpen())
    {
      cerr << "failed to read: serial_ is null or not open" << endl;
      return 0;
    }
    return serial_->read(buf, size);
  }

  string ReadStr()
  {
    lock_guard<recursive_mutex> lock_r(mtx_read_);
    if (!serial_ || !serial_->isOpen())
    {
      cerr << "failed to read: serial_ is null or not open" << endl;
      return "";
    }
    return serial_->read(8192);
  }

  uint32_t Write(const uint8_t* buf, uint32_t size)
  {
    lock_guard<recursive_mutex> lock_w(mtx_write_);
    if (!serial_ || !serial_->isOpen())
    {
      cerr << "failed to write: serial_ is null or not open" << endl;
      return 0;
    }
    return serial_->write(buf, size);
  }

private:
  mutable recursive_mutex mtx_read_;
  mutable recursive_mutex mtx_write_;

  shared_ptr<Serial> serial_;
};

}  // namespace cpcos