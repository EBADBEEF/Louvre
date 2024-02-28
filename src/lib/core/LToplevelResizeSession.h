#ifndef LTOPLEVELRESIZESESSION_H
#define LTOPLEVELRESIZESESSION_H

#include <LToplevelRole.h>

class Louvre::LToplevelResizeSession : public LObject
{
public:
    // TODO: add doc

    bool start(const LEvent &triggeringEvent,
               LToplevelRole::ResizeEdge edge,
               const LPoint &initDragPoint,
               const LSize &minSize = LSize(0, 0),
               Int32 L = LToplevelRole::EdgeDisabled, Int32 T = LToplevelRole::EdgeDisabled,
               Int32 R = LToplevelRole::EdgeDisabled, Int32 B = LToplevelRole::EdgeDisabled);

    void updateDragPoint(const LPoint &pos);

    const std::vector<LToplevelResizeSession*>::const_iterator stop();

    inline LToplevelRole *toplevel() const
    {
        return m_toplevel;
    }

    inline const LEvent &triggeringEvent() const
    {
        return *m_triggeringEvent.get();
    }

    inline bool isActive() const
    {
        return m_isActive;
    }

private:
    friend class LToplevelRole;
    LToplevelResizeSession();
    ~LToplevelResizeSession();
    void handleGeometryChange();
    LToplevelRole *m_toplevel;
    LPoint m_initPos;
    LSize m_initSize;
    LSize m_minSize;
    LPoint m_initDragPoint;
    LPoint m_currentDragPoint;
    LToplevelRole::ResizeEdge m_edge;
    LBox m_bounds;
    std::unique_ptr<LEvent> m_triggeringEvent;
    bool m_isActive { false };
};

#endif // LTOPLEVELRESIZESESSION_H