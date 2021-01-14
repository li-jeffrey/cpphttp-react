import { useState } from 'react';
import { useAsync } from 'react-async';
import DataTable from '../components/Datatable';
import SideDrawer from '../components/SideDrawer';
import './ProductsPanel.css';

function form2Obj(form) {
    var obj = {};
    var formData = new FormData(form);
    for (var key of formData.keys()) {
        obj[key] = formData.get(key);
    }

    return obj;
}

function ProductEditor(props) {
    if (!props.show) {
        return null;
    }

    const createSubmitHandler = cb => {
        return event => {
            event.preventDefault();
            const formData = form2Obj(event.target);
            cb(formData);
        };
    }

    const viewCfg = {
        create: {
            title: 'Create Product',
            buttons: [
                <button key="product-create-btn" type="submit">Submit</button>,
                <button key="product-reset-btn" type="reset">Reset</button>
            ],
            data: {
                Id: '',
                Name: '',
                Description: ''
            },
            onSubmit: createSubmitHandler(props.onCreate)
        },
        update: {
            title: 'Update Product',
            buttons: [
                <button key="product-update-btn" type="submit">Update</button>,
                <button
                    key="product-delete-btn"
                    className="warn-btn"
                    onClick={event => { event.preventDefault(); props.onDelete(props.data); }}>Delete</button>
            ],
            data: props.data,
            onSubmit: createSubmitHandler(props.onUpdate)
        }
    }[props.type];

    return (
        <div>
            <h3>{viewCfg.title}</h3>
            <hr></hr>
            <form onSubmit={viewCfg.onSubmit}>
                <p>
                    <label htmlFor="name">Name: </label>
                    <input
                        key={viewCfg.data["Id"]}
                        id="name"
                        type="text"
                        name="Name"
                        defaultValue={viewCfg.data["Name"]}
                        required />
                </p>
                <p>
                    <label htmlFor="description">Description: </label>
                    <input
                        key={viewCfg.data["Id"]}
                        id="description"
                        type="text"
                        name="Description"
                        defaultValue={viewCfg.data["Description"]}
                        required />
                </p>
                <p>
                    {viewCfg.buttons}
                    {props.isPending && <i className="product-status-text">Loading...</i>}
                    {props.isFulfilled && <i className="product-status-text">Done</i>}
                </p>
            </form>
            {props.error && <div className="warn-text">{props.error.message}</div>}
        </div>
    )
}

export default function ProductsPanel(props) {
    const { isPending, error, data, reload } = useAsync({ promiseFn: props.getFn });
    const [editorState, setEditorState] = useState({
        type: '',
        show: false,
        data: {},
        isPending: false,
        isFulfilled: false,
        error: null
    });

    const handleCreate = () => {
        setEditorState({
            type: 'create',
            show: true,
            data: {},
            isPending: false,
            isFulfilled: false,
            error: null
        });
    };

    const handleEdit = row => {
        setEditorState({
            type: 'update',
            show: true,
            data: row,
            isPending: false,
            isFulfilled: false,
            error: null
        });
    };

    const createEditorAction = action => {
        return data => {
            setEditorState({ ...editorState, isPending: true, isFulfilled: false });
            action(data)
                .then(() => {
                    reload();
                    setEditorState({ ...editorState, isPending: false, isFulfilled: true, error: null });
                })
                .catch(err => setEditorState({ ...editorState, isPending: false, isFulfilled: false, error: err }));
        };
    }

    const onCreate = createEditorAction(data => props.saveFn(['create', data]));
    const onUpdate = createEditorAction(data => props.saveFn(['update', { ...data, Id: editorState.data['Id'] }]));
    const onDelete = createEditorAction(data => props.saveFn(['delete', data]));

    const stopEditing = () => {
        if (editorState.show) {
            setEditorState({ ...editorState, show: false });
        }
    }

    return (
        <div>
            <DataTable
                title="All products"
                data={data}
                columns={["Id", "Name", "Description"]}
                keyCol="Id"
                isLoading={isPending}
                createAction={handleCreate}
                editAction={handleEdit} />
            {error && <div className="warn-text">{error.message}</div>}
            <SideDrawer show={editorState.show} onClose={stopEditing}>
                <ProductEditor {...editorState} onCreate={onCreate} onUpdate={onUpdate} onDelete={onDelete} />
            </SideDrawer>
        </div>
    )
}