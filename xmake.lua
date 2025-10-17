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

rule("def")
    set_extensions(".def")
    on_prepare_file(function (target, file, opt)
        if target:kind() == "shared" then
            target:add("shflags", "/def:" .. file)
        elseif target:kind() == "static" then
            target:add("arflags", "/def:" .. file)
        else
            target:add("ldflags", "/def:" .. file)
        end
    end)
rule_end()

add_includedirs("external")
if is_arch("x64") then
    add_linkdirs("external/x64")
end

target("external")
    set_kind("phony")
    add_headerfiles("external/**")
target_end()

includes("dxfwd")
includes("ntconk")
includes("umtest")

