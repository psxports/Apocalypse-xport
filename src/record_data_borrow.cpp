#include "record_data_borrow.h"
#include "record_source_internal.h"

RecordDataBorrow::RecordDataBorrow()
{
    ++rec_borrows;
}

RecordDataBorrow::~RecordDataBorrow()
{
    if (--rec_borrows)
        return;
    RecordSource *src = rec_srcs;
    while (src)
    {
        RecordSource *next = src->next;
        if (!src->references)
            DestroyRecSrc(src);
        src = next;
    }
}
