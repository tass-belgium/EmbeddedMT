# System includes
import socket
import displacement
import json

def getSocket():
    myIp = getMyIp()
    serverHost = myIp
    serverPort = 3232
    # Set up socket server
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind((serverHost, serverPort))
    serversocket.listen(5)
    return serverHost, serverPort, serversocket

def getMyIp():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # TODO: we should try to open a connection with the device so we are sure we have our right ip (e.g. when we got multiple ip's)
    s.connect(('8.8.8.8',80))
    ip = s.getsockname()[0]
    s.close()
    return ip

def runSocketServer(serversocket, displacements, threadSyncer):
    #accept connections from outside
    while True:
        try:
            (clientsocket, address) = serversocket.accept()
            break
        except:
            # retry 
            continue
    threadSyncer.listeningToSocket = True
    while True:
        chunk = clientsocket.recv(62)
        if(len(chunk) == 0):
            threadSyncer.continuePlotting = True
            break
        jsonString = bytes.decode(chunk[:-1])
        message = json.loads(jsonString)
        displacements.push_back_and_order(message['sequenceNo'], message['displacementX'], message['displacementY'])

    print('Closing socket')
    serversocket.close()
