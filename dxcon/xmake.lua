target("dxcon")
    set_kind("binary")

    add_headerfiles("**.h")
    add_files("**.cpp")

    add_deps("dxfwdu")

    set_values("windows.subsystem", "native")
    add_links("ntdllp", "libcntpr", "nt", "BufferOverflow", "win32u")
    add_ldflags("/nodefaultlib")
    
    on_load(function (target)
        local wdk = import("detect.sdks.find_wdk").main()
        local dir = path.join(wdk.libdir, wdk.sdkver, "km", "x64")
        target:add("linkdirs", dir)
    end)
    
    add_packages("phnt")
target_end()
