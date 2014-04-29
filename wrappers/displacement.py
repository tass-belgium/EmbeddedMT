class Displacement:
    """ A class representing a displacement """
    sequenceNo = 0
    x_dis = 0
    y_dis = 0

    def __lt__(a, b):
        return a.sequenceNo < b.sequenceNo
    def __ge__(a,b):
        return not (a < b)
    
class DisplacementCollection:
    """ A class representing a collection of displacements """
    displacementList = []
    automaticSequenceNo = 0
    
    def push_displacement(self, displacement):
        self.displacementList.append(displacement)
    def push_back(self, x, y):
        if(x != 0 and y != 0):
            tmp = Displacement()
            tmp.x_dis = x
            tmp.y_dis = y
            tmp.sequenceNo = self.automaticSequenceNo
            self.automaticSequenceNo = self.automaticSequenceNo + 1
            self.push_displacement(tmp)
    def calculateXY(self):
        sortedDisplacementList = sorted(self.displacementList)
        x_pos = 0
        y_pos = 0
        x = [0]
        y = [0]
        for displacement in sortedDisplacementList:
            x_pos += displacement.x_dis
            y_pos -= displacement.y_dis
            x.append(x_pos)
            y.append(y_pos)
        return x, y, x_pos, y_pos

    def push_back_and_order(self, sequenceNo, x,y):
        if(x != 0 and y != 0):
            tmp = Displacement()
            tmp.x_dis = x
            tmp.y_dis = y
            tmp.sequenceNo = sequenceNo
            self.push_displacement(tmp)
    def getXAbs(self):
        x,y,x_pos, y_pos = self.calculateXY()
        if(x_pos >= 0):
            return x
        else:
            xabs = []
            for i in range(len(x)):
                xabs.append(abs(x[i]))
            return xabs

