//
// cc_networks.h
//

#ifndef _CC_NETWORKS_H
#define _CC_NETWORKS_H

#include <sstream>
#include <string>
#include <algorithm>

#include <unistd.h>
#include <sys/socket.h>

#include "cc_numbers.h"
#include "cc_log.h"
#include "cc_autoset.h"

namespace cc {

class c_networks final {

public:
  inline static bool is_ipv4_address_format(const std::string& ip) {
    int8_t count_dots(0);
    int8_t count_numbers(0);

    std::ostringstream os;
    
    for (const auto& c : ip) {
      
      if (c >= '0' && c <= '9') {
        os << c;
        continue;
      }
      
      if (c == '.') {
        ++count_dots;
        
        if (!c_numbers::is_integer_in_range(os.str(), 0, 255)) {
          return false;
        }
        
        ++count_numbers;
        os.str("");
        continue;
      }
      
      return false;
    }

    if (!c_numbers::is_integer_in_range(os.str(), 0, 255)) {
      return false;
    }

    if (count_dots != 3 || count_numbers != 3) {
      return false;
    }
    
    return true;
  }
  
  inline static const char* get_errno_description(uint32_t number) {
    return ( number < 0 || number > (sizeof(_errno_desc_map)/sizeof(errno_desc_tuple)) ) ?
    "unknown errno" : ( !number ? "" : std::get<1>(_errno_desc_map[number-1]));
  }
  
  static bool close_fd(int32_t fd) { 
    
    bool result(true);
    
    std::ostringstream os;
    os << "shutdown socket "
    << fd;
    
    if (fd >= 0) {
      LOG_DEBUG("closing socket ", fd);
      
      int32_t code(0);
      
      // TODO close client write and clear socket before final close
      
      if ((code = shutdown(fd, SHUT_RDWR))) { // SHUT_RDWR SHUT_RD
        if (errno != ENOTCONN) { // TODO ENOTCONN happening on shutdown
          LOG_WARN("shutdown socket not ok ", code, " ", c_networks::get_errno_description(errno));
        } else {
          LOG_DEBUG("shutdown socket not ok ENOTCONN ignored");
        }
      }
      
      if ((code = close(fd))) {
        LOG_ERROR("close socket not ok ", code, " ", c_networks::get_errno_description(errno));
        result = false;
      }
    }
    
    return result;
  }
  
private:
  using errno_desc_map = std::array<std::tuple<size_t, const char*>, 106>;
  using errno_desc_tuple = std::tuple<uint32_t, const char*>;
  
