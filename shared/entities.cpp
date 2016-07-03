//
// Created by spooky on 2016-04-03.
//

#include "entities.h"

#include <boost/filesystem.hpp>

namespace db {

    Directory Directory::cd(const DirectoryName &dir) const {
        return Directory{concat(path, dir.name)};
    }

    File Directory::cd(const Filename &filename) const {
        return File{concat(path, filename.name)};
    }

    using std::vector;

    vector<File> files(const Directory &d) {
        using namespace boost::filesystem;
        vector<File> res{};

        path root{d.path};
        if (is_directory(root)) {
            for (directory_entry &entry : directory_iterator(root)) {
                if (is_regular_file(entry)) {
                    auto p = entry.path();
                    res.push_back(File{p.string()});
                }
            }
        }
        return res;
    }
}