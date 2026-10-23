#ifndef RESOURCE_SCRIPT_BORROW_H
#define RESOURCE_SCRIPT_BORROW_H

#include "psx.h"
#include "record_data_borrow.h"

struct ResourceScript;

class ResourceScriptBorrow
{
private:
    friend void ClearResourceScripts();
    static ResourceScript *retired_scripts;
    static uint32 script_borrows;

    RecordDataBorrow data_borrow;
    ResourceScriptBorrow &operator=(const ResourceScriptBorrow &);

public:
    ResourceScriptBorrow();
    ~ResourceScriptBorrow();

private:
    static void DestroyResourceScript(ResourceScript *rec);
    ResourceScriptBorrow(const ResourceScriptBorrow &);
};

#endif
