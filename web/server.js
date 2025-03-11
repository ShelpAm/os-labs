const express = require('express');

const app = express();

const port = 3000;

// Simulated process data with additional fields
const processes = [
    { id: 'P1', name: 'Process A', arrival: 0, burst: 5, totalTime: 5, priority: 2 },
    { id: 'P2', name: 'Process B', arrival: 1, burst: 3, totalTime: 3, priority: 1 },
    { id: 'P3', name: 'Process C', arrival: 2, burst: 4, totalTime: 4, priority: 3 },
    { id: 'P4', name: 'Process D', arrival: 3, burst: 2, totalTime: 2, priority: 4 }
];

// Helper function to deep copy process objects
function copyProcesses(procs) {
    return procs.map(p => ({ ...p }));
}

// FCFS Simulation
function simulateFCFS(processes) {
    let time = 0;
    let frames = [];
    let queue = copyProcesses(processes).sort((a, b) => a.arrival - b.arrival);
    let notReady = copyProcesses(processes);
    let ready = [];
    let finished = [];

    while (queue.length > 0 || ready.length > 0 || notReady.length > 0) {
        while (queue.length > 0 && queue[0].arrival <= time) {
            const proc = queue.shift();
            ready.push(proc);
            notReady = notReady.filter(p => p.id !== proc.id);
        }

        frames.push({
            time,
            ready: ready.map(p => ({ id: p.id, burst: p.burst })),
            notReady: notReady.map(p => ({ id: p.id, burst: p.burst })),
            finished: finished.map(p => ({ id: p.id, burst: 0 }))
        });

        if (ready.length > 0) {
            ready[0].burst--;
            if (ready[0].burst === 0) {
                finished.push(ready.shift());
            }
        }
        time++;
    }
    frames.push({
        time,
        ready: ready.map(p => ({ id: p.id, burst: p.burst })),
        notReady: notReady.map(p => ({ id: p.id, burst: p.burst })),
        finished: finished.map(p => ({ id: p.id, burst: 0 }))
    });
    return frames;
}

// SJF Simulation
function simulateSJF(processes) {
    let time = 0;
    let frames = [];
    let queue = copyProcesses(processes).sort((a, b) => a.arrival - b.arrival);
    let ready = [];
    let notReady = copyProcesses(processes);
    let finished = [];

    while (queue.length > 0 || ready.length > 0 || notReady.length > 0) {
        while (queue.length > 0 && queue[0].arrival <= time) {
            const proc = queue.shift();
            ready.push(proc);
            notReady = notReady.filter(p => p.id !== proc.id);
        }

        ready.sort((a, b) => a.burst - b.burst);
        frames.push({
            time,
            ready: ready.map(p => ({ id: p.id, burst: p.burst })),
            notReady: notReady.map(p => ({ id: p.id, burst: p.burst })),
            finished: finished.map(p => ({ id: p.id, burst: 0 }))
        });

        if (ready.length > 0) {
            ready[0].burst--;
            if (ready[0].burst === 0) {
                finished.push(ready.shift());
            }
        }
        time++;
    }
    return frames;
}

// Round Robin Simulation (quantum = 2)
function simulateRR(processes, quantum = 2) {
    let time = 0;
    let frames = [];
    let queue = copyProcesses(processes).sort((a, b) => a.arrival - b.arrival);
    let ready = [];
    let notReady = copyProcesses(processes);
    let finished = [];
    let currentQuantum = 0;

    while (queue.length > 0 || ready.length > 0 || notReady.length > 0) {
        while (queue.length > 0 && queue[0].arrival <= time) {
            const proc = queue.shift();
            ready.push(proc);
            notReady = notReady.filter(p => p.id !== proc.id);
        }

        frames.push({
            time,
            ready: ready.map(p => ({ id: p.id, burst: p.burst })),
            notReady: notReady.map(p => ({ id: p.id, burst: p.burst })),
            finished: finished.map(p => ({ id: p.id, burst: 0 }))
        });

        if (ready.length > 0) {
            ready[0].burst--;
            currentQuantum++;
            if (ready[0].burst === 0) {
                finished.push(ready.shift());
                currentQuantum = 0;
            } else if (currentQuantum === quantum) {
                ready.push(ready.shift());
                currentQuantum = 0;
            }
        }
        time++;
    }
    return frames;
}

// Priority Simulation
function simulatePriority(processes) {
    let time = 0;
    let frames = [];
    let queue = copyProcesses(processes).sort((a, b) => a.arrival - b.arrival);
    let ready = [];
    let notReady = copyProcesses(processes);
    let finished = [];

    while (queue.length > 0 || ready.length > 0 || notReady.length > 0) {
        while (queue.length > 0 && queue[0].arrival <= time) {
            const proc = queue.shift();
            ready.push(proc);
            notReady = notReady.filter(p => p.id !== proc.id);
        }

        ready.sort((a, b) => a.priority - b.priority);
        frames.push({
            time,
            ready: ready.map(p => ({ id: p.id, burst: p.burst })),
            notReady: notReady.map(p => ({ id: p.id, burst: p.burst })),
            finished: finished.map(p => ({ id: p.id, burst: 0 }))
        });

        if (ready.length > 0) {
            ready[0].burst--;
            if (ready[0].burst === 0) {
                finished.push(ready.shift());
            }
        }
        time++;
    }
    return frames;
}

// API endpoint
app.get('/api/simulate', (req, res) => {
    const algorithm = req.query.algorithm;
    let frames;

    switch (algorithm) {
        case 'fcfs':
            frames = simulateFCFS(processes);
            break;
        case 'sjf':
            frames = simulateSJF(processes);
            break;
        case 'rr':
            frames = simulateRR(processes);
            break;
        case 'priority':
            frames = simulatePriority(processes);
            break;
        default:
            return res.status(400).json({ error: 'Invalid algorithm' });
    }

    res.json({
        initialProcesses: processes.map(p => ({
            id: p.id,
            name: p.name,
            arrival: p.arrival,
            totalTime: p.totalTime,
            priority: p.priority
        })),
        frames
    });
});

app.use(express.static('public'));

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});
