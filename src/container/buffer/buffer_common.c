#include "..\..\inc\container\buffer\buffer_common.h"



// =============================================================================
// INTERNAL HELPERS
// =============================================================================

// d_buffer_common__element_at
//   internal: return a pointer to the element at _index in a contiguous
// array starting at _base with the given _element_size.
static inline void*
d_buffer_common__element_at(const void* _base,
                            size_t      _index,
                            size_t      _element_size)
{
    return (char*)_base + (_index * _element_size);
}


// d_buffer_common__move_elements
//   internal: shift a range of elements within a contiguous buffer.
// Handles overlapping regions via memmove.
static inline void
d_buffer_common__move_elements(void*  _base,
                               size_t _element_size,
                               size_t _from,
                               size_t _to,
                               size_t _n)
{
    memmove(d_buffer_common__element_at(_base, _to, _element_size),
            d_buffer_common__element_at(_base, _from, _element_size),
            _n * _element_size);
}


// =============================================================================
// INITIALIZATION
// =============================================================================

bool
d_buffer_common_init_sized(void**  _destination,
                           size_t* _count,
                           size_t* _capacity,
                           size_t  _element_size,
                           size_t  _initial_capacity)
{
    if (!_destination || !_count || !_capacity || _element_size == 0)
        return false;

    if (_initial_capacity == 0)
        _initial_capacity = D_BUFFER_DEFAULT_CAPACITY;

    *_destination = d_buffer_common_alloc(_element_size, _initial_capacity);
    if (!*_destination)
        return false;

    *_count    = 0;
    *_capacity = _initial_capacity;
    return true;
}


bool
d_buffer_common_init_from_data(void**      _destination,
                               size_t*     _count,
                               size_t*     _capacity,
                               size_t      _element_size,
                               const void* _source,
                               size_t      _source_count,
                               size_t      _extra_capacity)
{
    if (!_destination || !_count || !_capacity || _element_size == 0)
        return false;
    if (!_source && _source_count > 0)
        return false;

    size_t total = _source_count + _extra_capacity;
    if (total < D_BUFFER_DEFAULT_CAPACITY)
        total = D_BUFFER_DEFAULT_CAPACITY;

    *_destination = d_buffer_common_alloc(_element_size, total);
    if (!*_destination)
        return false;

    if (_source_count > 0)
        d_memcpy(*_destination, _source, _source_count * _element_size);

    *_count    = _source_count;
    *_capacity = total;
    return true;
}


bool
d_buffer_common_init_from_args(void**  _destination,
                               size_t* _count,
                               size_t* _capacity,
                               size_t  _element_size,
                               size_t  _arg_count,
                               va_list _args)
{
    if (!_destination || !_count || !_capacity || _element_size == 0)
        return false;

    size_t cap = _arg_count > D_BUFFER_DEFAULT_CAPACITY
                     ? _arg_count
                     : D_BUFFER_DEFAULT_CAPACITY;

    *_destination = d_buffer_common_alloc(_element_size, cap);
    if (!*_destination)
        return false;

    for (size_t i = 0; i < _arg_count; ++i)
    {
        void* arg = va_arg(_args, void*);
        d_memcpy(d_buffer_common__element_at(*_destination, i, _element_size),
                 arg,
                 _element_size);
    }

    *_count    = _arg_count;
    *_capacity = cap;
    return true;
}


bool
d_buffer_common_init_copy(void**      _destination,
                          size_t*     _count,
                          size_t*     _capacity,
                          size_t      _element_size,
                          const void* _source,
                          size_t      _source_count)
{
    return d_buffer_common_init_from_data(_destination,
                                          _count,
                                          _capacity,
                                          _element_size,
                                          _source,
                                          _source_count,
                                          0);
}


bool
d_buffer_common_init_fill(void**      _destination,
                          size_t*     _count,
                          size_t*     _capacity,
                          size_t      _element_size,
                          size_t      _size,
                          const void* _value)
{
    if (!_destination || !_count || !_capacity || _element_size == 0)
        return false;
    if (!_value)
        return false;

    size_t cap = _size > D_BUFFER_DEFAULT_CAPACITY
                     ? _size
                     : D_BUFFER_DEFAULT_CAPACITY;

    *_destination = d_buffer_common_alloc(_element_size, cap);
    if (!*_destination)
        return false;

    for (size_t i = 0; i < _size; ++i)
    {
        d_memcpy(d_buffer_common__element_at(*_destination, i, _element_size),
                 _value,
                 _element_size);
    }

    *_count    = _size;
    *_capacity = cap;
    return true;
}


// =============================================================================
// CAPACITY MANAGEMENT
// =============================================================================

void*
d_buffer_common_alloc(size_t _element_size, size_t _capacity)
{
    if (_element_size == 0 || _capacity == 0)
        return NULL;

    if (_capacity > SIZE_MAX / _element_size)
        return NULL;

    void* mem = malloc(_capacity * _element_size);
    if (mem)
        d_memset(mem, 0, _capacity * _element_size);
    return mem;
}


size_t
d_buffer_common_calc_growth(size_t _current_capacity,
                            size_t _required_capacity)
{
    if (_current_capacity == 0)
        _current_capacity = D_BUFFER_DEFAULT_CAPACITY;

    size_t grown = (size_t)((double)_current_capacity * D_BUFFER_GROWTH_FACTOR);

    if (grown <= _current_capacity)
        grown = _current_capacity + 1;

    return grown > _required_capacity ? grown : _required_capacity;
}


