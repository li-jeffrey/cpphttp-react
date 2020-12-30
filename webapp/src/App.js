import { useEffect, useState } from 'react';
import './App.css';

function ProductsTable(props) {
  const [products, setProducts] = useState([]);

  useEffect(() => {
    fetch("/api/v1/products/all")
      .then(res => res.json())
      .then(pdts => setProducts(pdts))
      .catch(err => props.raiseAlert("GetProducts: " + err));
  })

  if (products.length === 0) {
    return (
      <div>Nothing to show</div>
    )
  }

  const cols = Object.keys(products[0]);
  return (
    <div>
      <table className="pure-table">
        <thead>
          <tr>
            {cols.map(field => <th>{field}</th>)}
          </tr>
        </thead>
        <tbody>
          {products.map(pdt => (
            <tr>
              {cols.map(col => <td>{pdt[col]}</td>)}
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  )
}

function Alerter(props) {
  if (props.text.length === 0) {
    return null;
  }

  return (
    <h4 className="warning">{props.text}</h4>
  )
}

function App() {
  const [alert, setAlert] = useState('');

  return (
    <main>
      <h2>cpphttp-react</h2>
      <Alerter text={alert}/>
      <hr></hr>
      <ProductsTable raiseAlert={setAlert}/>
    </main>
  );
}

export default App;
