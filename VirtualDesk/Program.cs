using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using WindowsDesktop;

namespace VirtualDesk
{
    class Program
    {
        static void Main(string[] args)
        {
			Console.WriteLine("创建一个桌面:");
			VirtualDesktop.Create();
			VirtualDesktop[] desks = VirtualDesktop.GetDesktops();
			for (int i = 0; i < desks.Length; i++)
			{
				VirtualDesktop desk = desks[i];
				Console.WriteLine(desk.Id);
			}
			Console.WriteLine("当前桌面：" + VirtualDesktop.Current.Id);
			Console.WriteLine("移动到第2个桌面");
			desks[1].Switch();
			Console.WriteLine("当前桌面：" + VirtualDesktop.Current.Id);
			// 然后执行程序会在切换之后的桌面
			//Console.WriteLine("销毁第二个桌面:");
			//desks[1].Remove();
		}
    }
}