bool
d_buffer_common_ensure_capacity(void**  _elements,
                                size_t* _capacity,
                                size_t  _element_size,
                                size_t  _required_capacity)
{
    if (!_elements || !_capacity || _element_size == 0)
        return false;

    if (*_capacity >= _required_capacity)
        return true;

    size_t new_cap = d_buffer_common_calc_growth(*_capacity, _required_capacity);

    if (new_cap > SIZE_MAX / _element_size)
        return false;

    void* new_mem = realloc(*_elements, new_cap * _element_size);
    if (!new_mem)
        return false;

    d_memset((char*)new_mem + (*_capacity * _element_size),
             0,
             (new_cap - *_capacity) * _element_size);

    *_elements = new_mem;
    *_capacity = new_cap;
    return true;
}


bool
d_buffer_common_resize_to_fit(void**  _elements,
                              size_t* _capacity,
                              size_t  _element_size,
                              size_t  _count)
{
    if (!_elements || !_capacity || _element_size == 0)
        return false;

    if (_count == 0)
    {
        free(*_elements);
        *_elements = NULL;
        *_capacity = 0;
        return true;
    }

    if (*_capacity == _count)
        return true;

    void* new_mem = realloc(*_elements, _count * _element_size);
    if (!new_mem)
        return false;

    *_elements = new_mem;
    *_capacity = _count;
    return true;
}


// =============================================================================
// ELEMENT ACCESS
// =============================================================================

void*
d_buffer_common_get_element(const void* _elements,
                            size_t      _count,
                            size_t      _element_size,
                            d_index     _index)
{
    if (!_elements || (size_t)_index >= _count || _element_size == 0)
        return NULL;

    return d_buffer_common__element_at(_elements, (size_t)_index, _element_size);
}


bool
d_buffer_common_set_element(void*       _elements,
                            size_t      _count,
                            size_t      _element_size,
                            d_index     _index,
                            const void* _value)
{
    if (!_elements || !_value || (size_t)_index >= _count || _element_size == 0)
        return false;

    d_memcpy(d_buffer_common__element_at(_elements, (size_t)_index, _element_size),
             _value,
             _element_size);
    return true;
}


void*
d_buffer_common_peek_front(const void* _elements,
                           size_t      _count,
                           size_t      _element_size)
{
    if (!_elements || _count == 0 || _element_size == 0)
        return NULL;
    return (void*)_elements;
}


void*
d_buffer_common_peek_back(const void* _elements,
                          size_t      _count,
                          size_t      _element_size)
{
    if (!_elements || _count == 0 || _element_size == 0)
        return NULL;
    return d_buffer_common__element_at(_elements, _count - 1, _element_size);
}


// =============================================================================
// INSERTION  (RESIZE MODE)
// =============================================================================

bool
d_buffer_common_append_element(void*       _elements,
                               size_t*     _count,
                               size_t      _capacity,
                               size_t      _element_size,
                               const void* _value)
{
    if (!_elements || !_count || !_value || _element_size == 0)
        return false;
    if (*_count >= _capacity)
        return false;

    d_memcpy(d_buffer_common__element_at(_elements, *_count, _element_size),
             _value, _element_size);
    ++(*_count);
    return true;
}


bool
d_buffer_common_append_data(void*       _elements,
                            size_t*     _count,
                            size_t      _capacity,
                            size_t      _element_size,
                            const void* _data,
                            size_t      _data_count)
{
    if (!_elements || !_count || !_data || _element_size == 0)
        return false;
    if (*_count + _data_count > _capacity)
        return false;

    d_memcpy(d_buffer_common__element_at(_elements, *_count, _element_size),
             _data, _data_count * _element_size);
    *_count += _data_count;
    return true;
}


bool
d_buffer_common_prepend_element(void*       _elements,
                                size_t*     _count,
                                size_t      _capacity,
                                size_t      _element_size,
                                const void* _value)
{
    if (!_elements || !_count || !_value || _element_size == 0)
        return false;
    if (*_count >= _capacity)
        return false;

    d_buffer_common__move_elements(_elements, _element_size, 0, 1, *_count);
    d_memcpy(_elements, _value, _element_size);
    ++(*_count);
    return true;
}


bool
d_buffer_common_prepend_data(void*       _elements,
                             size_t*     _count,
                             size_t      _capacity,
                             size_t      _element_size,
                             const void* _data,
                             size_t      _data_count)
{
    if (!_elements || !_count || !_data || _element_size == 0)
        return false;
    if (*_count + _data_count > _capacity)
        return false;

    d_buffer_common__move_elements(_elements, _element_size,
                                   0, _data_count, *_count);
    d_memcpy(_elements, _data, _data_count * _element_size);
    *_count += _data_count;
    return true;
}


bool
d_buffer_common_insert_element(void*       _elements,
                               size_t*     _count,
                               size_t      _capacity,
                               size_t      _element_size,
                               const void* _value,
                               d_index     _index)
{
    if (!_elements || !_count || !_value || _element_size == 0)
        return false;
    if ((size_t)_index > *_count || *_count >= _capacity)
        return false;

    if ((size_t)_index < *_count)
    {
        d_buffer_common__move_elements(_elements, _element_size,
                                       (size_t)_index,
                                       (size_t)_index + 1,
                                       *_count - (size_t)_index);
    }

    d_memcpy(d_buffer_common__element_at(_elements, (size_t)_index, _element_size),
             _value, _element_size);
    ++(*_count);
    return true;
}


bool
d_buffer_common_insert_data(void*       _elements,
                            size_t*     _count,
                            size_t      _capacity,
                            size_t      _element_size,
                            const void* _data,
                            size_t      _data_count,
                            d_index     _index)
{
    if (!_elements || !_count || !_data || _element_size == 0)
        return false;
    if ((size_t)_index > *_count || *_count + _data_count > _capacity)
        return false;

    if ((size_t)_index < *_count)
    {
        d_buffer_common__move_elements(_elements, _element_size,
                                       (size_t)_index,
                                       (size_t)_index + _data_count,
                                       *_count - (size_t)_index);
    }

    d_memcpy(d_buffer_common__element_at(_elements, (size_t)_index, _element_size),
             _data, _data_count * _element_size);
    *_count += _data_count;
    return true;
}


