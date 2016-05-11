//
// Created by spooky on 2016-03-12.
//

#include "Segment.h"
#include <fstream>
#include <array>
#include <algorithm>
#include "../../shared/vfs.h"

namespace db {
    namespace fs {
        namespace impl {
            SegmentFile SegmentFileInit::create(size_t line_size, size_t number_of) const {
                using capacity = unsigned long long;
                FileWriter stream(m_file);
                //
                capacity target = line_size * number_of;
                std::array<char, db::vfs::page::default_size> buf{0};
                do {
                    size_t counter = std::min<size_t>(buf.size(), target);
                    target = target - counter;
                    stream.init_write(buf, counter);
                } while (target > 0);
                stream.flush();
            }
        }

    }
}