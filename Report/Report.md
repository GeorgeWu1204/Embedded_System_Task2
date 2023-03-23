
# Report
## Tasks Impementation
<!-- 16. An identification of all the tasks that are performed by the system with their method of implementation, thread or interrupt -->

| Task | <center>Task Description</center> | Implementation Method 
| -----| --------------------|-----------------------|
| `scanKey()` | Identify the keys pressed | <center>Thread  | 
| `joystick()` | Detect the moving direction of joystick for changing between sawtooth and sine wave| <center>Thread | 
| `decode()` | Decode the received message from the `msgInQ` (message input queue)| <center>Thread  | 
| `display()` | Display the information on the LED screen | <center>Thread  | 
| `writeToDoubleBuffer()` | This writing task will utilize one half of the buffer, and the reading task (sampleISR) will utilize the other half. Once both tasks have finished accessing their respective halves, they will swap pointers and begin accessing the opposite half. This approach ensures that both tasks share the available resources of the buffer and can operate concurrently without any data conflicts.| <center>Thread | 
| `sampleISR()` | Read the other half of the buffer and pass the ouput to the speaker by analogWrite  | <center>Interrupt |
| `CAN_TX_Task()` | Push the message that is going to be sent into `msgOutQ` (message output queue)| <center>Thread |   
| `configuration()`  | Locate the position of the keyboard | <center>Thread |
| `CAN_RX_ISR()` | Receive message from CAN bus and put inside `msgInQ` (message input queue) | <center>Interrupt |
| `CAN_TX_ISR()` | Push the message from `msgOutQ` (message output queue) onto the CAN bus | <center>Interrupt |

## Time Analysis
<!-- 17. A characterisation of each task with its theoretical minimum initiation interval and measured maximum execution time
-->

The timing analysis result is shown in the following table. The RMS priority is also ranked according to the initialization interval.

| Task |  Theoretical Minimum Initiation Interval | Measured Maximum Execution Time | RMS Priority | $\lceil \frac{\tau_n}{\tau_i} \rceil$ | $\lceil \frac{\tau_n}{\tau_i} \rceil T_i$ |
| -----|------------ | ---------- |-------|----|-----|
| `scanKey()` | <center>20ms</center> | <center>0.21 ms</center>| <center> 5 | 60 | 12.6ms |
| `joystick()` | <center>80ms</center> | <center>0.16 ms</center> | <center> 2 | 15 | 2.4ms |
| `decode()` | <center>25.2ms</center> | <center>0.1892 ms</center> | <center> 4</center>| 48 | 9.081ms |
| `display()` |  <center>100ms</center> | <center>18.01 ms</center> | <center> 1 | 12 | 216.12ms |
| `writeToDoubleBuffer()` | <center> 36ms | <center>9.2165 ms</center> | <center> 6 | 34 | 313.361ms
| `CAN_TX_Task()` | <center>60ms</center> | <center>0.9603 ms</center>| <center> 3 |   20 | 18.4435ms
| `sampleISR()` |<center>0.045ms</center>| <center>0.00927 ms</center>| <center> NaN | 26667 | 247.2ms | 
| `configuration()`  | <center>1200ms</center> | <center>1000.04 ms</center> | <center> 0 | 6 | 1000.04ms | 

### Notes
- Initially, an attempt was made to incorporate joystick code into the scankey task, but this caused the single execution time to increase from 0.21ms to 0.37ms, mainly due to the `analogRead()` function used in the joystick code. Therefore, a separate task was created to handle the joystick code.
- With regards to the decode task, using only 'R' messages results in a maximum execution time of 0.1892 ms, while using only 'P' messages reduces the execution time to 0.1768 ms.


## Theoratical Initiation Interval Calculation
- `scanKey()`: The scan task is manually set to be conducted once every $20$ ms by `vTaskDelayUntil()` to guarantee the key matrix is scanned quickly enough to detect the transient states. Hence, the initiation interval is $20$ ms.
- `joystick()`: This task is less critical since no other tasks except for `display()` task depend on the value read from the joystick. Therefore, it is a soft real-time task with no strict deadline. As a result, its priority can be set to low.
- `decode()`: Since the minimum time to send a CAN frame is $0.7$ ms. In the worst case, $36$ items in the queue needs to be executed. Hence the minimum interval should be $0.7 \times 36$ = $25.2$ ms
- `display()`: The initial interval is manually set to $100$ ms. This makes the priority of the `display()` task low
- `writeToDoubleBuffer()`: The double buffer size selected to be $800$ and the amount of data that needs to write to the buffer should be equal to the amount of data extracted from the other buffer by `sampleISR()` function to guarantee the quality of the generated sound. Since the interrupt is triggered every $0.045$ ms, it takes $800 \times 0.045 = 36$ ms to take the output. Therefore, the initial interval for this task is $18$ ms as well.
- `CAN_TX_Task()`: This task is required to complete one iteration every $1.67$ ms. With the implemention of 36 length queue, the initiation interval is $1.67 \times 36 = 60$ ms for 36 executions.
- `configuration()`: This task is considered to set to the lowest priority. This is because it is not often to be executed. Apart from that, there are many delays required during the execution as the communication between the boards need to be confirmed. The sum of all the delays gives the minimum initiation interval $1.2$ s.

