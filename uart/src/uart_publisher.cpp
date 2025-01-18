#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

#include <boost/asio.hpp>

#define layout_packed(Align) __attribute__((packed, aligned(Align)))

struct layout_packed(1) test_payload{
  std::uint32_t value;
}; // struct test_payload

struct layout_packed(1) package {
  std::uint8_t start;
  std::uint8_t size;
  std::uint8_t type;
  union layout_packed(1) {
    test_payload test;
  } payload;
  std::uint8_t checksum;
}; // struct package

class uart_publisher : public rclcpp::Node {

  using base = rclcpp::Node;

public:

  uart_publisher()
  : base{"uart_publisher"},
    _io_context{},
    _serial_port{_io_context} {
    _publisher = base::create_publisher<std_msgs::msg::String>("/uart/test_value", 10u);

    _serial_port.open("/dev/ttyUSB0");

    _serial_port.set_option(boost::asio::serial_port::baud_rate(115200));
    _serial_port.set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
    _serial_port.set_option(boost::asio::serial_port::character_size(boost::asio::serial_port::character_size(8)));
    _serial_port.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
    _serial_port.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
    
    boost::asio::async_read_until(_serial_port, boost::asio::dynamic_buffer(_read_buffer), 0x7E, [this](const boost::system::error_code& error_code, const std::size_t bytes_read) {
      this->_read_uart(error_code, bytes_read);
    });
  }

private:

  auto _read_uart(const boost::system::error_code& error_code, const std::size_t bytes_read) -> void {
    if (error_code) {
      RCLCPP_ERROR(this->get_logger(), "Error reading from UART: %s", error_code.message().c_str());
    }

    auto decoded = std::vector<std::uint8_t>{};
    decoded.reserve(bytes_read);
    auto need_decoding = false;

    for (auto i = 0u; i < bytes_read; ++i) {
      if (_read_buffer[i] == 0x7D) {
        need_decoding = true;
        continue;
      }

      if (need_decoding) {
        decoded.push_back(_read_buffer[i] ^ 0x20);
        need_decoding = false;
      } else {
        decoded.push_back(_read_buffer[i]);
      }
    }

    auto package = reinterpret_cast<struct package*>(decoded.data());

    auto message = std_msgs::msg::String{};

    RCLCPP_INFO(this->get_logger(), "Received test value: %u", package->payload.test.value);

    message.data = std::to_string(package->payload.test.value);

    _publisher->publish(message);

    boost::asio::async_read_until(_serial_port, boost::asio::dynamic_buffer(_read_buffer), 0x7E, [this](const boost::system::error_code& error_code, const std::size_t bytes_read) {
      this->_read_uart(error_code, bytes_read);
    });
  }

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr _publisher;

  boost::asio::io_context _io_context;
  boost::asio::serial_port _serial_port;
  std::vector<std::uint8_t> _read_buffer;

}; // class uart_publisher

auto main(int argc, char** argv) -> int {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<uart_publisher>());
  rclcpp::shutdown();
  return 0;
}
