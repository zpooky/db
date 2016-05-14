//
// Created by spooky on 2016-04-03.
//

#include "entities.h"


namespace db {

    Directory Directory::cd(const DirectoryName &dir) const{
        return Directory{concat(path, dir.name)};
    }
    File Directory::cd(const Filename &filename) const{
        return File{concat(path, filename.name)};
    }
}