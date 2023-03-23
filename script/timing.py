

# importing library
import numpy as np
import math
# display
# decode
# joystick detect
# CAN_TX_Task
# scanKey
# writeToDoubleBuffer


task = [
    [1,18.01],    # display
    # [2,0.1892],   # decode
    [3,0.16],    # joystick detect
    # [3,0.9603],   # CAN_TX_Task
    [4,0.21] ,    # scanKey
    [4,9.2165]    # writeToDoubleBuffer
]

Interval = [
    50, # display
    # 1,  # decode
    1,  # joystick detect
    # 1,  # CAN_TX_Task
    1,  # scanKey
    1   # writeToDoubleBuffer   --> this should be smaller than 40ms 
]

# Find the key with the maximum value
max_interval = max(Interval)


def check():
    total_latency = 0
    for i in range(len(Interval)):
        total_latency += math.ceil(max_interval/Interval[i]) * task[i][1]

    if total_latency < max_interval:
        #print("Passed:", total_latency, "<", max_interval)
        return True
    else:
        #print("Failed:", total_latency, ">", max_interval)
        return False



result = []

for o in range(40,50): #2
    # for j in range(20,30): #3
        for k in range(20,30): #3
            # for l in range(10,20): #4
                for m in range(10,20): #4
                    Interval = [50, o,k,m]
                    if(check()):
                        result.append(Interval)

# print(result)
total_interval = []
for interval in result:
    total_interval.append(sum(interval))

total_interval = np.array(total_interval)

minimum_combination = result[np.argmin(total_interval)]
print(minimum_combination)

print(result[np.argmin(total_interval)+1])

                    
