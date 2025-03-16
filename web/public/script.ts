import { Frame, Process } from 'interfaces';

function Process_table_row(p: Process): HTMLTableRowElement {
    const row = document.createElement('tr');
    row.innerHTML = `
            <td style="width: 60px;"><input for="id" type="number" value="${p.id}" class="process-column" /></td>
            <td style="width: 100px;"><input for="name" type="text" value="${p.name}" class="process-column" /></td>
            <td style="width: 80px;"><input for="arrival_time" type="number" value="${p.arrival_time}" class="process-column" /></td>
            <td style="width: 80px;"><input for="total_execution_time" type="number" value="${p.total_execution_time}" class="process-column" /></td>
            <td style="width: 60px;"><input for="priority" type="number" value="${p.extra.priority ?? '-'}" class="process-column" /></td>
            <td style="width: 80px;">${p.runtime_info.start_time ?? '-'}</td>
            <td style="width: 80px;">${p.runtime_info.finish_time ?? '-'}</td>
            <td style="width: 80px;">${p.runtime_info.execution_time ?? '-'}</td>
            <td style="width: 80px;">${p.runtime_info.remaining_time ?? '-'}</td>
            <td style="width: 80px;">${p.runtime_info.turnaround ?? '-'}</td>
            <td style="width: 100px;">${p.runtime_info.weighted_turnaround ?? '-'}</td>
        `;
    return row;
}

// DOM elements
const algorithmSelect = document.getElementById('algorithm') as HTMLSelectElement;
const speedInput = document.getElementById('speed') as HTMLInputElement;
const speedValueSpan = document.getElementById('speed-value') as HTMLSpanElement;
const startButton = document.getElementById('start-btn') as HTMLButtonElement;
const initialTableBody = document.querySelector('#initial-table tbody') as HTMLTableElement;
const readyQueueDiv = document.getElementById('ready-queue') as HTMLDivElement;
const notReadyQueueDiv = document.getElementById('not-ready-queue') as HTMLDivElement;
const finishedQueueDiv = document.getElementById('finished-queue') as HTMLDivElement;
const currentTimeSpan = document.getElementById('current-time') as HTMLSpanElement;
const new_row_btn = document.getElementById('new-row') as HTMLButtonElement;


// Simulation state
const process_by_id = new Map<number, Process>();
const API_URL = '/api/solve';

// Update displayed speed value
function updateSpeedDisplay() {
    const speedValue = parseInt(speedInput.value) / 10; // Convert 1-50 to 0.1x-5x
    speedValueSpan.textContent = `${speedValue.toFixed(1)}x`;
}

// Fetch simulation data from API
async function fetch_frames(processesData: Array<Process>, algorithm: string): Promise<Array<Frame>> {
    try {
        const response = await axios.post(API_URL,
            { algorithm: algorithm, processes: processesData },
            { headers: { 'Content-Type': 'application/json' } });
        const data: any = response.data; // Axios auto-parses JSON
        return data.frames;
    } catch (error: any) {
        if (!error.response || !error.response.data) {
            console.log('Error: Cannot find axios');
        } else {
            console.error('Error fetching simulation data:', error.response.data);
        }
        alert('Failed to load simulation data. Check console for details.');
        return [];
    }
}

// Compare queues to detect changes
function getQueueChanges(prevQueue: Array<number>, nextQueue: Array<number>) {
    const entering = nextQueue.filter(p => !prevQueue.includes(p));
    const exiting = prevQueue.filter(p => !nextQueue.includes(p));
    return { entering, exiting };
}

function render_processes_list(processes: Array<Process>) {
    initialTableBody.innerHTML = '';
    processes.forEach(p => {
        const row = Process_table_row(p)
        initialTableBody.appendChild(row);
    });
}

// Render a queue with animations
function renderQueue(queueDiv: HTMLDivElement, currentQueue: Array<number>, prevQueue: Array<number>) {
    // queueDiv.innerHTML = ''
    const { entering, exiting } = getQueueChanges(prevQueue, currentQueue);

    // Slow here, but flexible and generic.
    exiting.forEach(pid => {
        const exitingDiv = Array.from(queueDiv.children).find(div => (div as HTMLDivElement).dataset.id === pid.toString());
        if (!exitingDiv) {
            throw new Error("Exiting div not found"); // Or handle it differently
        }
        exitingDiv.classList.add('exit');
        // exitingDiv.remove();
        setTimeout(() => exitingDiv.remove(), 400);
    });

    currentQueue.forEach(id => {
        const process = process_by_id.get(id) as Process;
        let processDiv = Array.from(queueDiv.children).find(div => (div as HTMLDivElement).dataset.id === id.toString());
        let div: HTMLDivElement;
        if (!processDiv) {
            div = document.createElement('div');
            if (!process_by_id.has(id)) {
                throw new Error('id doesn\'t exist in process_by_id');
            }
            div.innerHTML = `${id} (${((process_by_id).get(id) as Process).runtime_info.remaining_time ?? '-'})`;
            div.className = 'process';
            div.dataset.id = id.toString();
            queueDiv.appendChild(div);
            if (entering.includes(id)) {
                requestAnimationFrame(() => div.classList.add('enter'));
            }
        } else {
            processDiv.innerHTML = `${id} (${process.runtime_info.remaining_time ?? '-'})`;
        }
    });
}

