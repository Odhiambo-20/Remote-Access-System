#!/usr/bin/env python3
from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.label import Label
from kivy.uix.button import Button
from kivy.uix.scrollview import ScrollView
from kivy.clock import Clock
from kivy.core.window import Window
import socket
import threading
import json

Window.clearcolor = (0.15, 0.15, 0.15, 1)

class PCListItem(BoxLayout):
    def __init__(self, pc_data, app, **kwargs):
        super().__init__(**kwargs)
        self.orientation = 'horizontal'
        self.size_hint_y = None
        self.height = 80
        self.padding = 10
        self.spacing = 10
        
        self.pc_data = pc_data
        self.app = app
        
        # Status indicator
        status_box = BoxLayout(size_hint_x=0.1)
        status_label = Label(
            text='●',
            color=(0, 1, 0, 1),
            font_size='24sp'
        )
        status_box.add_widget(status_label)
        
        # PC Info
        info_box = BoxLayout(orientation='vertical', size_hint_x=0.6)
        
        pc_name = Label(
            text=pc_data.get('hostname', 'Unknown PC'),
            font_size='18sp',
            bold=True,
            halign='left',
            valign='middle'
        )
        pc_name.bind(size=pc_name.setter('text_size'))
        
        user_info = Label(
            text=f"User: {pc_data.get('username', 'unknown')}\nID: {pc_data.get('pc_id', 'N/A')}",
            font_size='12sp',
            halign='left',
            valign='middle',
            color=(0.7, 0.7, 0.7, 1)
        )
        user_info.bind(size=user_info.setter('text_size'))
        
        info_box.add_widget(pc_name)
        info_box.add_widget(user_info)
        
        # Connect button
        connect_btn = Button(
            text='Connect',
            size_hint_x=0.3,
            background_color=(0.2, 0.6, 1, 1),
            font_size='16sp'
        )
        connect_btn.bind(on_press=self.on_connect)
        
        self.add_widget(status_box)
        self.add_widget(info_box)
        self.add_widget(connect_btn)
    
    def on_connect(self, instance):
        """Handle connect button press"""
        print(f"[Mobile] Connecting to PC: {self.pc_data.get('pc_id')}")
        self.app.connect_to_pc(self.pc_data)

