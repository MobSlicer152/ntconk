target("dxfwdu")
    set_kind("shared")

    add_files("dummy.cpp", "dxfwdu.def")

    add_rules("def")
    add_links("win32u")
    add_shflags("/noentry", "/subsystem:native")
target_end()

target("dxfwdk")
    set_kind("shared")

    add_files("dummy.cpp", "dxfwdk.def")

    add_rules("def")
    add_links("dxgkrnl")
    add_shflags("/noentry", "/subsystem:native")
target_end()
