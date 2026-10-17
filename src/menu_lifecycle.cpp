#include "orbit_camera.h"
extern "C"
{
#include "draft_first_adapters.h"
}
#include "menu_lifecycle.h"
#include "menu_window.h"
#include "camera.h"

// Native extraction of the existing adjusted menu destructor boundary
void DestroyMenuObject(uint32 object)
{
    uint32 table = r_u32(object);
    uint32 target = r_u32(table + 12);
    uint32 adjusted = object + (uint32)(sint32)(sint16)r_u16(table + 8);
    if (target == 0x80011990)
        OrbitCamera::Attach(adjusted).OrbitCamera::Destroy(3);
    else if (target == 0x800152C4)
        MenuWindow::Attach(adjusted).MenuWindow::Destroy(3);
    else
    {
        // TODO Bind any other original menu class destructor
        xport_draft_guest_call2(target, adjusted, 3);
    }
}

extern "C" void apocalypse_menu_destroy(uint32 object)
{
    DestroyMenuObject(object);
}
