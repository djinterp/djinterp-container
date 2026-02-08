/******************************************************************************
* djinterp [container]                                                table.h
*
*   A `d_table` is a two-dimensional, row-oriented container backed by a
* contiguous array of user-defined row structs.  Each column maps to a
* struct field via a `d_table_column_desc` descriptor, giving typed
* access to individual cells through byte offsets — no virtual dispatch,
* no per-cell allocation, no hidden indirection.
*   This module wraps `table_common.h` into a self-contained struct with
* capacity management, row insertion / removal, cell read / write, search,
* sort, and ownership tracking.
*   A table may be created in three modes:
*     1. Static / stack view  — zero allocation, zero overhead.
*        Use `D_TABLE_VIEW` or `D_TABLE_INIT` to wrap existing arrays.
*     2. Heap-owned           — table allocates and owns the row buffer.
*        Use `d_table_new` or `d_table_new_from_rows`.
*     3. Deep copy            — table owns both data and column descriptors.
*        Use `d_table_new_copy`.
*   The `flags` field tracks what the table owns; only owned resources are
* freed by `d_table_free`.
*   Column structure is fixed for the lifetime of a `d_table`.  To add or
* remove columns, merge rows, or attach metadata, use the supplementary
* `table_fn.h` module.
*
*
* path:      \inc\container\table\table.h
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2026.02.07
******************************************************************************/

/*
TABLE OF CONTENTS
=================
I.    CONFIGURATION
      -------------
      1.  D_TABLE_DEFAULT_CAPACITY
      2.  D_TABLE_GROWTH_FACTOR_NUM
      3.  D_TABLE_GROWTH_FACTOR_DEN

II.   TYPE DEFINITIONS
      -----------------
      1.  Ownership flags
          a.  D_TABLE_FLAG_NONE
          b.  D_TABLE_FLAG_OWNS_DATA
          c.  D_TABLE_FLAG_OWNS_DESCS
      2.  Struct types
          a.  d_table

III.  STATIC INITIALIZATION MACROS
      ----------------------------
      1.  D_TABLE_VIEW
      2.  D_TABLE_EMPTY

IV.   CONVENIENCE ACCESSOR MACROS
      ----------------------------
      1.  Row access
          a.  D_TABLE_ROW_PTR
          b.  D_TABLE_ROW_AS
      2.  Cell access
          a.  D_TABLE_CELL_PTR
          b.  D_TABLE_CELL_VAL

V.    CONSTRUCTOR FUNCTIONS
      ---------------------
      1.  d_table_new
      2.  d_table_new_from_rows
      3.  d_table_new_copy
      4.  d_table_new_fill

VI.   CAPACITY FUNCTIONS
      -------------------
      1.  d_table_reserve
      2.  d_table_shrink_to_fit
      3.  d_table_ensure_capacity
      4.  d_table_grow
      5.  d_table_available

VII.  ROW MUTATION FUNCTIONS
      ----------------------
      1.  d_table_push_row
      2.  d_table_push_row_empty
      3.  d_table_insert_row
      4.  d_table_insert_rows
      5.  d_table_remove_row
      6.  d_table_remove_range
      7.  d_table_pop_row
      8.  d_table_clear

VIII. CELL ACCESS FUNCTIONS
      ---------------------
      1.  d_table_cell_ptr
      2.  d_table_cell_get
      3.  d_table_cell_set

IX.   ROW ACCESS FUNCTIONS
      --------------------
      1.  d_table_row_ptr
      2.  d_table_row_get
      3.  d_table_row_set

X.    QUERY FUNCTIONS
      ---------------
      1.  d_table_row_count
      2.  d_table_column_count
      3.  d_table_capacity
      4.  d_table_struct_size
      5.  d_table_is_empty
      6.  d_table_is_full
      7.  d_table_data

XI.   SEARCH FUNCTIONS
      ----------------
      1.  d_table_find_row
      2.  d_table_find_row_last
      3.  d_table_contains
      4.  d_table_count_value

XII.  UTILITY FUNCTIONS
      -----------------
      1.  d_table_swap_rows
      2.  d_table_reverse
      3.  d_table_sort_by_column
      4.  d_table_copy_to
      5.  d_table_validate

XIII. DESTRUCTOR FUNCTIONS
      --------------------
      1.  d_table_free
      2.  d_table_free_deep
*/

#ifndef DJINTERP_C_CONTAINER_TABLE_
#define DJINTERP_C_CONTAINER_TABLE_ 1

