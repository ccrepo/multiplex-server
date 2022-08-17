//
// cc_file.h
//

#ifndef _CC_FILE_H
#define _CC_FILE_H

#include <string>
#include <fstream>
#include <sstream>

#include <sys/stat.h>

namespace cc {

class c_ifile;
class c_ofile;

class c_file {

public:
	virtual ~c_file() {
	}

  inline bool valid() const {
    return _valid;
  }

  inline const std::ostringstream& os() const {
    return _os;
  };
  
private:
	std::ostringstream _os;
	bool _valid;

private:
  friend class c_ifile;
  friend class c_ofile;

private:
  c_file() :
    _os(), _valid(false) {
  }

private:
	c_file(c_file&) = delete;
	c_file operator=(c_file&) = delete;
};

class c_ifile final : public c_file {

public:
  c_ifile(const std::string &filename) : c_file(), _file(filename, std::ios::in) {
    _valid = read();
  }

  ~c_ifile() {
    _file.close();
  }

private:
  bool read() {
    std::string line;

    if (!_file.is_open() || eof()) {
      return false;
    }

    while (getline(_file, line)) {
      _os << line;
      
      if(eof()) {
        break;
      }
    }
    
    return true;
  }
  
  inline bool eof() {
    return _file.peek() == EOF;
  }

private:
  std::ifstream _file;

private:
  c_ifile(c_ifile&) = delete;
  c_ifile operator=(c_ifile&) = delete;
  
};

class c_ofile final : public c_file {

public:
  c_ofile(const std::string &filename, bool truncate) : c_file(), _file(filename, (std::ios::out | (truncate ? std::ios::trunc : std::ios::app))) {
    _valid = !_file;
  }
  
  ~c_ofile() {
    _file.close();
  }
  
  bool write(const std::string& buffer) {
    std::unique_lock < std::mutex > lock(_mutex);
    _file << buffer;
    _file.flush();
    return static_cast<bool>(_file);
  }
  
private:
  std::ofstream _file;
  std::mutex    _mutex;
  
private:
  c_ofile(c_ofile&) = delete;
  c_ofile& operator=(c_ofile&) = delete;
};


}

#endif /* _CC_FILE_H */
