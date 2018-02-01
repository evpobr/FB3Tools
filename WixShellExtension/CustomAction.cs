using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Deployment.WindowsInstaller;

namespace WixShellExtension
{
    public class CustomActions
    {
        [CustomAction]
        public static ActionResult NotifyShell(Session session)
        {
            session.Log("Begin NotifyShell");

            return ActionResult.Success;
        }
    }
}
