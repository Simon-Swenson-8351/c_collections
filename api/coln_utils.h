#pragma once

#define COLN_POD_COPY(dest_ptr, src_ptr) (*dest_ptr = *src_ptr, true)
#define COLN_POD_COPY_MANY(dest_ptr, src_ptr, count) (memcpy((dest_ptr), (src_ptr), sizeof(COLN_DATA_TYPE) * (count)), true)
#define COLN_COPY_MANY_USING_COPY(dest_ptr, src_ptr, count)
#define COLN_POD_MOVE(dest_ptr, src_ptr) (*dest_ptr = *src_ptr, true)
#define COLN_POD_MOVE_MANY(dest_ptr, src_ptr, count, size) (memcpy((dest_ptr), (src_ptr), (size) * (count)), true)
