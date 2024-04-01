#include <protocols/RelativePointer/GRelativePointerManager.h>
#include <protocols/FractionalScale/GFractionalScaleManager.h>
#include <protocols/PointerConstraints/GPointerConstraints.h>
#include <protocols/TearingControl/GTearingControlManager.h>
#include <protocols/XdgDecoration/GXdgDecorationManager.h>
#include <protocols/GammaControl/GGammaControlManager.h>
#include <protocols/PointerGestures/GPointerGestures.h>
#include <protocols/SessionLock/GSessionLockManager.h>
#include <protocols/PresentationTime/GPresentation.h>
#include <protocols/Wayland/GDataDeviceManager.h>
#include <protocols/LinuxDMABuf/GLinuxDMABuf.h>
#include <protocols/Viewporter/GViewporter.h>
#include <protocols/Wayland/GSubcompositor.h>
#include <protocols/XdgShell/GXdgWmBase.h>
#include <protocols/Wayland/GCompositor.h>
#include <protocols/Wayland/GSeat.h>
#include <LSessionLockManager.h>
#include <LSessionLockRole.h>
#include <LCompositor.h>
#include <LToplevelRole.h>
#include <LCursor.h>
#include <LSubsurfaceRole.h>
#include <LPointer.h>
#include <LKeyboard.h>
#include <LTouch.h>
#include <LSurface.h>
#include <LDND.h>
#include <LClipboard.h>
#include <LOutput.h>
#include <LSeat.h>
#include <LPopupRole.h>
#include <LCursorRole.h>
#include <LLog.h>
#include <LXCursor.h>
#include <LClient.h>
#include <LDNDIconRole.h>
#include <cstring>

using namespace Louvre;
using namespace Louvre::Protocols;

//! [createGlobalsRequest]
bool LCompositor::createGlobalsRequest()
{
    // Allow clients to create surfaces and regions
    createGlobal<Wayland::GCompositor>();

    // Allow clients to receive pointer, keyboard, and touch events
    createGlobal<Wayland::GSeat>();

    // Provides detailed information of pointer movement
    createGlobal<RelativePointer::GRelativePointerManager>();

    // Allow clients to request setting pointer constraints
    createGlobal<PointerConstraints::GPointerConstraints>();

    // Allow clients to receive swipe, pinch, and hold pointer gestures
    createGlobal<PointerGestures::GPointerGestures>();

    // Enable drag & drop and clipboard data sharing between clients
    createGlobal<Wayland::GDataDeviceManager>();

    // Allow clients to create subsurface roles
    createGlobal<Wayland::GSubcompositor>();

    // Allow clients to create toplevel and popup roles
    createGlobal<XdgShell::GXdgWmBase>();

    // Allow negotiation of server-side or client-side decorations
    createGlobal<XdgDecoration::GXdgDecorationManager>();

    // Allow clients to adjust their surfaces buffers to fractional scales
    createGlobal<FractionalScale::GFractionalScaleManager>();

    // Allow clients to request setting the gamma LUT of outputs
    createGlobal<GammaControl::GGammaControlManager>();

    // Allow clients to create DMA buffers
    createGlobal<LinuxDMABuf::GLinuxDMABuf>();

    // Provides detailed information of how the surfaces are presented
    createGlobal<PresentationTime::GPresentation>();

    // Allows clients to request locking the user session with arbitrary graphics
    createGlobal<SessionLock::GSessionLockManager>();

    // Allows clients to notify their preference of vsync for specific surfaces
    createGlobal<TearingControl::GTearingControlManager>();

    // Allow clients to clip and scale buffers
    createGlobal<Viewporter::GViewporter>();

    return true;
}
//! [createGlobalsRequest]

//! [initialized]
void LCompositor::initialized()
{
    // Set a keyboard map with "latam" layout
    seat()->keyboard()->setKeymap(nullptr, nullptr, "latam", nullptr);

    Int32 totalWidth = 0;

    // Initialize and arrange outputs from left to right
    for (LOutput *output : seat()->outputs())
    {
        // Set scale 2 to outputs with DPI >= 200
        output->setScale(output->dpi() >= 200 ? 2.f : 1.f);
        output->setTransform(LFramebuffer::Normal);

        output->setPos(LPoint(totalWidth, 0));
        totalWidth += output->size().w();
        addOutput(output);
        output->repaint();
    }
}
//! [initialized]

//! [uninitialized]
void LCompositor::uninitialized()
{
    /* No default implementation */
}
//! [uninitialized]

//! [cursorInitialized]
void LCompositor::cursorInitialized()
{
    // Example to load an XCursor

    /*
    // Loads the "hand1" cursor
    LXCursor *handCursor = LXCursor::loadXCursorB("hand1");

    // Returns nullptr if not found
    if (handCursor)
    {
        // Set as the cursor texture
        cursor()->setTextureB(handCursor->texture(), handCursor->hotspotB());
    }
    */
}
//! [cursorInitialized]

//! [createOutputRequest]
LOutput *LCompositor::createOutputRequest(const void *params)
{
    return new LOutput(params);
}
//! [createOutputRequest]

//! [createClientRequest]
LClient *LCompositor::createClientRequest(const void *params)
{
    return new LClient(params);
}
//! [createClientRequest]

//! [createSurfaceRequest]
LSurface *LCompositor::createSurfaceRequest(const void *params)
{
    return new LSurface(params);
}
//! [createSurfaceRequest]

//! [createSeatRequest]
LSeat *LCompositor::createSeatRequest(const void *params)
{
    return new LSeat(params);
}
//! [createSeatRequest]

