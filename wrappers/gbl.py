from enum import Enum
import subprocess

class RetCodes(Enum):
    """ Enumeration of return codes """
    RESULT_OK = 0
    RESULT_FAILURE = 1

class threadSyncVariables:
    continuePlotting = False
    doneBuilding = False
    startExecuting = False
    listeningToSocket = False

def execute(command):    
    for term in command:
        print(term + ' ', end="")
    print('')
    popen = subprocess.Popen(command, stdout=subprocess.PIPE)
    lines_iterator = iter(popen.stdout.readline, b"")
    for line in lines_iterator:
        try:
            print(line.decode(), end="") # yield line
        except:
            pass
    popen.wait()
    if popen.returncode is not 0:
        print("Command unsuccessful. Exiting.")
        exit(1)
