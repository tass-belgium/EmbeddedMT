from SCons.Builder import Builder
import os
import thirdparty_config
import opencv_config

opencv_module_includes = list()

def build_opencv_emitter(target, source, env):
    ''' Emitter for openCV builder '''
    headerExtensions = ['.h'] # This will select .hpp files as well
    module = os.path.basename(os.path.normpath(env['opencv_module']))
    target.append('opencv_{module}'.format(module = module))

    # Add module hpp file
    source.extend(opencv_config.getFilesInFolder('{module}/include/opencv2'.format(module = env['opencv_module']), headerExtensions))
    env.Install('{includeDir}/opencv2'.format(includeDir=env['THIRD_PARTY_INCLUDE_DIR'], module = module), source)

    # If core module, add general includes
    if 'core' in env['opencv_module']:
        # Add general headers
        source.extend(opencv_config.getFilesInFolder('{module}/../../include/opencv2'.format(module = env['opencv_module']), headerExtensions))
        source.extend(opencv_config.getFilesInFolder('{module}/../../include/opencv'.format(module = env['opencv_module']), headerExtensions))
	# TODO: remove this dirty macport fix
	opencv_module_includes.append('/opt/local/include')
        # Add opencl header files to includes
        if opencv_config.ccmake['WITH_OPENCL']:
            opencv_module_includes.append('{module}../../3rdparty/include/opencl/1.2'.format(module = env['opencv_module']))

    # Install header files
    header_files = list()
    header_files.extend(opencv_config.getFilesInFolder('{module}/include/opencv2/{module_name}'.format(module = env['opencv_module'], module_name = module), ['.h']))
    env.Install('{includeDir}/opencv2/{module}'.format(includeDir=env['THIRD_PARTY_INCLUDE_DIR'], module = module), header_files)
    
    # Add module includes to path for other modules that depend on this one
    opencv_module_includes.append('{module}/include'.format(module = env['opencv_module']))

    # Search additional library dependencies
    try:
	addLibs,addLinkFlags = opencv_config.getAdditionalLibs[str(module)]()
	try:
        	env['OPENCVBUILDER_ADDITIONAL_LIBRARIES'].extend(addLibs)
	except KeyError:
		env['OPENCVBUILDER_ADDITIONAL_LIBRARIES'] = addLibs
	try:
		env['OPENCVBUILDER_ADDITIONAL_FLAGS'].extend(addLinkFlags)
	except KeyError:
		env['OPENCVBUILDER_ADDITIONAL_FLAGS'] = addLinkFlags
    except KeyError:
        pass

    return target,source

def build_opencv_generator(source, target, env, for_signature):
    ''' Generator for openCV builder '''
    module = os.path.basename(os.path.normpath(env['opencv_module']))

    # Configure build environment for opencv
    env_opencv = env.Clone()
    # Empty defines to avoid unnecessary rebuilding of the library
    env_opencv['CPPDEFINES'] = []
    defines,options = opencv_config.getDefinesAndCompileOptions()
    env_opencv['CPPDEFINES'].extend(defines)
    env_opencv['CXXFLAGS'].extend(options)
    env_opencv['CPPPATH'].append(opencv_module_includes)
    env_opencv['CPPPATH'].append('{module}/src'.format(module = env['opencv_module']))

    if module == 'core':
        # TODO: generate these properly
        env.Install('{includeDir}'.format(includeDir=env['THIRD_PARTY_INCLUDE_DIR']), 'version_string.inc')
        env.Install('{includeDir}/opencv2'.format(includeDir=env['THIRD_PARTY_INCLUDE_DIR']), 'opencv_modules.hpp')
    
    # Build module
    sources = list()
    sources.extend(opencv_config.getFilesInFolder('{module}/src'.format(module = env['opencv_module']), ['.c', '.mm']))
    sources.append('{module}/src/opencl_kernels.cpp'.format(module = env['opencv_module']))
    try:
        sources,additionalIncludes,additionalLibs = opencv_config.modulesToFilter[str(module)](sources, env['opencv_module'])
        env_opencv['CPPPATH'].extend(additionalIncludes)
    except KeyError:
#        print 'Nothing special to do with the sources of module {module}. Continuing happily'.format(module = module)
        pass

    lib = env_opencv.Library('{lib}'.format(lib = target[0]), sources)
    installed_lib = env_opencv.Install("{libs_dir}".format(libs_dir=env['THIRD_PARTY_LIBS_DIR']), lib)
    return installed_lib

def config_opencv_emitter(target, source, env):
    ''' Emitter for opencv config '''
    source = [opencv_config.configFile]
    target = []
   
    # Store ccmake values for building 
    for configParam,value in env['opencv_config'].iteritems():
        opencv_config.ccmake[configParam] = value

    # Add additional include paths
    opencv_config.opencvBuilderAdditionalIncludePaths = env['OPENCVBUILDER_INCLUDE_PATHS']
    return target,source

