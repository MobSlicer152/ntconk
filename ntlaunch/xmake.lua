target("ntlaunch")
    set_kind("binary")

    add_files("**.cpp")

    add_packages("phnt")

    set_group("Tools")
target_end()
