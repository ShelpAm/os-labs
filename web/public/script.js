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

// Simulation state
let simulationData = [];
let initialProcesses = [];
let currentFrame = 0;
let intervalId = null;

const API_URL = '/api/simulate';
const BASE_INTERVAL = 1000; // Base interval in milliseconds (1 second)

// Update displayed speed value
function updateSpeedDisplay() {
    const speedValue = parseInt(speedInput.value) / 10; // Convert 1-50 to 0.1x-5x
    speedValueSpan.textContent = `${speedValue.toFixed(1)}x`;
}

// Fetch simulation data from API
async function fetchSimulationData(algorithm) {
    try {
        const response = await fetch(`${API_URL}?algorithm=${algorithm}`);
        if (!response.ok) throw new Error('API request failed');
        const data = await response.json();
        initialProcesses = data.initialProcesses;
        return data.frames;
    } catch (error) {
        console.error('Error fetching simulation data:', error);
        alert('Failed to load simulation data. Check console for details.');
        return [];
    }
}

// Render initial process list
function renderInitialProcesses() {
    initialTableBody.innerHTML = '';
    initialProcesses.forEach(proc => {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${proc.id}</td>
            <td>${proc.name}</td>
            <td>${proc.arrival}</td>
            <td>${proc.totalTime}</td>
            <td>${proc.priority !== undefined ? proc.priority : '-'}</td>
        `;
        initialTableBody.appendChild(row);
    });
}

// Compare queues to detect changes
function getQueueChanges(prevQueue, nextQueue) {
    const prevIds = prevQueue.map(p => p.id);
    const nextIds = nextQueue.map(p => p.id);
    const entering = nextQueue.filter(p => !prevIds.includes(p.id));
    const exiting = prevQueue.filter(p => !nextIds.includes(p.id));
    return { entering, exiting };
}

// Render a queue with animations
function renderQueue(queueDiv, currentQueue, prevQueue = []) {
    const { entering, exiting } = getQueueChanges(prevQueue, currentQueue);

    exiting.forEach(exitProc => {
        const exitingDiv = Array.from(queueDiv.children).find(
            div => div.dataset.id === exitProc.id
        );
        if (exitingDiv) {
            exitingDiv.classList.add('exit');
            setTimeout(() => exitingDiv.remove(), 400);
        }
    });

    currentQueue.forEach((process, index) => {
        let processDiv = Array.from(queueDiv.children).find(
            div => div.dataset.id === process.id
        );

        if (!processDiv) {
            processDiv = document.createElement('div');
            processDiv.className = 'process';
            processDiv.dataset.id = process.id;
            queueDiv.appendChild(processDiv);
            if (entering.some(p => p.id === process.id)) {
                requestAnimationFrame(() => processDiv.classList.add('enter'));
            }
        }

        processDiv.textContent = `${process.id} (${process.burst})`;
        processDiv.style.order = index;
    });
}

// Render a single frame
function renderFrame(frame, prevFrame = { ready: [], notReady: [], finished: [] }) {
    currentTimeSpan.textContent = frame.time;
    renderQueue(readyQueueDiv, frame.ready, prevFrame.ready);
    renderQueue(notReadyQueueDiv, frame.notReady, prevFrame.notReady);
    renderQueue(finishedQueueDiv, frame.finished, prevFrame.finished);
}

// Start the animation loop with the current speed
function startAnimation() {
    console.log(simulationData)
    if (intervalId) clearInterval(intervalId);

    const speedValue = parseInt(speedInput.value) / 10; // 1-50 maps to 0.1x-5x
    const intervalDuration = BASE_INTERVAL / speedValue; // Higher speed = shorter interval

    intervalId = setInterval(() => {
        currentFrame++;
        if (currentFrame < simulationData.length) {
            renderFrame(simulationData[currentFrame], simulationData[currentFrame - 1]);
        } else {
            clearInterval(intervalId);
        }
    }, intervalDuration);
}

// Initialize simulation (fetch data and render first frame)
function initializeSimulation() {
    const algorithm = algorithmSelect.value;
    currentFrame = 0;

    fetchSimulationData(algorithm).then(data => {
        simulationData = data;
        if (simulationData.length === 0) return;

        renderInitialProcesses();
        readyQueueDiv.innerHTML = '';
        notReadyQueueDiv.innerHTML = '';
        finishedQueueDiv.innerHTML = '';
        renderFrame(simulationData[0]);
        startAnimation(); // Start the animation after initialization
    });
}

// Update speed without restarting the sequence
function updateSpeed() {
    if (simulationData.length === 0 || currentFrame >= simulationData.length) return; // No active simulation
    startAnimation(); // Restart interval with new speed, keeping current frame
}

// Event listeners
startButton.addEventListener('click', initializeSimulation);
speedInput.addEventListener('input', () => {
    updateSpeedDisplay();
    updateSpeed(); // Adjust speed without resetting
});

// Initialize speed display
updateSpeedDisplay();
