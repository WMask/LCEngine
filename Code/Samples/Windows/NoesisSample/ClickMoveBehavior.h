/**
* ClickMoveBehavior.h
* 31.01.2023
* (c) Denis Romakhov
*/

#include <NsCore/Noesis.h>
#include <NsCore/TypeOf.h>
#include <NsCore/ReflectionImplement.h>
#include <NsCore/ReflectionDeclare.h>
#include <NsGui/FrameworkElement.h>
#include <NsApp/Behavior.h>


/**
* ClickMove behavior */
class ClickMoveBehavior final : public NoesisApp::BehaviorT<Noesis::FrameworkElement>
{
public:
    ClickMoveBehavior() {}
    ~ClickMoveBehavior() {}


protected:
    virtual void OnAttached() override;
    virtual void OnDetaching() override;
    virtual Noesis::Ptr<Noesis::Freezable> CreateInstanceCore() const;
    void OnMouseLeftButtonDown(BaseComponent*, const Noesis::MouseButtonEventArgs&);


private:
    NS_DECLARE_REFLECTION(ClickMoveBehavior, NoesisApp::BehaviorT<Noesis::FrameworkElement>);

};
