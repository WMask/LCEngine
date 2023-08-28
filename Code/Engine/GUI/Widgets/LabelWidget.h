/**
* LabelWidget.h
* 28.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "../Module.h"
#include "World/Widgets.h"


/**
* Label widget */
class LcLabelWidget : public LcWidgetBase
{
public:
    LcLabelWidget(LcWidgetData inWidget, const std::wstring& inText, const std::string& inFontName, unsigned short inFontSize) :
        LcWidgetBase(inWidget), text(inText), fontName(inFontName), fontSize(inFontSize)
    {
        RebuildText();
    }
    //
    ~LcLabelWidget() {}
    //
    virtual void RebuildText() {}
    //
    void SetText(const std::wstring& inText)
    {
        text = inText;
        RebuildText();
    }
    //
    void SetFontSize(unsigned short inFontSize)
    {
        fontSize = inFontSize;
        RebuildText();
    }
    //
    const std::wstring& GetText() const { return text; }
    //
    const std::string& GetFontName() const { return fontName; }
    //
    unsigned short GetFontSize() const { return fontSize; }


protected:
    std::wstring text;
    //
    std::string fontName;
    //
    unsigned short fontSize;

};