## Critical Instant Analysis of Rate Monotonic Scheduler(RMS)
<!-- 18. A critical instant analysis of the rate monotonic scheduler, showing that all deadlines are met under worst-case conditions   -->
Considering the worst-case instant in time for the lowest-priority task `configuration()`, the CPU must satisfy the requirement,
$$L_n = \sum_i \lceil \frac{\tau_n}{\tau_i} \rceil T_i \le \tau_n $$
Where $\tau_n$ is the initiation interval for the worst case. According to the calculation, $L_n = 819.2055$ ms, which is less than the $\tau_n = 1200$ ms. Therefore, this RMS model satisfy the critical instant analysis.


## CPU utilisation
<!-- 19. A quantification of total CPU utilisation  -->
The overall run time statistics for different tasks is recorded by `vTaskGetRunTimeStats()` function. The result is shown in the following table.
| Task | CPU Utilization |
|------|-----------------|
|`scanKey()` | <center>$\lt$ 1% |
| `joystick()` | <center> 1.5% |
| `decode()` | <center> $\lt$ 1% |
| `display()` | <center> 16% |
| `writeToDoubleBuffer()` | <center> 43% |
| `CAN_TX_Task()`| <center> $\lt$ 1% |
| `configuration()` | <center> 2% |
|`IDLE` |<center> 33%|

It can be observed that the `writeToDoubleBuffer()` function utilizes the most CPU. This may be due to its high priority and relatively lengthy execution time.




## Data and Resource Sharing
<!-- 20. An identification of all the shared data structures and the methods used to guarantee safe access synchronisation -->

|Variable | Shared data structure | Access Tasks / Interrupts | Synchronisation Method |
|---------|-----------------------| -------------- | --------------|
|`sound_table`|   Array[7] of struct(octave  + keyindex) | `display()`, `scanKey()`, `CAN_TX_Task()` | Semaphore by taking the mutex and copy to the local array|
|`octave` | uint8_t | `scanKey()`, `decode()`, `configuration()` |Atomic access |
| `msgInQ` | Queue length 36 and 8 bytes in total | `CAN_RX_ISR()`, `decode()` | When calling `xQueueReceive()`, the CPU will be yielded to other tasks and the function will block until a message becomes available in the queue. |
| `msgOutQ` | Queue length 36 and 8 bytes in total | `CAN_TX_Task()`, `scanKey()`, `configuration()` | It is guarded by a counting semaphore initialized with the value three for the transmit thread to take up to three times. |
| `sampleBuffer1` & `sampleBuffer0` | Both array [800] of uint_8 | `writeToDoubleBuffer()`, `sampleISR()` |Secured by Semaphore and use writeBuffer1 to decide which buffer to write and read |
|other global arrays | | | The rest of the global arrays are all secured by the semaphore.|
|other global variables | | | The rest of all global variables atomic access|
<!-- |globalRX_Message & globalTX_Message | uint8_t array[8] | scanKey &  -->

### Note: 
- The function `sendMessage()`, which is used to push the transmitting message into the `msgOutQ`, is shared by 2 different tasks (i.e., `scanKey()` and `configuration()`). Since multiple tasks may try to access the function simultaneously, it is necessary to make it a critical section to prevent race conditions. To achieve this, a semaphore lock is implemented for the function, ensuring that only one task can access it at a time. The code implementation is shown below:
    ```C++
    void sendMessage(uint8_t msg0, uint8_t msg1, uint8_t msg2){
        xSemaphoreTake(critical_section_mutex, portMAX_DELAY);
        TX_Message[0] = msg0;
        TX_Message[1] = msg1;
        TX_Message[2] = msg2;
        xQueueSend(msgOutQ,TX_Message,portMAX_DELAY);
        xSemaphoreGive(critical_section_mutex);
    }
    ```



## Inter-task blocking dependencies
<!-- 21. An analysis of inter-task blocking dependencies that shows any possibility of deadlock -->

![Task Dependency](Task_dependency.png "Task Dependency")

| Semaphore | Type | Access Tasks / Interrupts |
|-----------|------|----------------------------|
|sound_tableMutex| Mutex| `display()`, `write_to_buffer()`, `scanKey()` `communication()`|
|westeastArrayMutex| Mutex | `scanKey()`, `configuration()` |
|CAN_TX_Semaphore| Counting | `CAN_TX_Semaphore()` |
|sampleBufferSemaphore| Binary |`writeToDoubleBuffer()`, `sampleISR()` |
|critical_section_mutex| Mutex |`configuration()`, `scanKey()`|

In our code, there are two tasks, scanKey() and configuration(), which use two locks - westeastArrayMutex and critical_section_mutex. Deadlock can only happen when both tasks share these two locks together. However, since westeastArrayMutex only locks the localwesteastArray resource and no other resource gets locked, and critical_section_mutex does not need to access localwesteastArray to complete the lock, there is no possible deadlock case in our code.







