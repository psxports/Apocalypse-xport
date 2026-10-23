#ifndef RECORD_DATA_BORROW_H
#define RECORD_DATA_BORROW_H

class RecordDataBorrow
{
private:
    RecordDataBorrow &operator=(const RecordDataBorrow &);

public:
    RecordDataBorrow();
    ~RecordDataBorrow();

private:
    RecordDataBorrow(const RecordDataBorrow &);
};

#endif
