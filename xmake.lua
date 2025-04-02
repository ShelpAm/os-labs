add_rules("mode.debug", "mode.release")

set_languages("cxxlatest")
add_includedirs("./")
add_includedirs("./third-party/fast_io/include")
set_rundir("$(projectdir)/")
set_toolchains("llvm")
set_runtimes("c++_static")

add_requires("nlohmann_json")
add_requires("cpp-httplib")
add_requires("gtest")


target("fast-io-ext")
set_kind("headeronly")
add_headerfiles("./fast-io-ext/fixed.hpp", { public = true })

target("process")
set_kind("static")
add_files("./process/*.cpp")
add_deps("fast-io-ext")
add_packages("nlohmann_json", { public = true }) -- To allow inheritance

target("process.detail.std-optional-to-json.test")
set_kind("binary")
add_files("./process/detail/std-optional-to-json.test.cpp")
add_tests("default")
add_deps("process")

-- target("queue-cli")
-- set_kind("binary")
-- add_files("./queue-cli/main.cpp")
--
-- target("fcfs")
-- set_kind("binary")
-- add_files("./fcfs/main.cpp")
-- add_deps("fast-io-ext")
--
-- target("spf")
-- set_kind("binary")
-- add_files("./spf/main.cpp", "./spf/spf-queue.cpp")
-- add_deps("process")
--
-- target("round-robin")
-- set_kind("binary")
-- add_files("./round-robin/main.cpp")
-- add_deps("process")
--
-- target("priority-scheduling")
-- set_kind("binary")
-- add_files("./priority-scheduling/main.cpp")
-- add_deps("process")

-- target("fast_io.test")
-- set_kind("binary")
-- add_files("./tests/fast_io.test.cpp")
-- add_deps("fast-io-ext")


-- Above are lagacies. Below are new code.

target("algorithms")
set_kind("static")
add_files(
    "./algorithms/process-scheduling.cpp",
    "./algorithms/bankers-algo.cpp"
-- "./algorithms/memory/fixed-size-allocator.cpp"
)
add_deps("process", { public = true })

target("algorithms.bankers-algo.test")
set_kind("binary")
add_files("./algorithms/bankers-algo.test.cpp")
add_tests("default")
add_deps("algorithms")

target("algorithms.memory.fixed-size-allocator.test")
set_kind("binary")
add_files("./algorithms/memory/fixed-size-allocator.test.cpp")
add_tests("default")
add_deps("algorithms")
add_packages("gtest")


target("server-cli")
set_kind("binary")
add_files("./server-cli/main.cpp", "./spf/spf-queue.cpp")
add_packages("nlohmann_json")
add_packages("cpp-httplib")
add_deps("algorithms")




-- Tests

target("tests.test_fcfs.test")
set_kind("binary")
add_files("./tests/test_fcfs.cpp")
add_files("./spf/spf-queue.cpp")
add_tests("default")
add_deps("algorithms")
add_deps("process")
add_deps("algorithms")

target("tests.test_ps.test")
set_kind("binary")
add_files("./tests/test_ps.cpp")
add_files("./spf/spf-queue.cpp")
add_tests("default")
add_deps("algorithms")
add_deps("process")
add_deps("algorithms")

target("tests.test_spf.test")
set_kind("binary")
add_files("./tests/test_spf.cpp")
add_files("./spf/spf-queue.cpp")
add_tests("default")
add_deps("algorithms")
add_deps("process")
add_deps("algorithms")

target("tests.test_rr.test")
set_kind("binary")
add_files("./tests/test_rr.cpp")
add_files("./spf/spf-queue.cpp")
add_tests("default")
add_deps("algorithms")
add_deps("process")
add_deps("algorithms")

target("json.test")
set_kind("binary")
add_tests("default")
add_files("./tests/json.test.cpp")
add_packages("nlohmann_json")
