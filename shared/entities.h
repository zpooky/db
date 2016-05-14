//
// Created by spooky on 2016-04-03.
//

#ifndef SHARED_ENTITIES_H
#define SHARED_ENTITIES_H

#include <string>
#include <utility>

namespace db {

    using std::string;

    struct File {
        const string path;

        explicit File(string &&p_path) : path{p_path} {
        }

        operator string() const {
            return path;
        }
    };

    struct Filename {
        const string name;

        Filename(string &&p_name) : name{p_name} {

        }

        operator string() const {
            return name;
        }
    };

    struct DirectoryName {
        const string name;

        explicit DirectoryName(string &&p_name) : name{p_name} {
        }

        operator string() const {
            return name;
        }
    };

    struct Directory {
    public:
        const string path;

        explicit Directory(string &&p_path) : path{p_path} {

        }

    private:
//        template<typename Col>
//        string concat(const string &p_path, const Col &d);

        template<typename Col>
        string concat(const string &p_path, const Col &d) const {
            string copy = p_path;
            if (copy.at(copy.length() - 1) != '/') {
                copy.append("/");
            }
            copy.append({d.begin(), d.end()});
            return copy;
        }

    public:

        template<typename Col>
        Directory cd(Col &&d) const;

        template<size_t N>
        Directory cd(const char (&d)[N]) const;

        File cd(const Filename &filename) const;

        Directory cd(const DirectoryName &name) const;
    };

    template<typename Col>
    Directory Directory::cd(Col &&d) const {
        return Directory{concat(path, d)};
    }

    template<size_t N>
    Directory Directory::cd(const char (&d)[N]) const {
        string s{d, N};
        return cd(s);
    }

};

#endif //SHARED_ENTITIES_H
