import os

class configParameters:
    FALSE = 0;
    TRUE = 1;
    INHERIT = 2;

# Default configuration
ccmake = {
        # 3rd party builds
        'BUILD_JASPER' : False,
        'BUILD_JPEG' : True,
        'BUILD_OPENEXR' : False,
        'BUILD_PNG' : False,
        'BUILD_SHARED_LIBS' : configParameters.INHERIT,
        'BUILD_TBB' : False,
        'BUILD_TIFF' : False,
        'BUILD_ZLIB' : True,
        'BZIP2_LIBRARIES' : '/lib64/libbz2.so',
        'BUILD_WITH_DEBUG_INFO' : False,

        # Enable stuff
        'ENABLE_AVX' : False,
        'ENABLE_COVERAGE' : False,
        'ENABLE_FAST_MATH' : False,
        'ENABLE_OMIT_POINTER_FRAME' : True,
        'ENABLE_PRECOMPILED_HEADER' : True,
        'ENABLE_PROFILING' : False,
        'ENABLE_SOLUTION_FOLDERS' : False,
        'ENABLE_SSE' : True,
        'ENABLE_SSE2' : True,
        'ENABLE_SSE3' : True,
        'ENABLE_SSE41' : False,
        'ENABLE_SSE42' : False,
        'ENABLE_SSSE3' : False,

        # With stuff
        'WITH_1394' : True,
        'WITH_CARBON' : False,
        'WITH_CLP' : False,
        'WITH_CUBLAS' : False,
        'WITH_CUDA' : False,
        'WITH_CUFFT' : False,
        'WITH_EIGEN' : False,
        'WITH_FFMPEG' : True,
        'WITH_GIGEAPI' : False,
        'WITH_GSTREAMER' : True,
        'WITH_GSTREAMER_0_10' : False,
        'WITH_GTK' : True,
        'WITH_IPP' : True,
        'WITH_IPP_A' : False,
        'WITH_JASPER' : False,
        'WITH_JPEG' : True,
        'WITH_LIBV4L' : True,
        'WITH_MIL' : False,
        'WITH_NVCUVID' : False,
        'WITH_OPENCL' : False,
        'WITH_OPENCLAMDBLAS' : False,
        'WITH_OPENCV_CLAMDFFT' : False,
        'WITH_OPENEXR' : False,
        'WITH_OPENGL' : False,
        'WITH_OPENMP' : configParameters.INHERIT,
        'WITH_OPENNI' : False,
        'WITH_PNG' : False,
        'WITH_PVAPI' : True,
        'WITH_QT' : False,
        'WITH_TBB' : False,
        'WITH_TIFF' : False,
        'WITH_TYZX' : False,
        'WITH_UNICAP' : False,
        'WITH_V4L' : True,
        'WITH_VFW' : False,
        'WITH_VTK' :  False,
        'WITH_WIN32UI' : False,
        'WITH_WEBP' : False,
        'WITH_XIMEA' : False,
        'WITH_XINE' : False
}

class configFileGenerator(object):
    @staticmethod
    def notDefined(value):
        return None

    @staticmethod
    def defUndef(value, string):
        if(value):
            return '#define {string}\n'.format(string = string)
        else:
            return '/* #undef {string} */\n'.format(string = string)

    @staticmethod
    def withJpeg(value):
        return configFileGenerator.defUndef(value, 'HAVE_JPEG')

    @staticmethod
    def withXimea(value):
        return configFileGenerator.defUndef(value, 'HAVE_XIMEA')

    @staticmethod
    def withFfmpeg(value):
        defines = configFileGenerator.defUndef(value, 'HAVE_FFMPEG')
        defines = defines + configFileGenerator.defUndef(value, 'HAVE_FFMPEG_SWSCALE')
        defines = defines + configFileGenerator.defUndef(value, 'HAVE_GENTOO_FFMPEG')
        return defines
    @staticmethod
    def withLibv4l(value):
        defines = configFileGenerator.defUndef(value, 'HAVE_LIBV4L')
        defines = defines + configFileGenerator.defUndef(value, 'HAVE_CAMV4L')
        defines = defines + configFileGenerator.defUndef(value, 'HAVE_CAMV4L2')
        return defines
    @staticmethod
    def withGtk(value):
        defines = configFileGenerator.defUndef(value, 'HAVE_GTHREAD')
        defines = defines + configFileGenerator.defUndef(value, 'HAVE_GTK')
        return defines

