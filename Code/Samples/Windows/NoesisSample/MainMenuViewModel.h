/**
* MainMenuViewModel.cpp
* 31.01.2023
* (c) Denis Romakhov
*/

#include <NsCore/Noesis.h>
#include <NsCore/TypeOf.h>
#include <NsCore/ReflectionImplement.h>
#include <NsCore/ReflectionDeclare.h>
#include <NsGui/DependencyObject.h>
#include <NsGui/FrameworkElement.h>


/**
* MainMenu view model */
class MainMenuViewModel final : public Noesis::DependencyObject
{
public:
    MainMenuViewModel();
    ~MainMenuViewModel();

    void SetPosX(float posX);
    void SetPosY(float posY);
    float GetPosX() const;
    float GetPosY() const;


public:
    static const Noesis::DependencyProperty* PosXProperty;
    static const Noesis::DependencyProperty* PosYProperty;
    
    
protected:
    bool OnPropertyChanged(const Noesis::DependencyPropertyChangedEventArgs& e);


private:
    NS_DECLARE_REFLECTION(MainMenuViewModel, DependencyObject);

};