// =============================================================================
// APPEND MODE  (CHUNKED OPERATIONS)
// =============================================================================

// d_buffer_common__chunk_list_push
//   internal: link a new chunk onto the tail of _list.
static void
d_buffer_common__chunk_list_push(struct d_buffer_chunk_list* _list,
                                 struct d_buffer_chunk*      _chunk)
{
    if (_list->tail)
        _list->tail->next = _chunk;
    else
        _list->head = _chunk;

    _list->tail = _chunk;
    ++_list->chunk_count;
}


struct d_buffer_chunk*
d_buffer_common_chunk_new(size_t _element_size, size_t _capacity)
{
    if (_element_size == 0 || _capacity == 0)
        return NULL;

    struct d_buffer_chunk* chunk = malloc(sizeof(struct d_buffer_chunk));
    if (!chunk)
        return NULL;

    chunk->elements = d_buffer_common_alloc(_element_size, _capacity);
    if (!chunk->elements)
    {
        free(chunk);
        return NULL;
    }

    chunk->count    = 0;
    chunk->capacity = _capacity;
    chunk->next     = NULL;
    return chunk;
}


void
d_buffer_common_chunk_free(struct d_buffer_chunk* _chunk)
{
    if (!_chunk)
        return;
    free(_chunk->elements);
    free(_chunk);
}


void
d_buffer_common_chunk_list_init(struct d_buffer_chunk_list* _list)
{
    if (!_list)
        return;
    _list->head        = NULL;
    _list->tail        = NULL;
    _list->chunk_count = 0;
    _list->total_count = 0;
}


void
d_buffer_common_chunk_list_free(struct d_buffer_chunk_list* _list)
{
    if (!_list)
        return;

    struct d_buffer_chunk* cur = _list->head;
    while (cur)
    {
        struct d_buffer_chunk* next = cur->next;
        d_buffer_common_chunk_free(cur);
        cur = next;
    }

    d_buffer_common_chunk_list_init(_list);
}


bool
d_buffer_common_append_element_chunked(struct d_buffer_chunk_list* _list,
                                       size_t                      _element_size,
                                       const void*                 _value,
                                       size_t                      _chunk_capacity)
{
    if (!_list || !_value || _element_size == 0)
        return false;

    // use existing tail if it has space
    if (_list->tail && _list->tail->count < _list->tail->capacity)
    {
        d_memcpy(d_buffer_common__element_at(_list->tail->elements,
                                              _list->tail->count,
                                              _element_size),
                 _value, _element_size);
        ++_list->tail->count;
        ++_list->total_count;
        return true;
    }

    // allocate a new chunk: at least 1, at least _chunk_capacity
    size_t cap = _chunk_capacity > 1 ? _chunk_capacity : 1;

    struct d_buffer_chunk* chunk =
        d_buffer_common_chunk_new(_element_size, cap);
    if (!chunk)
        return false;

    d_memcpy(chunk->elements, _value, _element_size);
    chunk->count = 1;

    d_buffer_common__chunk_list_push(_list, chunk);
    _list->total_count += 1;
    return true;
}


bool
d_buffer_common_append_data_chunked(struct d_buffer_chunk_list* _list,
                                    size_t                      _element_size,
                                    const void*                 _data,
                                    size_t                      _data_count,
                                    size_t                      _chunk_capacity)
{
    if (!_list || !_data || _element_size == 0 || _data_count == 0)
        return false;

    const char* src       = (const char*)_data;
    size_t      remaining = _data_count;

    // fill existing tail space first
    if (_list->tail && _list->tail->count < _list->tail->capacity)
    {
        size_t space = _list->tail->capacity - _list->tail->count;
        size_t batch = remaining < space ? remaining : space;

        d_memcpy(d_buffer_common__element_at(_list->tail->elements,
                                              _list->tail->count,
                                              _element_size),
                 src, batch * _element_size);

        _list->tail->count += batch;
        _list->total_count += batch;
        src       += batch * _element_size;
        remaining -= batch;
    }

    // allocate new chunks for the rest
    while (remaining > 0)
    {
        // chunk capacity: at least _remaining, at least _chunk_capacity
        size_t cap = _chunk_capacity > remaining ? _chunk_capacity : remaining;

        struct d_buffer_chunk* chunk =
            d_buffer_common_chunk_new(_element_size, cap);
        if (!chunk)
            return false;

        size_t batch = remaining < cap ? remaining : cap;

        d_memcpy(chunk->elements, src, batch * _element_size);
        chunk->count = batch;

        d_buffer_common__chunk_list_push(_list, chunk);
        _list->total_count += batch;

        src       += batch * _element_size;
        remaining -= batch;
    }

    return true;
}


bool
d_buffer_common_consolidate(void**                      _elements,
                            size_t*                     _count,
                            size_t*                     _capacity,
                            size_t                      _element_size,
                            struct d_buffer_chunk_list* _list)
{
    if (!_elements || !_count || !_capacity || !_list || _element_size == 0)
        return false;

    if (_list->chunk_count == 0)
        return true;

    size_t total = *_count + _list->total_count;

    if (total > *_capacity)
    {
        if (total > SIZE_MAX / _element_size)
            return false;

        void* new_mem = realloc(*_elements, total * _element_size);
        if (!new_mem)
            return false;

        *_elements = new_mem;
        *_capacity = total;
    }

