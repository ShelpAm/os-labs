add_rules("mode.debug", "mode.release")

set_languages("cxxlatest")
add_includedirs("./")
add_includedirs("./third-party/fast_io/include")
set_rundir("$(projectdir)/")
set_toolchains("llvm")
set_runtimes("c++_static")

add_requires("nlohmann_json")
add_requires("cpp-httplib")


target("fast-io-ext")
set_kind("headeronly")
add_headerfiles("./fast-io-ext/fixed.hpp", { public = true })

target("process")
set_kind("shared")
add_files("./process/*.cpp")
add_deps("fast-io-ext")
add_packages("nlohmann_json", { public = true }) -- To allow inheritance

target("process.detail.std-optional-to-json.test")
set_kind("binary")
add_files("./process/detail/std-optional-to-json.test.cpp")
add_tests("default")
add_deps("process")

target("queue-cli")
set_kind("binary")
add_files("./queue-cli/main.cpp")

target("fcfs")
set_kind("binary")
add_files("./fcfs/main.cpp")
add_deps("fast-io-ext")

target("spf")
set_kind("binary")
add_files("./spf/main.cpp", "./spf/spf-queue.cpp")
add_deps("process")

target("round-robin")
set_kind("binary")
add_files("./round-robin/main.cpp")
add_deps("process")

target("priority-scheduling")
set_kind("binary")
add_files("./priority-scheduling/main.cpp")
add_deps("process")

-- target("fast_io.test")
-- set_kind("binary")
-- add_files("./tests/fast_io.test.cpp")
-- add_deps("fast-io-ext")

target("server")
set_kind("binary")
add_files("./server/main.cpp", "./spf/spf-queue.cpp")
add_packages("nlohmann_json")
add_packages("cpp-httplib")
add_deps("process")

target("tests.test_process.test")
set_kind("binary")
add_files("./tests/test_process.cpp")
add_tests("default")
add_deps("process")