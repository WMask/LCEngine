using Microsoft.Xaml.Behaviors;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace common
{
    public partial class ClickMoveBehavior : Behavior<FrameworkElement>
    {
        public static readonly DependencyProperty MoveTargetProperty;

        static ClickMoveBehavior()
        {
            MoveTargetProperty = DependencyProperty.Register("MoveTarget", typeof(FrameworkElement), typeof(ClickMoveBehavior));
        }

        protected override void OnAttached()
        {
            base.OnAttached();
            AssociatedObject.MouseDown += AssociatedObject_MouseDown;
        }

        protected override void OnDetaching()
        {
            base.OnDetaching();
            AssociatedObject.MouseDown -= AssociatedObject_MouseDown;
        }

        private void AssociatedObject_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (MoveTarget != null)
            {
                MoveTarget.SetValue(Canvas.LeftProperty, e.GetPosition(AssociatedObject).X);
                MoveTarget.SetValue(Canvas.TopProperty, e.GetPosition(AssociatedObject).Y);
            }
        }

        public FrameworkElement MoveTarget
        {
            get { return (FrameworkElement)GetValue(MoveTargetProperty); }
            set { SetValue(MoveTargetProperty, value); }
        }
    }
}
