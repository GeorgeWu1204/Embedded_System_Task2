## Tasks Breakdown
<!-- 16. An identification of all the tasks that are performed by the system with their method of implementation, thread or interrupt -->

## Time Analysis
<!-- 17. A characterisation of each task with its theoretical minimum initiation interval and measured maximum execution time
18. A critical instant analysis of the rate monotonic scheduler, showing that all deadlines are met under worst-case conditions  -->

## Data and Resource Sharing
<!-- 20. An identification of all the shared data structures and the methods used to guarantee safe accessand synchronisation -->

## Dependency
<!-- 21. An analysis of inter-task blocking dependencies that shows any possibility of deadlock -->

## CPU utilisation
<!-- 19. A quantification of total CPU utilisation  -->


# System Task Analysis

## Task Identification and Characterization

| Task | Task Description | Implementation Method | Theoretical Minimum Initiation Interval | Measured Maximum Execution Time |
| -----| -----------------|-----------------------|---------------------------------------- | ------------------------------- |
| scanKey | Identifies the keys pressed | Thread  | | |
| decode | Decode the received message from the msgInQ | Thread  | | |
| display | Display the information on the LED screen | Thread  | | |
| writeToDoubleBuffer | | Thread  | | |
| CAN_TX_Task | Push the message that is going to be sent into msgOutQ| Thread | | |    
| configuration  | Locate the position of the keyboard | Thread | | |
| sampleISR | | Interrupt |||
| CAN_RX_ISR | Receive message from CAN bus and put inside msgInQ| Interrupt |||
| CAN_TX_ISR | Push the message from msgOutQ onto the CAN bus| Interrupt |||

## Critical Instant Analysis of Rate Monotonic Scheduler

The rate monotonic scheduler was analyzed for critical instant to ensure that all deadlines are met under worst-case conditions. The analysis showed that all tasks meet their deadlines with sufficient margin.

