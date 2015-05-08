#!/usr/bin/python3
import os
import argparse
import subprocess

def execute(command):    
        print(command)
        popen = subprocess.Popen(command, stdout=subprocess.PIPE)
        lines_iterator = iter(popen.stdout.readline, b"")
        for line in lines_iterator:
            print(line.decode(), end="") # yield line
        popen.wait()
        if popen.returncode is not 0:
            print("Command unsuccessful. Exiting.")
            exit(1)

def build(scriptDir, buildJobs, application, target, mode, profile, multiThreading, showStuff, rebuild):
    if(rebuild):
        targetBuildDir = scriptDir + '/build/' + target + '/' + mode
        if os.path.exists(targetBuildDir):
            rebuildCommand = []
            rebuildCommand.append('rm')
            rebuildCommand.append('-rf')
            rebuildCommand.append(targetBuildDir)
            execute(rebuildCommand)
        else:
            print(targetBuildDir + ' does not exists. Continuing happily.')

    buildCommand = []
    buildCommand.append('scons')
    buildCommand.append('--directory')
    buildCommand.append(scriptDir)
    buildCommand.append('--jobs')
    buildCommand.append(buildJobs)
    buildCommand.append('target=' + target)
    buildCommand.append('mode=' + mode)
    buildCommand.append('profile=' + profile)
    buildCommand.append('multiThreading=' + multiThreading)
    buildCommand.append('showstuff=' + showStuff)
    buildCommand.append(application)
    execute(buildCommand)
    return 0

def main():
    scriptDir = os.path.dirname(os.path.realpath(__file__))

    parser = argparse.ArgumentParser(description='Build EmbeddedMT')
    parser.add_argument('-t', '--target', dest='target', default='native',
                            help='The target for which to compile')
    parser.add_argument('-p', '--path', dest='path', default='{scriptDir}/..'.format(scriptDir = scriptDir),
                            help='The path to the EmbeddedMT root folder')
    parser.add_argument('--application', dest='application', default='pipeline',
                            help='EmbeddedMT application to build')
    parser.add_argument('-m', '--mode', dest='mode', default='debug',
            help='The mode in which to build: <debug> or release')
    parser.add_argument('-l', '--log', '--logLevel', dest='logLevel', default='debug',
            help='The logLevel for the build: <debug>,warning, error, none ')
    parser.add_argument('--profile', dest='profile', default='no',
            help='Build with profiling support (if required): <no>, perf, gprof, callgrind')
    parser.add_argument('-d', '--multithreading', dest='multiThreading', default='none',
            help='Build with multithreading support:<none>, openmp')
    parser.add_argument('-b', '--show-stuff', dest='showStuff', action='store_true',
            help='Build with support for showing what is happening under the hood')
    parser.add_argument('-r', '--rebuild', dest='rebuild', action='store_true',
            help='Force rebuild')
    parser.add_argument('-j', '--jobs', dest='buildJobs', default = '10',
            help='Build using the specified number of build jobs')

    args = parser.parse_args()

    if(args.showStuff):
        showStuff = 'yes'
    else:
        showStuff = 'no'

    return build(args.path, args.buildJobs, args.application, args.target, args.mode, args.profile, args.multiThreading, showStuff, args.rebuild)

if __name__ == "__main__":
    main()
