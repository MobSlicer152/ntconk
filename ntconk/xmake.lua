target("ntconk")
    add_rules("wdk.driver", "wdk.env.wdm")

    add_headerfiles("**.h")
    add_files("**.cpp", "*.rc")

    add_packages("phnt")
target_end()
