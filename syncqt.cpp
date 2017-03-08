#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <primitives/filesystem.h>

#include <iostream>
#include <regex>

namespace po = boost::program_options;

struct File
{
    bool public_header = true;
    bool qpa_header = true;
    bool no_master_include = false;
    bool clean = true;
    String requires;
};

const std::map<String, path> module_dirs {
    { "QtGui", "src/gui" },
    { "QtWidgets", "src/widgets" },
    { "QtPrintSupport", "src/printsupport" },
    { "QtOpenGL", "src/opengl" },
    { "QtCore", "src/corelib" },
    { "QtXml", "src/xml" },
    { "QtSql", "src/sql" },
    { "QtNetwork", "src/network" },
    { "QtTest", "src/testlib" },
    { "QtDBus", "src/dbus" },
    { "QtConcurrent", "src/concurrent" },
    { "QtAccessibilitySupport", "src/platformsupport/accessibility" },
    { "QtLinuxAccessibilitySupport", "src/platformsupport/linuxaccessibility" },
    { "QtClipboardSupport", "src/platformsupport/clipboard" },
    { "QtDeviceDiscoverySupport", "src/platformsupport/devicediscovery" },
    { "QtEventDispatcherSupport", "src/platformsupport/eventdispatchers" },
    { "QtFontDatabaseSupport", "src/platformsupport/fontdatabases" },
    { "QtInputSupport", "src/platformsupport/input" },
    { "QtPlatformCompositorSupport", "src/platformsupport/platformcompositor" },
    { "QtServiceSupport", "src/platformsupport/services" },
    { "QtThemeSupport", "src/platformsupport/themes" },
    { "QtGraphicsSupport", "src/platformsupport/graphics" },
    { "QtCglSupport", "src/platformsupport/cglconvenience" },
    { "QtEglSupport", "src/platformsupport/eglconvenience" },
    { "QtFbSupport", "src/platformsupport/fbconvenience" },
    { "QtGlxSupport", "src/platformsupport/glxconvenience" },
    { "QtKmsSupport", "src/platformsupport/kmsconvenience" },
    { "QtPlatformHeaders", "src/platformheaders" },
    //"QtANGLE/KHR", "!src/3rdparty/angle/include/KHR",
    //"QtANGLE/GLES2", "!src/3rdparty/angle/include/GLES2",
    //"QtANGLE/GLES3", "!src/3rdparty/angle/include/GLES3",
    //"QtANGLE/EGL", "!src/3rdparty/angle/include/EGL",
    //"QtZlib", "!>src/corelib;src/3rdparty/zlib",
    //"QtOpenGLExtensions", "src/openglextensions",
    //"QtEglFSDeviceIntegration", "src/plugins/platforms/eglfs",
};

const StringMap moduleheaders {
    { "QtEglFSDeviceIntegration", "api" },
};

const std::map<String, StringSet> classnames {
   { "qglobal.h", {"QtGlobal"} },
   { "qendian.h", {"QtEndian"} },
   { "qconfig.h", {"QtConfig"} },
   { "qplugin.h", {"QtPlugin"} },
   { "qalgorithms.h", {"QtAlgorithms"} },
   { "qcontainerfwd.h", {"QtContainerFwd"} },
   { "qdebug.h", {"QtDebug"} },
   { "qevent.h", {"QtEvents"} },
   { "qnamespace.h", {"Qt"} },
   { "qnumeric.h", {"QtNumeric"} },
   { "qvariant.h", {"QVariantHash","QVariantList","QVariantMap"} },
   { "qgl.h", {"QGL"} },
   { "qsql.h", {"QSql"} },
   { "qssl.h", {"QSsl"} },
   { "qtest.h", {"QTest"} },
   { "qtconcurrentmap.h", {"QtConcurrentMap"} },
   { "qtconcurrentfilter.h", {"QtConcurrentFilter"} },
   { "qtconcurrentrun.h", {"QtConcurrentRun"} },
};

