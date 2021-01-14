import './Datatable.css';

function byKey(keyExtractor) {
    return (p1, p2) => {
        if (keyExtractor(p1) < keyExtractor(p2)) {
            return -1;
        }

        if (keyExtractor(p1) > keyExtractor(p2)) {
            return 1;
        }

        return 0;
    }
}

function DataTable(props) {
    const keyCol = props.keyCol;
    let data = props.data || [];
    data = data.sort(byKey(p => p[keyCol]));
    const cols = props.columns;

    const createAction = event => {
        event.preventDefault();
        props.createAction();
    }

    const editAction = (event, rowNum) => {
        event.preventDefault();
        props.editAction(data[rowNum]);
    };

    let trows;
    if (props.isLoading) {
        trows = <tr><td colSpan={cols.length}>Loading...</td></tr>
    } else if (data.length === 0) {
        trows = <tr><td colSpan={cols.length}>No data</td></tr>
    } else {
        trows = data.map((pdt, idx) => {
            const key = pdt[keyCol];
            return (
                <tr key={key}>
                    {cols.map(col => <td key={`${col}-${key}`}>{pdt[col]}</td>)}
                    <td key={`edit-${key}`}>
                        <button className="datatable-edit-btn" onClick={e => editAction(e, idx)}>Edit</button>
                    </td>
                </tr>
            );
        });
    }

    return (
        <div className="datatable-container">
            <h3 className="datatable-title">{props.title}</h3>
            <button className="datatable-create-btn" onClick={createAction}>Create</button>
            <table>
                <thead>
                    <tr>
                        {cols.map(field => <th key={field}>{field}</th>)}
                    </tr>
                </thead>
                <tbody>
                    {trows}
                </tbody>
            </table>
        </div>
    )
}

export default DataTable;