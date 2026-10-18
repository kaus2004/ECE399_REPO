import asyncio
import threading
import tkinter as tk
from bleak import BleakScanner, BleakClient

# Configuration matching your ModusToolbox XML
DEVICE_NAME = "Hello"  # From your Device Name property
CHARACTERISTIC_UUID = "10702ab5-5dec-4e82-81b0-0efad36bfa21" # Key_Input Characteristic UUID
DISTANCE_UUID = "2bed917d-c6f4-a14d-99ae-c441fbfd75ed" # Replace with your new distance characteristic UUID when created

class BLEControllerApp:
    def __init__(self, root, loop):
        self.root = root
        self.loop = loop
        self.client = None
        self.speed_boost = False

        self.root.title("Hello Device & Telemetry Controller")
        self.root.geometry("350x620")  # Slightly taller for the new read button

        # Status Label
        self.status_label = tk.Label(root, text="Status: Disconnected", fg="red", font=("Arial", 12))
        self.status_label.pack(pady=5)

        # Distance Telemetry Label & Read Button Frame
        telemetry_frame = tk.Frame(root)
        telemetry_frame.pack(pady=5)

        self.distance_label = tk.Label(telemetry_frame, text="Distance: --- mm", font=("Arial", 16, "bold"), fg="blue")
        self.distance_label.pack(side=tk.TOP, pady=2)

        self.btn_read_distance = tk.Button(telemetry_frame, text="Read Distance (Manual)", command=self.on_read_distance_click, bg="lightblue", font=("Arial", 9))
        self.btn_read_distance.pack(side=tk.TOP, pady=2)

        # Connect Button
        self.connect_btn = tk.Button(root, text="Connect to Device", command=self.on_connect_click, bg="lightgrey")
        self.connect_btn.pack(pady=5)

        # Speed Lock Checkbox
        self.speed_var = tk.BooleanVar()
        self.speed_check = tk.Checkbutton(
            root, text="Speed Lock (Fast - Uppercase)", variable=self.speed_var, 
            command=self.toggle_speed, font=("Arial", 10, "bold")
        )
        self.speed_check.pack(pady=10)

        # D-Pad Frame for Directional Arrows
        pad_frame = tk.Frame(root)
        pad_frame.pack(pady=5)

        self.btn_up = tk.Button(pad_frame, text="▲\n(W)", width=8, height=3, command=lambda: self.send_command('w'))
        self.btn_up.grid(row=0, column=1, padx=5, pady=5)

        self.btn_left = tk.Button(pad_frame, text="◄\n(A)", width=8, height=3, command=lambda: self.send_command('a'))
        self.btn_left.grid(row=1, column=0, padx=5, pady=5)

        self.btn_down = tk.Button(pad_frame, text="▼\n(S)", width=8, height=3, command=lambda: self.send_command('s'))
        self.btn_down.grid(row=2, column=1, padx=5, pady=5)

        self.btn_right = tk.Button(pad_frame, text="►\n(D)", width=8, height=3, command=lambda: self.send_command('d'))
        self.btn_right.grid(row=1, column=2, padx=5, pady=5)

        # Break / Stop Button
        self.btn_break = tk.Button(root, text="BREAK / STOP\n(B)", width=24, height=2, command=lambda: self.send_command('b'), bg="lightcoral", font=("Arial", 10, "bold"))
        self.btn_break.pack(pady=10)

        # Keyboard Bindings (W, A, S, D, and B)
        self.root.bind("<w>", lambda event: self.send_command('w'))
        self.root.bind("<a>", lambda event: self.send_command('a'))
        self.root.bind("<s>", lambda event: self.send_command('s'))
        self.root.bind("<d>", lambda event: self.send_command('d'))
        self.root.bind("<b>", lambda event: self.send_command('b'))

    def toggle_speed(self):
        self.speed_boost = self.speed_var.get()
        state = "ON (UPPERCASE)" if self.speed_boost else "OFF (lowercase)"
        print(f"Speed Lock: {state}")

    def on_connect_click(self):
        self.status_label.config(text="Status: Searching...", fg="orange")
        asyncio.run_coroutine_threadsafe(self.connect_ble(), self.loop)

    def on_read_distance_click(self):
        asyncio.run_coroutine_threadsafe(self.fetch_distance_read(), self.loop)

    def distance_notification_handler(self, sender, data):
        if len(data) >= 2:
            distance_mm = int.from_bytes(data[:2], byteorder="little")
            print(f"Distance (Notify): {distance_mm} mm")
            self.root.after(
                0,
                lambda: self.distance_label.config(text=f"Distance: {distance_mm} mm")
            )

    async def fetch_distance_read(self):
        if self.client and self.client.is_connected:
            try:
                data = await self.client.read_gatt_char(DISTANCE_UUID)
                if len(data) >= 2:
                    distance_mm = int.from_bytes(data[:2], byteorder="little")
                    print(f"Distance (Read): {distance_mm} mm")
                    self.root.after(
                        0,
                        lambda: self.distance_label.config(text=f"Distance: {distance_mm} mm")
                    )
            except Exception as e:
                print(f"Read distance failed: {e}")
        else:
            print("Not connected to BLE device!")

    async def connect_ble(self):
        try:
            print(f"Scanning for {DEVICE_NAME}...")
            device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10.0)
            
            if not device:
                self.status_label.config(text="Status: Device Not Found", fg="red")
                return

            self.status_label.config(text="Status: Connecting...", fg="orange")
            self.client = BleakClient(device)
            await self.client.connect()

            if self.client.is_connected:
                self.status_label.config(text=f"Connected to {DEVICE_NAME}", fg="green")
                self.connect_btn.config(state=tk.DISABLED)

                # 1. Fetch initial distance value right away via Read
                await self.fetch_distance_read()

                # 2. Subscribe to live notifications
                try:
                    await self.client.start_notify(DISTANCE_UUID, self.distance_notification_handler)
                    print("Subscribed to distance notifications successfully")
                except Exception as notify_err:
                    print(f"Could not subscribe to distance characteristic: {notify_err}")

        except Exception as e:
            print(f"Connection Error: {e}")
            self.status_label.config(text="Status: Connection Failed", fg="red")

    def send_command(self, base_char):
        char_to_send = base_char.upper() if self.speed_boost else base_char.lower()
        asyncio.run_coroutine_threadsafe(self.write_ascii(char_to_send), self.loop)

    async def write_ascii(self, char):
        if self.client and self.client.is_connected:
            try:
                data = char.encode('ascii')
                await self.client.write_gatt_char(CHARACTERISTIC_UUID, data)
                print(f"Sent: '{char}'")
            except Exception as e:
                print(f"Write failed: {e}")
        else:
            print("Not connected to BLE device!")

def start_asyncio_thread(loop):
    asyncio.set_event_loop(loop)
    loop.run_forever()

if __name__ == "__main__":
    async_loop = asyncio.new_event_loop()
    threading.Thread(target=start_asyncio_thread, args=(async_loop,), daemon=True).start()

    root = tk.Tk()
    app = BLEControllerApp(root, async_loop)
    root.mainloop()