const std::map<String, StringMap> deprecatedheaders {
    {"QtGui", {
        {"QGenericPlugin", "QtGui/QGenericPlugin"},
        {"QGenericPluginFactory", "QtGui/QGenericPluginFactory"},
    }},
    {"QtSql", {
        {"qsql.h", "QtSql/qtsqlglobal.h"},
    }},
    {"QtDBus", {
        {"qdbusmacros.h", "QtDbus/qtdbusglobal.h"},
    }},
};

const StringSet angle_headers {
    "egl.h", "eglext.h", "eglplatform.h", "gl2.h", "gl2ext.h", "gl2platform.h", "ShaderLang.h", "khrplatform.h"
};

const StringSet internal_zlib_headers {
    "crc32.h", "deflate.h", "gzguts.h", "inffast.h", "inffixed.h", "inflate.h", "inftrees.h", "trees.h", "zutil.h"
};

const StringSet zlib_headers {
    "zconf.h", "zlib.h"
};

const StringSet ignore_headers = internal_zlib_headers;

StringSet ignore_for_include_check {
    "qsystemdetection.h", "qcompilerdetection.h", "qprocessordetection.h"
};

StringSet ignore_for_qt_begin_namespace_check {
    "qt_windows.h"
};

const std::map<String, StringSet> inject_headers {
    {"src/corelib/global", { "qconfig.h", "qconfig_p.h" }}
};

const std::set<String> qpa_headers {
    "^(?!qplatformheaderhelper)qplatform", "^qwindowsystem"
};

std::regex r_header("^[-a-z0-9_]*\\.h$");
std::regex r_skip_header("^qt[a-z0-9]+-config(_p)?\\.h$");
std::regex r_private_header("_p(ch)?\\.h$");
std::vector<std::regex> r_qpa_headers;
std::regex r_qt_class("^#pragma qt_class\\(([^)]*)\\)[\r\n]*$");
std::regex r_class(R"((class|struct)\s+(\w+\s+)?(Q[0-9a-zA-Z]+))");
std::regex r_typedef(R"(typedef.*\W(Q\w+);$)");
std::regex r_require(R"xx(^QT_REQUIRE_CONFIG\((.*)\);[\r\n]*$)xx");

std::map<String, std::map<path, File>> modules;

void init()
{
    ignore_for_include_check.insert(zlib_headers.begin(), zlib_headers.end());
    ignore_for_include_check.insert(angle_headers.begin(), angle_headers.end());

    ignore_for_qt_begin_namespace_check.insert(zlib_headers.begin(), zlib_headers.end());
    ignore_for_qt_begin_namespace_check.insert(angle_headers.begin(), angle_headers.end());

    for (auto &qpa : qpa_headers)
        r_qpa_headers.emplace_back(qpa);
}

// elaborate more
StringSet class_names(std::map<path, File>::value_type &ff)
{
    StringSet classes;

    auto lines = read_lines(ff.first);
    auto &f = ff.second;

    std::smatch m;
    for (auto &line : lines)
    {
        if (line[0] == '#')
        {
            if (line.find("#pragma qt_no_master_include") == 0)
                f.no_master_include = true;
            if (line.find("#pragma qt_sync_skip_header_check") == 0)
                f.clean = false;
            if (line.find("#pragma qt_sync_stop_processing") == 0)
                return classes;
            if (std::regex_search(line, m, r_qt_class))
                classes.insert(m[1]);
            continue;
        }
        if (line.find(';') == -1 && std::regex_search(line, m, r_class))
            classes.insert(m[3]);
        if (std::regex_search(line, m, r_typedef))
            classes.insert(m[1]);
        if (std::regex_search(line, m, r_require))
            f.requires = m[1];
    }
    return classes;
}

