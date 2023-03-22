## Tasks Breakdown
<!-- 16. An identification of all the tasks that are performed by the system with their method of implementation, thread or interrupt -->

## Time Analysis
<!-- 17. A characterisation of each task with its theoretical minimum initiation interval and measured maximum execution time
18. A critical instant analysis of the rate monotonic scheduler, showing that all deadlines are met under worst-case conditions  -->

## Data and Resource Sharing
<!-- 20. An identification of all the shared data structures and the methods used to guarantee safe access synchronisation -->

|Variable | Shared data structure | Access Threads | Synchronisation Method |
|---------|-----------------------| -------------- | --------------|
| KeyArray|    Array [7]   | Display & scanKey & CAN_TX_Task | Semaphore |


## Dependency
<!-- 21. An analysis of inter-task blocking dependencies that shows any possibility of deadlock -->

## CPU utilisation
<!-- 19. A quantification of total CPU utilisation  -->


# System Task Analysis

## Task Identification and Characterization

| Task | Task Description | Implementation Method | Theoretical Minimum Initiation Interval | Measured Maximum Execution Time |
| -----| -----------------|-----------------------|---------------------------------------- | ------------------------------- |
| scanKey | Identifies the keys pressed | Thread  | | 0.21 ms|
| joystick detect | detecting the moving direction of joystick | Thread | | 0.16 ms |
| decode | Decode the received message from the msgInQ | Thread  | | 0.1892 ms |
| display | Display the information on the LED screen | Thread  | | 18.01 ms |
| writeToDoubleBuffer | | Thread  | | 9.2165 ms |
| CAN_TX_Task | Push the message that is going to be sent into msgOutQ| Thread | | 0.9603 ms|    
| configuration  | Locate the position of the keyboard | Thread | | 1000.04 ms |
| sampleISR | | Interrupt || 0.00927 ms|
| CAN_RX_ISR | Receive message from CAN bus and put inside msgInQ | Interrupt |  |  |
| CAN_TX_ISR | Push the message from msgOutQ onto the CAN bus | Interrupt | | |


## Notes
- adding joystick code to scankey task would increase single run time from 0.21ms to 0.37ms (mainly due to 'analogRead' in Joystick code), hence make a separate task for it
- for decode task, the max exec time 0.1892 ms is obtained by only using 'R' messages. If only use 'P' messages, the time would be 0.1768 ms


## Critical Instant Analysis of Rate Monotonic Scheduler

The rate monotonic scheduler was analyzed for critical instant to ensure that all deadlines are met under worst-case conditions. The analysis showed that all tasks meet their deadlines with sufficient margin.

