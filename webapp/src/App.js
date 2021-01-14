import ProductsPanel from './panels/ProductsPanel';
import './App.css';

const BASE_URL = process.env.REACT_APP_API_URL;

async function getProducts() {
    const response = await fetch(`${BASE_URL}/products/all`);
    if (!response.ok) throw new Error(response.statusText);
    if (response.status === 204) {
        return [];
    }

    return await response.json();
}

async function saveProduct([action, product]) {
    let response;
    switch (action) {
        case 'create':
            response = await fetch(`${BASE_URL}/products/create`, { method: 'POST', body: JSON.stringify(product) });
            if (!response.ok) throw new Error(response.statusText);
            break;
        case 'update':
            response = await fetch(`${BASE_URL}/products/update`, { method: 'POST', body: JSON.stringify(product) });
            if (!response.ok) throw new Error(response.statusText);
            break;
        case 'delete':
            response = await fetch(`${BASE_URL}/products/delete?id=${product["Id"]}`, { method: 'POST' });
            if (!response.ok) throw new Error(response.statusText);
            break;
        default:
            throw new Error("Unknown action: " + action);
    }
}

function App() {
  return (
    <main>
      <h2>cpphttp-react</h2>
      <hr></hr>
      <ProductsPanel getFn={getProducts} saveFn={saveProduct}/>
    </main>
  );
}

export default App;
