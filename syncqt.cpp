/*
local_settings:
    #use_shared_libs: false
    #build_dir_type: local
    build:
        generator: Xcode
 
c++: 14

name: syncqt

dependencies:
    public:
        pvt.egorpugin.primitives.filesystem:
            version: master

*/

#include <primitives/filesystem.h>

const std::map<String, path> modules {
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

const StringMap classnames {
   { "qglobal.h", "QtGlobal" },
   { "qendian.h", "QtEndian" },
   { "qconfig.h", "QtConfig" },
   { "qplugin.h", "QtPlugin" },
   { "qalgorithms.h", "QtAlgorithms" },
   { "qcontainerfwd.h", "QtContainerFwd" },
   { "qdebug.h", "QtDebug" },
   { "qevent.h", "QtEvents" },
   { "qnamespace.h", "Qt" },
   { "qnumeric.h", "QtNumeric" },
   { "qvariant.h", "QVariantHash },QVariantList },QVariantMap" },
   { "qgl.h", "QGL" },
   { "qsql.h", "QSql" },
   { "qssl.h", "QSsl" },
   { "qtest.h", "QTest" },
   { "qtconcurrentmap.h", "QtConcurrentMap" },
   { "qtconcurrentfilter.h", "QtConcurrentFilter" },
   { "qtconcurrentrun.h", "QtConcurrentRun" },
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
    {"/src/corelib/global", { "qconfig.h", "qconfig_p.h" }}
};

//qpa_headers = ( qr/^(?!qplatformheaderhelper)qplatform/, qr/^qwindowsystem/ );

void init()
{
    ignore_for_include_check.insert(zlib_headers.begin(), zlib_headers.end());
    ignore_for_include_check.insert(angle_headers.begin(), angle_headers.end());

    ignore_for_qt_begin_namespace_check.insert(zlib_headers.begin(), zlib_headers.end());
    ignore_for_qt_begin_namespace_check.insert(angle_headers.begin(), angle_headers.end());
}

int main(int argc, char **argv)
{
    init();

    /*
     args:
     base dir (src)
     out dir (bin)
     modules to sync - list
     */

    /*
     create new dirs:
     */

    /*
     populate master includes:
     */

    /*
     create deprecated headers:
     */

    return 0;
}
