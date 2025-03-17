export enum Status {
    not_started,
    ready,
    running,
    finished,
}

export interface Process {
    id: number,
    name: string,
    arrival_time: number,
    total_execution_time: number,
    runtime_info: {
        start_time?: number,
        finish_time?: number,
        execution_time?: number,
        remaining_time?: number,
        turnaround?: number,
        weighted_turnaround?: number,
        status?: Status
    },
    extra: {
        priority?: number,
    },
}

export interface Frame {
    system_time: number,
    processes: Array<Process>,
    running_process?: Process,
    not_ready_queue: Array<number>,
    ready_queue: Array<number>,
    finish_queue: Array<number>,
}