int main(int argc, char **argv)
try
{
    init();

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("sdir,s", po::value<String>()->required(), "sdir")
        ("bdir,b", po::value<String>(), "bdir")
        ("modules,m", po::value<Strings>()->multitoken(), "bdir")
        ("version,v", po::value<String>()->required(), "version")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    path sdir = vm["sdir"].as<String>();
    path bdir = sdir;
    if (vm.count("bdir"))
        bdir = vm["bdir"].as<String>();

    String version = vm["version"].as<String>();

    Strings selected_modules;
    for (auto &m : module_dirs)
        selected_modules.push_back(m.first);
    if (vm.count("modules"))
        selected_modules = vm["modules"].as<Strings>();

    // find files
    for (auto &m : selected_modules)
    {
        auto im = module_dirs.find(m);
        if (im == module_dirs.end())
        {
            std::cout << "bad module: " << m << "\n";
            continue;
        }

        auto files = enumerate_files(sdir / im->second);
        for (auto &f : files)
        {
            auto fn = f.filename().string();
            if (!(
                std::regex_search(fn, r_header) &&
                !std::regex_search(fn, r_skip_header) &&
                fn.find("ui_") != 0 &&
                fn.find("q") == 0
                ))
                continue;

            modules[m][f];
        }
    }

    // process
    for (auto &mm : modules)
    {
        auto m = mm.first;

        std::ofstream master((bdir / "include" / m / m).string());
        master << "#pragma once" << "\n";
        master << "#include <" << m << "/" << m << "Depends>" << "\n";

        for (auto &ff : mm.second)
        {
            auto p = ff.first;
            auto fn = p.filename().string();
            auto &f = ff.second;

            if (fn.find('_') != -1 || fn.find("qconfig") != -1)
                f.no_master_include = true;

            // check qpa
            for (auto &qpa : r_qpa_headers)
            {
                f.qpa_header = std::regex_search(fn, qpa);
                if (f.qpa_header)
                {
                    f.public_header = false;
                    break;
                }
            }

            if (!f.qpa_header)
                f.public_header = !std::regex_search(fn, r_private_header);

            StringSet classes;
            if (f.public_header)
                classes = class_names(ff);

            auto ic = classnames.find(fn);
            if (ic != classnames.end())
                classes.insert(ic->second.begin(), ic->second.end());

            auto oheader = bdir / "include" / m;
            if (f.public_header)
            {
                for (auto &c : classes)
                    write_file(oheader / c, "#include \"" + fn + "\"\n");

                if (!f.no_master_include)
                {
                    if (!f.requires.empty())
                        master << "#if QT_CONFIG(" << f.requires << ")\n";
                    master << "#include \"" << fn << "\"\n";
                    if (!f.requires.empty())
                        master << "#endif\n";
                }
            }
            else if (f.qpa_header)
                oheader /= path(version) / m / "qpa";
            else
                oheader /= path(version) / m / "private";
            oheader /= fn;
            write_file(oheader, "#include \"" + normalize_path(p) + "\"\n");
        }

        master << "#include \"" << boost::to_lower_copy(m) + "version.h\"" << "\n";

        int ma, mi, pa;
        sscanf(&version[0], "%d.%d.%d", &ma, &mi, &pa);
        char x[10];
        sprintf(x, "0x%02d%02d%02d", ma, mi, pa);

        std::ofstream v((bdir / "include" / m / (boost::to_lower_copy(m) + "version.h")).string());
        v << "#pragma once" << "\n";
        v << "\n";
        v << "#define " << boost::to_upper_copy(m) + "_VERSION_STR \"" << version << "\"\n";
        v << "\n";
        v << "#define " << boost::to_upper_copy(m) + "_VERSION " << x << "\n";

        std::ofstream v2((bdir / "include" / m / (m + "Version")).string());
        v2 << "#include \"" << boost::to_lower_copy(m) + "version.h" <<  "\"\n";
    }

    return 0;
}
catch (const std::exception &e)
{
    std::cerr << e.what() << "\n";
    return 1;
}
catch (...)
{
    std::cerr << "Unhandled unknown exception" << "\n";
    return 1;
}