#include <stdarg.h>
#include <stdlib.h>
#include "..\..\djinterp.h"
#include "..\..\dmemory.h"
#include "..\container.h"
#include ".\table_common.h"


///////////////////////////////////////////////////////////////////////////////
///             I.    CONFIGURATION                                         ///
///////////////////////////////////////////////////////////////////////////////

#ifndef D_TABLE_DEFAULT_CAPACITY
    // D_TABLE_DEFAULT_CAPACITY
    //   constant: default number of row slots allocated when a table is
    // created without an explicit capacity.
    #define D_TABLE_DEFAULT_CAPACITY 16
#endif  // D_TABLE_DEFAULT_CAPACITY

#ifndef D_TABLE_GROWTH_FACTOR_NUM
    // D_TABLE_GROWTH_FACTOR_NUM
    //   constant: numerator of the growth factor applied when the row
    // buffer needs to expand.  Default 3/2 = 1.5×.
    #define D_TABLE_GROWTH_FACTOR_NUM 3
#endif  // D_TABLE_GROWTH_FACTOR_NUM

#ifndef D_TABLE_GROWTH_FACTOR_DEN
    // D_TABLE_GROWTH_FACTOR_DEN
    //   constant: denominator of the growth factor.
    #define D_TABLE_GROWTH_FACTOR_DEN 2
#endif  // D_TABLE_GROWTH_FACTOR_DEN


///////////////////////////////////////////////////////////////////////////////
///             II.   TYPE DEFINITIONS                                      ///
///////////////////////////////////////////////////////////////////////////////

// --- II.1  Ownership flags ---

// D_TABLE_FLAG_NONE
//   constant: table does not own any of its buffers.  Used for static
// views and stack-allocated tables.
#define D_TABLE_FLAG_NONE       0x00u

// D_TABLE_FLAG_OWNS_DATA
//   constant: the table owns .data and will free it on destruction.
// Set automatically by constructor functions that allocate the row
// buffer.
#define D_TABLE_FLAG_OWNS_DATA  0x01u

// D_TABLE_FLAG_OWNS_DESCS
//   constant: the table owns .column_descs and will free the
// descriptor array on destruction.  Set when the table deep-copies
// or dynamically builds the descriptor array.
#define D_TABLE_FLAG_OWNS_DESCS 0x02u


// --- II.2  Struct types ---

// d_table
//   struct: a row-oriented table backed by a contiguous byte buffer.
// Fields .data through .column_count match the D_TABLE_INIT macro
// in table_common.h so that static initializer lists work directly
// with this struct.
//   .capacity is the number of row slots currently allocated.  A
// capacity of 0 means the table is a view or has not yet been given
// a heap buffer.
//   .flags tracks which resources the table owns and must free.
struct d_table
{
    void*                       data;           // contiguous row buffer
    struct d_table_column_desc* column_descs;   // column layout array
    size_t                      struct_size;    // bytes per row
    size_t                      row_count;      // current number of rows
    size_t                      column_count;   // number of columns
    size_t                      capacity;       // allocated row slots
    uint32_t                    flags;          // D_TABLE_FLAG_* bits
};


///////////////////////////////////////////////////////////////////////////////
///             III.  STATIC INITIALIZATION MACROS                          ///
///////////////////////////////////////////////////////////////////////////////

// D_TABLE_VIEW
//   macro: create a non-owning view over existing row and column
// descriptor arrays.  The resulting d_table has capacity = 0 and
// flags = D_TABLE_FLAG_NONE, meaning it will never attempt to
// reallocate or free the underlying buffers.
// Usage:
//   struct my_row rows[] = { ... };
//   static const struct d_table_column_desc cols[] = { ... };
//   struct d_table view = D_TABLE_VIEW(
//       rows,
//       sizeof(struct my_row),
//       D_TABLE_ROW_COUNT(struct my_row, rows),
//       cols,
//       D_TABLE_COLUMN_COUNT(cols));
#define D_TABLE_VIEW(row_array,                                       \
                     row_sz,                                          \
                     num_rows,                                        \
                     col_desc_array,                                  \
                     num_columns)                                     \
    {                                                                 \
        .data         = (void*)(row_array),                           \
        .column_descs = (struct d_table_column_desc*)(col_desc_array),\
        .struct_size  = (row_sz),                                     \
        .row_count    = (num_rows),                                   \
        .column_count = (num_columns),                                \
        .capacity     = 0,                                            \
        .flags        = D_TABLE_FLAG_NONE                             \
    }