class RemoteAccessApp(App):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.relay_host = '127.0.0.1'
        self.relay_port = 2810
        self.connected_pc = None
        self.socket = None
        
    def build(self):
        self.title = 'Remote Access Mobile'
        
        # Main layout
        main_layout = BoxLayout(orientation='vertical', padding=10, spacing=10)
        
        # Header
        header = Label(
            text='Available PCs',
            size_hint_y=0.1,
            font_size='24sp',
            bold=True
        )
        
        # Status label
        self.status_label = Label(
            text='Connected PCs: 0',
            size_hint_y=0.05,
            font_size='14sp',
            color=(0.7, 0.7, 0.7, 1)
        )
        
        # PC List container
        self.pc_list_container = BoxLayout(orientation='vertical', size_hint_y=None, spacing=5)
        self.pc_list_container.bind(minimum_height=self.pc_list_container.setter('height'))
        
        scroll_view = ScrollView(size_hint=(1, 0.75))
        scroll_view.add_widget(self.pc_list_container)
        
        # Query button
        query_btn = Button(
            text='Query PC List',
            size_hint_y=0.1,
            background_color=(0.2, 0.6, 1, 1),
            font_size='18sp'
        )
        query_btn.bind(on_press=self.query_pc_list)
        
        main_layout.add_widget(header)
        main_layout.add_widget(self.status_label)
        main_layout.add_widget(scroll_view)
        main_layout.add_widget(query_btn)
        
        # Auto-query on start
        Clock.schedule_once(lambda dt: self.query_pc_list(None), 1)
        
        return main_layout
    
    def query_pc_list(self, instance):
        """Query list of connected PCs"""
        print("[Mobile] Querying PC list...")
        threading.Thread(target=self._query_pc_list_thread, daemon=True).start()
    
    def _query_pc_list_thread(self):
        """Query PC list in background thread"""
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect((self.relay_host, self.relay_port))
            
            # Send query command
            sock.send(b"GET_PCS\n")
            
            # Receive response
            response = sock.recv(4096).decode('utf-8').strip()
            print(f"[Mobile] Received: {response}")
            
            sock.close()
            
            # Parse response
            if response.startswith("PC_LIST|"):
                parts = response.split("|", 2)
                pc_count = int(parts[1])
                
                if pc_count > 0:
                    pc_data_str = parts[2]
                    pc_list = json.loads(pc_data_str)
                    Clock.schedule_once(lambda dt: self.update_pc_list(pc_list, pc_count))
                else:
                    Clock.schedule_once(lambda dt: self.update_pc_list([], 0))
            
        except Exception as error:
            error_msg = str(error)
            print(f"[Mobile] Query error: {error_msg}")
            Clock.schedule_once(lambda dt: self.show_error(error_msg))
    
    def update_pc_list(self, pc_list, count):
        """Update UI with PC list"""
        print(f"[Mobile] Updating PC list: {count} PCs")
        self.status_label.text = f'Connected PCs: {count}'
        
        # Clear existing list
        self.pc_list_container.clear_widgets()
        
        # Add PC items
        for pc in pc_list:
            pc_item = PCListItem(pc, self)
            self.pc_list_container.add_widget(pc_item)
        
        if count == 0:
            no_pc_label = Label(
                text='No PCs connected',
                font_size='16sp',
                color=(0.7, 0.7, 0.7, 1)
            )
            self.pc_list_container.add_widget(no_pc_label)
    
    def connect_to_pc(self, pc_data):
        """Connect to selected PC"""
        print(f"[Mobile] Attempting to connect to {pc_data.get('hostname')}")
        threading.Thread(target=self._connect_to_pc_thread, args=(pc_data,), daemon=True).start()
    
    def _connect_to_pc_thread(self, pc_data):
        """Connect to PC in background thread"""
        try:
            pc_id = pc_data.get('pc_id')
            
            # Create new socket for this connection
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(10)
            sock.connect((self.relay_host, self.relay_port))
            
            # Send connect command
            connect_cmd = f"CONNECT|{pc_id}\n"
            print(f"[Mobile] Sending: {connect_cmd.strip()}")
            sock.send(connect_cmd.encode('utf-8'))
            
            # Wait for response
            response = sock.recv(1024).decode('utf-8').strip()
            print(f"[Mobile] Response: {response}")
            
            if response == "CONNECTED":
                self.socket = sock
                self.connected_pc = pc_data
                success_msg = f"Connected to {pc_data.get('hostname')}"
                Clock.schedule_once(lambda dt: self.show_success(success_msg))
                
                # Open control interface
                Clock.schedule_once(lambda dt: self.open_control_interface())
            else:
                sock.close()
                error_msg = f"Failed to connect: {response}"
                Clock.schedule_once(lambda dt: self.show_error(error_msg))
                
        except Exception as error:
            error_msg = f"Connection error: {str(error)}"
            print(f"[Mobile] {error_msg}")
            Clock.schedule_once(lambda dt: self.show_error(error_msg))
    
    def open_control_interface(self):
        """Open PC control interface"""
        print("[Mobile] Opening control interface...")
        hostname = self.connected_pc.get('hostname', 'PC')
        success_msg = f"Connected to {hostname}!\nControl interface coming soon..."
        self.show_success(success_msg)
    
    def show_error(self, message):
        """Show error message"""
        self.status_label.text = f'Error: {message}'
        self.status_label.color = (1, 0, 0, 1)
        Clock.schedule_once(lambda dt: self.reset_status_color(), 3)
    
    def show_success(self, message):
        """Show success message"""
        self.status_label.text = message
        self.status_label.color = (0, 1, 0, 1)
        Clock.schedule_once(lambda dt: self.reset_status_color(), 3)
    
    def reset_status_color(self):
        """Reset status label color"""
        self.status_label.color = (0.7, 0.7, 0.7, 1)

if __name__ == '__main__':
    RemoteAccessApp().run()
