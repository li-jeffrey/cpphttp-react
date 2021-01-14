import './SideDrawer.css';

function SideDrawer(props) {
    const className = props.show ? "side-drawer open" : "side-drawer";
    return (
        <div className={className}>
            <button className="side-drawer-close-btn" onClick={props.onClose}>&times;</button>
            <div className="side-drawer-content">
                {props.show ? props.children : null}
            </div>
        </div>
    )
}

export default SideDrawer;