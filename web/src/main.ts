import { Time_point, Process } from './interfaces';
import { Process_table_row } from './components';
import { render_processes_list } from './script';

const new_row_btn = document.getElementById('new-row') as HTMLButtonElement;
const initialTableBody = document.querySelector('#initial-table tbody') as HTMLTableElement;

const example_processes: Array<Process> = [
    {
        id: 1001,
        name: 'Love in the Dark',
        arrival_time: 2,
        // arrival_time: Time_point.from_time(2),
        total_execution_time: 3,
        stats: {},
        extra: { priority: 2, },
    },
    {
        id: 1206,
        name: 'zyx',
        arrival_time: 3,
        // arrival_time: Time_point.from_time(3),
        total_execution_time: 5,
        stats: {},
        extra: { priority: 1, },
    },
    {
        id: 817,
        name: 'yyx',
        arrival_time: 5,
        // arrival_time: Time_point.from_time(5),
        total_execution_time: 1,
        stats: {},
        extra: { priority: 3, },
    },
];

// example_processes.forEach(p => {
//     initialTableBody.appendChild(Process_table_row(p)); // Add examples to the list
// });

render_processes_list(example_processes);

new_row_btn.addEventListener('click', () => {
    const item = example_processes[0];
    const children = initialTableBody.children;
    const last_table_row = children.item(children.length - 1) as HTMLDivElement;
    const id_table_col = last_table_row.children.item(0) as HTMLTableColElement;
    const id_input = id_table_col.children.item(0) as HTMLInputElement;
    item.id = Number(id_input.value) + 1;
    initialTableBody.appendChild(Process_table_row(item))
});

// render_processes_list([example_proc]); // Initialize the list

// Just for functionality test
// const canvas = document.getElementById("canvas") as HTMLCanvasElement;
// const ctx = canvas.getContext("2d")!;
// canvas.width = 800;
// canvas.height = 300;
//
// interface Process1 {
//     pid: string;
//     duration: number;
//     color: string;
//     x: number;
//     y: number;
//     startTime: number;  // When this process should start executing
//     executed: boolean;  // Whether the process has finished execution
// }
//
// const processes: Process1[] = [
//     { pid: "P1", duration: 3, color: "red", x: 50, y: 200, startTime: 0, executed: false },
//     { pid: "P2", duration: 2, color: "blue", x: 50, y: 250, startTime: 3, executed: false },
//     { pid: "P3", duration: 4, color: "green", x: 50, y: 300, startTime: 5, executed: false },
// ];
//
// let elapsedTime = 0; // Track CPU time
//
// function drawProcess1es() {
//     ctx.clearRect(0, 0, canvas.width, canvas.height);
//     ctx.font = "16px Arial";
//
//     // Draw CPU execution timeline
//     ctx.fillStyle = "black";
//     ctx.fillText("CPU Execution Timeline", 50, 50);
//     ctx.strokeRect(50, 60, 700, 50);
//
//     // Draw processes
//     processes.forEach((p) => {
//         ctx.fillStyle = p.color;
//         ctx.fillRect(p.x, p.y, 50, 30);
//         ctx.fillStyle = "white";
//         ctx.fillText(p.pid, p.x + 10, p.y + 20);
//
//         // Move process only when its startTime has passed
//         if (!p.executed && elapsedTime >= p.startTime) {
//             if (p.y > 70) {
//                 p.y -= 2; // Move up to CPU timeline
//             } else if (p.x < 700 - p.duration * 50) {
//                 p.x += 2; // Move right on the timeline
//             } else {
//                 p.executed = true; // Mark process as completed
//             }
//         }
//     });
//
//     elapsedTime += 0.05; // Increment CPU time
//     requestAnimationFrame(drawProcess1es);
// }
//
// drawProcess1es();
