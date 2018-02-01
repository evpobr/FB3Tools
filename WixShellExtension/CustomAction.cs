using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Deployment.WindowsInstaller;

namespace WixShellExtension
{
    public class CustomActions
    {
        [CustomAction]
        public static ActionResult NotifyShellAssociationChanged(Session session)
        {
            try
            {
                session.Log("Begin NotifyShell");
                NativeMethods.SHChangeNotify(SHCNE.SHCNE_ASSOCCHANGED, 0, IntPtr.Zero, IntPtr.Zero);
            }
            catch (Exception ex)
            {
                session.Log("ERROR in custom action ConfigureEwsFilter {0}", ex.ToString());
                return ActionResult.Failure;
            }

            return ActionResult.Success;
        }
    }
}
