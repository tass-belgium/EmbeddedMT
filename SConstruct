opts = Variables()

x64_compiler_path = Dir('/usr/bin').abspath
armv6_compiler_path = Dir('toolchains/arm-bcm2708-linux-gnueabi/bin').abspath

map_arch_to_toolchain_path = {
    'x64' : x64_compiler_path,
    'armv6' : armv6_compiler_path
}

map_arch_to_toolchain = {
    'x64' : '',
    'armv6' : 'arm-bcm2708-linux-gnueabi-'
}

opts.Add(EnumVariable('arch', 'Set target architecture', 'x64',
                    allowed_values=('x64', 'armv6'),
                    map={},
                    ignorecase=2))

opts.Add(EnumVariable('mode', 'Set build mode', 'debug',
                    allowed_values=('debug', 'release'),
                    map={},
                    ignorecase=2))

env=Environment(variables=opts)
Export('env')

arch=env['arch']

rootDir = Dir('.').abspath
buildDir = Dir('build/{arch}/{mode}'.format(arch=arch, mode=env['mode'])).abspath
sourceDir = Dir('src').abspath
VariantDir(buildDir, sourceDir)

opencv_dir = Dir('3rdparty/opencv/{arch}'.format(arch=arch)).abspath

env['AR'] = '{toolchainpath}/{toolchain}ar'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=map_arch_to_toolchain[arch])
env['AS'] = '{toolchainpath}/{toolchain}as'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=map_arch_to_toolchain[arch])
env['CC'] = '{toolchainpath}/{toolchain}gcc'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=map_arch_to_toolchain[arch])
env['CXX'] = '{toolchainpath}/{toolchain}g++'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=map_arch_to_toolchain[arch])
env['LD'] = '{toolchainpath}/{toolchain}ld'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=map_arch_to_toolchain[arch])
env['NM'] = '{toolchainpath}/{toolchain}nm'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=map_arch_to_toolchain[arch])
env['STRIP'] = '{toolchainpath}/{toolchain}strip'.format(toolchainpath=map_arch_to_toolchain_path[arch], toolchain=map_arch_to_toolchain[arch])
env['CPPPATH'] = []
env['CPPFLAGS'] = []

env['CPPPATH'].append('{buildDir}'.format(buildDir=buildDir))
env['CPPPATH'].append('{opencv_dir}/include'.format(opencv_dir=opencv_dir))
env['CPPPATH'].append('{opencv_dir}/include/opencv'.format(opencv_dir=opencv_dir))
env['CPPPATH'].append('{opencv_dir}/include/opencv2'.format(opencv_dir=opencv_dir))

if env['mode'] == 'release':
    env['CPPFLAGS'].append('-Ofast')
else:
    env['CPPFLAGS'].append('-O0')

env['LIBS_DIR'] = '{buildDir}/libs'.format(buildDir = buildDir)
env['openCV_LIBS_DIR'] = '{opencv_dir}/lib'.format(opencv_dir=opencv_dir)

print("Building in {buildDir}".format(buildDir=buildDir))
print("Target architecture: {arch}".format(arch=arch))
if(arch == 'x64'):
    print("Compiler toolchain: native")
else:
    print("Compiler toolchain: {toolchain}").format(toolchain=map_arch_to_toolchain[arch])
print("Compiler: {compiler}".format(compiler=env['CC']))
print("C++ Compiler: {compiler}".format(compiler=env['CXX']))

SConscript("{buildDir}/SConscript".format(buildDir=buildDir))