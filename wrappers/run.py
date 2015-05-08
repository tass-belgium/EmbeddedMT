#!/usr/bin/python3
import os
import argparse
import subprocess
import matplotlib.pyplot as plt
import time
import threading

# Helper includes
import gbl
import socketHelper
import displacement as disp
import plotHelper as pltHelper

def executeApplication(fname, target, mode, sequence, alg, output, logLevel, profile, serverHost, serverPort, threadSyncer):
    threadSyncer.doneBuilding = True
    while(threadSyncer.startExecuting):
        # Do nothing
        continue
        
    # Execute application
    cmd = '{fname} {sequence} {alg} {address} {port}'.format(fname=fname, sequence=sequence, alg=alg, address=serverHost, port=serverPort)
    if profile == 'perf':
        print("Using perf record...")
        cmd = "perf record -c 1000 " + cmd

    if profile == 'callgrind':
        print("Using callgrind...")
        cmd = "valgrind --tool=callgrind " + cmd

    print(cmd)

    # Time execution
    start_time = time.time()
    ret = os.system(cmd)
    if(ret != 0):
            print('Processing returned error (code: {errorcode}). Exiting'.format(errorcode=ret))
            return gbl.RetCodes.RESULT_FAILURE
    else:
        end_time = time.time()
        print("Elapsed time measured by Python was %g seconds" % (end_time - start_time))

    if profile == 'perf':
        cmd = 'perf report'
        print(cmd)
        ret = os.system(cmd)

    if profile == 'gprof':
        readableOutputFile = 'profileAnalysis.txt'
        cmd = 'gprof {binary} {profileOutput} > {readableOutput}'.format(binary = fname, profileOutput = 'gmon.out', readableOutput = readableOutputFile)
        print(cmd)
        ret = os.system(cmd)
        print('You can find the result of the gprof profiling in {readableOutput}'.format(readableOutput = readableOutputFile))

    return gbl.RetCodes.RESULT_OK


def run(fname, target, mode, sequence, alg, output, logLevel, profile):
    serverHost,serverPort,serversocket = socketHelper.getSocket()
    displacements = disp.DisplacementCollection()
    color = 'b'
    threadSyncer = gbl.threadSyncVariables()

    t = threading.Thread(target=executeApplication, args=(fname, target, mode, sequence, alg, output, logLevel, profile, serverHost, serverPort, threadSyncer))
    t.start()

    while(threadSyncer.doneBuilding == False):
        continue

    threadSyncer.startExecuting = True

    t2 = threading.Thread(target=socketHelper.runSocketServer, args=(serversocket, displacements, threadSyncer))
    t2.start()

    pltHelper.plotReceivedPoints(displacements, 'b', threadSyncer)
    plt.close()
    plt.figure()
    pltHelper.processResults(displacements, color, interpolate)

    return 0

def main():
    parser = argparse.ArgumentParser(description='Build EmbeddedMT')
    parser.add_argument('-t', '--target', dest='target', default='native',
                            help='The target for which to compile')
    parser.add_argument('-m', '--mode', dest='mode', default='debug',
            help='The mode in which to build: <debug> or release')
    parser.add_argument('-s', '--sequence', dest='sequence', default='0',
            help='Select the sequence to use, default: webcam')
    parser.add_argument('-a', '--algorithm', dest='algorithm', default='10',
            help='The algorithm to use for the image processing')
    parser.add_argument('-o', '--output-file', dest='outputFile', default="output/displacements.result"
,
            help='The algorithm to use for the image processing')
    parser.add_argument('-l', '--log', '--logLevel', dest='logLevel', default='debug',
            help='The logLevel for the build: <debug>,warning, error, none ')
    parser.add_argument('--profile', dest='profile', default='no',
            help='Build with profiling support (if required): <no>, perf, gprof, callgrind')
    parser.add_argument('--application', dest='application', default='pipeline',
                            help='EmbeddedMT application to build')

    args = parser.parse_args()

    scriptDir = os.path.dirname(os.path.realpath(__file__))
    targetBinDir = scriptDir + '/../build/' + args.target + '/' + args.mode + '/bin/'
    application = targetBinDir + args.application

    return run(application, args.target, args.mode, args.sequence, args.algorithm, args.outputFile, args.logLevel, args.profile)

if __name__ == "__main__":
    main()
