function raw(strings: TemplateStringsArray, ...values: any[]): string {
    return strings.raw[0]; // Access the raw string content
}

export class FUCK {
    private minutes: number;

    private constructor(t: number) {
        this.minutes = t;
    }

    static from_string(s: string): FUCK {
        const r = s.match(raw`(\d+):(\d+)`);
        const hours = Number(r?.at(1) as string)
        const minutes = Number(r?.at(2) as string)
        return new FUCK(hours * 60 + minutes);
    }

    static from_time(minutes: number): FUCK {
        return new FUCK(minutes);
    }

    toString(): string {
        const hours = Math.floor(this.minutes / 60);
        const minutes = this.minutes % 60;
        return `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}`;
    }

    getMinutes(): number {
        return this.minutes;
    }
}

export type Algorithm = 'first_come_first_served' | 'shortest_process_first' | 'round_robin' | 'priority_scheduling';

export type Time_point = number;

export enum Status {
    not_started,
    ready,
    running,
    finished,
}

export interface Process {
    id: number,
    name: string,
    arrival_time: Time_point,
    total_execution_time: number,
    stats: {
        start_time?: Time_point,
        finish_time?: Time_point,
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
    system_time: Time_point,
    processes: Array<Process>,
    running_process?: Process,
    not_ready_queue: Array<number>,
    ready_queue: Array<number>,
    finish_queue: Array<number>,
    extra: {
        "cpu.slice_execution_time": number,
    },
}

export interface Request {
    algorithm: Algorithm,
    processes: Array<Process>,
    extra: {
        time_quantum?: number,
    },
};

export interface Response {
    frames: Array<Frame>;
};
