// dllmain.h : Declaration of module class.

class FB3ShellModule : public ATL::CAtlDllModuleT< FB3ShellModule >
{
public:
    DECLARE_LIBID(LIBID_FB3Shell)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_FB3SHELL, "{D8BC01C5-7E0A-4C68-9DDB-7AF4F41E4F74}")
};

extern class FB3ShellModule _AtlModule;
