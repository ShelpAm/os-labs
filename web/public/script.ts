"use strict";

(function() {
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

    enum Status {
        not_started,
        ready,
        running,
        finished,
    }

    interface Process {
        id: number,
        name: string,
        arrival_time: number,
        total_execution_time: number,
        priority: number,
        runtime_info: {
            start_time: number,
            finish_time: number,
            execution_time: number,
            remaining_time: number,
            turnaround: number,
            weighted_turnaround: number,
            status: Status
        },
    }

    interface Frame {
        system_time: number,
        processes: Array<Process>,
        not_ready_queue: Array<number>,
        ready_queue: Array<number>,
        finish_queue: Array<number>,
    }

    // Simulation state
    let process_by_id = new Map<number, Process>();


    /** @type {typeof import('axios').default} */
    const axios = (window as any).axios;

    const API_URL = '/api/solve';

    // Update displayed speed value
    function updateSpeedDisplay() {
        const speedValue = parseInt(speedInput.value) / 10; // Convert 1-50 to 0.1x-5x
        speedValueSpan.textContent = `${speedValue.toFixed(1)}x`;
    }

    // Fetch simulation data from API
    async function fetch_frames(algorithm: string): Promise<Array<Frame>> {
        const processesData = [{
            id: 1206,
            name: 'zyx',
            arrival_time: 3,
            total_execution_time: 5,
            priority: 1,
        }, {
            id: 817,
            name: 'yyx',
            arrival_time: 5,
            total_execution_time: 1,
            priority: 3,
        }]
        try {
            const response = await axios.post(API_URL, { processes: processesData }, {
                headers: { 'Content-Type': 'application/json' }
            });
            const data = response.data; // Axios auto-parses JSON
            return data.frames;
        } catch (error: any) {
            console.error('Error fetching simulation data:', error.response.data);
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
            const row = document.createElement('tr');
            row.innerHTML = `
            <td>${p.id}</td>
            <td>${p.name}</td>
            <td>${p.arrival_time}</td>
            <td>${p.total_execution_time}</td>
            <td>${p.priority !== undefined ? p.priority : '-'}</td>
            <td>${p.runtime_info.start_time !== undefined ? p.runtime_info.start_time : '-'}</td>
            <td>${p.runtime_info.finish_time !== undefined ? p.runtime_info.finish_time : '-'}</td>
            <td>${p.runtime_info.execution_time !== undefined ? p.runtime_info.execution_time : '-'}</td>
            <td>${p.runtime_info.remaining_time !== undefined ? p.runtime_info.remaining_time : '-'}</td>
        `;
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
            exitingDiv.remove();
            // setTimeout(() => exitingDiv.remove(), 400);
        });

        currentQueue.forEach(id => {
            let processDiv = Array.from(queueDiv.children).find(div => (div as HTMLDivElement).dataset.id === id.toString());
            if (!processDiv) {
                let div = document.createElement('div');
                if (!process_by_id.has(id)) {
                    throw new Error('id isn\'t in process_by_id');
                }
                div.innerHTML = `${id} (${((process_by_id).get(id) as Process).runtime_info.remaining_time})`;
                div.className = 'process';
                div.dataset.id = id.toString();
                queueDiv.appendChild(div);
                if (entering.includes(id)) {
                    requestAnimationFrame(() => div.classList.add('enter'));
                }
            }
            // processDiv.style.order = index;
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

    // Initialize simulation (fetch data and render first frame)
    async function start_simulation() {
        console.log("Initializing simulation data.")
        const algorithm = algorithmSelect.value;

        let frames = await fetch_frames(algorithm);
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

})();

