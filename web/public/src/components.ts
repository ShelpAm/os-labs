import { Process } from './interfaces.js';

export function Process_table_row(p: Process): HTMLTableRowElement {
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

export function Process_div(p: Process): HTMLDivElement {
    const div = document.createElement('div');
    div.innerHTML = `Process ${p.id} (remaining: ${p.runtime_info.remaining_time ?? '-'})`;
    div.className = 'process';
    div.dataset.id = p.id.toString();
    return div;
}

