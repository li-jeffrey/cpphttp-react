import { screen, render } from "@testing-library/react";
import DataTable from "./Datatable";

const tableData = [
    {Id: 1, Name: 'Alice', Role: 'Staff'},
    {Id: 2, Name: 'Bob', Role: 'Contractor'}
];

test('display table', async () => {
    render(
        <DataTable 
        title="Employees"
                data={tableData}
                columns={["Id", "Name", "Role"]}
                keyCol="Id"
                isLoading={false}
                createAction={jest.fn()}
                editAction={jest.fn()} 
        />
    );

    // assert table title
    const tableTitle = await screen.findByText('Employees');
    expect(tableTitle).toBeVisible();
    expect(tableTitle).toHaveClass('datatable-title');

    // assert create button
    const createBtn = screen.getByRole('button', {name: 'Create'});
    expect(createBtn).toBeVisible();
    expect(createBtn).toBeEnabled();
    expect(createBtn).toHaveClass('datatable-create-btn');

    // assert table headers
    for (const colName of ['Id', 'Name', 'Role']) {
        const colHeader = screen.getByText(colName);
        expect(colHeader).toBeVisible();
        expect(colHeader.nodeName).toBe('TH');
    }

    const table = await screen.findByRole('table');
    const tBody = table.getElementsByTagName('tbody')[0];
    expect(tBody.childElementCount).toBe(tableData.length);

    // assert table body
    for (let row = 0; row < tBody.childElementCount; row++) {
        const tRow = tBody.childNodes[row];
        expect(tRow.nodeName).toBe('TR');
        const values = Object.values(tableData[row]);
        const numFields = values.length;
        expect(tRow.childElementCount).toBe(numFields + 1);
        for (let col = 0; col < numFields; col++) {
            const tCell = tRow.childNodes[col];
            expect(tCell.nodeName).toBe('TD');
            expect(tCell).toHaveTextContent(values[col].toString());
        }

        const editBtnCell = tRow.childNodes[numFields];
        expect(editBtnCell.nodeName).toBe('TD');
        expect(editBtnCell.childElementCount).toBe(1);

        const editBtn = editBtnCell.childNodes[0];
        expect(editBtn.nodeName).toBe('BUTTON');
        expect(editBtn).toHaveTextContent('Edit');
        expect(editBtn).toHaveClass('datatable-edit-btn');
    }
});

test('display table with empty array data', async () => {
    render(
        <DataTable 
        title="Employees"
                data={[]}
                columns={["Id", "Name", "Role"]}
                keyCol="Id"
                isLoading={false}
                createAction={jest.fn()}
                editAction={jest.fn()} 
        />
    );

    const tableText = await screen.findByText('No data');
    expect(tableText.nodeName).toBe('TD');
    expect(tableText).toHaveAttribute('colSpan', '3');
});

test('display table with null data', async () => {
    render(
        <DataTable 
        title="Employees"
                data={null}
                columns={["Id", "Name", "Role"]}
                keyCol="Id"
                isLoading={false}
                createAction={jest.fn()}
                editAction={jest.fn()} 
        />
    );

    const tableText = await screen.findByText('No data');
    expect(tableText.nodeName).toBe('TD');
    expect(tableText).toHaveAttribute('colSpan', '3');
});

test('display table loading text', async () => {
    render(
        <DataTable 
        title="Employees"
                data={null}
                columns={["Id", "Name", "Role"]}
                keyCol="Id"
                isLoading={true}
                createAction={jest.fn()}
                editAction={jest.fn()} 
        />
    );

    const tableText = await screen.findByText('Loading...');
    expect(tableText.nodeName).toBe('TD');
    expect(tableText).toHaveAttribute('colSpan', '3');
});

test('table create button', async () => {
    const cb = jest.fn();
    render(
        <DataTable 
        title="Employees"
                data={[]}
                columns={["Id", "Name", "Role"]}
                keyCol="Id"
                isLoading={false}
                createAction={cb}
                editAction={jest.fn()} 
        />
    );

    const createBtn = await screen.findByRole('button', {name: 'Create'});
    createBtn.click();

    expect(cb.mock.calls.length).toBe(1);
});

test('table edit button', async () => {
    const cb = jest.fn();
    render(
        <DataTable 
        title="Employees"
                data={tableData}
                columns={["Id", "Name", "Role"]}
                keyCol="Id"
                isLoading={false}
                createAction={jest.fn()}
                editAction={cb} 
        />
    );

    const editBtns = await screen.findAllByRole('button', {name: 'Edit'});
    expect(editBtns).toHaveLength(2);

    editBtns[0].click();
    expect(cb.mock.calls.length).toBe(1);

    const row1 = cb.mock.calls[0][0];
    expect(row1).toBe(tableData[0]);

    editBtns[1].click();
    expect(cb.mock.calls.length).toBe(2);

    const row2 = cb.mock.calls[1][0];
    expect(row2).toBe(tableData[1]);
});