# Config file functions
ccmakeToCvconfig = {
        # 3rd party builds
        'BUILD_JASPER' : configFileGenerator.notDefined,
        'BUILD_JPEG' : configFileGenerator.notDefined,
        'BUILD_OPENEXR' : configFileGenerator.notDefined,
        'BUILD_PNG' : configFileGenerator.notDefined,
        'BUILD_SHARED_LIBS' : configFileGenerator.notDefined,
        'BUILD_TBB' : configFileGenerator.notDefined,
        'BUILD_TIFF' : configFileGenerator.notDefined,
        'BUILD_ZLIB' : configFileGenerator.notDefined,
        'BZIP2_LIBRARIES' : configFileGenerator.notDefined,
        'BUILD_WITH_DEBUG_INFO' : configFileGenerator.notDefined,

        # Enable stuff
        'ENABLE_AVX' : configFileGenerator.notDefined,
        'ENABLE_COVERAGE' : configFileGenerator.notDefined,
        'ENABLE_FAST_MATH' : configFileGenerator.notDefined,
        'ENABLE_OMIT_POINTER_FRAME' : configFileGenerator.notDefined,
        'ENABLE_PRECOMPILED_HEADER' : configFileGenerator.notDefined,
        'ENABLE_PROFILING' : configFileGenerator.notDefined,
        'ENABLE_SOLUTION_FOLDERS' : configFileGenerator.notDefined,
        'ENABLE_SSE' : configFileGenerator.notDefined,
        'ENABLE_SSE2' : configFileGenerator.notDefined,
        'ENABLE_SSE3' : configFileGenerator.notDefined,
        'ENABLE_SSE41' : configFileGenerator.notDefined,
        'ENABLE_SSE42' : configFileGenerator.notDefined,
        'ENABLE_SSSE3' : configFileGenerator.notDefined,

        # With stuff
        'WITH_1394' : configFileGenerator.notDefined,
        'WITH_CARBON' : configFileGenerator.notDefined,
        'WITH_CLP' : configFileGenerator.notDefined,
        'WITH_CUBLAS' : configFileGenerator.notDefined,
        'WITH_CUDA' : configFileGenerator.notDefined,
        'WITH_CUFFT' : configFileGenerator.notDefined,
        'WITH_EIGEN' : configFileGenerator.notDefined,
        'WITH_FFMPEG' : configFileGenerator.withFfmpeg,
        'WITH_GIGEAPI' : configFileGenerator.notDefined,
        'WITH_GSTREAMER' : configFileGenerator.notDefined,
        'WITH_GSTREAMER_0_10' : configFileGenerator.notDefined,
        'WITH_GTK' : configFileGenerator.withGtk,
        'WITH_IPP' : configFileGenerator.notDefined,
        'WITH_IPP_A' : configFileGenerator.notDefined,
        'WITH_JASPER' : configFileGenerator.notDefined,
        'WITH_JPEG' : configFileGenerator.withJpeg,
        'WITH_LIBV4L' : configFileGenerator.withLibv4l,
        'WITH_MIL' : configFileGenerator.notDefined,
        'WITH_NVCUVID' : configFileGenerator.notDefined,
        'WITH_OPENCL' : configFileGenerator.notDefined,
        'WITH_OPENCLAMDBLAS' : configFileGenerator.notDefined,
        'WITH_OPENCV_CLAMDFFT' : configFileGenerator.notDefined,
        'WITH_OPENEXR' : configFileGenerator.notDefined,
        'WITH_OPENGL' : configFileGenerator.notDefined,
        'WITH_OPENMP' : configFileGenerator.notDefined,
        'WITH_OPENNI' : configFileGenerator.notDefined,
        'WITH_PNG' : configFileGenerator.notDefined,
        'WITH_PVAPI' : configFileGenerator.notDefined,
        'WITH_QT' : configFileGenerator.notDefined,
        'WITH_TBB' : configFileGenerator.notDefined,
        'WITH_TIFF' : configFileGenerator.notDefined,
        'WITH_TYZX' : configFileGenerator.notDefined,
        'WITH_UNICAP' : configFileGenerator.notDefined,
        'WITH_V4L' : configFileGenerator.notDefined,
        'WITH_VFW' : configFileGenerator.notDefined,
        'WITH_VTK' :  configFileGenerator.notDefined,
        'WITH_WIN32UI' : configFileGenerator.notDefined,
        'WITH_WEBP' : configFileGenerator.notDefined,
        'WITH_XIMEA' : configFileGenerator.withXimea,
        'WITH_XINE' : configFileGenerator.notDefined
}

