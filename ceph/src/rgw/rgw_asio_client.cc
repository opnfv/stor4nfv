// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/write.hpp>
#include <beast/http/read.hpp>

#include "rgw_asio_client.h"

#define dout_context g_ceph_context
#define dout_subsys ceph_subsys_rgw

#undef dout_prefix
#define dout_prefix (*_dout << "asio: ")

using namespace rgw::asio;

ClientIO::ClientIO(tcp::socket& socket,
                   parser_type& parser,
                   beast::flat_streambuf& buffer)
  : socket(socket), parser(parser), buffer(buffer), txbuf(*this)
{
}

ClientIO::~ClientIO() = default;

void ClientIO::init_env(CephContext *cct)
{
  env.init(cct);

  const auto& request = parser.get();
  const auto& headers = request.fields;
  for (auto header = headers.begin(); header != headers.end(); ++header) {
    const auto& name = header->name();
    const auto& value = header->value();

    if (boost::algorithm::iequals(name, "content-length")) {
      env.set("CONTENT_LENGTH", value);
      continue;
    }
    if (boost::algorithm::iequals(name, "content-type")) {
      env.set("CONTENT_TYPE", value);
      continue;
    }
    if (boost::algorithm::iequals(name, "connection")) {
      conn_keepalive = boost::algorithm::iequals(value, "keep-alive");
      conn_close = boost::algorithm::iequals(value, "close");
    }

    static const boost::string_ref HTTP_{"HTTP_"};

    char buf[name.size() + HTTP_.size() + 1];
    auto dest = std::copy(std::begin(HTTP_), std::end(HTTP_), buf);
    for (auto src = name.begin(); src != name.end(); ++src, ++dest) {
      if (*src == '-') {
        *dest = '_';
      } else {
        *dest = std::toupper(*src);
      }
    }
    *dest = '\0';

    env.set(buf, value);
  }

  env.set("REQUEST_METHOD", request.method);

  // split uri from query
  auto url = boost::string_ref{request.url};
  auto pos = url.find('?');
  auto query = url.substr(pos + 1);
  url = url.substr(0, pos);

  env.set("REQUEST_URI", url);
  env.set("QUERY_STRING", query);
  env.set("SCRIPT_URI", url); /* FIXME */

  char port_buf[16];
  snprintf(port_buf, sizeof(port_buf), "%d", socket.local_endpoint().port());
  env.set("SERVER_PORT", port_buf);
  // TODO: set SERVER_PORT_SECURE if using ssl
  // TODO: set REMOTE_USER if authenticated
}

size_t ClientIO::write_data(const char* buf, size_t len)
{
  boost::system::error_code ec;
  auto bytes = boost::asio::write(socket, boost::asio::buffer(buf, len), ec);
  if (ec) {
    derr << "write_data failed: " << ec.message() << dendl;
    throw rgw::io::Exception(ec.value(), std::system_category());
  }
  /* According to the documentation of boost::asio::write if there is
   * no error (signalised by ec), then bytes == len. We don't need to
   * take care of partial writes in such situation. */
  return bytes;
}

size_t ClientIO::read_data(char* buf, size_t max)
{
  auto& message = parser.get();
  auto& body_remaining = message.body;
  body_remaining = boost::asio::mutable_buffer{buf, max};

  boost::system::error_code ec;

  dout(30) << this << " read_data for " << max << " with "
      << buffer.size() << " bytes buffered" << dendl;

  while (boost::asio::buffer_size(body_remaining) && !parser.is_complete()) {
    auto bytes = beast::http::read_some(socket, buffer, parser, ec);
    buffer.consume(bytes);
    if (ec == boost::asio::error::connection_reset ||
        ec == boost::asio::error::eof ||
        ec == beast::http::error::partial_message) {
      break;
    }
    if (ec) {
      derr << "failed to read body: " << ec.message() << dendl;
      throw rgw::io::Exception(ec.value(), std::system_category());
    }
  }
  return max - boost::asio::buffer_size(body_remaining);
}

size_t ClientIO::complete_request()
{
  return 0;
}

void ClientIO::flush()
{
  txbuf.pubsync();
}

size_t ClientIO::send_status(int status, const char* status_name)
{
  static constexpr size_t STATUS_BUF_SIZE = 128;

  char statusbuf[STATUS_BUF_SIZE];
  const auto statuslen = snprintf(statusbuf, sizeof(statusbuf),
                                  "HTTP/1.1 %d %s\r\n", status, status_name);

  return txbuf.sputn(statusbuf, statuslen);
}

size_t ClientIO::send_100_continue()
{
  const char HTTTP_100_CONTINUE[] = "HTTP/1.1 100 CONTINUE\r\n\r\n";
  const size_t sent = txbuf.sputn(HTTTP_100_CONTINUE,
                                  sizeof(HTTTP_100_CONTINUE) - 1);
  flush();
  return sent;
}

static constexpr size_t TIME_BUF_SIZE = 128;
static size_t dump_date_header(char (&timestr)[TIME_BUF_SIZE])
{
  const time_t gtime = time(nullptr);
  struct tm result;
  struct tm const * const tmp = gmtime_r(&gtime, &result);
  if (tmp == nullptr) {
    return 0;
  }
  return strftime(timestr, sizeof(timestr),
                  "Date: %a, %d %b %Y %H:%M:%S %Z\r\n", tmp);
}

size_t ClientIO::complete_header()
{
  size_t sent = 0;

  char timestr[TIME_BUF_SIZE];
  if (dump_date_header(timestr)) {
    sent += txbuf.sputn(timestr, strlen(timestr));
  }

  if (conn_keepalive) {
    constexpr char CONN_KEEP_ALIVE[] = "Connection: Keep-Alive\r\n";
    sent += txbuf.sputn(CONN_KEEP_ALIVE, sizeof(CONN_KEEP_ALIVE) - 1);
  } else if (conn_close) {
    constexpr char CONN_KEEP_CLOSE[] = "Connection: close\r\n";
    sent += txbuf.sputn(CONN_KEEP_CLOSE, sizeof(CONN_KEEP_CLOSE) - 1);
  }

  constexpr char HEADER_END[] = "\r\n";
  sent += txbuf.sputn(HEADER_END, sizeof(HEADER_END) - 1);

  flush();
  return sent;
}

size_t ClientIO::send_header(const boost::string_ref& name,
                             const boost::string_ref& value)
{
  static constexpr char HEADER_SEP[] = ": ";
  static constexpr char HEADER_END[] = "\r\n";

  size_t sent = 0;

  sent += txbuf.sputn(name.data(), name.length());
  sent += txbuf.sputn(HEADER_SEP, sizeof(HEADER_SEP) - 1);
  sent += txbuf.sputn(value.data(), value.length());
  sent += txbuf.sputn(HEADER_END, sizeof(HEADER_END) - 1);

  return sent;
}

size_t ClientIO::send_content_length(uint64_t len)
{
  static constexpr size_t CONLEN_BUF_SIZE = 128;

  char sizebuf[CONLEN_BUF_SIZE];
  const auto sizelen = snprintf(sizebuf, sizeof(sizebuf),
                                "Content-Length: %" PRIu64 "\r\n", len);

  return txbuf.sputn(sizebuf, sizelen);
}
