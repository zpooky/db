//
// Created by spooky on 2016-04-03.
//

#ifndef SHARED_ENTITIES_H
#define SHARED_ENTITIES_H

#include <array>
#include <cassert>
#include <string>
#include <utility>
#include <vector>

//#include <boost/filesystem.hpp>

namespace db {

//    using boost::filesystem::path;

struct Filename {
  const std::string name;

  explicit Filename(std::string &&p_name) : name{p_name} {
  }

  const char *c_str() const {
    return name.c_str();
  }
  //        operator string() const {
  //            return name;
  //        }
};
struct File {
private:
public:
  const std::string path;

  explicit File(const std::string &p_path) : path{p_path} {
  }
  explicit File(std::string &&p_path) : path{std::move(p_path)} {
  }
  //        explicit File(const boost::filesystem::path &p) : path{p.string()}{
  //
  //        }

  Filename filename() const {
    auto index = path.rfind('/');
    if (index != std::string::npos) {
      auto file = path.substr(index + 1);
      return Filename{std::move(file)};
    }
    return Filename{std::string(path)};
  }

  const char *c_str() const {
    return path.c_str();
  }

  std::string parent() const {
    auto index = path.rfind('/');
    if (index != std::string::npos) {
      return path.substr(0, index);
    }
    assert(0);
  }

  //        operator string() const {
  //            return path;
  //        }
};

struct DirectoryName {
private:
public:
  const std::string name;

  explicit DirectoryName(std::string &&p_name) : name{p_name} {
  }

  //        operator string() const {
  //            return name;
  //        }
};

struct Directory {
private:
public:
  const std::string path;

  //        explicit Directory(const Directory &o) : path{o.path} {
  //        }

  explicit Directory(std::string &&p_path) : path{p_path} {
  }

  //        boost::filesystem::path to_path() const {
  //            return {path};
  //        }
  //
  const char *c_str() const {
    return path.c_str();
  }

private:
  //        template<typename Col>
  //        std::string concat(const std::string &p_path, const Col &d);

  template <typename Col>
  std::string concat(const std::string &p_path, const Col &d) const {
    std::string copy = p_path;
    //            printf("'%s'\n", copy.c_str());
    if (copy.at(copy.length() - 1) != '/') {
      copy.append("/");
      //                printf(":%s:\n", bah.c_str());
    }

    std::string xcc{d.begin(), d.end()};

    //            printf("|%s|\n", xcc.c_str());
    copy.append(xcc.c_str());
    //            printf("_%s_\n", copy.c_str());
    return copy;
  }

public:
  template <typename Col>
  Directory cdx(Col &&d) const;

  template <size_t t_size, typename t_type>
  Directory cdx(std::array<t_type, t_size> &&d) const;

  template <size_t N>
  Directory cd(const char (&d)[N]) const;

  File cd(const Filename &filename) const;

  Directory cd(const DirectoryName &name) const;

  Directory parent() const;
};

template <typename Col>
Directory Directory::cdx(Col &&d) const {
  return Directory{concat(path, d)};
}

template <size_t t_size, typename t_type>
Directory Directory::cdx(std::array<t_type, t_size> &&d) const {
  t_type a[t_size + 1];
  auto it = d.begin();
  auto ait = &a[0];
  while (it != d.end() && *it) {
    *ait = *it;
    ++it;
    ++ait;
  }
  *ait = 0;

  std::string str{a};
  return Directory{concat(path, str)};
}

template <size_t N>
Directory Directory::cd(const char (&d)[N]) const {
  return cdx(std::string{d, N});
}

std::vector<File> files(const Directory &);

} // END namespace

// std::ostream &operator<<(std::ostream &o, const db::DirectoryName &d) {
//   o << d.name;
//   return o;
// }
// std::ostream &operator<<(std::ostream &o, const db::Directory &d) {
//   o << d.path;
//   return o;
// }
// std::ostream &operator<<(std::ostream &o, const db::Filename &f) {
//   o << f.name;
//   return o;
// }
// std::ostream &operator<<(std::ostream &o, const db::File &f) {
//   o << f.path;
//   return o;
// }

#endif // SHARED_ENTITIES_H
