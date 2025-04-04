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