    char* dst = (char*)(*_elements) + (*_count * _element_size);
    struct d_buffer_chunk* cur = _list->head;
    while (cur)
    {
        if (cur->count > 0)
        {
            d_memcpy(dst, cur->elements, cur->count * _element_size);
            dst += cur->count * _element_size;
        }
        cur = cur->next;
    }

    *_count = total;
    d_buffer_common_chunk_list_free(_list);
    return true;
}


size_t
d_buffer_common_total_count(size_t                            _primary_count,
                            const struct d_buffer_chunk_list* _list)
{
    size_t total = _primary_count;
    if (_list)
        total += _list->total_count;
    return total;
}


void*
d_buffer_common_get_element_chunked(const void*                       _primary_elements,
                                    size_t                            _primary_count,
                                    size_t                            _element_size,
                                    const struct d_buffer_chunk_list* _list,
                                    d_index                           _index)
{
    size_t idx = (size_t)_index;

    if (idx < _primary_count)
    {
        return d_buffer_common__element_at(_primary_elements,
                                            idx, _element_size);
    }

    idx -= _primary_count;
    if (!_list)
        return NULL;

    const struct d_buffer_chunk* cur = _list->head;
    while (cur)
    {
        if (idx < cur->count)
        {
            return d_buffer_common__element_at(cur->elements,
                                                idx, _element_size);
        }
        idx -= cur->count;
        cur = cur->next;
    }

    return NULL;
}


// =============================================================================
// REMOVAL
// =============================================================================

bool
d_buffer_common_remove_element(void*   _elements,
                               size_t* _count,
                               size_t  _element_size,
                               d_index _index)
{
    if (!_elements || !_count || _element_size == 0)
        return false;
    if ((size_t)_index >= *_count)
        return false;

    size_t idx = (size_t)_index;
    if (idx < *_count - 1)
    {
        d_buffer_common__move_elements(_elements, _element_size,
                                       idx + 1, idx, *_count - idx - 1);
    }

    --(*_count);
    d_memset(d_buffer_common__element_at(_elements, *_count, _element_size),
             0, _element_size);
    return true;
}


bool
d_buffer_common_remove_range(void*   _elements,
                             size_t* _count,
                             size_t  _element_size,
                             d_index _start,
                             d_index _end)
{
    if (!_elements || !_count || _element_size == 0)
        return false;

    size_t s = (size_t)_start;
    size_t e = (size_t)_end;
    if (s > e || e > *_count)
        return false;

    size_t removed = e - s;
    if (removed == 0)
        return true;

    if (e < *_count)
    {
        d_buffer_common__move_elements(_elements, _element_size,
                                       e, s, *_count - e);
    }

    *_count -= removed;
    d_memset(d_buffer_common__element_at(_elements, *_count, _element_size),
             0, removed * _element_size);
    return true;
}


bool
d_buffer_common_consume_front(void*   _elements,
                              size_t* _count,
                              size_t  _element_size,
                              size_t  _amount)
{
    if (!_elements || !_count || _element_size == 0)
        return false;
    if (_amount > *_count)
        return false;
    if (_amount == 0)
        return true;

    d_buffer_common__move_elements(_elements, _element_size,
                                   _amount, 0, *_count - _amount);
    *_count -= _amount;
    d_memset(d_buffer_common__element_at(_elements, *_count, _element_size),
             0, _amount * _element_size);
    return true;
}


bool
d_buffer_common_consume_back(void*   _elements,
                             size_t* _count,
                             size_t  _element_size,
                             size_t  _amount)
{
    if (!_elements || !_count || _element_size == 0)
        return false;
    if (_amount > *_count)
        return false;

    *_count -= _amount;
    d_memset(d_buffer_common__element_at(_elements, *_count, _element_size),
             0, _amount * _element_size);
    return true;
}


// =============================================================================
// STATE QUERIES
// =============================================================================

void   d_buffer_common_clear(size_t* _count)         { if (_count) *_count = 0; }
bool   d_buffer_common_is_empty(size_t _count)       { return _count == 0; }
bool   d_buffer_common_is_full(size_t _count,
                               size_t _capacity)     { return _count >= _capacity; }

double
d_buffer_common_utilization(size_t _count, size_t _capacity)
{
    if (_capacity == 0) return 0.0;
    return (double)_count / (double)_capacity;
}


// =============================================================================
// SEARCH
// =============================================================================

bool
d_buffer_common_contains(const void*   _elements,
                         size_t        _count,
                         size_t        _element_size,
                         const void*   _value,
                         fn_comparator _comparator)
{
    return d_buffer_common_find(_elements, _count, _element_size,
                                _value, _comparator) >= 0;
}


ssize_t
d_buffer_common_find(const void*   _elements,
                     size_t        _count,
                     size_t        _element_size,
                     const void*   _value,
                     fn_comparator _comparator)
{
    if (!_elements || !_value || !_comparator || _element_size == 0)
        return -1;

    for (size_t i = 0; i < _count; ++i)
    {
        if (_comparator(d_buffer_common__element_at(_elements, i, _element_size),
                        _value) == 0)
            return (ssize_t)i;
    }
    return -1;
}


ssize_t
d_buffer_common_find_last(const void*   _elements,
                          size_t        _count,
                          size_t        _element_size,
                          const void*   _value,
                          fn_comparator _comparator)
{
    if (!_elements || !_value || !_comparator || _element_size == 0 || _count == 0)
        return -1;

    for (size_t i = _count; i > 0; --i)
    {
        if (_comparator(d_buffer_common__element_at(_elements, i - 1, _element_size),
                        _value) == 0)
            return (ssize_t)(i - 1);
    }
    return -1;
}


// =============================================================================
// FILTER
// =============================================================================