// D_TABLE_EMPTY
//   macro: create an empty table with a known column schema but no
// rows.  Useful as a starting point before calling d_table_reserve
// or d_table_push_row.
// Usage:
//   struct d_table t = D_TABLE_EMPTY(
//       sizeof(struct my_row), cols,
//       D_TABLE_COLUMN_COUNT(cols));
#define D_TABLE_EMPTY(row_sz,                                         \
                      col_desc_array,                                 \
                      num_columns)                                    \
    {                                                                 \
        .data         = NULL,                                         \
        .column_descs = (struct d_table_column_desc*)(col_desc_array),\
        .struct_size  = (row_sz),                                     \
        .row_count    = 0,                                            \
        .column_count = (num_columns),                                \
        .capacity     = 0,                                            \
        .flags        = D_TABLE_FLAG_NONE                             \
    }


///////////////////////////////////////////////////////////////////////////////
///             IV.   CONVENIENCE ACCESSOR MACROS                           ///
///////////////////////////////////////////////////////////////////////////////
//
// Thin wrappers that extract the table's own fields so the caller does
// not need to repeat data, struct_size, column_descs, etc.  These
// expand to the underlying table_common macros with zero overhead.
// All indices are raw size_t; for d_index support use the function
// accessors in sections VIII and IX.

// --- IV.1  Row access ---

// D_TABLE_ROW_PTR
//   macro: void* to the start of row `row` in table `tbl_ptr`.
// Note: #undef overrides the compound-literal helper from table_common.h
// with a convenience wrapper that extracts the table's own fields.
#undef D_TABLE_ROW_PTR
#define D_TABLE_ROW_PTR(tbl_ptr, row)                                 \
    D_TABLE_ROW_AT( (tbl_ptr)->data,                                  \
                    (tbl_ptr)->struct_size,                            \
                    (row) )

// D_TABLE_ROW_AS
//   macro: typed pointer to row `row` in table `tbl_ptr`.
// Usage:
//   struct my_row* r = D_TABLE_ROW_AS(struct my_row*, &t, 0);
#define D_TABLE_ROW_AS(type, tbl_ptr, row)                            \
    D_TABLE_ROW_AT_T( type,                                           \
                      (tbl_ptr)->data,                                \
                      (tbl_ptr)->struct_size,                         \
                      (row) )

// --- IV.2  Cell access ---

// D_TABLE_CELL_PTR
//   macro: void* to the cell at (row, col) in table `tbl_ptr`.
// Note: #undef overrides the compound-literal helper from table_common.h
// with a convenience wrapper that extracts the table's own fields.
#undef D_TABLE_CELL_PTR
#define D_TABLE_CELL_PTR(tbl_ptr, row, col)                           \
    D_TABLE_CELL_AT( (tbl_ptr)->data,                                 \
                     (tbl_ptr)->struct_size,                          \
                     (tbl_ptr)->column_descs,                         \
                     (row),                                           \
                     (col) )

// D_TABLE_CELL_VAL
//   macro: read the cell at (row, col) as a value of the given type.
// Usage:
//   int id = D_TABLE_CELL_VAL(int, &t, 0, 0);
#define D_TABLE_CELL_VAL(type, tbl_ptr, row, col)                     \
    D_TABLE_CELL_AS( type,                                            \
                     (tbl_ptr)->data,                                 \
                     (tbl_ptr)->struct_size,                          \
                     (tbl_ptr)->column_descs,                         \
                     (row),                                           \
                     (col) )


///////////////////////////////////////////////////////////////////////////////
///             V.    CONSTRUCTOR FUNCTIONS                                 ///
///////////////////////////////////////////////////////////////////////////////

// d_table_new
//   Allocate an empty table with the given column schema and initial
// row capacity.  Ownership: OWNS_DATA.
struct d_table* d_table_new(size_t _struct_size, const struct d_table_column_desc* _column_descs, size_t _column_count, size_t _initial_capacity);

// d_table_new_from_rows
//   Allocate a table and copy `_row_count` rows from `_source` into
// it.  The resulting table has capacity == _row_count.
// Ownership: OWNS_DATA.
struct d_table* d_table_new_from_rows(size_t _struct_size, const void* _source, size_t _row_count, const struct d_table_column_desc* _column_descs, size_t _column_count);

