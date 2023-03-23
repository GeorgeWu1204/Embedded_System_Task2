
# Report
## Tasks Impementation
<!-- 16. An identification of all the tasks that are performed by the system with their method of implementation, thread or interrupt -->

| Task | <center>Task Description</center> | Implementation Method 
| -----| -----------------|-----------------------|
| scanKey | Identifies the keys pressed | <center>Thread  | 
| joystick detect | detecting the moving direction of joystick | <center>Thread | 
| decode | Decode the received message from the msgInQ | <center>Thread  | 
| display | Display the information on the LED screen | <center>Thread  | 
| writeToDoubleBuffer | Write the calculated output voltage into half of the buffer according to writeBuffer1 | <center>Thread | 
| sampleISR | Read the other half of the buffer | <center>Interrupt |
| CAN_TX_Task | Push the message that is going to be sent into msgOutQ| <center>Thread |   
| configuration  | Locate the position of the keyboard | <center>Thread |
| CAN_RX_ISR | Receive message from CAN bus and put inside msgInQ | <center>Interrupt |
| CAN_TX_ISR | Push the message from msgOutQ onto the CAN bus | <center>Interrupt |

## Time Analysis
<!-- 17. A characterisation of each task with its theoretical minimum initiation interval and measured maximum execution time
-->
The timing analysis result is shown in the following table. The RMS priority is also ranked according to the initialization interval.

| Task |  Theoretical Minimum Initiation Interval | Measured Maximum Execution Time | RMS Priority | $\lceil \frac{\tau_n}{\tau_i} \rceil$ | $\lceil \frac{\tau_n}{\tau_i} \rceil T_i$ |
| -----|------------ | ---------- |-------|----|-----|
| scanKey | <center>20ms</center> | <center>0.21 ms</center>| <center> 5 | 60 | 12.6ms |
| joystick detect | <center>80ms</center> | <center>0.16 ms</center> | <center> 2 | 15 | 2.4 ms |
| decode | <center>25.2ms</center> | <center>0.1892 ms</center> | <center> 4</center>| 48 | 9.081ms |
| display |  <center>100ms</center> | <center>18.01 ms</center> | <center> 1 | 12 | 216.12ms |
| writeToDoubleBuffer | <center> 18ms | <center>9.2165 ms</center> | <center> 6 | 67 | 617.505ms
| CAN_TX_Task | <center>60ms</center> | <center>0.9603 ms</center>| <center> 3 |   20 | 18.4435ms
| configuration  | <center>1200ms</center> | <center>1000.04 ms</center> | <center> 0 | 6 | 1000.04ms | 
| sampleISR |<center>0.045ms</center>| <center>0.00927 ms</center>| <center> NaN | NaN | NaN | 


### Notes
- Incorporating joystick code to scankey task would increase single run time from 0.21ms to 0.37ms (mainly due to 'analogRead' in Joystick code), hence make a separate task for it
- Considering the decode task, the max exec time 0.1892 ms is obtained by only using 'R' messages. If only use 'P' messages, the time would be 0.1768 ms

### Theoratical Initiation Interval Calculation
- scanKey: The scan task is manually set to be conducted once every 20ms by vTaskDelayUntil to guarantee the key matrix is scanned quickly enough to detect the transient states. Hence, the initiation interval is $20ms$.
- joystick detect: It is an unimportant task and it is a soft real-time task which does not has a deadline. Hence, the priority for this task is low.
- decode: Since the minimum time to send a CAN frame is 0.7ms. In the worst case, 36 items in the queue needs to be executed. Hence the minimum interval should be $0.7 \times 36 = 25.2ms$
- display: Similar to scanKey, the initial interval is manually set to $100ms$.
- writeToDoubleBuffer: The double buffer size selected to be 800 and the amount of data that needs to write to the buffer should be equal to the amount of data extracted from the other buffer by sampleISR function to guarantee the quality of the generated sound. Since the interrupt is triggered every $0.045ms$, it takes $400 \times 0.045 = 18 ms$ to take the output. Therefore, the initial interval for this task is 18ms as well.
- CAN_TX_Task: This task is required to complete one iteration every $1.67 ms$. With the implemention of 36 length queue, the initiation interval is $1.67 \times 36 = 60ms$ for 36 executions.
- configuration: This task is considered to set to the lowest priority. This is because it is not often to be executed. Hence, the initial interval is manually set to 1.2s.

## Critical Instant Analysis of Rate Monotonic Scheduler(RMS)
<!-- 18. A critical instant analysis of the rate monotonic scheduler, showing that all deadlines are met under worst-case conditions   -->
Considering the worst-case instant in time for the lowest-priority task *configuration*, the CPU must satisfy the requirement,
$$L_n = \sum_i \lceil \frac{\tau_n}{\tau_i} \rceil T_i \le \tau_n $$
Where $\tau_n$ is the initiation interval for the worst case. According to the calculation, $L_n = 876.1495ms$, which is less than the $\tau_n = 1200ms $ Therefore, this RMS model satisfy the critical instant analysis.


## CPU utilisation
<!-- 19. A quantification of total CPU utilisation  -->
The overall run time statistics for different tasks is recorded by vTaskGetRunTimeStats() function. The result is shown in the following table.
| Task | CPU Utilization |
|------|-----------------|
|scanKey | <center>$\lt 1\%$ |
| joystick detect | <center> $1.5\%$ |
| decode | <center> $\lt 1\%$ |
| display | <center> $16\%$ |
| writeToDoubleBuffer | <center> $43\%$ |
| CAN_TX_Task| <center> $\lt 1\%$ |
| configuration | <center> $2\%$ |
|IDLE |<center> $33\%$|




## Data and Resource Sharing
<!-- 20. An identification of all the shared data structures and the methods used to guarantee safe access synchronisation -->

|Variable | Shared data structure | Access Tasks | Synchronisation Method |
|---------|-----------------------| -------------- | --------------|
|sound_table|   <center> Array[7] of struct(octave  + keyindex) | Display & scanKey & CAN_TX_Task | Semaphore by taking the mutex and copy to the local array|
|octave | <center>uint8_t | scanKey & decode & configuration |Atomic access |
|globalRX_Message & globalTX_Message | uint8_t array[8] | scanKey & 





## Inter-task blocking dependencies
<!-- 21. An analysis of inter-task blocking dependencies that shows any possibility of deadlock -->




