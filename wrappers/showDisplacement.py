#!/usr/bin/python
import os
import sys
import getopt
from enum import Enum
import csv
import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d
import time

class RetCodes(Enum):
    """ Enumeration of return codes """
    RESULT_OK = 0
    RESULT_FAILURE = 1
    
class Displacement:
    """ A class representing a displacement """
    x_dis = 0
    y_dis = 0
    
class DisplacementCollection:
    """ A class representing a collection of displacements """
    displacementList = []
    x_pos = 0
    y_pos = 0
    x = [0]
    y = [0]
    
    def push_displacement(self, displacement):
        self.displacementList.append(displacement)
    def push_back(self, x, y):
        if(x != 0 and y != 0):
            tmp = Displacement()
            tmp.x_pos = x
            tmp.y_pos = y
            self.x_pos += + x
            self.y_pos -= y
            self.x.append(self.x_pos)
            self.y.append(self.y_pos)
            self.push_displacement(tmp)
    def getXAbs(self):
        if(self.x_pos >= 0):
            return x
        else:
            xabs = []
            for i in range(len(self.x)):
                xabs.append(abs(self.x[i]))
            return xabs
    
def usage():
    print("Wrapper for plotting the displacement")
    print("options:")
    print("\t-t <target>      Define target: <native>, rpi")
    print("\t-m <mode>      Define mode: <debug>, release")
    print("\t-p             Only plot result")
    print("\t-h             Print this help")
    print("\t-o <file>      Define output file")
    print("\t-s <sequence>  Define path to sequence to use")
    print("\t-c <alg>       Define algorithm to use: <0>:SURF+BFM, 1:SURF+FLANN, 2:SIFT+BFM, 3:SIFT+FLANN")
    print("\t-l <logLevel>  Define log level: <debug>, warning or error")
    print("\t-r             Clean and rebuild")
    print("\t-i             Do not interpolate result")
    
def main():
    """ Entry function """
    scriptDir = os.path.dirname(os.path.realpath(__file__))
    try:
        opts, args = getopt.getopt(sys.argv[1:], "t:m:pho:s:c:l:ri", ["help", "output="])
    except getopt.GetoptError as err:
        # print help information and exit:
        print(str(err))
        usage()
        return 1
    target = 'native'
    mode = 'debug'
    output = "displacement.result"
    skipProcessing = False
    sequence = os.path.abspath('{scriptDir}/../testSequences/cam_ball1_640'.format(scriptDir=scriptDir))
    alg = 0
    logLevel = 'debug'
    rebuild = False
    interpolate = True
    for o, a in opts:
        if o == "-t":
            target = a
        elif o == "-m":
            mode = a  
        elif o in ("-h", "--help"):
            usage()
            return 0
        elif o in ("-o", "--output"):
            output = a
        elif o == "-p":
            skipProcessing = True
        elif o == "-s":
            sequence = a
        elif o == "-c":
            alg = a
        elif o == "-l":
            logLevel = a
        elif o == "-r":
            rebuild = True
        elif o == "-i":
            interpolate = False
        else:
            assert False, "unhandled option"
            usage()
            return 1

    if(rebuild):
        fname = '{scriptDir}/../build/{target}/{mode}/proofOfConcept'.format(scriptDir=scriptDir,target=target, mode=mode)
        if(os.path.isfile(fname)):
           cmd = 'rm -rf {scriptDir}/../build/{target}/{mode}'.format(scriptDir=scriptDir,target=target,mode=mode)
           os.system(cmd)
    
    # Execute
    if(not skipProcessing):
        if(executeApplication(target, mode, sequence, alg, output, logLevel) != RetCodes.RESULT_OK):
            print("An error occurred during building")
            return 1
    
    # Process results
    if(processResults(output, 'b', interpolate) != RetCodes.RESULT_OK):
        print("An error occurred during processing")
        return 1
        
    return 0

def executeApplication(target, mode, sequence, alg, output, logLevel):
    """ Build and execute application """
    scriptDir = os.path.dirname(os.path.realpath(__file__))
    fname = '{scriptDir}/../build/{target}/{mode}/proofOfConcept'.format(scriptDir=scriptDir,target=target, mode=mode)
    
    # build application
    cmd = 'scons --directory {scriptDir}/.. --jobs 10 target={target} mode={mode} logLevel={logLevel} {buildTarget}'.format(scriptDir=scriptDir, target=target, mode=mode, logLevel=logLevel, buildTarget='demo')
    print(cmd)
    ret = os.system(cmd)
    if(ret != 0):
        print('Building returned error (code: {errorcode}). Exiting'.format(errorcode=ret))
        return RetCodes.RESULT_FAILURE

    # Execute application
    cmd = '{fname} {sequence} {alg} {output}'.format(fname=fname, sequence=sequence, alg=alg, output=output)

    # Time execution
    start_time = time.time()
    ret = os.system(cmd)
    if(ret != 0):
            print('Processing returned error (code: {errorcode}). Exiting'.format(errorcode=ret))
            return RetCodes.RESULT_FAILURE
    else:
        end_time = time.time()
        print("Elapsed time measured by Python was %g seconds" % (end_time - start_time))

    return RetCodes.RESULT_OK

def processResults(output, color, interpolate):
    displacements = DisplacementCollection()
    with open(output, newline='') as csvfile:
        csvreader = csv.reader(csvfile, delimiter='\t')
        for row in csvreader:
            displacements.push_back(int(row[0]), int(row[1]))
            
    try:
        if(interpolate):
            x_abs = displacements.getXAbs()
            f2 = interp1d(x_abs, displacements.y, kind='cubic')
            
            # Define more points
            xnew = np.linspace(min(displacements.x), max(displacements.x), 500)
            
            # f2() needs positive data at its input
            xpos = []
            for i in range(len(xnew)):
                xpos.append(abs(xnew[i]))
            
            plt.plot(displacements.x, displacements.y,'{color}x'.format(color=color), xnew, f2(xpos), '{color}-'.format(color=color))
        else:
            plt.plot(displacements.x, displacements.y,'{color}x'.format(color=color))
    except:       
        print("Could not interpolate points")
        plt.plot(displacements.x, displacements.y,'{color}x'.format(color=color))
    plt.xlabel("Displacement in x direction (pixels)")
    plt.ylabel("Displacement in y direction (pixels)")
    plt.title("Displacement")
    plt.show()
    return RetCodes.RESULT_OK

if __name__ == "__main__":
    main()
