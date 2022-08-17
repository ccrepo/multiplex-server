//
// cc_files.h
//

#ifndef _CC_FILES_H
#define _CC_FILES_H

namespace cc {

class c_files final {

public:
	static constexpr char slash { '/' };
  
public:
	static constexpr const char* basename_slash(const char *s) {
		return exists_slash(s) ? clip_slash(end(s)) : s;
	}

private:
	static constexpr const char* end(const char *s) {
		return *s ? end(s + 1) : s;
	}

	static constexpr bool exists_slash(const char *s) {
		return *s == slash ? true : (*s ? exists_slash(s + 1) : false);
	}

	static constexpr const char* clip_slash(const char *s) {
		return *s == slash ? (s + 1) : clip_slash(s - 1);
	}

private:
  c_files() = delete;
	c_files(c_files&) = delete;
	c_files& operator=(c_files&) = delete;
  ~c_files() = delete;
};

}

#endif /* _CC_FILES_H */
