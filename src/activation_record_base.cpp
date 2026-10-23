#include "visual_object.h"
#include "activation_record.h"
#include "activation_record_base.h"
#include "record_data.h"
#include "record_factory.h"

uint32 ActivationRecordBase::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8006FDC8, "SLUS_003.73");
    if (reason & 1)
        ReleaseStorage();
    return 0;
}
