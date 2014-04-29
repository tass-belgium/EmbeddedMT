from enum import Enum

class RetCodes(Enum):
    """ Enumeration of return codes """
    RESULT_OK = 0
    RESULT_FAILURE = 1

class threadSyncVariables:
    continuePlotting = False
    doneBuilding = False
    startExecuting = False
    listeningToSocket = False
