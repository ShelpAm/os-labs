add_rules("mode.debug", "mode.release")

set_languages("cxxlatest")
add_includedirs("./")
add_includedirs("./third-party/fast_io/include")

target("fast-io-ext")
set_kind("headeronly")
add_headerfiles("./fast-io-ext/fixed.hpp")

target("process")
set_kind("static")
add_files("./process/*.cpp")
add_deps("fast-io-ext")

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

-- target("priority-scheduling")
-- set_kind("binary")
-- add_files("./priority-scheduling/main.cpp")
-- add_deps("process")

target("fast_io.test")
set_kind("binary")
add_files("./tests/fast_io.test.cpp")
add_deps("fast-io-ext")

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--
