import { Process_table_row, Process_div } from './components.js';
function sort_process_queue(div, compare) {
    console.log('pre', div);
    // Convert HTMLCollection to array
    const childrenArray = Array.from(div.children);
    console.log('childrenArray', childrenArray);
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
    console.log('post', div);
}
// DOM elements
const algorithmSelect = document.getElementById('algorithm');
const speedInput = document.getElementById('speed');
const speedValueSpan = document.getElementById('speed-value');
const startButton = document.getElementById('start-btn');
const initialTableBody = document.querySelector('#initial-table tbody');
const notReadyQueueDiv = document.getElementById('not-ready-queue');
const readyQueueDiv = document.getElementById('ready-queue');
const running_process_div = document.getElementById('running-process');
const finishedQueueDiv = document.getElementById('finished-queue');
const currentTimeSpan = document.getElementById('current-time');
// Simulation state
const process_by_id = new Map();
const API_URL = '/api/solve';
// Update displayed speed value
function updateSpeedDisplay() {
    const speedValue = parseInt(speedInput.value) / 10; // Convert 1-50 to 0.1x-5x
    speedValueSpan.textContent = `${speedValue.toFixed(1)}x`;
}
// Fetch simulation data from API
async function fetch_frames(processesData, algorithm) {
    try {
        const response = await axios.post(API_URL, { algorithm: algorithm, processes: processesData }, { headers: { 'Content-Type': 'application/json' } });
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
}
// Compare queues to detect changes
function getQueueChanges(prevQueue, nextQueue) {
    const entering = nextQueue.filter(p => !prevQueue.includes(p));
    const exiting = prevQueue.filter(p => !nextQueue.includes(p));
    const living = nextQueue.filter(p => prevQueue.includes(p));
    return { entering, living, exiting };
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
function renderQueue(queueDiv, currentQueue, prevQueue, sort_by_priority = false) {
    const { entering, living, exiting } = getQueueChanges(prevQueue, currentQueue);
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
            // if (entering.includes(pid)) {
            //     requestAnimationFrame(() => div.classList.add('enter'));
            // }
        }
        else {
            processDiv.innerHTML = Process_div(process).innerHTML;
        }
    });
    entering.forEach(pid => {
        const div = Array.from(queueDiv.children).find(child => child.dataset.id === pid.toString());
        requestAnimationFrame(() => div.classList.add('enter'));
    });
    living.forEach(pid => {
        const div = Array.from(queueDiv.children).find(child => child.dataset.id === pid.toString());
        div.classList.remove('enter');
        requestAnimationFrame(() => div.classList.add('living'));
    });
    if (sort_by_priority) {
        console.log(entering, exiting);
        sort_process_queue(queueDiv, (lhs, rhs) => {
            if (lhs.extra.priority == undefined || rhs.extra.priority == undefined) {
                throw new Error('priority doesn\'t exist in process');
            }
            return lhs.extra.priority - rhs.extra.priority;
        });
    }
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
    console.log("Rendering frame: ", cur);
    render_processes_list(cur.processes);
    currentTimeSpan.textContent = cur.system_time.toString();
    renderQueue(notReadyQueueDiv, cur.not_ready_queue, prev.not_ready_queue ?? []);
    renderQueue(readyQueueDiv, cur.ready_queue, prev.ready_queue ?? [], true);
    renderQueue(finishedQueueDiv, cur.finish_queue, prev.finish_queue ?? []);
    renderQueue(running_process_div, cur.running_process ? [cur.running_process.id] : [], prev.running_process ? [prev.running_process.id] : []);
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
async function start_simulation() {
    console.log("Initializing simulation data.");
    const algorithm = algorithmSelect.value;
    console.log(algorithm);
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
    let frames = await fetch_frames(processes_data, algorithm);
    console.log("Frames: ", frames);
    if (frames.length === 0)
        return;
    // Initialize global variables
    currentTimeSpan.innerHTML = '0';
    notReadyQueueDiv.innerHTML = '';
    readyQueueDiv.innerHTML = '';
    running_process_div.innerHTML = '';
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