bool
d_buffer_common_filter(const void*                 _elements,
                       size_t                      _count,
                       size_t                      _element_size,
                       const struct d_filter_expr*  _expression,
                       void**                      _out_elements,
                       size_t*                     _out_count)
{
    if (!_out_elements || !_out_count)
        return false;

    *_out_elements = NULL;
    *_out_count    = 0;

    if (!_elements || _count == 0 || !_expression || _element_size == 0)
        return true;

    // first pass: count matches
    size_t matches = 0;
    for (size_t i = 0; i < _count; ++i)
    {
        if (d_filter_evaluate(_expression,
                d_buffer_common__element_at(_elements, i, _element_size)))
            ++matches;
    }

    if (matches == 0)
        return true;

    void* out = malloc(matches * _element_size);
    if (!out)
        return false;

    // second pass: copy
    size_t w = 0;
    for (size_t i = 0; i < _count; ++i)
    {
        const void* elem =
            d_buffer_common__element_at(_elements, i, _element_size);
        if (d_filter_evaluate(_expression, elem))
        {
            d_memcpy(d_buffer_common__element_at(out, w, _element_size),
                     elem, _element_size);
            ++w;
        }
    }

    *_out_elements = out;
    *_out_count    = matches;
    return true;
}


bool
d_buffer_common_filter_in_place(void*                       _elements,
                                size_t*                     _count,
                                size_t                      _element_size,
                                const struct d_filter_expr*  _expression)
{
    if (!_elements || !_count || _element_size == 0 || !_expression)
        return false;

    size_t write = 0;
    for (size_t read = 0; read < *_count; ++read)
    {
        const void* elem =
            d_buffer_common__element_at(_elements, read, _element_size);
        if (d_filter_evaluate(_expression, elem))
        {
            if (write != read)
            {
                d_memcpy(d_buffer_common__element_at(_elements, write, _element_size),
                         elem, _element_size);
            }
            ++write;
        }
    }

    if (write < *_count)
    {
        d_memset(d_buffer_common__element_at(_elements, write, _element_size),
                 0, (*_count - write) * _element_size);
    }

    *_count = write;
    return true;
}


bool
d_buffer_common_filter_indices(const void*                 _elements,
                               size_t                      _count,
                               size_t                      _element_size,
                               const struct d_filter_expr*  _expression,
                               d_index**                   _out_indices,
                               size_t*                     _out_count)
{
    if (!_out_indices || !_out_count)
        return false;

    *_out_indices = NULL;
    *_out_count   = 0;

    if (!_elements || _count == 0 || !_expression || _element_size == 0)
        return true;

    size_t matches = 0;
    for (size_t i = 0; i < _count; ++i)
    {
        if (d_filter_evaluate(_expression,
                d_buffer_common__element_at(_elements, i, _element_size)))
            ++matches;
    }

    if (matches == 0)
        return true;

    d_index* indices = malloc(matches * sizeof(d_index));
    if (!indices)
        return false;

    size_t w = 0;
    for (size_t i = 0; i < _count; ++i)
    {
        if (d_filter_evaluate(_expression,
                d_buffer_common__element_at(_elements, i, _element_size)))
            indices[w++] = (d_index)i;
    }

    *_out_indices = indices;
    *_out_count   = matches;
    return true;
}


size_t
d_buffer_common_count_matching(const void*                 _elements,
                               size_t                      _count,
                               size_t                      _element_size,
                               const struct d_filter_expr*  _expression)
{
    if (!_elements || _count == 0 || !_expression || _element_size == 0)
        return 0;

    size_t matches = 0;
    for (size_t i = 0; i < _count; ++i)
    {
        if (d_filter_evaluate(_expression,
                d_buffer_common__element_at(_elements, i, _element_size)))
            ++matches;
    }
    return matches;
}


bool
d_buffer_common_filter_chunked(const void*                       _primary_elements,
                               size_t                            _primary_count,
                               size_t                            _element_size,
                               const struct d_buffer_chunk_list* _list,
                               const struct d_filter_expr*        _expression,
                               void**                            _out_elements,
                               size_t*                           _out_count)
{
    if (!_out_elements || !_out_count)
        return false;

    *_out_elements = NULL;
    *_out_count    = 0;

    if (!_expression || _element_size == 0)
        return true;

    size_t total = d_buffer_common_total_count(_primary_count, _list);
    if (total == 0)
        return true;

    // first pass: count
    size_t matches = 0;

    for (size_t i = 0; i < _primary_count; ++i)
    {
        if (d_filter_evaluate(_expression,
                d_buffer_common__element_at(_primary_elements, i, _element_size)))
            ++matches;
    }

    if (_list)
    {
        const struct d_buffer_chunk* cur = _list->head;
        while (cur)
        {
            for (size_t i = 0; i < cur->count; ++i)
            {
                if (d_filter_evaluate(_expression,
                        d_buffer_common__element_at(cur->elements, i, _element_size)))
                    ++matches;
            }
            cur = cur->next;
        }
    }

    if (matches == 0)
        return true;

    void* out = malloc(matches * _element_size);
    if (!out)
        return false;

    // second pass: copy
    size_t w = 0;

    for (size_t i = 0; i < _primary_count; ++i)
    {
        const void* elem =
            d_buffer_common__element_at(_primary_elements, i, _element_size);
        if (d_filter_evaluate(_expression, elem))
        {
            d_memcpy(d_buffer_common__element_at(out, w, _element_size),
                     elem, _element_size);
            ++w;
        }
    }

    if (_list)
    {
        const struct d_buffer_chunk* cur = _list->head;
        while (cur)
        {
            for (size_t i = 0; i < cur->count; ++i)
            {
                const void* elem =
                    d_buffer_common__element_at(cur->elements, i, _element_size);
                if (d_filter_evaluate(_expression, elem))
                {
                    d_memcpy(d_buffer_common__element_at(out, w, _element_size),
                             elem, _element_size);
                    ++w;
                }
            }
            cur = cur->next;
        }
    }

    *_out_elements = out;
    *_out_count    = matches;
    return true;
}