// Render a single frame
function render_frame(cur: Frame, prev: Partial<Frame> = { not_ready_queue: [], ready_queue: [], finish_queue: [] }) {
    console.log("Rendering frame: ", cur)
    render_processes_list(cur.processes)
    currentTimeSpan.textContent = cur.system_time.toString();
    renderQueue(readyQueueDiv, cur.ready_queue, prev.ready_queue ?? []);
    renderQueue(notReadyQueueDiv, cur.not_ready_queue, prev.not_ready_queue ?? []);
    renderQueue(finishedQueueDiv, cur.finish_queue, prev.finish_queue ?? []);
}

let intervalId: number;
// Start the animation loop with the current speed
function start_animation(frames: Array<Frame>) {
    console.log("Starting animation.")
    console.log("Frames: ", frames);

    if (intervalId) clearInterval(intervalId);

    const base_interval = 1000; // Base interval in milliseconds (1 second)
    const speedValue = parseInt(speedInput.value) / 10; // 1-50 maps to 0.1x-5x
    const intervalDuration = base_interval / speedValue; // Higher speed = shorter interval

    // Mimics a for-loop
    let i = 0;
    intervalId = setInterval(() => {
        if (i < frames.length) {
            process_by_id.clear();
            frames[i].processes.forEach((p) => {
                process_by_id.set(p.id, p);
            });

            console.log("Current frame: ", i);
            console.log("Current processes: ", frames[i].processes);
            console.log("process_by_id: ", process_by_id);

            if (i == 0) {
                render_frame(frames[i]);
            } else {
                render_frame(frames[i], frames[i - 1]);
            }
        } else {
            console.log("Animation finished. Stopping.")
            clearInterval(intervalId);
        }
        ++i;
    }, intervalDuration);
}

function get_processes_from_list(table: HTMLTableElement) {
    return Array.from(table.querySelectorAll("tbody tr")).map(row => {
        let cells = row.children;

        const p: Process = {
            id: parseInt((cells[0].querySelector("input") as HTMLInputElement).value, 10),
            name: (cells[1].querySelector("input") as HTMLInputElement).value,
            arrival_time: parseInt((cells[2].querySelector("input") as HTMLInputElement).value, 10),
            total_execution_time: parseInt((cells[3].querySelector("input") as HTMLInputElement).value, 10),
            runtime_info: {}, // Empty runtime_info as per your example
            extra: { priority: parseInt((cells[4].querySelector("input") as HTMLInputElement).value, 10) },
        };

        return p;
    });
}

// Initialize simulation (fetch data and render first frame)
async function start_simulation() {
    console.log("Initializing simulation data.")
    const algorithm = algorithmSelect.value;


    function has_duplicate_ids(processes: Array<Process>) {
        const seen = new Set<number>();
        return processes.some(p => {
            if (seen.has(p.id)) return true; // Duplicate found
            seen.add(p.id);
            return false;
        });
    }
    const processes_data = get_processes_from_list(initialTableBody);
    if (has_duplicate_ids(processes_data)) {
        alert('Duplicate IDs found! You should make every id of processes unique.');
        return;
    }

    let frames = await fetch_frames(processes_data, algorithm);
    console.log("Frames: ", frames);
    if (frames.length === 0) return;

    // Initialize global variables
    readyQueueDiv.innerHTML = '';
    notReadyQueueDiv.innerHTML = '';
    finishedQueueDiv.innerHTML = '';
    start_animation(frames); // Start the animation after initialization
}

// Update speed without restarting the sequence
function updateSpeed() {
    // TODO not implemented

    // if (frames.length === 0 || currentFrame >= frames.length) return; // No active simulation
    // startAnimation(); // Restart interval with new speed, keeping current frame
}


// Initialize speed display
updateSpeedDisplay();

// Event listeners
startButton.addEventListener('click', start_simulation);
speedInput.addEventListener('input', () => {
    updateSpeedDisplay();
    updateSpeed(); // Adjust speed without resetting
});

const example_processes: Array<Process> = [
    {
        id: 1001,
        name: 'Love in the Dark',
        arrival_time: 2,
        total_execution_time: 3,
        runtime_info: {},
        extra: { priority: 2, },
    },
    {
        id: 1206,
        name: 'zyx',
        arrival_time: 3,
        total_execution_time: 5,
        runtime_info: {},
        extra: { priority: 1, },
    },
    {
        id: 817,
        name: 'yyx',
        arrival_time: 5,
        total_execution_time: 1,
        runtime_info: {},
        extra: { priority: 3, },
    },
];

example_processes.forEach(p => {
    initialTableBody.appendChild(Process_table_row(p)); // Add an example to the list
});

new_row_btn.addEventListener('click', () => {
    initialTableBody.appendChild(Process_table_row(example_processes[0]))
});

// render_processes_list([example_proc]); // Initialize the list