//! [createPointerRequest]
LPointer *LCompositor::createPointerRequest(const void *params)
{
    return new LPointer(params);
}
//! [createPointerRequest]

//! [createKeyboardRequest]
LKeyboard *LCompositor::createKeyboardRequest(const void *params)
{
    return new LKeyboard(params);
}
//! [createKeyboardRequest]

//! [createTouchRequest]
LTouch *LCompositor::createTouchRequest(const void *params)
{
    return new LTouch(params);
}
//! [createTouchRequest]

//! [createDNDRequest]
LDND *LCompositor::createDNDRequest(const void *params)
{
    return new LDND(params);
}
//! [createDNDRequest]

//! [createClipboardRequest]
LClipboard *LCompositor::createClipboardRequest(const void *params)
{
    return new LClipboard(params);
}
//! [createClipboardRequest]

//! [createSessionLockManagerRequest]
LSessionLockManager *LCompositor::createSessionLockManagerRequest(const void *params)
{
    return new LSessionLockManager(params);
}
//! [createSessionLockManagerRequest]

//! [createToplevelRoleRequest]
LToplevelRole *LCompositor::createToplevelRoleRequest(const void *params)
{
    return new LToplevelRole(params);
}
//! [createToplevelRoleRequest]

//! [createPopupRoleRequest]
LPopupRole *LCompositor::createPopupRoleRequest(const void *params)
{
    return new LPopupRole(params);
}
//! [createPopupRoleRequest]

//! [createSubsurfaceRoleRequest]
LSubsurfaceRole *LCompositor::createSubsurfaceRoleRequest(const void *params)
{
    return new LSubsurfaceRole(params);
}
//! [createSubsurfaceRoleRequest]

//! [createCursorRoleRequest]
LCursorRole *LCompositor::createCursorRoleRequest(const void *params)
{
    return new LCursorRole(params);
}
//! [createCursorRoleRequest]

//! [createDNDIconRoleRequest]
LDNDIconRole *LCompositor::createDNDIconRoleRequest(const void *params)
{
    return new LDNDIconRole(params);
}

//! [createDNDIconRoleRequest]

//! [createSessionLockRoleRequest]
LSessionLockRole *LCompositor::createSessionLockRoleRequest(const void *params)
{
    return new LSessionLockRole(params);
}
//! [createSessionLockRoleRequest]

//! [destroyOutputRequest]
void LCompositor::destroyOutputRequest(LOutput *output)
{
    L_UNUSED(output);
}
//! [destroyOutputRequest]

//! [destroyClientRequest]
void LCompositor::destroyClientRequest(LClient *client)
{
    L_UNUSED(client);
}
//! [destroyClientRequest]

//! [destroySurfaceRequest]
void LCompositor::destroySurfaceRequest(LSurface *surface)
{
    L_UNUSED(surface);
}
//! [destroySurfaceRequest]

//! [destroySeatRequest]
void LCompositor::destroySeatRequest(LSeat *seat)
{
    L_UNUSED(seat);
}
//! [destroySeatRequest]

//! [destroyPointerRequest]
void LCompositor::destroyPointerRequest(LPointer *pointer)
{
    L_UNUSED(pointer);
}
//! [destroyPointerRequest]

//! [destroyTouchRequest]
void LCompositor::destroyTouchRequest(LTouch *touch)
{
    L_UNUSED(touch);
}
//! [destroyTouchRequest]

//! [destroyKeyboardRequest]
void LCompositor::destroyKeyboardRequest(LKeyboard *keyboard)
{
    L_UNUSED(keyboard);
}
//! [destroyKeyboardRequest]

//! [destroyDNDRequest]
void LCompositor::destroyDNDRequest(LDND *dnd)
{
    L_UNUSED(dnd);
}
//! [destroyDNDRequest]

//! [destroyClipboardRequest]
void LCompositor::destroyClipboardRequest(LClipboard *clipboard)
{
    L_UNUSED(clipboard);
}
//! [destroyClipboardRequest]

//! [destroySessionLockManagerRequest]
void LCompositor::destroySessionLockManagerRequest(LSessionLockManager *sessionLockManager)
{
    L_UNUSED(sessionLockManager);
}
//! [destroySessionLockManagerRequest]

//! [destroyToplevelRoleRequest]
void LCompositor::destroyToplevelRoleRequest(LToplevelRole *toplevel)
{
    L_UNUSED(toplevel);
}
//! [destroyToplevelRoleRequest]

//! [destroyPopupRoleRequest]
void LCompositor::destroyPopupRoleRequest(LPopupRole *popup)
{
    L_UNUSED(popup);
}
//! [destroyPopupRoleRequest]

//! [destroySubsurfaceRoleRequest]
void LCompositor::destroySubsurfaceRoleRequest(LSubsurfaceRole *subsurface)
{
    L_UNUSED(subsurface);
}
//! [destroySubsurfaceRoleRequest]

//! [destroyCursorRoleRequest]
void LCompositor::destroyCursorRoleRequest(LCursorRole *cursorRole)
{
    L_UNUSED(cursorRole);
}
//! [destroyCursorRoleRequest]

//! [destroyDNDIconRoleRequest]
void LCompositor::destroyDNDIconRoleRequest(LDNDIconRole *icon)
{
    L_UNUSED(icon)
}
//! [destroyDNDIconRoleRequest]

//! [destroySessionLockRoleRequest]
void LCompositor::destroySessionLockRoleRequest(LSessionLockRole *sessionLockRole)
{
    L_UNUSED(sessionLockRole);
}
//! [destroySessionLockRoleRequest]