// =============================================================================
// COPY
// =============================================================================

bool
d_buffer_common_copy_to(const void* _source,
                        size_t      _source_count,
                        size_t      _element_size,
                        void*       _destination,
                        size_t      _destination_capacity,
                        size_t*     _copied_count)
{
    if (!_source || !_destination || _element_size == 0)
        return false;

    size_t to_copy = _source_count < _destination_capacity
                         ? _source_count : _destination_capacity;

    d_memcpy(_destination, _source, to_copy * _element_size);
    if (_copied_count)
        *_copied_count = to_copy;
    return true;
}


bool
d_buffer_common_copy_range_to(const void* _source,
                              size_t      _source_count,
                              size_t      _element_size,
                              d_index     _start,
                              d_index     _end,
                              void*       _destination,
                              size_t      _destination_capacity,
                              size_t*     _copied_count)
{
    if (!_source || !_destination || _element_size == 0)
        return false;

    size_t s = (size_t)_start;
    size_t e = (size_t)_end;
    if (s > e || e > _source_count)
        return false;

    size_t range   = e - s;
    size_t to_copy = range < _destination_capacity ? range : _destination_capacity;

    d_memcpy(_destination,
             d_buffer_common__element_at(_source, s, _element_size),
             to_copy * _element_size);

    if (_copied_count)
        *_copied_count = to_copy;
    return true;
}


// =============================================================================
// ORDERING
// =============================================================================

bool
d_buffer_common_reverse(void*  _elements,
                        size_t _count,
                        size_t _element_size)
{
    if (!_elements || _element_size == 0 || _count < 2)
        return _elements != NULL;

    char  stack_buf[256];
    char* tmp = _element_size <= sizeof(stack_buf)
                    ? stack_buf
                    : (char*)malloc(_element_size);
    if (!tmp)
        return false;

    size_t lo = 0;
    size_t hi = _count - 1;
    while (lo < hi)
    {
        void* a = d_buffer_common__element_at(_elements, lo, _element_size);
        void* b = d_buffer_common__element_at(_elements, hi, _element_size);
        d_memcpy(tmp, a,   _element_size);
        d_memcpy(a,   b,   _element_size);
        d_memcpy(b,   tmp, _element_size);
        ++lo;
        --hi;
    }

    if (tmp != stack_buf)
        free(tmp);
    return true;
}


void
d_buffer_common_sort(void*         _elements,
                     size_t        _count,
                     size_t        _element_size,
                     fn_comparator _comparator)
{
    if (!_elements || _count < 2 || _element_size == 0 || !_comparator)
        return;
    qsort(_elements, _count, _element_size, _comparator);
}


// =============================================================================
// VALIDATION
// =============================================================================

bool d_buffer_common_validate_params(size_t _element_size)   { return _element_size > 0; }
bool d_buffer_common_validate_capacity(size_t _capacity)     { return _capacity > 0; }
bool d_buffer_common_validate_index(d_index _index,
                                    size_t  _count)          { return (size_t)_index < _count; }


// =============================================================================
// DESTRUCTION
// =============================================================================

void
d_buffer_common_free_data(void* _elements)
{
    free(_elements);
}


void
d_buffer_common_free_data_deep(size_t  _count,
                               void**  _elements,
                               fn_free _free_fn)
{
    if (!_elements || !_free_fn)
        return;

    for (size_t i = 0; i < _count; ++i)
    {
        if (_elements[i])
            _free_fn(_elements[i]);
    }

    free(_elements);
}
.
  _out_indices:  out pointer to receive the index array.
  _out_count:    out pointer to receive the match count.
Return:
  A boolean value corresponding to either:
  - true, if filtering succeeded (including zero matches), or
  - false, if allocation failed or output pointers are NULL.
*/
bool
d_buffer_common_filter_indices
(
    const void*                 _elements,
    size_t                      _count,
    size_t                      _element_size,
    const struct d_filter_expr* _expression,
    d_index**                   _out_indices,
    size_t*                     _out_count
)
{
    size_t   matches;
    size_t   i;
    size_t   w;
    d_index* indices;

    // validate output pointers
    if ( (!_out_indices) ||
         (!_out_count) )
    {
        return false;
    }

    *_out_indices = NULL;
    *_out_count   = 0;

    // vacuous success on empty input
    if ( (!_elements)   ||
         (_count == 0)  ||
         (!_expression) ||
         (_element_size == 0) )
    {
        return true;
    }

    // first pass: count matches
    matches = 0;

    for (i = 0; i < _count; ++i)
    {
        if (d_filter_evaluate(_expression,
                              d_buffer_common__element_at(_elements,
                                                          i,
                                                          _element_size)))
        {
            ++matches;
        }
    }

    // no matches
    if (matches == 0)
    {
        return true;
    }

    // allocate index array
    indices = malloc(matches * sizeof(d_index));

    // check allocation
    if (!indices)
    {
        return false;
    }

    // second pass: record indices
    w = 0;

    for (i = 0; i < _count; ++i)
    {
        if (d_filter_evaluate(_expression,
                              d_buffer_common__element_at(_elements,
                                                          i,
                                                          _element_size)))
        {
            indices[w++] = (d_index)i;
        }
    }

    *_out_indices = indices;
    *_out_count   = matches;

    return true;
}


