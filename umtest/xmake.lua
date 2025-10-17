target("umtest")
    set_kind("binary")

    add_files("**.cpp")

    add_links("gdi32")
    add_packages("phnt")
target_end()
