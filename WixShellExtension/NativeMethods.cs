using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WixShellExtension
{
    internal enum SHCNE: uint
    {
        SHCNE_ASSOCCHANGED = 0x08000000
    }

    internal static class NativeMethods
    {
        [DllImport("shell32.dll", CharSet = CharSet.Auto)]
        internal static extern void SHChangeNotify(SHCNE wEventId, uint uFlags, IntPtr dwItem1, IntPtr dwItem2);
    }
}