/*
d_buffer_common_count_matching
  Returns the number of elements that satisfy _expression.

Parameter(s):
  _elements:     pointer to the buffer data.
  _count:        number of occupied elements.
  _element_size: size of each element in bytes.
  _expression:   filter expression tree.
Return:
  The number of matching elements, or 0 if the buffer is empty or
_expression is NULL.
*/
size_t
d_buffer_common_count_matching
(
    const void*                 _elements,
    size_t                      _count,
    size_t                      _element_size,
    const struct d_filter_expr* _expression
)
{
    size_t matches;
    size_t i;

    // validate parameters
    if ( (!_elements)   ||
         (_count == 0)  ||
         (!_expression) ||
         (_element_size == 0) )
    {
        return 0;
    }

    matches = 0;

    for (i = 0; i < _count; ++i)
    {
        if (d_filter_evaluate(_expression,
                              d_buffer_common__element_at(_elements,
                                                          i,
                                                          _element_size)))
        {
            ++matches;
        }
    }

    return matches;
}


/*
d_buffer_common_filter_chunked
  Filters across primary allocation + overflow chunks.

Parameter(s):
  _primary_elements: pointer to the primary buffer data.
  _primary_count:    number of elements in the primary buffer.
  _element_size:     size of each element in bytes.
  _list:             pointer to the chunk list.
  _expression:       filter expression tree.
  _out_elements:     out pointer to receive the result allocation.
  _out_count:        out pointer to receive the match count.
Return:
  A boolean value corresponding to either:
  - true, if filtering succeeded (including zero matches), or
  - false, if allocation failed or output pointers are NULL.
*/
bool
d_buffer_common_filter_chunked
(
    const void*                       _primary_elements,
    size_t                            _primary_count,
    size_t                            _element_size,
    const struct d_buffer_chunk_list* _list,
    const struct d_filter_expr*       _expression,
    void**                            _out_elements,
    size_t*                           _out_count
)
{
    size_t                       total;
    size_t                       matches;
    size_t                       i;
    size_t                       w;
    const struct d_buffer_chunk* cur;
    const void*                  elem;
    void*                        out;

    // validate output pointers
    if ( (!_out_elements) ||
         (!_out_count) )
    {
        return false;
    }

    *_out_elements = NULL;
    *_out_count    = 0;

    // vacuous success
    if ( (!_expression) ||
         (_element_size == 0) )
    {
        return true;
    }

    total = d_buffer_common_total_count(_primary_count, _list);

    // empty input
    if (total == 0)
    {
        return true;
    }

    // first pass: count matches across primary + chunks
    matches = 0;

    for (i = 0; i < _primary_count; ++i)
    {
        elem = d_buffer_common__element_at(_primary_elements,
                                            i,
                                            _element_size);

        if (d_filter_evaluate(_expression, elem))
        {
            ++matches;
        }
    }

    if (_list)
    {
        cur = _list->head;

        while (cur)
        {
            for (i = 0; i < cur->count; ++i)
            {
                elem = d_buffer_common__element_at(cur->elements,
                                                    i,
                                                    _element_size);

                if (d_filter_evaluate(_expression, elem))
                {
                    ++matches;
                }
            }

            cur = cur->next;
        }
    }

    // no matches
    if (matches == 0)
    {
        return true;
    }

    // allocate result
    out = malloc(matches * _element_size);

    // check allocation
    if (!out)
    {
        return false;
    }

    // second pass: copy matching elements
    w = 0;

    for (i = 0; i < _primary_count; ++i)
    {
        elem = d_buffer_common__element_at(_primary_elements,
                                            i,
                                            _element_size);

        if (d_filter_evaluate(_expression, elem))
        {
            d_memcpy(d_buffer_common__element_at(out,
                                                  w,
                                                  _element_size),
                     elem,
                     _element_size);
            ++w;
        }
    }

    if (_list)
    {
        cur = _list->head;

        while (cur)
        {
            for (i = 0; i < cur->count; ++i)
            {
                elem = d_buffer_common__element_at(cur->elements,
                                                    i,
                                                    _element_size);

                if (d_filter_evaluate(_expression, elem))
                {
                    d_memcpy(d_buffer_common__element_at(out,
                                                          w,
                                                          _element_size),
                             elem,
                             _element_size);
                    ++w;
                }
            }

            cur = cur->next;
        }
    }

    *_out_elements = out;
    *_out_count    = matches;

    return true;
}


// =============================================================================
// X.    copy
// =============================================================================

/*
d_buffer_common_copy_to
  Copies elements from source to destination.

Parameter(s):
  _source:               pointer to source data.
  _source_count:         number of elements in source.
  _element_size:         size of each element in bytes.
  _destination:          pointer to destination buffer.
  _destination_capacity: capacity of destination in elements.
  _copied_count:         out pointer to receive the copy count; may
                         be NULL.
Return:
  A boolean value corresponding to either:
  - true, if the copy succeeded, or
  - false, if parameters are invalid.
*/
bool
d_buffer_common_copy_to
(
    const void* _source,
    size_t      _source_count,
    size_t      _element_size,
    void*       _destination,
    size_t      _destination_capacity,
    size_t*     _copied_count
)
{
    size_t to_copy;

    // validate parameters
    if ( (!_source)      ||
         (!_destination) ||
         (_element_size == 0) )
    {
        return false;
    }

    to_copy = _source_count < _destination_capacity
                  ? _source_count
                  : _destination_capacity;

    d_memcpy(_destination,
             _source,
             to_copy * _element_size);

    if (_copied_count)
    {
        *_copied_count = to_copy;
    }

    return true;
}


