#!/usr/bin/python3
import os
import argparse
import matplotlib.pyplot as plt
import time
import threading

# Helper includes
import gbl
import socketHelper
import displacement as disp
import plotHelper as pltHelper

def getProfilePrefix(profile):
    prefix = []
    if profile == 'perf':
        print("Using perf...")
        prefix.append('perf')
        prefix.append('record')
        prefix.append('-c')
        prefix.append(str(1000))
    
    if profile == 'calgrind':
        print("Using callgrind...")
        prefix.append('valgrind')
        prefix.append('--tool=callgrind')

    return prefix


def executeApplicationHelper(fname, sequence, alg, address, port, profile):
    command = getProfilePrefix(profile)
    command.append(fname)
    command.append(sequence)
    command.append(alg)
    command.append(address)
    command.append(str(port))

    # Time execution
    start_time = time.time()
    gbl.execute(command)
    end_time = time.time()
    print("Elapsed time measured by Python was %g seconds" % (end_time - start_time))

def processProfilingInformation(profile):
    if profile == 'perf':
        command = ['perf', 'report']
        gbl.execute(command)

    if profile == 'gprof':
        readableOutputFile = 'profileAnalysis.txt'
        command = []
        command.append('gprof')
        command.append(fname)
        command.append('gmon.out')
        command.append(readableOutputFile)

        gbl.execute(command)
        print('You can find the result of the gprof profiling in {readableOutput}'.format(readableOutput = readableOutputFile))

def executeApplication(fname, target, mode, sequence, alg, output, profile, serverHost, serverPort, threadSyncer):
    threadSyncer.doneBuilding = True
    while(threadSyncer.startExecuting):
        # Do nothing
        continue
        
    # Execute application
    executeApplicationHelper(fname, sequence, alg, serverHost, serverPort, profile)
    return gbl.RetCodes.RESULT_OK

def run(fname, target, mode, sequence, alg, output, profile):
    serverHost,serverPort,serversocket = socketHelper.getSocket()
    displacements = disp.DisplacementCollection()
    color = 'b'
    threadSyncer = gbl.threadSyncVariables()

    # Check if executable exists
    if not os.path.exists(fname):
        print("Error: {binary} does not exist. Can not run using the specified options.".format(binary = fname))
        exit(1)

    t = threading.Thread(target=executeApplication, args=(fname, target, mode, sequence, alg, output, profile, serverHost, serverPort, threadSyncer))
    t.start()

    t2 = threading.Thread(target=socketHelper.runSocketServer, args=(serversocket, displacements, threadSyncer))
    t2.start()

    threadSyncer.startExecuting = True

    pltHelper.plotReceivedPoints(displacements, 'b', threadSyncer)
    plt.close()
    plt.figure()
    pltHelper.processResults(displacements, color)

    processProfilingInformation(profile)

    return 0

def main():
    parser = argparse.ArgumentParser(description='Build EmbeddedMT')
    parser.add_argument('-t', '--target', dest='target', default='native',
                            help='The target for which to compile')
    parser.add_argument('-m', '--mode', dest='mode', default='debug',
            help='The mode to run (binary should exist): <debug> or release')
    parser.add_argument('-s', '--sequence', dest='sequence', default='0',
            help='Select the sequence to use, default: webcam')
    parser.add_argument('-a', '--algorithm', dest='algorithm', default='10',
            help='The algorithm to use for the image processing')
    parser.add_argument('-o', '--output-file', dest='outputFile', default="output/displacements.result"
,
            help='The algorithm to use for the image processing')
    parser.add_argument('--profile', dest='profile', default='no',
            help='Build with profiling support (if required): <no>, perf, gprof, callgrind')
    parser.add_argument('--application', dest='application', default='pipeline',
                            help='EmbeddedMT application to build')

    args = parser.parse_args()

    profileDir = ''
    if args.profile != 'no':
        profileDir = '/profile'

    scriptDir = os.path.dirname(os.path.realpath(__file__))
    targetBinDir = scriptDir + '/../build/' + args.target + '/' + args.mode + profileDir + '/bin/'
    application = targetBinDir + args.application

    return run(application, args.target, args.mode, args.sequence, args.algorithm, args.outputFile, args.profile)

if __name__ == "__main__":
    main()
