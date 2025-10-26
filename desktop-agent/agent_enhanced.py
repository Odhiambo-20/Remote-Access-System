#!/usr/bin/env python3
import socket
import threading
import hashlib
import platform
import os
import json
import time
from file_handler import FileHandler

class DesktopAgent:
    def __init__(self, relay_host='127.0.0.1', relay_port=2810):
        self.relay_host = relay_host
        self.relay_port = relay_port
        self.pc_id = self.generate_pc_id()
        self.username = os.getenv('USER') or os.getenv('USERNAME') or 'unknown'
        self.hostname = platform.node()
        self.socket = None
        self.file_handler = FileHandler()
        self.running = False
        
    def generate_pc_id(self):
        """Generate unique PC ID"""
        machine_info = f"{platform.node()}{platform.machine()}{platform.processor()}"
        return "PC-" + hashlib.md5(machine_info.encode()).hexdigest()
    
    def connect(self):
        """Connect to relay server"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(10)
            self.socket.connect((self.relay_host, self.relay_port))
            
            print(f"[DesktopAgent] Connected to relay server")
            
            # Register with server
            register_cmd = f"REGISTER|{self.pc_id}|{self.username}|{self.hostname}\n"
            self.socket.send(register_cmd.encode('utf-8'))
            
            response = self.socket.recv(1024).decode('utf-8').strip()
            print(f"[DesktopAgent] Registration response: {response}")
            
            if response == "REGISTERED":
                print(f"[DesktopAgent] Successfully registered!")
                self.running = True
                return True
            else:
                print(f"[DesktopAgent] Registration failed: {response}")
                return False
                
        except Exception as e:
            print(f"[DesktopAgent] Failed to connect: {e}")
            return False
    
    def handle_commands(self):
        """Handle incoming commands from relay server"""
        try:
            while self.running:
                data = self.socket.recv(4096).decode('utf-8').strip()
                if not data:
                    print("[DesktopAgent] Connection closed by server")
                    break
                
                print(f"[DesktopAgent] Received command: {data}")
                
                if data.startswith("CMD|"):
                    self.execute_command(data)
                elif data.startswith("FILE_REQUEST|"):
                    self.handle_file_request(data)
                elif data == "PING":
                    self.socket.send(b"PONG\n")
                else:
                    print(f"[DesktopAgent] Unknown command: {data}")
                    
        except Exception as e:
            print(f"[DesktopAgent] Command handler error: {e}")
        finally:
            self.running = False
            print("[DesktopAgent] Command handler stopped")
    
    def execute_command(self, data):
        """Execute shell command"""
        try:
            parts = data.split("|", 1)
            command = parts[1]
            
            print(f"[DesktopAgent] Executing: {command}")
            
            import subprocess
            result = subprocess.run(
                command,
                shell=True,
                capture_output=True,
                text=True,
                timeout=30
            )
            
            output = result.stdout if result.stdout else result.stderr
            response = f"CMD_RESULT|{output}\n"
            self.socket.send(response.encode('utf-8'))
            
        except Exception as e:
            error_msg = f"CMD_ERROR|{str(e)}\n"
            self.socket.send(error_msg.encode('utf-8'))
    
    def handle_file_request(self, data):
        """Handle file transfer request"""
        try:
            parts = data.split("|", 1)
            file_path = parts[1]
            
            print(f"[DesktopAgent] File requested: {file_path}")
            
            if os.path.exists(file_path):
                with open(file_path, 'rb') as f:
                    file_data = f.read()
                
                # Send file info
                file_info = f"FILE_INFO|{os.path.basename(file_path)}|{len(file_data)}\n"
                self.socket.send(file_info.encode('utf-8'))
                
                # Send file data
                self.socket.send(file_data)
                print(f"[DesktopAgent] File sent: {len(file_data)} bytes")
            else:
                error_msg = f"FILE_ERROR|File not found: {file_path}\n"
                self.socket.send(error_msg.encode('utf-8'))
                
        except Exception as e:
            error_msg = f"FILE_ERROR|{str(e)}\n"
            self.socket.send(error_msg.encode('utf-8'))
    
    def start(self):
        """Start the desktop agent"""
        print("[DesktopAgent] Starting...")
        print(f"[DesktopAgent] PC ID: {self.pc_id}")
        print(f"[DesktopAgent] User: {self.username}")
        print(f"[DesktopAgent] Hostname: {self.hostname}")
        
        if self.connect():
            print("[DesktopAgent] Connected and registered successfully")
            
            # Start command handler thread
            handler_thread = threading.Thread(target=self.handle_commands, daemon=True)
            handler_thread.start()
            
            # Keep main thread alive
            try:
                while self.running:
                    time.sleep(1)
            except KeyboardInterrupt:
                print("\n[DesktopAgent] Shutting down...")
                self.running = False
        else:
            print("[DesktopAgent] Failed to connect to relay server")

if __name__ == "__main__":
    agent = DesktopAgent()
    agent.start()
