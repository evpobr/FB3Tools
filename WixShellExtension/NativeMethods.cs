using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WixShellExtension
{
    internal enum SHCNE: uint
    {
        SHCNE_ASSOCCHANGED,
        SHCNE_ATTRIBUTES,
        SHCNE_CREATE,
        SHCNE_DELETE,
        SHCNE_DRIVEADD,
        SHCNE_DRIVEADDGUI,
        SHCNE_DRIVEREMOVED,
        SHCNE_EXTENDED_EVENT,
        SHCNE_FREESPACE,

    }

    internal static class NativeMethods
    {
        [DllImport("shell32.dll", CharSet = CharSet.Auto)]
        internal static void SHChangeNotify(uint wEventId, uint uFlags, IntPtr dwItem1, IntPtr dwItem2);
    }
}