def config_opencv_generator(source, target, env, for_signature):
    ''' Generator for openCV builder '''
    configDirectory = 'opencv'

    # write config file
    with open('{configDirectory}/{configFile}'.format(configDirectory = configDirectory, configFile = opencv_config.configFile), 'w') as f:
        generateConfigFile(f, opencv_config.ccmake)

    install_config = env.Install("{includeDir}".format(includeDir=env['THIRD_PARTY_INCLUDE_DIR']), '{configDirectory}/{configFile}'.format(configDirectory=configDirectory, configFile = opencv_config.configFile))
    return install_config 

def generateConfigFile(configFile, config = {}):
    ''' Generate the config file '''
    for param,value in config.iteritems():
        string = opencv_config.ccmakeToCvconfig[str(param)](value)
        if string != None:
           configFile.write(string)

def opencl_opencv_emitter(target, source, env):
    ''' Opencl builder emitter '''
    target = []
    source = ['{module}/src/opencl_kernels.hpp'.format(module = env['opencv_module'])]
    return target, source

def opencl_opencv_generator(source, target, env, for_signature):
    ''' Opencl builder generator '''
    module = os.path.basename(os.path.normpath(env['opencv_module']))
    cmd = 'cmake -DMODULE_NAME="{module_name}" -DCL_DIR="{module}/src/opencl" -DOUTPUT=$TARGET -P {module}/../../cmake/cl2cpp.cmake'.format(module = env['opencv_module'], module_name = module)
    opencl_files = env.Command("{module}/src/opencl_kernels.hpp".format(module = env['opencv_module']),'', cmd)
    opencl_files = env.Command("{module}/src/opencl_kernels.cpp".format(module = env['opencv_module']),'', cmd)
    return opencl_files

def thirdparty_opencv_emitter(target, source, env):
    ''' Emitter for 3rdparty openCV stuff '''
    headerExtensions = ['.h'] # This will select .hpp files as well
    module = os.path.basename(os.path.normpath(env['opencv_3rdparty']))
    target.append('{module}'.format(module = module))

    # Add module hpp file
    source.extend(opencv_config.getFilesInFolder('{module}'.format(module = env['opencv_3rdparty']), headerExtensions))

    # Install header files
    header_files = list()
    header_files.extend(opencv_config.getFilesInFolder('{module}'.format(module = env['opencv_3rdparty']), headerExtensions))
    env.Install('{includeDir}'.format(includeDir=env['THIRD_PARTY_INCLUDE_DIR']), header_files)
    
    # Add module includes to path for other modules that depend on this one
    opencv_module_includes.append('{module}'.format(module = env['opencv_3rdparty']))
    return target,source

def thirdparty_opencv_generator(source, target, env, for_signature):
    ''' Generator for 3rdparty openCV stuff '''
    module = os.path.basename(os.path.normpath(env['opencv_3rdparty']))

    # Configure build environment for opencv
    env_opencv = env.Clone()
    # Empty defines to avoid unnecessary rebuilding of the library
    env_opencv['CPPDEFINES'] = []
    env_opencv['CPPPATH'].append(opencv_module_includes)
    env_opencv['CPPPATH'].append('{module}'.format(module = env['opencv_3rdparty']))

    # Build module
    sources = list()
    sources.extend(opencv_config.getFilesInFolder('{module}'.format(module = env['opencv_3rdparty']), ['.c']))

    lib = env_opencv.Library('{lib}'.format(lib = target[0]), sources)
    installed_lib = env_opencv.Install("{libs_dir}".format(libs_dir=env['THIRD_PARTY_LIBS_DIR']), lib)
    return installed_lib

def exists(env):
    return env.Detect('buildOpencv') and env.Detect('configOpencv')

def generate(env):
    configOpencvBuilder = Builder(emitter = config_opencv_emitter, generator = config_opencv_generator)
    env.Append(BUILDERS = {'configOpencv' : configOpencvBuilder})
    thirdpartyOpencvBuilder = Builder(emitter = thirdparty_opencv_emitter, generator = thirdparty_opencv_generator)
    env.Append(BUILDERS = {'thirdpartyOpencv' : thirdpartyOpencvBuilder})
    openclOpencvBuilder = Builder(emitter = opencl_opencv_emitter, generator = opencl_opencv_generator)
    env.Append(BUILDERS = {'openclOpencv' : openclOpencvBuilder})
    opencvBuilder = Builder(emitter = build_opencv_emitter, generator = build_opencv_generator)
    env.Append(BUILDERS = {'buildOpencv' : opencvBuilder})

#def getFilesInFolder(folder, headerExtensions = ['.h']):
#    headers = list()
#    try:
#        content = os.listdir('{folder}'.format(folder = folder))
#        for ext in headerExtensions:
#            header_content = ['{folder}/{file}'.format(folder = folder, file = header_file) for header_file in content if ext in header_file]
#           headers.extend(header_content)
#    except OSError:
#        print '{folder} not found. No problem: carpe diem'.format(folder = folder)
#        pass
#    return headers
