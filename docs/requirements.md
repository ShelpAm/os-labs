# Requirements

## Common specifications

- [ ] Language: Chinese
- [ ] Allow for slower transition (animation) of elements.
- [ ] For every class of algorithm, make simulations to them.
- [ ] More detailed info should be displayed.
- [ ] Information should be displayed explicitly. (Every event should be shown)
- [ ] Interactive program, user can input as they want.
- [ ] High performance, specifically, support x clients accessing simultaneounsly.
- [ ] Support reading from file.
- [ ] Time should be displayed as HH:MM format.
- [ ] Show average value of stats.

## Process scheduling

### Communication JSON structures

-   Algorithms
    -   **first-come-first-served**: 
    -   **shortest-process-first**: 
    -   **round-robin**: 
    -   **priority-scheduling**: 

-   Process (for example):
    ```json
    {
        "id": "int",
        "name": "string",
        "arrival_time": "minutes",
        "total_execution_time": "int",
        "stats": {
            "start_time": "minutes",
            "finish_time": "minutes",
            "execution_time": "minutes",
            "remaining_time": "minutes",
            "turnaround": "int",
            "weighted_turnaround": "double",
            "status": "Status"
        },
        "extra": {
            "User defined variable": "User defined value",
            ...
        }
    }
    ```
-   A state frame:
    ```json
    {
        "system_time": "minutes",
        "processes": [Process_1, Process_2, ..., Process_n],
        "running_process": "Process",
        "not_ready_queue": ["id_1", "id_2", "...", "id_m"],
        "ready_queue": "Array of processes id",
        "finish_queue": "Array of processes id"
    }
    ```
-   Request (initial processes states):
    ```json
    {
        "algorithm": "Algorithm",
        "processes": [Process_1_without_runtime_info, Process_2_without_runtime_info, ..., Process_n_without_runtime_info]
    }
    ```
-   Response (State frames):
    ```json
    {
        "frames": [frame_1, frame_2, ..., frame_n]
    }
    ```

### Process-specific specifications

User can input process info from handing writing, file.

Common properties of a process:
- ID
- Arrival time
- Total execution time

Common runtime info of a process:
- Start time
- Finish time
- Execution time
- Remaining time
- Turnaround
- Weighed turnaround
- Status (not_started, ready, running, finished)

### Priority scheduling

Properties specific to priority scheduling:
- Priority


# Solutions

## Communication

We use JSON to perform communication between processes.
