add_rules("plugin.vsxmake.autoupdate", "mode.debug", "mode.release")

set_policy("check.auto_ignore_flags", false)

set_project("ntconk")

set_allowedplats("windows")
set_allowedmodes("debug", "release")
set_allowedarchs("x64")

set_languages("clatest", "cxxlatest")
set_warnings("all")
set_exceptions("none")

add_requires("phnt")

add_includedirs("external")
target("external")
    set_kind("phony")
    add_headerfiles("external/**")
target_end()

includes("ntconk")

