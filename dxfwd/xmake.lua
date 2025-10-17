target("dxfwd")
    set_kind("shared")

    add_files("dummy.cpp", "dxfwd.def")

    add_rules("def")
    add_links("dxgkrnl")
    add_shflags("/noentry", "/subsystem:native")
target_end()