class modulesToFilterFunctions(object):
    @staticmethod
    def optionToSources(option, sources,source):
        if not ccmake[option]:
            sources = modulesToFilterFunctions.removeFromList(sources, source)
        return sources
    @staticmethod
    def removeFromList(sources, valueToRemove):
        try:
            sources.remove(valueToRemove)
        except:
            print "{valueToRemove} was not in source list".format(valueToRemove = valueToRemove)
        return sources
    @staticmethod
    def nothingSpecial(sources, modulePath):
        return sources,None,None
    @staticmethod
    def highgui(sources, modulePath):
        additionalIncludes = list()
        additionalLibs = list()
        sources = modulesToFilterFunctions.optionToSources('WITH_XIMEA', sources, '{module}/src/cap_ximea.cpp'.format(module = modulePath))
        # TODO: Add ximea 32-bit lib if needed 
        additionalLibs.append('m3apiX64')
        sources = modulesToFilterFunctions.optionToSources('WITH_GIGEAPI', sources, '{module}/src/cap_giganetix.cpp'.format(module = modulePath))
        sources = modulesToFilterFunctions.optionToSources('WITH_VFW', sources, '{module}/src/cap_vfw.cpp'.format(module = modulePath))
        if not ccmake['WITH_GSTREAMER'] and not ccmake['WITH_GSTREAMER_0_10']:
            sources = modulesToFilterFunctions.optionToSources('WITH_GSTREAMER', sources, '{module}/src/cap_gstreamer.cpp'.format(module = modulePath))
        else:
            if ccmake['WITH_GSTREAMER']:
                additionalIncludes.append(findGstreamer())
            else:
                additionalIncludes.append(findGstreamer010())
            additionalIncludes.append('/usr/include/glib-2.0')
            additionalIncludes.append('/usr/lib/glib-2.0/include')
        if ccmake['WITH_GTK']:
            additionalIncludes.extend(findGtk2())
            
        sources = modulesToFilterFunctions.optionToSources('WITH_CARBON', sources, '{module}/src/window_carbon.cpp'.format(module = modulePath))
        sources = modulesToFilterFunctions.optionToSources('WITH_UNICAP', sources, '{module}/src/cap_unicap.cpp'.format(module = modulePath))
        sources = modulesToFilterFunctions.optionToSources('WITH_WIN32UI', sources, '{module}/src/window_w32.cpp'.format(module = modulePath))
        sources = modulesToFilterFunctions.optionToSources('WITH_QT', sources, '{module}/src/cap_qt.cpp'.format(module = modulePath))
        sources = modulesToFilterFunctions.optionToSources('WITH_QT', sources, '{module}/src/cap_qtkit.mm'.format(module = modulePath))
        # Backwards compatibility with 2.4.8
        sources = modulesToFilterFunctions.optionToSources('WITH_TYZX', sources, '{module}/src/cap_tyzx.cpp'.format(module = modulePath))
        sources = modulesToFilterFunctions.optionToSources('WITH_MIL', sources, '{module}/src/cap_mil.cpp'.format(module = modulePath))
        return sources,additionalIncludes,additionalLibs
    @staticmethod
    def nonfree(sources, modulePath):
        additionalIncludes = list()
        additionalLibs = list()
