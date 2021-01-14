import { render, screen } from "@testing-library/react";
import ProductsPanel from "./ProductsPanel";

let products = [];

async function getProducts() {
    return products.filter(p => p !== null);
}

async function getProductsWithError() {
    throw new Error('Not Found');
}

async function saveProduct([action, product]) {
    switch (action) {
        case 'create':
            products.push({ ...product, Id: products.length + 1 });
            break;
        case 'update':
            products[product['Id'] - 1] = { ...product };
            break;
        case 'delete':
            products[product['Id'] - 1] = null;
            break;
        default:
            throw new Error('Unknown action: ' + action);
    }
}

async function saveProductWithError() {
    throw new Error('Not Found');
}

beforeEach(() => {
    products = [
        { Id: 1, Name: 'Volvo', Description: 'SUV' },
        { Id: 2, Name: 'Toyota', Description: 'Camry' },
    ];
});

test('create product side drawer', async () => {
    render(
        <ProductsPanel getFn={getProducts} saveFn={saveProduct} />
    );

    const createBtn = await screen.findByRole('button', { name: 'Create' });
    createBtn.click();

    const sideDrawerTitle = await screen.findByText('Create Product');
    expect(sideDrawerTitle).toBeVisible();

    const nameInput = screen.getByLabelText(/Name/);
    nameInput.value = 'Toyota';

    const descInput = screen.getByLabelText(/Description/);
    descInput.value = 'Corolla';

    const submitBtn = screen.getByRole('button', { name: 'Submit' });
    expect(submitBtn).toBeVisible();
    submitBtn.click();

    await screen.findByText('Loading...');
    await screen.findByText('Done');

    const table = screen.getByRole('table');
    const tBody = table.getElementsByTagName('tbody')[0];
    expect(tBody.childElementCount).toBe(3);

    const closeBtn = screen.getByRole('button', { name: String.fromCharCode(215) });
    expect(closeBtn).toBeVisible();
    closeBtn.click();

    expect(screen.queryByText('Create Product')).toBeNull();
});

test('create product with failure', async () => {
    render(
        <ProductsPanel getFn={getProducts} saveFn={saveProductWithError} />
    );

    const createBtn = await screen.findByRole('button', { name: 'Create' });
    createBtn.click();

    await screen.findByText('Create Product');

    const nameInput = screen.getByLabelText(/Name/);
    nameInput.value = 'Toyota';

    const descInput = screen.getByLabelText(/Description/);
    descInput.value = 'Corolla';

    const submitBtn = screen.getByRole('button', { name: 'Submit' });
    submitBtn.click();

    const errMsg = await screen.findByText(/Not Found/);
    expect(screen.queryByText('Done')).toBeNull();
    expect(errMsg).toBeVisible();
});

test('update product', async () => {
    render(
        <ProductsPanel getFn={getProducts} saveFn={saveProduct} />
    );

    const editBtns = await screen.findAllByRole('button', { name: 'Edit' });
    expect(editBtns).toHaveLength(2);

    // open edit panel
    editBtns[0].click();
    const sideDrawerTitle = await screen.findByText('Update Product');
    expect(sideDrawerTitle).toBeVisible();

    // expect form values
    let nameInput = screen.getByLabelText(/Name/);
    expect(nameInput).toHaveValue('Volvo');

    let descInput = screen.getByLabelText(/Description/);
    expect(descInput).toHaveValue('SUV');
    descInput.value = 'Truck';

    const updateBtn = screen.getByRole('button', { name: 'Update' });
    updateBtn.click();

    await screen.findByText('Loading...');
    await screen.findByText('Done');

    expect(products[0]['Description']).toBe('Truck');

    const updatedCell = await screen.findByText('Truck');
    expect(updatedCell.nodeName).toBe('TD');

    const closeBtn = screen.getByRole('button', { name: String.fromCharCode(215) });
    closeBtn.click();

    expect(screen.queryByText('Update Product')).toBeNull();
});

test('update product with failure', async () => {
    render(
        <ProductsPanel getFn={getProducts} saveFn={saveProductWithError} />
    );

    const editBtns = await screen.findAllByRole('button', { name: 'Edit' });
    expect(editBtns).toHaveLength(2);

    // open edit panel
    editBtns[0].click();
    await screen.findByText('Update Product');

    let descInput = screen.getByLabelText(/Description/);
    descInput.value = 'Truck';

    const updateBtn = screen.getByRole('button', { name: 'Update' });
    updateBtn.click();

    const errMsg = await screen.findByText(/Not Found/);
    expect(screen.queryByText('Done')).toBeNull();
    expect(errMsg).toBeVisible();

    expect(products[0]['Description']).toBe('SUV');
});

test('delete product', async () => {
    render(
        <ProductsPanel getFn={getProducts} saveFn={saveProduct} />
    );

    const editBtns = await screen.findAllByRole('button', { name: 'Edit' });
    editBtns[1].click();

    const deleteBtn = await screen.findByRole('button', { name: 'Delete' });

    let nameInput = screen.getByLabelText(/Name/);
    expect(nameInput).toHaveValue('Toyota');

    let descInput = screen.getByLabelText(/Description/);
    expect(descInput).toHaveValue('Camry');

    deleteBtn.click();

    await screen.findByText('Loading...');
    await screen.findByText('Done');

    const table = screen.getByRole('table');
    const tBody = table.getElementsByTagName('tbody')[0];
    expect(tBody.childElementCount).toBe(1);

    const deletedCell = screen.queryByText('Camry');
    expect(deletedCell).toBeNull();
});

test('delete product with failure', async () => {
    render(
        <ProductsPanel getFn={getProducts} saveFn={saveProductWithError} />
    );

    const editBtns = await screen.findAllByRole('button', { name: 'Edit' });
    expect(editBtns).toHaveLength(2);

    // open edit panel
    editBtns[0].click();
    await screen.findByText('Update Product');

    const deleteBtn = screen.getByRole('button', { name: 'Delete' });
    deleteBtn.click();

    const errMsg = await screen.findByText(/Not Found/);
    expect(screen.queryByText('Done')).toBeNull();
    expect(errMsg).toBeVisible();

    const table = screen.getByRole('table');
    const tBody = table.getElementsByTagName('tbody')[0];
    expect(tBody.childElementCount).toBe(2);
});

test('display products error', async () => {
    render(
        <ProductsPanel getFn={getProductsWithError} saveFn={saveProduct} />
    );

    const tableBodyContent = await screen.findByText('No data');
    expect(tableBodyContent.nodeName).toBe('TD');

    const errMsg = screen.getByText(/Not Found/);
    expect(errMsg).toBeVisible();
});