// d_table_new_copy
//   Deep-copy another table.  Both the row buffer and the column
// descriptor array are duplicated.
// Ownership: OWNS_DATA | OWNS_DESCS.
struct d_table* d_table_new_copy(const struct d_table* _other);

// d_table_new_fill
//   Allocate a table with `_row_count` rows, each initialized to the
// bytes at `_fill_value` (which must point to _struct_size bytes).
// Ownership: OWNS_DATA.
struct d_table* d_table_new_fill(size_t _struct_size, size_t _row_count, const struct d_table_column_desc* _column_descs, size_t _column_count, const void* _fill_value);


///////////////////////////////////////////////////////////////////////////////
///             VI.   CAPACITY FUNCTIONS                                    ///
///////////////////////////////////////////////////////////////////////////////

// d_table_reserve
//   Ensure the table can hold at least `_new_capacity` rows without
// further allocation.  If the table does not own its data, promotes
// it to an owned heap copy first.
bool   d_table_reserve(struct d_table* _table, size_t _new_capacity);

// d_table_shrink_to_fit
//   Reallocate the row buffer to exactly fit `row_count` rows,
// releasing excess capacity.
bool   d_table_shrink_to_fit(struct d_table* _table);

// d_table_ensure_capacity
//   Ensure room for at least `_required` total rows.  Grows by the
// configured growth factor if the current capacity is insufficient.
bool   d_table_ensure_capacity(struct d_table* _table, size_t _required);

// d_table_grow
//   Grow the row buffer by the configured growth factor.  Equivalent
// to ensure_capacity(table, capacity * GROWTH_FACTOR).
bool   d_table_grow(struct d_table* _table);

// d_table_available
//   Return the number of row slots available before the next
// reallocation would be required.
size_t d_table_available(const struct d_table* _table);


///////////////////////////////////////////////////////////////////////////////
///             VII.  ROW MUTATION FUNCTIONS                                ///
///////////////////////////////////////////////////////////////////////////////

// d_table_push_row
//   Append a single row at the end of the table.  `_row_data` must
// point to at least `struct_size` bytes.  Grows automatically.
bool   d_table_push_row(struct d_table* _table, const void* _row_data);

// d_table_push_row_empty
//   Append a zeroed row at the end of the table.
bool   d_table_push_row_empty(struct d_table* _table);

// d_table_insert_row
//   Insert a single row at `_index`, shifting subsequent rows down.
// Supports negative indexing via d_index.
bool   d_table_insert_row(struct d_table* _table, d_index _index, const void* _row_data);

// d_table_insert_rows
//   Insert `_count` contiguous rows starting at `_index`.
bool   d_table_insert_rows(struct d_table* _table, d_index _index, const void* _source, size_t _count);

// d_table_remove_row
//   Remove the row at `_index`, shifting subsequent rows up.
// Supports negative indexing.
bool   d_table_remove_row(struct d_table* _table, d_index _index);

// d_table_remove_range
//   Remove rows from `_start` to `_end` inclusive.  Both indices
// support negative indexing.
bool   d_table_remove_range(struct d_table* _table, d_index _start, d_index _end);

// d_table_pop_row
//   Remove the last row.  If `_out_row` is non-NULL, the row data is
// copied into it before removal.  `_out_row` must point to at least
// `struct_size` bytes.
bool   d_table_pop_row(struct d_table* _table, void* _out_row);

// d_table_clear
//   Set row_count to 0 without freeing or reallocating the buffer.
void   d_table_clear(struct d_table* _table);


///////////////////////////////////////////////////////////////////////////////
///             VIII. CELL ACCESS FUNCTIONS                                 ///
///////////////////////////////////////////////////////////////////////////////

// d_table_cell_ptr
//   Return a void* to the cell at (`_row`, `_col`).  Both indices
// support negative indexing.  Returns NULL if out of bounds.
void*  d_table_cell_ptr(const struct d_table* _table, d_index _row, d_index _col);

// d_table_cell_get
//   Copy the cell at (`_row`, `_col`) into the buffer at `_out`.
// `_out` must be at least as large as the column's element_size.
bool   d_table_cell_get(const struct d_table* _table, d_index _row, d_index _col, void* _out);

// d_table_cell_set
//   Write `_value` (element_size bytes) into the cell at
// (`_row`, `_col`).
bool   d_table_cell_set(struct d_table* _table, d_index _row, d_index _col, const void* _value);


///////////////////////////////////////////////////////////////////////////////
///             IX.   ROW ACCESS FUNCTIONS                                  ///
///////////////////////////////////////////////////////////////////////////////