/*
d_buffer_common_copy_range_to
  Copies a range of elements from source to destination.

Parameter(s):
  _source:               pointer to source data.
  _source_count:         number of elements in source.
  _element_size:         size of each element in bytes.
  _start:                first index to copy.
  _end:                  one-past-last index to copy.
  _destination:          pointer to destination buffer.
  _destination_capacity: capacity of destination in elements.
  _copied_count:         out pointer to receive the copy count; may
                         be NULL.
Return:
  A boolean value corresponding to either:
  - true, if the copy succeeded, or
  - false, if out of range or parameters are invalid.
*/
bool
d_buffer_common_copy_range_to
(
    const void* _source,
    size_t      _source_count,
    size_t      _element_size,
    d_index     _start,
    d_index     _end,
    void*       _destination,
    size_t      _destination_capacity,
    size_t*     _copied_count
)
{
    size_t s;
    size_t e;
    size_t range;
    size_t to_copy;

    // validate parameters
    if ( (!_source)      ||
         (!_destination) ||
         (_element_size == 0) )
    {
        return false;
    }

    s = (size_t)_start;
    e = (size_t)_end;

    // validate range
    if ( (s > e) ||
         (e > _source_count) )
    {
        return false;
    }

    range   = e - s;
    to_copy = range < _destination_capacity
                  ? range
                  : _destination_capacity;

    d_memcpy(_destination,
             d_buffer_common__element_at(_source,
                                          s,
                                          _element_size),
             to_copy * _element_size);

    if (_copied_count)
    {
        *_copied_count = to_copy;
    }

    return true;
}


// =============================================================================
// XI.   ordering
// =============================================================================

/*
d_buffer_common_reverse
  Reverses the order of elements in the buffer.

Parameter(s):
  _elements:     pointer to the buffer data.
  _count:        number of occupied elements.
  _element_size: size of each element in bytes.
Return:
  A boolean value corresponding to either:
  - true, if the reversal succeeded or was a no-op, or
  - false, if _elements is NULL.
*/
bool
d_buffer_common_reverse
(
    void*  _elements,
    size_t _count,
    size_t _element_size
)
{
    char   stack_buf[256];
    char*  tmp;
    size_t lo;
    size_t hi;
    void*  a;
    void*  b;

    // validate parameters
    if (!_elements)
    {
        return false;
    }

    // trivial case
    if ( (_element_size == 0) ||
         (_count < 2) )
    {
        return true;
    }

    // use stack for small elements, heap for large
    tmp = _element_size <= sizeof(stack_buf)
              ? stack_buf
              : (char*)malloc(_element_size);

    // check allocation
    if (!tmp)
    {
        return false;
    }

    // swap from ends toward centre
    lo = 0;
    hi = _count - 1;

    while (lo < hi)
    {
        a = d_buffer_common__element_at(_elements,
                                         lo,
                                         _element_size);
        b = d_buffer_common__element_at(_elements,
                                         hi,
                                         _element_size);
        d_memcpy(tmp, a,   _element_size);
        d_memcpy(a,   b,   _element_size);
        d_memcpy(b,   tmp, _element_size);
        ++lo;
        --hi;
    }

    // free heap buffer if used
    if (tmp != stack_buf)
    {
        free(tmp);
    }

    return true;
}


/*
d_buffer_common_sort
  Sorts the buffer elements using qsort.

Parameter(s):
  _elements:     pointer to the buffer data.
  _count:        number of occupied elements.
  _element_size: size of each element in bytes.
  _comparator:   comparison function.
Return:
  none.
*/
void
d_buffer_common_sort
(
    void*         _elements,
    size_t        _count,
    size_t        _element_size,
    fn_comparator _comparator
)
{
    // validate parameters
    if ( (!_elements)   ||
         (_count < 2)   ||
         (_element_size == 0) ||
         (!_comparator) )
    {
        return;
    }

    qsort(_elements, _count, _element_size, _comparator);

    return;
}


// =============================================================================
// XII.  validation
// =============================================================================

/*
d_buffer_common_validate_params
  Checks that _element_size is non-zero.

Parameter(s):
  _element_size: size of each element in bytes.
Return:
  A boolean value corresponding to either:
  - true, if _element_size > 0, or
  - false, otherwise.
*/
bool
d_buffer_common_validate_params
(
    size_t _element_size
)
{
    return _element_size > 0;
}


/*
d_buffer_common_validate_capacity
  Checks that _capacity is non-zero.

Parameter(s):
  _capacity: capacity to validate.
Return:
  A boolean value corresponding to either:
  - true, if _capacity > 0, or
  - false, otherwise.
*/
bool
d_buffer_common_validate_capacity
(
    size_t _capacity
)
{
    return _capacity > 0;
}


/*
d_buffer_common_validate_index
  Checks that _index is within bounds.

Parameter(s):
  _index: index to validate.
  _count: number of occupied elements.
Return:
  A boolean value corresponding to either:
  - true, if _index < _count, or
  - false, otherwise.
*/
bool
d_buffer_common_validate_index
(
    d_index _index,
    size_t  _count
)
{
    return (size_t)_index < _count;
}


// =============================================================================
// XIII. destruction
// =============================================================================

/*
d_buffer_common_free_data
  Frees the buffer data allocation.

Parameter(s):
  _elements: pointer to the buffer data; may be NULL.
Return:
  none.
*/
void
d_buffer_common_free_data
(
    void* _elements
)
{
    free(_elements);

    return;
}


/*
d_buffer_common_free_data_deep
  Frees each pointed-to element and then frees the pointer array.

Parameter(s):
  _count:    number of elements.
  _elements: array of pointers to free.
  _free_fn:  function to call on each non-NULL element.
Return:
  none.
*/
void
d_buffer_common_free_data_deep
(
    size_t  _count,
    void**  _elements,
    fn_free _free_fn
)
{
    size_t i;

    // validate parameters
    if ( (!_elements) ||
         (!_free_fn) )
    {
        return;
    }

    // free each element
    for (i = 0; i < _count; ++i)
    {
        if (_elements[i])
        {
            _free_fn(_elements[i]);
        }
    }

    free(_elements);

    return;
}
