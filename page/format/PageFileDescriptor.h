#ifndef _DB_PAGE_FILE_DESCRIPTOR
#define _DB_PAGE_FILES_PARSER

#include "../../shared/entities.h"
#include "../io/FilePageMeta.h"

namespace page {

/*
 * Header:
 * size in bytes:
 * |byte|unsigned short|db::table::name::length|unsigned short|unsigned short|
 *
 * header fields:
 * .-------------------------------------------------------------------------------.
 * |endianess|db segment format version|Table name|Line size|Table version|
 * `-------------------------------------------------------------------------------`
 *---
 *partition type|partition meta data length|meta data|
 * # partition type
 * none                     0
 * day based partitioning   1
 * # endianess
 * - big endian     0
 * - little endian  1
 */
template<typename Meta_t>
class PageFileDescriptorV1 {
private:
  
public:
  
  FilePageMeta write(const db::File &f) {
  }

};
}

#endif
