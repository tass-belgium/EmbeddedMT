#!/usr/bin/python
import os
import sys
import getopt
from enum import Enum
import csv
import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d

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
    
def main():
    """ Entry function """
    try:
        opts, args = getopt.getopt(sys.argv[1:], "a:m:pho:s:c:", ["help", "output="])
    except getopt.GetoptError as err:
        # print help information and exit:
        print(str(err))
        usage()
        return 1
    arch = 'x64'
    mode = 'debug'
    output = "displacement.result"
    skipProcessing = False
    sequence = os.path.abspath('{scriptDir}/../testSequences/cam_ball1_640')
    alg = 0
    for o, a in opts:
        if o == "-a":
            arch = a
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
        else:
            assert False, "unhandled option"
            usage()
            return 1
    
    # Execute
    if(not skipProcessing):
        if(executeApplication(arch, mode, sequence, alg, output) != RetCodes.RESULT_OK):
            print("An error occurred during building")
            return 1
    
    # Process results
    if(processResults(output) != RetCodes.RESULT_OK):
        print("An error occurred during processing")
        return 1
        
    return 0

def executeApplication(arch, mode, sequence, alg, output):
    """ Build and execute application """
    scriptDir = os.path.dirname(os.path.realpath(__file__))
    fname = '{scriptDir}/../build/{arch}/{mode}/proofOfConcept'.format(scriptDir=scriptDir,arch=arch, mode=mode)
    
    if(not os.path.isfile(fname)):
        # build application
        cmd = 'scons --directory {scriptDir}/.. --jobs 10 arch={arch} mode={mode}'.format(scriptDir=scriptDir, arch=arch, mode=mode)
        ret = os.system(cmd)
        if(ret != 0):
            print('Building returned error (code: {errorcode}). Exiting'.format(errorcode=ret))
            return RetCodes.RESULT_FAILURE
    # Execute application
    cmd = '{fname} {sequence} {alg} {output}'.format(fname=fname, sequence=sequence, alg=alg, output=output)
    ret = os.system(cmd)
    if(ret != 0):
            print('Processing returned error (code: {errorcode}). Exiting'.format(errorcode=ret))
            return RetCodes.RESULT_FAILURE
    return RetCodes.RESULT_OK

def processResults(output):
    displacements = DisplacementCollection()
    with open(output, newline='') as csvfile:
        csvreader = csv.reader(csvfile, delimiter='\t')
        for row in csvreader:
            displacements.push_back(int(row[0]), int(row[1]))
            
    try:
        x_abs = displacements.getXAbs()
        f2 = interp1d(x_abs, displacements.y, kind='cubic')
        
        # Define more points
        xnew = np.linspace(min(displacements.x), max(displacements.x), 500)
        
        # f2() needs positive data at its input
        xpos = []
        for i in range(len(xnew)):
            xpos.append(abs(xnew[i]))
        
        plt.plot(displacements.x, displacements.y,'bx', xnew, f2(xpos), 'b-')
        plt.show()
    except:       
        print("Could not interpolate points")
        plt.plot(displacements.x, displacements.y,'bx')
        plt.show()
    return RetCodes.RESULT_OK

if __name__ == "__main__":
    main()