#        sources = modulesToFilterFunctions.optionToSources('WITH_OPENCL', sources, '{module}/src/surf.ocl.cpp'.format(module = modulePath))
        additionalIncludes.append('../../3rdparty/include/opencl/1.2')
        return sources,additionalIncludes,additionalLibs
    @staticmethod
    def features2d(sources, modulePath):
        additionalIncludes = list()
        additionalLibs = list()
        sources.extend(getFilesInFolder('{module}/src/akaze'.format(module = modulePath), '.c'))
        sources.extend(getFilesInFolder('{module}/src/kaze'.format(module = modulePath), '.c'))
        return sources,additionalIncludes,additionalLibs

def findGstreamer():
    # TODO
    return '/usr/include/gstreamer-1.0'

def findGstreamer010():
    # TODO
    return '/usr/include/gstreamer-0.10'

def findGtk2():
    # TODO
    return ['/usr/include/gtk-2.0', '/usr/lib/gtk-2.0/include', '/usr/include/cairo', '/usr/include/pango-1.0', '/usr/include/gdk-pixbuf-2.0', '/usr/include/atk-1.0']

modulesToFilter = {
        'highgui' : modulesToFilterFunctions.highgui,
        'nonfree' : modulesToFilterFunctions.nonfree,
        'features2d'    : modulesToFilterFunctions.features2d
}

class getAdditionalLibsFunctions(object):
    @staticmethod
    def core():
        libs = []
        if ccmake['WITH_JPEG']:
            libs.extend([
                'libjpeg'
                ])
        if ccmake['BUILD_ZLIB']:
            libs.extend([
                'zlib'
                ])
        return libs
    @staticmethod
    def highgui():
        libs = []
        if ccmake['WITH_GTK']:
            libs.extend([
		'gtk-x11-2.0',
		'gdk-x11-2.0',
		'cairo',
		'gdk_pixbuf-2.0',
		'gobject-2.0',
		'glib-2.0'
                ])
        if ccmake['WITH_FFMPEG']:
            libs.extend([
		'avformat',
		'avcodec',
		'avutil',
		'swscale'
                ])
        if ccmake['WITH_LIBV4L'] or ccmake['WITH_V4L']:
            libs.extend([
		'v4l2',
		'v4l1',
                ])
        return libs

getAdditionalLibs = {
        'core' : getAdditionalLibsFunctions.core,
        'highgui' : getAdditionalLibsFunctions.highgui
}

def getDefinesAndCompileOptions():
    defines = ['__OPENCV_BUILD', 'OPENCV_NOSTL']
    options = ['-fPIC', '-static']
    if ccmake['ENABLE_SSE']:
        options.append('-msse')
    if ccmake['ENABLE_SSE2']:
        options.append('-msse2')
    if ccmake['ENABLE_SSE3']:
        options.append('-msse3')
    if not ccmake['BUILD_WITH_DEBUG_INFO']:
        defines.append('NDEBUG')
    return defines,options

configFile = 'cvconfig.h'

def getFilesInFolder(folder, headerExtensions = ['.h']):
    headers = list()
    try:
        content = os.listdir('{folder}'.format(folder = folder))
        for ext in headerExtensions:
            header_content = ['{folder}/{file}'.format(folder = folder, file = header_file) for header_file in content if ext in header_file]
            headers.extend(header_content)
    except OSError:
#        print '{folder} not found. No problem: carpe diem'.format(folder = folder)
        pass
    return headers