// d_table_row_ptr
//   Return a void* to the start of row `_row`.  Supports negative
// indexing.  Returns NULL if out of bounds.
void*  d_table_row_ptr(const struct d_table* _table, d_index _row);

// d_table_row_get
//   Copy the entire row at `_row` into `_out`.  `_out` must be at
// least `struct_size` bytes.
bool   d_table_row_get(const struct d_table* _table, d_index _row, void* _out);

// d_table_row_set
//   Overwrite the row at `_row` with `struct_size` bytes from
// `_source`.
bool   d_table_row_set(struct d_table* _table, d_index _row, const void* _source);


///////////////////////////////////////////////////////////////////////////////
///             X.    QUERY FUNCTIONS                                       ///
///////////////////////////////////////////////////////////////////////////////

// d_table_row_count
//   Return the current number of rows.
size_t d_table_row_count(const struct d_table* _table);

// d_table_column_count
//   Return the number of columns.
size_t d_table_column_count(const struct d_table* _table);

// d_table_capacity
//   Return the current row capacity.
size_t d_table_capacity(const struct d_table* _table);

// d_table_struct_size
//   Return the size in bytes of one row.
size_t d_table_struct_size(const struct d_table* _table);

// d_table_is_empty
//   Return true if the table has zero rows.
bool   d_table_is_empty(const struct d_table* _table);

// d_table_is_full
//   Return true if row_count == capacity.
bool   d_table_is_full(const struct d_table* _table);

// d_table_data
//   Return a pointer to the raw row buffer.
void*  d_table_data(const struct d_table* _table);


///////////////////////////////////////////////////////////////////////////////
///             XI.   SEARCH FUNCTIONS                                      ///
///////////////////////////////////////////////////////////////////////////////

// d_table_find_row
//   Forward linear search for the first row whose cell in column
// `_col` compares equal to `_value` via `_comparator`.  Returns the
// row index, or -1 if not found.
ssize_t d_table_find_row(const struct d_table* _table, size_t _col, const void* _value, fn_comparator _comparator);

// d_table_find_row_last
//   Reverse linear search for the last matching row.
ssize_t d_table_find_row_last(const struct d_table* _table, size_t _col, const void* _value, fn_comparator _comparator);

// d_table_contains
//   Return true if any row's cell in column `_col` matches `_value`.
bool    d_table_contains(const struct d_table* _table, size_t _col, const void* _value, fn_comparator _comparator);

// d_table_count_value
//   Count the number of rows whose cell in column `_col` matches
// `_value`.
size_t  d_table_count_value(const struct d_table* _table, size_t _col, const void* _value, fn_comparator _comparator);


///////////////////////////////////////////////////////////////////////////////
///             XII.  UTILITY FUNCTIONS                                     ///
///////////////////////////////////////////////////////////////////////////////

// d_table_swap_rows
//   Swap the contents of rows at `_row_a` and `_row_b`.  Both
// indices support negative indexing.
bool    d_table_swap_rows(struct d_table* _table, d_index _row_a, d_index _row_b);

// d_table_reverse
//   Reverse the order of all rows in the table.
bool    d_table_reverse(struct d_table* _table);

// d_table_sort_by_column
//   Sort all rows in place by the values in column `_col`, using
// `_comparator` to order elements.
bool    d_table_sort_by_column(struct d_table* _table, size_t _col, fn_comparator _comparator);

// d_table_copy_to
//   Copy the row data into a caller-supplied buffer.
bool    d_table_copy_to(const struct d_table* _table, void* _destination, size_t _dest_capacity);

// d_table_validate
//   Validate the table's column descriptors against its struct_size.
bool    d_table_validate(const struct d_table* _table);


///////////////////////////////////////////////////////////////////////////////
///             XIII. DESTRUCTOR FUNCTIONS                                  ///
///////////////////////////////////////////////////////////////////////////////

// d_table_free
//   Free all owned resources (data, column_descs) according to the
// table's flags, then free the table struct itself.  Safe to call
// with NULL.
void    d_table_free(struct d_table* _table);

// d_table_free_deep
//   Like d_table_free, but first applies `_free_fn` to each cell in
// columns marked D_TABLE_COLUMN_FLAG_NULLABLE or containing
// pointers.  Useful for tables whose cells hold heap-allocated
// strings or nested structures.
void    d_table_free_deep(struct d_table* _table, fn_free _free_fn);


#endif  // DJINTERP_C_CONTAINER_TABLE_