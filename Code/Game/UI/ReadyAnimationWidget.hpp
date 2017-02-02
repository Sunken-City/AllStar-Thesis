#pragma once
#include "Engine/UI/WidgetBase.hpp"

class ReadyAnimationWidget : public WidgetBase
{
public:
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    ReadyAnimationWidget(const char* gamemodeName);
    virtual ~ReadyAnimationWidget();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void BuildFromXMLNode(XMLNode& node) override;
    virtual void RecalculateBounds() override;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    const char* m_titleName;
    const char* m_getReady = "Get Ready!";
};