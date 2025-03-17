import { Process_table_row } from './components.js';
const new_row_btn = document.getElementById('new-row');
const initialTableBody = document.querySelector('#initial-table tbody');
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
    initialTableBody.appendChild(Process_table_row(p)); // Add examples to the list
});
new_row_btn.addEventListener('click', () => {
    initialTableBody.appendChild(Process_table_row(example_processes[0]));
});
// render_processes_list([example_proc]); // Initialize the list
