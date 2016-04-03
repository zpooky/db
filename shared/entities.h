//
// Created by spooky on 2016-04-03.
//

#ifndef SHARED_ENTITIES_H
#define SHARED_ENTITIES_H

#include <string>
#include <utility>

namespace db {

    using std::string;

    struct Filename {
        const string name;

        explicit Filename(string &&p_name) : name{p_name} {

        }

        operator string() const {
            return name;
        }
    };

    struct Directoryname {
        const string name;

        explicit Directoryname(string &&p_name) : name{p_name} {
        }

        operator string() const {
            return name;
        }
    };

    struct Directory {
        const string path;

        explicit Directory(string &&p_path) : path{p_path} {

        }

        Directory cd(string &&d) const;
    };

    struct File {
        const string path;

        explicit File(string &&p_path) : path{p_path} {
        }

        operator string() const {
            return path;
        }
    };


};

#endif //SHARED_ENTITIES_H
