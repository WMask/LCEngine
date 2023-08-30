/**
* LabelWidget.h
* 28.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "../Module.h"
#include "World/Widgets.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)


/**
* Label widget */
class GUI_API LcLabelWidget : public LcWidgetBase
{
public:
    LcLabelWidget(const std::wstring& inText, const std::wstring& inFontName, unsigned short inFontSize,
        LcColor4 inTextColor, LcVector3 inPos, LcSizef inSize, LcFontWeight inFontWeight = LcFontWeight::Normal) :
        LcWidgetBase(LcWidgetData("LcLabelWidget", inPos)), text(inText),
        fontName(inFontName), fontSize(inFontSize), fontWeight(inFontWeight), textColor(LcDefaults::White4), font(nullptr)
    {
        size = inSize;
    }
    //
    ~LcLabelWidget() {}
    //
    void SetText(const std::wstring& inText) { text = inText; }
    //
    void SetTextColor(LcColor4 inTextColor) { textColor = inTextColor; }
    //
    const std::wstring& GetText() const { return text; }
    //
    const std::wstring& GetFontName() const { return fontName; }
    //
    unsigned short GetFontSize() const { return fontSize; }
    //
    LcFontWeight GetFontWeight() const { return fontWeight; }


public:// IWidget interface implementation
    //
    virtual void Init(class IWidgetRender& render) override;
    //
    virtual void Render(class IWidgetRender& render) const override;


protected:
    std::wstring text;
    //
    LcColor4 textColor;
    //
    std::wstring fontName;
    //
    unsigned short fontSize;
    //
    LcFontWeight fontWeight;
    //
    const ITextFont* font;

};