  static constexpr errno_desc_map _errno_desc_map {
    errno_desc_tuple(1,"1 EPERM Operation not permitted"),
    errno_desc_tuple(2,"2 ENOENT No such file or directory"),
    errno_desc_tuple(3,"3 ESRCH No such process"),
    errno_desc_tuple(4,"4 EINTR Interrupted system call"),
    errno_desc_tuple(5,"5 EIO Input/output error"),
    errno_desc_tuple(6,"6 ENXIO Device not configured"),
    errno_desc_tuple(7,"7 E2BIG Argument list too long"),
    errno_desc_tuple(8,"8 ENOEXEC Exec format error"),
    errno_desc_tuple(9,"9 EBADF Bad file descriptor"),
    errno_desc_tuple(10,"10 ECHILD No child processes"),
    errno_desc_tuple(11,"11 EDEADLK Resource deadlock avoided"),
    errno_desc_tuple(12,"12 ENOMEM Cannot allocate memory"),
    errno_desc_tuple(13,"13 EACCES Permission denied"),
    errno_desc_tuple(14,"14 EFAULT Bad address"),
    errno_desc_tuple(15,"15 ENOTBLK Block device required"),
    errno_desc_tuple(16,"16 EBUSY Device / Resource busy"),
    errno_desc_tuple(17,"17 EEXIST File exists"),
    errno_desc_tuple(18,"18 EXDEV Cross-device link"),
    errno_desc_tuple(19,"19 ENODEV Operation not supported by device"),
    errno_desc_tuple(20,"20 ENOTDIR Not a directory"),
    errno_desc_tuple(21,"21 EISDIR Is a directory"),
    errno_desc_tuple(22,"22 EINVAL Invalid argument"),
    errno_desc_tuple(23,"23 ENFILE Too many open files in system"),
    errno_desc_tuple(24,"24 EMFILE Too many open files"),
    errno_desc_tuple(25,"25 ENOTTY Inappropriate ioctl for device"),
    errno_desc_tuple(26,"26 ETXTBSY Text file busy"),
    errno_desc_tuple(27,"27 EFBIG File too large"),
    errno_desc_tuple(28,"28 ENOSPC No space left on device"),
    errno_desc_tuple(29,"29 ESPIPE Illegal seek"),
    errno_desc_tuple(30,"30 EROFS Read-only file system"),
    errno_desc_tuple(31,"31 EMLINK Too many links"),
    errno_desc_tuple(32,"32 EPIPE Broken pipe"),
    errno_desc_tuple(33,"33 EDOM Numerical argument out of domain"),
    errno_desc_tuple(34,"34 ERANGE Result too large"),
    errno_desc_tuple(35,"35 EAGAIN Resource temporarily unavailable"),
    errno_desc_tuple(36,"36 EINPROGRESS Operation now in progress"),
    errno_desc_tuple(37,"37 EALREADY Operation already in progress"),
    errno_desc_tuple(38,"38 ENOTSOCK Socket operation on non-socket"),
    errno_desc_tuple(39,"39 EDESTADDRREQ Destination address required"),
    errno_desc_tuple(40,"40 EMSGSIZE Message too long"),
    errno_desc_tuple(41,"41 EPROTOTYPE Protocol wrong type for socket"),
    errno_desc_tuple(42,"42 ENOPROTOOPT Protocol not available"),
    errno_desc_tuple(43,"43 EPROTONOSUPPORT Protocol not supported"),
    errno_desc_tuple(44,"44 ESOCKTNOSUPPORT Socket type not supported"),
    errno_desc_tuple(45,"45 ENOTSUP Operation not supported"),
    errno_desc_tuple(46,"46 EPFNOSUPPORT Protocol family not supported"),
    errno_desc_tuple(47,"47 EAFNOSUPPORT Address family not supported by protocol family"),
    errno_desc_tuple(48,"48 EADDRINUSE Address already in use"),
    errno_desc_tuple(49,"49 EADDRNOTAVAIL Can't assign requested address"),
    errno_desc_tuple(50,"50 ENETDOWN Network is down"),
    errno_desc_tuple(51,"51 ENETUNREACH Network is unreachable"),
    errno_desc_tuple(52,"52 ENETRESET Network dropped connection on reset"),
    errno_desc_tuple(53,"53 ECONNABORTED Software caused connection abort"),
    errno_desc_tuple(54,"54 ECONNRESET Connection reset by peer"),
    errno_desc_tuple(55,"55 ENOBUFS No buffer space available"),
    errno_desc_tuple(56,"56 EISCONN Socket is already connected"),
    errno_desc_tuple(57,"57 ENOTCONN Socket is not connected"),
    errno_desc_tuple(58,"58 ESHUTDOWN Can't send after socket shutdown"),
    errno_desc_tuple(59,"59 ETOOMANYREFS Too many references: can't splice"),
    errno_desc_tuple(60,"60 ETIMEDOUT Operation timed out"),
    errno_desc_tuple(61,"61 ECONNREFUSED Connection refused"),
    errno_desc_tuple(62,"62 ELOOP Too many levels of symbolic links"),
    errno_desc_tuple(63,"63 ENAMETOOLONG File name too long"),
    errno_desc_tuple(64,"64 EHOSTDOWN Host is down"),
    errno_desc_tuple(65,"65 EHOSTUNREACH No route to host"),
    errno_desc_tuple(66,"66 ENOTEMPTY Directory not empty"),
    errno_desc_tuple(67,"67 EPROCLIM Too many processes"),
    errno_desc_tuple(68,"68 EUSERS Too many users"),
    errno_desc_tuple(69,"69 EDQUOT Disc quota exceeded"),
    errno_desc_tuple(70,"70 ESTALE Stale NFS file handle"),
    errno_desc_tuple(71,"71 EREMOTE Too many levels of remote in path"),
    errno_desc_tuple(72,"72 EBADRPC RPC struct is bad"),
    errno_desc_tuple(73,"73 ERPCMISMATCH RPC version wrong"),
    errno_desc_tuple(74,"74 EPROGUNAVAIL RPC prog. not avail"),
    errno_desc_tuple(75,"75 EPROGMISMATCH Program version wrong"),
    errno_desc_tuple(76,"76 EPROCUNAVAIL Bad procedure for program"),
    errno_desc_tuple(77,"77 ENOLCK No locks available"),
    errno_desc_tuple(78,"78 ENOSYS Function not implemented"),
    errno_desc_tuple(79,"79 EFTYPE Inappropriate file type or format"),
    errno_desc_tuple(80,"80 EAUTH Authentication error"),
    errno_desc_tuple(81,"81 ENEEDAUTH Need authenticator"),
    errno_desc_tuple(82,"82 EPWROFF Device power is off"),
    errno_desc_tuple(83,"83 EDEVERR Device error e.g. paper out"),
    errno_desc_tuple(84,"84 EOVERFLOW Value too large to be stored in data type"),
    errno_desc_tuple(85,"85 EBADEXEC Bad executable"),
    errno_desc_tuple(86,"86 EBADARCH Bad CPU type in executable"),
    errno_desc_tuple(87,"87 ESHLIBVERS Shared library version mismatch"),
    errno_desc_tuple(88,"88 EBADMACHO Malformed Macho file"),
    errno_desc_tuple(89,"89 ECANCELED Operation canceled"),
    errno_desc_tuple(90,"90 EIDRM Identifier removed"),
    errno_desc_tuple(91,"91 ENOMSG No message of desired type"),
    errno_desc_tuple(92,"92 EILSEQ Illegal byte sequence"),
    errno_desc_tuple(93,"93 ENOATTR Attribute not found"),
    errno_desc_tuple(94,"94 EBADMSG Bad message"),
    errno_desc_tuple(95,"95 EMULTIHOP Reserved"),
    errno_desc_tuple(96,"96 ENODATA No message available on STREAM"),
    errno_desc_tuple(97,"97 ENOLINK Reserved"),
    errno_desc_tuple(98,"98 ENOSR No STREAM resources"),
    errno_desc_tuple(99,"99 ENOSTR Not a STREAM"),
    errno_desc_tuple(100,"100 EPROTO Protocol error"),
    errno_desc_tuple(101,"101 ETIME STREAM ioctl timeout"),
    errno_desc_tuple(102,"102 EOPNOTSUPP Operation not supported on socket"),
    errno_desc_tuple(103,"103 ENOPOLICY No such policy registered"),
    errno_desc_tuple(104,"104 ENOTRECOVERABLE State not recoverable"),
    errno_desc_tuple(105,"105 EOWNERDEAD Previous owner died"),
    errno_desc_tuple(106,"106 EQFULL Interface output queue is full")
  };

private:
  c_networks() = delete;
  c_networks(c_networks&) = delete;
  c_networks& operator=(c_networks&) = delete;
	~c_networks() = delete;
};

}

#endif /* _CC_NETWORKS_H */

