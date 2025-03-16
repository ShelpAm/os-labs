var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
function Process_table_row(p) {
    var _a, _b, _c, _d, _e, _f, _g;
    const row = document.createElement('tr');
    row.innerHTML = `
            <td style="width: 60px;"><input for="id" type="number" value="${p.id}" class="process-column" /></td>
            <td style="width: 100px;"><input for="name" type="text" value="${p.name}" class="process-column" /></td>
            <td style="width: 80px;"><input for="arrival_time" type="number" value="${p.arrival_time}" class="process-column" /></td>
            <td style="width: 80px;"><input for="total_execution_time" type="number" value="${p.total_execution_time}" class="process-column" /></td>
            <td style="width: 60px;"><input for="priority" type="number" value="${(_a = p.extra.priority) !== null && _a !== void 0 ? _a : '-'}" class="process-column" /></td>
            <td style="width: 80px;">${(_b = p.runtime_info.start_time) !== null && _b !== void 0 ? _b : '-'}</td>
            <td style="width: 80px;">${(_c = p.runtime_info.finish_time) !== null && _c !== void 0 ? _c : '-'}</td>
            <td style="width: 80px;">${(_d = p.runtime_info.execution_time) !== null && _d !== void 0 ? _d : '-'}</td>
            <td style="width: 80px;">${(_e = p.runtime_info.remaining_time) !== null && _e !== void 0 ? _e : '-'}</td>
            <td style="width: 80px;">${(_f = p.runtime_info.turnaround) !== null && _f !== void 0 ? _f : '-'}</td>
            <td style="width: 100px;">${(_g = p.runtime_info.weighted_turnaround) !== null && _g !== void 0 ? _g : '-'}</td>
        `;
    return row;
}
function Process_div(p) {
    var _a;
    const div = document.createElement('div');
    div.innerHTML = `${p.id} (${(_a = p.runtime_info.remaining_time) !== null && _a !== void 0 ? _a : '-'})`;
    div.className = 'process';
    div.dataset.id = p.id.toString();
    return div;
}
// DOM elements
const algorithmSelect = document.getElementById('algorithm');
const speedInput = document.getElementById('speed');
const speedValueSpan = document.getElementById('speed-value');
const startButton = document.getElementById('start-btn');
const initialTableBody = document.querySelector('#initial-table tbody');
const readyQueueDiv = document.getElementById('ready-queue');
const notReadyQueueDiv = document.getElementById('not-ready-queue');
const finishedQueueDiv = document.getElementById('finished-queue');
const currentTimeSpan = document.getElementById('current-time');
const new_row_btn = document.getElementById('new-row');
// Simulation state
const process_by_id = new Map();
const API_URL = '/api/solve';
// Update displayed speed value
function updateSpeedDisplay() {
    const speedValue = parseInt(speedInput.value) / 10; // Convert 1-50 to 0.1x-5x
    speedValueSpan.textContent = `${speedValue.toFixed(1)}x`;
}
// Fetch simulation data from API
function fetch_frames(processesData, algorithm) {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const response = yield axios.post(API_URL, { algorithm: algorithm, processes: processesData }, { headers: { 'Content-Type': 'application/json' } });
            const data = response.data; // Axios auto-parses JSON
            return data.frames;
        }
        catch (error) {
            if (!error.response || !error.response.data) {
                console.log('Error: Cannot find axios');
            }
            else {
                console.error('Error fetching simulation data:', error.response.data);
            }
            alert('Failed to load simulation data. Check console for details.');
            return [];
        }
    });
}
// Compare queues to detect changes
function getQueueChanges(prevQueue, nextQueue) {
    const entering = nextQueue.filter(p => !prevQueue.includes(p));
    const exiting = prevQueue.filter(p => !nextQueue.includes(p));
    return { entering, exiting };
}
function render_processes_list(processes) {
    initialTableBody.innerHTML = '';
    processes.forEach(p => {
        const row = Process_table_row(p);
        initialTableBody.appendChild(row);
    });
}
// Render a queue with animations
// method: Clear and redraw the quque.
function renderQueue(queueDiv, currentQueue, prevQueue) {
    const { entering, exiting } = getQueueChanges(prevQueue, currentQueue);
    currentQueue.forEach(pid => {
        if (!process_by_id.has(pid)) {
            throw new Error('id doesn\'t exist in process_by_id');
        }
        const processDiv = Array.from(queueDiv.children).find(div => div.dataset.id === pid.toString());
        const process = process_by_id.get(pid);
        // Create or update
        if (!processDiv) {
            const div = Process_div(process);
            queueDiv.appendChild(div);
            if (entering.includes(pid)) {
                requestAnimationFrame(() => div.classList.add('enter'));
            }
        }
        else {
            processDiv.innerHTML = Process_div(process).innerHTML;
        }
    });
    function sort_process_queue(div, compare) {
        // Convert HTMLCollection to array
        const childrenArray = Array.from(div.children);
        // Sort by textContent
        childrenArray.sort((lhs, rhs) => {
            const l = lhs;
            const pl = process_by_id.get(Number(l.dataset.id));
            const r = rhs;
            const pr = process_by_id.get(Number(r.dataset.id));
            return compare(pl, pr);
        });
        // Clear and re-append
        div.replaceChildren();
        childrenArray.forEach(child => div.appendChild(child));
    }
    sort_process_queue(queueDiv, (lhs, rhs) => {
        if (lhs.extra.priority == undefined || rhs.extra.priority == undefined) {
            throw new Error('priority doesn\'t exist in process');
        }
        return lhs.extra.priority - rhs.extra.priority;
    });
    // Slow here, but flexible and generic.
    exiting.forEach(pid => {
        const exitingDiv = Array.from(queueDiv.children).find(div => div.dataset.id === pid.toString());
        if (!exitingDiv) {
            throw new Error("Exiting div not found"); // Or handle it differently
        }
        exitingDiv.classList.add('exit');
        // exitingDiv.remove();
        setTimeout(() => exitingDiv.remove(), 400);
    });
}
// Render a single frame
function render_frame(cur, prev = { not_ready_queue: [], ready_queue: [], finish_queue: [] }) {
    var _a, _b, _c;
    console.log("Rendering frame: ", cur);
    render_processes_list(cur.processes);
    currentTimeSpan.textContent = cur.system_time.toString();
    renderQueue(notReadyQueueDiv, cur.not_ready_queue, (_a = prev.not_ready_queue) !== null && _a !== void 0 ? _a : []);
    renderQueue(readyQueueDiv, cur.ready_queue, (_b = prev.ready_queue) !== null && _b !== void 0 ? _b : []);
    renderQueue(finishedQueueDiv, cur.finish_queue, (_c = prev.finish_queue) !== null && _c !== void 0 ? _c : []);
}
let intervalId;
// Start the animation loop with the current speed
function start_animation(frames) {
    console.log("Starting animation.");
    console.log("Frames: ", frames);
    if (intervalId)
        clearInterval(intervalId);
    const base_interval = 1000; // Base interval in milliseconds (1 second)
    const speedValue = parseInt(speedInput.value) / 10; // 1-50 maps to 0.1x-5x
    const intervalDuration = base_interval / speedValue; // Higher speed = shorter interval
    // Mimics a for-loop
    let i = 0;
    intervalId = setInterval(() => {
        if (i < frames.length) {
            try {
                process_by_id.clear();
                frames[i].processes.forEach((p) => {
                    process_by_id.set(p.id, p);
                });
                console.log("Current frame: ", i);
                if (i == 0) {
                    render_frame(frames[i]);
                }
                else {
                    render_frame(frames[i], frames[i - 1]);
                }
            }
            catch (error) {
                console.log('Error:', error);
                clearInterval(intervalId);
            }
        }
        else {
            console.log("Animation finished. Stopping.");
            clearInterval(intervalId);
        }
        ++i;
    }, intervalDuration);
}
function get_processes_from_list(table) {
    return Array.from(table.querySelectorAll("tbody tr")).map(row => {
        let cells = row.children;
        const p = {
            id: parseInt(cells[0].querySelector("input").value, 10),
            name: cells[1].querySelector("input").value,
            arrival_time: parseInt(cells[2].querySelector("input").value, 10),
            total_execution_time: parseInt(cells[3].querySelector("input").value, 10),
            runtime_info: {}, // Empty runtime_info as per your example
            extra: { priority: parseInt(cells[4].querySelector("input").value, 10) },
        };
        return p;
    });
}
// Initialize simulation (fetch data and render first frame)
function start_simulation() {
    return __awaiter(this, void 0, void 0, function* () {
        console.log("Initializing simulation data.");
        const algorithm = algorithmSelect.value;
        function has_duplicate_ids(processes) {
            const seen = new Set();
            return processes.some(p => {
                if (seen.has(p.id))
                    return true; // Duplicate found
                seen.add(p.id);
                return false;
            });
        }
        const processes_data = get_processes_from_list(initialTableBody);
        if (has_duplicate_ids(processes_data)) {
            alert('Duplicate IDs found! You should make every id of processes unique.');
            return;
        }
        let frames = yield fetch_frames(processes_data, algorithm);
        console.log("Frames: ", frames);
        if (frames.length === 0)
            return;
        // Initialize global variables
        readyQueueDiv.innerHTML = '';
        notReadyQueueDiv.innerHTML = '';
        finishedQueueDiv.innerHTML = '';
        start_animation(frames); // Start the animation after initialization
    });
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
const example_processes = [
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
    initialTableBody.appendChild(Process_table_row(example_processes[0]));
});
export {};
// render_processes_list([example_proc]); // Initialize the list
