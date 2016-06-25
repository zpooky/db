//
// Created by spooky on 2016-04-03.
//

#ifndef SHARED_ENTITIES_H
#define SHARED_ENTITIES_H

#include <string>
#include <utility>
#include <array>

namespace db {

    using std::string;

    struct File {
        const string path;

        explicit File(string &&p_path) : path{p_path} {
        }

//        operator string() const {
//            return path;
//        }
    };

    struct Filename {
        const string name;

        explicit Filename(string &&p_name) : name{p_name} {

        }

//        operator string() const {
//            return name;
//        }
    };

    struct DirectoryName {
        const string name;

        explicit DirectoryName(string &&p_name) : name{p_name} {
        }

//        operator string() const {
//            return name;
//        }
    };

    struct Directory {
    public:
        const string path;

//        explicit Directory(const Directory &o) : path{o.path} {
//        }

        explicit Directory(string &&p_path) : path{p_path} {
        }

    private:
//        template<typename Col>
//        string concat(const string &p_path, const Col &d);

        template<typename Col>
        string concat(const string &p_path, const Col &d) const {
            string copy = p_path;
//            printf("'%s'\n", copy.c_str());
            if (copy.at(copy.length() - 1) != '/') {
                copy.append("/");
//                printf(":%s:\n", bah.c_str());
            }

            string xcc{d.begin(), d.end()};

//            printf("|%s|\n", xcc.c_str());
            copy.append(xcc.c_str());
//            printf("_%s_\n", copy.c_str());
            return copy;
        }

    public:

        template<typename Col>
        Directory cdx(Col &&d) const;

        template<size_t t_size, typename t_type>
        Directory cdx(std::array<t_type, t_size> &&d) const;

        template<size_t N>
        Directory cd(const char (&d)[N]) const;

        File cd(const Filename &filename) const;

        Directory cd(const DirectoryName &name) const;
    };

    template<typename Col>
    Directory Directory::cdx(Col &&d) const {
        return Directory{concat(path, d)};
    }

    template<size_t t_size, typename t_type>
    Directory Directory::cdx(std::array<t_type, t_size> &&d) const {
        t_type a[t_size + 1];
        auto it = d.begin();
        auto ait = &a[0];
        while (*it && it != d.end()) {
            *ait = *it;
            ++it;
            ++ait;
        }
        *ait = 0;

        string str{a};
        return Directory{concat(path, str)};
    }

    template<size_t N>
    Directory Directory::cd(const char (&d)[N]) const {
        return cdx(string{d, N});
    }

};

#endif //SHARED_ENTITIES_H
