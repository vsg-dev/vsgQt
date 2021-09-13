#pragma once

#include <QKeyEvent>

#include <vsg/ui/KeyEvent.h>

namespace vsgQt
{

    class KeyboardMap : public vsg::Inherit<vsg::Object, KeyboardMap>
    {
    public:
        KeyboardMap();

        bool getKeySymbol(const QKeyEvent* e, vsg::KeySymbol& keySymbol, vsg::KeySymbol& modifiedKeySymbol, vsg::KeyModifier& keyModifier);

    protected:
        using VirtualKeyToKeySymbolMap = std::map<uint32_t, vsg::KeySymbol>;
        VirtualKeyToKeySymbolMap _keycodeMap;
    };

} // namespace vsgQt
