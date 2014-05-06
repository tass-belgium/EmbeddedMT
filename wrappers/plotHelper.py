# System includes
import matplotlib
import matplotlib.pyplot as plt
import pylab
import numpy as np
from scipy.interpolate import interp1d

# Extra includes
import gbl

def processResults(displacements, color, interpolate):
    x, y, x_pos, y_pos = displacements.calculateXY()
    try:
        if(interpolate):
            x_abs = displacements.getXAbs()
            f2 = interp1d(x_abs, y, kind='cubic')
            
            # Define more points
            xnew = np.linspace(min(x), max(x), 500)
            
            # f2() needs positive data at its input
            xpos = []
            for i in range(len(xnew)):
                xpos.append(abs(xnew[i]))
            
            plt.plot(x, y,'{color}x'.format(color=color), xnew, f2(xpos), '{color}-'.format(color=color))
        else:
            plt.plot(x, y,'{color}x'.format(color=color))
    except:       
        print("Could not interpolate points")
        plt.plot(x, y,'{color}x'.format(color=color))
    plt.xlabel("Displacement in x direction (pixels)")
    plt.ylabel("Displacement in y direction (pixels)")
    plt.title("Displacement")
    plt.show()
    return gbl.RetCodes.RESULT_OK

def plotReceivedPoints(displacements, color, threadSyncer):
    """ Plot all received points """
    fig = plt.figure()
    plt.plot([0], [0],'{color}x'.format(color=color))
    plt.xlabel("Displacement in x direction (pixels)")
    plt.ylabel("Displacement in y direction (pixels)")
    plt.title("Displacement")

    while True:
        x,y,x_pos,y_pos = displacements.calculateXY()
        plt.clf()
        plt.plot(x, y,'{color}x'.format(color=color))
        fig.canvas.draw()
        plt.draw()
        plt.pause(0.01)
        if(threadSyncer.continuePlotting == True):
            break;
