# Script to check if the selected similar places within one trajectory make
# sense. They make sense if brighter pattern were detected, marked with red
# pixels in the showed image. Use this script for checks only.

import numpy as np
import cv2

# path_to_folder, path where the estimated similar places binary has outputted
# the simPlaces and computed scores
path2folder = "<path_to_folder>"
# path2folder = "/home/olga/projects/online_place_recognition/build/"
simPlaces = path2folder + "vggSim.txt"
costs = path2folder + "scores.txt"
traj_size = 1078

costMatrix = np.zeros((traj_size, traj_size))

inputfile = open(costs)
minEl = 1000000
with inputfile as f:
    for line in f:
        arr = line.split()
        row = arr[0]
        col = arr[1]
        simValue = float(arr[2])

        costMatrix[row, col] = simValue
        # print minEl, simValue
        if simValue < minEl:
            minEl = simValue

costMatrix = cv2.normalize(costMatrix, costMatrix, alpha=0,
                           beta=255, norm_type=cv2.NORM_MINMAX, dtype=cv2.CV_8UC1)

# // convert to color
costMatrix = cv2.cvtColor(costMatrix, cv2.COLOR_GRAY2RGB)
# overlay selected image pairs
with open(simPlaces) as f:
    for line in f:
        arr = line.split()
        costMatrix[arr[0], arr[1]] = [0, 0, 255]

cv2.namedWindow('costs', cv2.WINDOW_NORMAL)
cv2.imshow('costs', costMatrix)
cv2.waitKey(0)
cv2.destroyAllWindows()
