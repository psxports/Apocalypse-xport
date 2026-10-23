#include <stdlib.h>

#include "resource_script_borrow.h"
#include "resource_script.h"

ResourceScript *ResourceScriptBorrow::retired_scripts;
uint32 ResourceScriptBorrow::script_borrows;

void ResourceScriptBorrow::DestroyResourceScript(ResourceScript *rec)
{
    rec->~ResourceScript();
    free(rec);
}

ResourceScriptBorrow::ResourceScriptBorrow()
{
    ++script_borrows;
}

ResourceScriptBorrow::~ResourceScriptBorrow()
{
    if (--script_borrows)
        return;
    while (retired_scripts)
    {
        ResourceScript *rec = retired_scripts;
        retired_scripts = rec->retired_next;
        DestroyResourceScript(rec);
    }
}
