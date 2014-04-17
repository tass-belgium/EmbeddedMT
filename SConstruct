opts = Variables()

x64_compiler_path = Dir('/usr/bin').abspath
armv6_compiler_path = Dir('toolchains/arm-bcm2708hardfp-linux-gnueabi/bin').abspath

map_target_to_arch = {
    'native' : 'x64',
    'rpi' : 'armv6zk'
}

map_arch_to_static_lib = {
    'x64' : False,
    'armv6zk' : True
}

map_arch_to_toolchain_path = {
    'x64' : x64_compiler_path,
    'armv6zk' : armv6_compiler_path
}

map_arch_to_toolchain = {
    'x64' : '',
    'armv6zk' : 'arm-bcm2708hardfp-linux-gnueabi-'
}

map_toolchain_to_openmp_support = {
    '' : True,
    'arm-bcm2708hardfp-linux-gnueabi-' : False,
}

map_logLevel_to_define = {
    'profile' : 'LOG_LEVEL_PROFILE',
    'debug' : 'LOG_LEVEL_DEBUG',
    'warning' : 'LOG_LEVEL_WARNING',
    'error' : 'LOG_LEVEL_ERROR',
    'none' : 'LOG_LEVEL_NONE'
}

opts.Add(EnumVariable('target', 'Set target', 'native',
                    allowed_values=('native', 'rpi'),
                    map={},
                    ignorecase=2))

opts.Add(EnumVariable('mode', 'Set build mode', 'debug',
                    allowed_values=('debug', 'release'),
                    map={},
                    ignorecase=2))

opts.Add(EnumVariable('logLevel', 'Set log mode', 'debug',
                    allowed_values=('debug', 'warning', 'error','none', 'profile'),
                    map={},
                    ignorecase=2))

env=Environment(variables=opts)
Export('env')

target=env['target']
arch=map_target_to_arch[target]
toolchain=map_arch_to_toolchain[arch]

rootDir = Dir('.').abspath
buildDir = Dir('build/{target}/{mode}'.format(target=target, mode=env['mode'])).abspath
sourceDir = Dir('src').abspath
VariantDir(buildDir, sourceDir)

opencv_dir = Dir('3rdparty/opencv'.format(arch=arch)).abspath

env['AR'] = '{toolchainpath}/{toolchain}ar'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=toolchain)
env['AS'] = '{toolchainpath}/{toolchain}as'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=toolchain)
env['CC'] = '{toolchainpath}/{toolchain}gcc'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=toolchain)
env['CXX'] = '{toolchainpath}/{toolchain}g++'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=toolchain)
env['LD'] = '{toolchainpath}/{toolchain}ld'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=toolchain)
env['NM'] = '{toolchainpath}/{toolchain}nm'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=toolchain)
env['STRIP'] = '{toolchainpath}/{toolchain}strip'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=toolchain)
env['CPPPATH'] = []
env['CPPFLAGS'] = []
env['LDFLAGS'] = []

env['CPPPATH'].append('{buildDir}'.format(buildDir=buildDir))
env['CPPPATH'].append('{opencv_dir}/include'.format(opencv_dir=opencv_dir))
env['CPPPATH'].append('{opencv_dir}/include/opencv'.format(opencv_dir=opencv_dir))
env['CPPPATH'].append('{opencv_dir}/include/opencv2'.format(opencv_dir=opencv_dir))

env['CPPDEFINES'] = []
env['CPPDEFINES'].append(map_logLevel_to_define[env['logLevel']])

env['LINKFLAGS'].append(['-pthread'])
env['USE_STATIC_LIBS'] = map_arch_to_static_lib[arch]

if env['mode'] == 'release':
    env['CPPFLAGS'].append('-Ofast')
else:
    env['CPPFLAGS'].append(['-g', '-O0'])

env['CPPFLAGS'].append(['-Wall', '-Wextra', '-Wshadow',  '-Wpointer-arith'])
#env['CPPFLAGS'].append(['-Wcast-qual'])

if map_toolchain_to_openmp_support[toolchain] == True:
    env['CPPFLAGS'].append(['-fopenmp']);
    env['LINKFLAGS'].append(['-fopenmp']);
    
if(target == 'rpi'):
    env['CPPFLAGS'].append(['-mfpu=vfp', '-mfloat-abi=hard', '-march=armv6zk', '-mtune=arm1176jzf-s'])
    # Suppress mangling va thing
    env['CPPFLAGS'].append('-Wno-psabi')

env['LIBS_DIR'] = '{buildDir}/libs'.format(buildDir = buildDir)
env['STD_LIBS'] = [
    'rt',
    'm',
    'dl',
    'stdc++'
]
env['openCV_LIBS_DIRS'] = [
    '{opencv_dir}/lib/{arch}'.format(arch=arch,opencv_dir=opencv_dir),
    '{opencv_dir}/3rdparty/lib/{arch}'.format(arch=arch,opencv_dir=opencv_dir)
]

print("Target: {target}".format(target=target))
print("Target architecture: {arch}".format(arch=arch))
print("Building in {buildDir}".format(buildDir=buildDir))
if(arch == 'x64'):
    print("Compiler toolchain: native")
else:
    print("Compiler toolchain: {toolchain}").format(toolchain=toolchain)
print("Compiler: {compiler}".format(compiler=env['CC']))
print("C++ Compiler: {compiler}".format(compiler=env['CXX']))

SConscript("{buildDir}/SConscript".format(buildDir=buildDir))