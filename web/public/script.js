"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (g && (g = 0, op[0] && (_ = 0)), _) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
(function () {
    function make_process_table_row(p) {
        var _a, _b, _c, _d, _e, _f, _g;
        var row = document.createElement('tr');
        row.innerHTML = "\n            <td style=\"width: 60px;\"><input for=\"id\" type=\"number\" value=\"".concat(p.id, "\" class=\"process-column\" /></td>\n            <td style=\"width: 100px;\"><input for=\"name\" type=\"text\" value=\"").concat(p.name, "\" class=\"process-column\" /></td>\n            <td style=\"width: 80px;\"><input for=\"arrival_time\" type=\"number\" value=\"").concat(p.arrival_time, "\" class=\"process-column\" /></td>\n            <td style=\"width: 80px;\"><input for=\"total_execution_time\" type=\"number\" value=\"").concat(p.total_execution_time, "\" class=\"process-column\" /></td>\n            <td style=\"width: 60px;\"><input for=\"priority\" type=\"number\" value=\"").concat((_a = p.extra.priority) !== null && _a !== void 0 ? _a : '-', "\" class=\"process-column\" /></td>\n            <td style=\"width: 80px;\">").concat((_b = p.runtime_info.start_time) !== null && _b !== void 0 ? _b : '-', "</td>\n            <td style=\"width: 80px;\">").concat((_c = p.runtime_info.finish_time) !== null && _c !== void 0 ? _c : '-', "</td>\n            <td style=\"width: 80px;\">").concat((_d = p.runtime_info.execution_time) !== null && _d !== void 0 ? _d : '-', "</td>\n            <td style=\"width: 80px;\">").concat((_e = p.runtime_info.remaining_time) !== null && _e !== void 0 ? _e : '-', "</td>\n            <td style=\"width: 80px;\">").concat((_f = p.runtime_info.turnaround) !== null && _f !== void 0 ? _f : '-', "</td>\n            <td style=\"width: 100px;\">").concat((_g = p.runtime_info.weighted_turnaround) !== null && _g !== void 0 ? _g : '-', "</td>\n        ");
        return row;
    }
    // DOM elements
    var algorithmSelect = document.getElementById('algorithm');
    var speedInput = document.getElementById('speed');
    var speedValueSpan = document.getElementById('speed-value');
    var startButton = document.getElementById('start-btn');
    var initialTableBody = document.querySelector('#initial-table tbody');
    var readyQueueDiv = document.getElementById('ready-queue');
    var notReadyQueueDiv = document.getElementById('not-ready-queue');
    var finishedQueueDiv = document.getElementById('finished-queue');
    var currentTimeSpan = document.getElementById('current-time');
    var new_row_btn = document.getElementById('new-row');
    var Status;
    (function (Status) {
        Status[Status["not_started"] = 0] = "not_started";
        Status[Status["ready"] = 1] = "ready";
        Status[Status["running"] = 2] = "running";
        Status[Status["finished"] = 3] = "finished";
    })(Status || (Status = {}));
    // Simulation state
    var process_by_id = new Map();
    /** @type {typeof import('axios').default} */
    var axios = window.axios;
    var API_URL = '/api/solve';
    // Update displayed speed value
    function updateSpeedDisplay() {
        var speedValue = parseInt(speedInput.value) / 10; // Convert 1-50 to 0.1x-5x
        speedValueSpan.textContent = "".concat(speedValue.toFixed(1), "x");
    }
    // Fetch simulation data from API
    function fetch_frames(processesData, algorithm) {
        return __awaiter(this, void 0, void 0, function () {
            var response, data, error_1;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _a.trys.push([0, 2, , 3]);
                        return [4 /*yield*/, axios.post(API_URL, { processes: processesData }, {
                                headers: { 'Content-Type': 'application/json' }
                            })];
                    case 1:
                        response = _a.sent();
                        data = response.data;
                        return [2 /*return*/, data.frames];
                    case 2:
                        error_1 = _a.sent();
                        console.error('Error fetching simulation data:', error_1.response.data);
                        alert('Failed to load simulation data. Check console for details.');
                        return [2 /*return*/, []];
                    case 3: return [2 /*return*/];
                }
            });
        });
    }
    // Compare queues to detect changes
    function getQueueChanges(prevQueue, nextQueue) {
        var entering = nextQueue.filter(function (p) { return !prevQueue.includes(p); });
        var exiting = prevQueue.filter(function (p) { return !nextQueue.includes(p); });
        return { entering: entering, exiting: exiting };
    }
    function render_processes_list(processes) {
        initialTableBody.innerHTML = '';
        processes.forEach(function (p) {
            var row = make_process_table_row(p);
            initialTableBody.appendChild(row);
        });
    }
    // Render a queue with animations
    function renderQueue(queueDiv, currentQueue, prevQueue) {
        // queueDiv.innerHTML = ''
        var _a = getQueueChanges(prevQueue, currentQueue), entering = _a.entering, exiting = _a.exiting;
        // Slow here, but flexible and generic.
        exiting.forEach(function (pid) {
            var exitingDiv = Array.from(queueDiv.children).find(function (div) { return div.dataset.id === pid.toString(); });
            if (!exitingDiv) {
                throw new Error("Exiting div not found"); // Or handle it differently
            }
            exitingDiv.classList.add('exit');
            // exitingDiv.remove();
            setTimeout(function () { return exitingDiv.remove(); }, 400);
        });
        currentQueue.forEach(function (id) {
            var _a, _b;
            var process = process_by_id.get(id);
            var processDiv = Array.from(queueDiv.children).find(function (div) { return div.dataset.id === id.toString(); });
            var div;
            if (!processDiv) {
                div = document.createElement('div');
                if (!process_by_id.has(id)) {
                    throw new Error('id doesn\'t exist in process_by_id');
                }
                div.innerHTML = "".concat(id, " (").concat((_a = (process_by_id).get(id).runtime_info.remaining_time) !== null && _a !== void 0 ? _a : '-', ")");
                div.className = 'process';
                div.dataset.id = id.toString();
                queueDiv.appendChild(div);
                if (entering.includes(id)) {
                    requestAnimationFrame(function () { return div.classList.add('enter'); });
                }
            }
            else {
                processDiv.innerHTML = "".concat(id, " (").concat((_b = process.runtime_info.remaining_time) !== null && _b !== void 0 ? _b : '-', ")");
            }
        });
    }
    // Render a single frame
    function render_frame(cur, prev) {
        var _a, _b, _c;
        if (prev === void 0) { prev = { not_ready_queue: [], ready_queue: [], finish_queue: [] }; }
        console.log("Rendering frame: ", cur);
        render_processes_list(cur.processes);
        currentTimeSpan.textContent = cur.system_time.toString();
        renderQueue(readyQueueDiv, cur.ready_queue, (_a = prev.ready_queue) !== null && _a !== void 0 ? _a : []);
        renderQueue(notReadyQueueDiv, cur.not_ready_queue, (_b = prev.not_ready_queue) !== null && _b !== void 0 ? _b : []);
        renderQueue(finishedQueueDiv, cur.finish_queue, (_c = prev.finish_queue) !== null && _c !== void 0 ? _c : []);
    }
    var intervalId;
    // Start the animation loop with the current speed
    function start_animation(frames) {
        console.log("Starting animation.");
        console.log("Frames: ", frames);
        if (intervalId)
            clearInterval(intervalId);
        var base_interval = 1000; // Base interval in milliseconds (1 second)
        var speedValue = parseInt(speedInput.value) / 10; // 1-50 maps to 0.1x-5x
        var intervalDuration = base_interval / speedValue; // Higher speed = shorter interval
        // Mimics a for-loop
        var i = 0;
        intervalId = setInterval(function () {
            if (i < frames.length) {
                process_by_id.clear();
                frames[i].processes.forEach(function (p) {
                    process_by_id.set(p.id, p);
                });
                console.log("Current frame: ", i);
                console.log("Current processes: ", frames[i].processes);
                console.log("process_by_id: ", process_by_id);
                if (i == 0) {
                    render_frame(frames[i]);
                }
                else {
                    render_frame(frames[i], frames[i - 1]);
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
        return Array.from(table.querySelectorAll("tbody tr")).map(function (row) {
            var cells = row.children;
            var p = {
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
        return __awaiter(this, void 0, void 0, function () {
            function has_duplicate_ids(processes) {
                var seen = new Set();
                return processes.some(function (p) {
                    if (seen.has(p.id))
                        return true; // Duplicate found
                    seen.add(p.id);
                    return false;
                });
            }
            var algorithm, processes_data, frames;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        console.log("Initializing simulation data.");
                        algorithm = algorithmSelect.value;
                        processes_data = get_processes_from_list(initialTableBody);
                        if (has_duplicate_ids(processes_data)) {
                            alert('Duplicate IDs found! You should make every id of processes unique.');
                            return [2 /*return*/];
                        }
                        return [4 /*yield*/, fetch_frames(processes_data, algorithm)];
                    case 1:
                        frames = _a.sent();
                        console.log("Frames: ", frames);
                        if (frames.length === 0)
                            return [2 /*return*/];
                        // Initialize global variables
                        readyQueueDiv.innerHTML = '';
                        notReadyQueueDiv.innerHTML = '';
                        finishedQueueDiv.innerHTML = '';
                        start_animation(frames); // Start the animation after initialization
                        return [2 /*return*/];
                }
            });
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
    speedInput.addEventListener('input', function () {
        updateSpeedDisplay();
        updateSpeed(); // Adjust speed without resetting
    });
    var example_processes = [
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
    example_processes.forEach(function (p) {
        initialTableBody.appendChild(make_process_table_row(p)); // Add an example to the list
    });
    new_row_btn.addEventListener('click', function () {
        initialTableBody.appendChild(make_process_table_row(example_processes[0]));
    });
    // render_processes_list([example_proc]); // Initialize the list
})();
