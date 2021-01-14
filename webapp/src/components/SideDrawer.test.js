import { screen, render } from "@testing-library/react"
import SideDrawer from "./SideDrawer";

test('open side drawer', async () => {
    render(
        <div>
            <div>
                <h1>Main header</h1>
            </div>
            <SideDrawer show={true} onClose={jest.fn()}>
                <h3>Drawer header</h3>
                <p>Drawer content</p>
            </SideDrawer>
        </div>
    );

    const drawerHeader = await screen.findByText('Drawer header');
    expect(drawerHeader).toBeVisible();

    const drawerContent = screen.getByText('Drawer content');
    expect(drawerContent).toBeVisible();

    const closeBtn = screen.getByRole('button', { name: String.fromCharCode(215) });
    expect(closeBtn).toBeVisible();
});

test('closed side drawer', async () => {
    render(
        <div>
            <div>
                <h1>Main header</h1>
            </div>
            <SideDrawer show={false} onClose={jest.fn()}>
                <h3>Drawer header</h3>
                <p>Drawer content</p>
            </SideDrawer>
        </div>
    );

    await screen.findByRole('button', { name: String.fromCharCode(215) });

    const drawerHeader = screen.queryByText('Drawer header');
    expect(drawerHeader).toBeNull();

    const drawerContent = screen.queryByText('Drawer content');
    expect(drawerContent).toBeNull();
});

test('side drawer close btn', async() => {
    const cb = jest.fn();
    render(
        <div>
            <div>
                <h1>Main header</h1>
            </div>
            <SideDrawer show={true} onClose={cb}>
                <h3>Drawer header</h3>
                <p>Drawer content</p>
            </SideDrawer>
        </div>
    );

    const closeBtn = await screen.findByRole('button', { name: String.fromCharCode(215) });
    closeBtn.click();

    expect(cb.mock.calls.length).toBe(1);
});