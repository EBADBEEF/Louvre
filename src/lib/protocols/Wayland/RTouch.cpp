#include <protocols/Wayland/private/RTouchPrivate.h>
#include <protocols/Wayland/RSurface.h>
#include <protocols/Wayland/GSeat.h>
#include <private/LClientPrivate.h>
#include <algorithm>

static struct wl_touch_interface touch_implementation =
{
#if LOUVRE_WL_SEAT_VERSION >= 3
        .release = &RTouch::RTouchPrivate::release
#endif
};

RTouch::RTouch
    (
        GSeat *gSeat,
        Int32 id
    )
    :LResource
    (
        gSeat->client(),
        &wl_touch_interface,
        gSeat->version(),
        id,
        &touch_implementation
    ),
    LPRIVATE_INIT_UNIQUE(RTouch)
{
    imp()->gSeat = gSeat;
    gSeat->m_touchRes.emplace_back(this);
}

RTouch::~RTouch()
{
    if (seatGlobal())
        LVectorRemoveOneUnordered(seatGlobal()->m_touchRes, this);
}

GSeat *RTouch::seatGlobal() const
{
    return imp()->gSeat;
}

bool RTouch::down(const LTouchDownEvent &event, RSurface *rSurface)
{
    auto &clientEvents = client()->imp()->events.touch.down;

    auto it = std::find_if(
        clientEvents.begin(),
        clientEvents.end(),
        [event](const LTouchDownEvent &ev)
    {
        return ev.id() == event.id();
    });

    if (it == clientEvents.end())
        clientEvents.push_back(event);
    else if (it->serial() != event.serial())
        *it = event;

    wl_touch_send_down(resource(),
                       event.serial(),
                       event.ms(),
                       rSurface->resource(),
                       event.id(),
                       wl_fixed_from_double(event.localPos.x()),
                       wl_fixed_from_double(event.localPos.y()));
    return true;
}

bool RTouch::up(const LTouchUpEvent &event)
{
    auto &clientEvents = client()->imp()->events.touch.up;

    auto it = std::find_if(
        clientEvents.begin(),
        clientEvents.end(),
        [event](const LTouchUpEvent &ev)
        {
            return ev.id() == event.id();
        });

    if (it == clientEvents.end())
        clientEvents.push_back(event);
    else if (it->serial() != event.serial())
        *it = event;

    wl_touch_send_up(resource(), event.serial(), event.ms(), event.id());
    return true;
}

bool RTouch::motion(UInt32 time, Int32 id, Float24 x, Float24 y)
{
    wl_touch_send_motion(resource(), time, id, x, y);
    return true;
}

bool RTouch::frame()
{
    wl_touch_send_frame(resource());
    return true;
}

bool RTouch::cancel()
{
    wl_touch_send_cancel(resource());
    return true;
}

bool RTouch::shape(Int32 id, Float24 major, Float24 minor)
{
#if LOUVRE_WL_SEAT_VERSION >= 6
    if (version() >= 6)
    {
        wl_touch_send_shape(resource(), id, major, minor);
        return true;
    }
#endif
    L_UNUSED(id);
    L_UNUSED(major);
    L_UNUSED(minor);
    return false;
}

bool RTouch::orientation(Int32 id, Float24 orientation)
{
#if LOUVRE_WL_SEAT_VERSION >= 6
    if (version() >= 6)
    {
        wl_touch_send_orientation(resource(), id, orientation);
        return true;
    }
#endif
    L_UNUSED(id);
    L_UNUSED(orientation);
    return false;
}
