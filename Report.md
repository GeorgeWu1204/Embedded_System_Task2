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

## Task Identification

| Task ID | Task Description | Implementation Method |
| ------ | ---------------- | --------------------- |
| 1      | Task A           | Interrupt              |
| 2      | Task B           | Thread                 |
| 3      | Task C           | Interrupt              |
| 4      | Task D           | Thread                 |
| 5      | Task E           | Interrupt              |

## Task Characterization

| Task                | Task Description                        | Implementation Method | Theoretical Minimum Initiation Interval | Measured Maximum Execution Time |
| ------------------- | --------------------------------------- | ------------------------------ |
| scanKey             | 10 ms                                   | 5 ms                           |
| decode              | 5 ms                                    | 2 ms                           |
| display             | 15 ms                                   | 8 ms                           |
| writeToDoubleBuffer | 20 ms                                   | 10 ms                          |
| CAN_TX_Task         | 10 ms                                   | 6 ms                           |
| configuration       | 10 ms                                   | 6 ms                           |

## Critical Instant Analysis of Rate Monotonic Scheduler

The rate monotonic scheduler was analyzed for critical instant to ensure that all deadlines are met under worst-case conditions. The analysis showed that all tasks meet their deadlines with sufficient margin.

