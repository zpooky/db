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

        explicit Filename(string &&p_name) : name{p_name} {

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
        string path;

        explicit Directory(string &&p_path) : path{p_path} {

        }

        template<typename Col>
        Directory cd(Col &&d) const;

        File cd(const Filename &filename) const;
    };

    template<typename Col>
    Directory Directory::cd(Col &&d) const {
        string copy = path;
        if (copy.at(copy.length() - 1) == '/') {
            copy.append("/");
        }
        copy.append({d.begin(), d.end()});
        return Directory{std::move(copy)};
    }



};

#endif //SHARED_ENTITIES_H
