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
    public partial class MainMenuViewModel : DependencyObject
    {
        public static readonly DependencyProperty PosXProperty;
        public static readonly DependencyProperty PosYProperty;

        static MainMenuViewModel()
        {
            PosXProperty = DependencyProperty.Register("PosX", typeof(double), typeof(MainMenuViewModel));
            PosYProperty = DependencyProperty.Register("PosY", typeof(double), typeof(MainMenuViewModel));
        }

        public MainMenuViewModel()
        {
            PosX = 100.0f;
            PosY = 100.0f;
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        public double PosX
        {
            get { return (double)GetValue(PosXProperty); }
            set { SetValue(PosXProperty, value); }
        }
        public double PosY
        {
            get { return (double)GetValue(PosYProperty); }
            set { SetValue(PosYProperty, value); }
        }
    }
}
