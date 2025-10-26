#!/usr/bin/env python3
import socket
import threading
import json
import signal
import sys

class RelayServer:
    def __init__(self, host='0.0.0.0', port=2810):
        self.host = host
        self.port = port
        self.connected_pcs = {}  # {pc_id: {'socket': socket, 'username': str, 'hostname': str}}
        self.lock = threading.Lock()
        self.running = False
        
    def start(self):
        """Start the relay server"""
        self.running = True
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((self.host, self.port))
        server_socket.listen(5)
        
        print(f"[RelayServer] Listening on {self.host}:{self.port}")
        
        # Setup signal handler for graceful shutdown
        signal.signal(signal.SIGINT, self.signal_handler)
        
        try:
            while self.running:
                try:
                    server_socket.settimeout(1.0)
                    client_socket, address = server_socket.accept()
                    print(f"[RelayServer] New connection from {address}")
                    
                    thread = threading.Thread(target=self.handle_client, args=(client_socket, address))
                    thread.daemon = True
                    thread.start()
                except socket.timeout:
                    continue
                except Exception as e:
                    if self.running:
                        print(f"[RelayServer] Accept error: {e}")
        finally:
            server_socket.close()
            print("[RelayServer] Server socket closed")
    
    def signal_handler(self, sig, frame):
        """Handle Ctrl+C gracefully"""
        print("\n[RelayServer] Received interrupt signal")
        self.running = False
        print("[RelayServer] Shutting down...")
        sys.exit(0)
    
    def handle_client(self, client_socket, address):
        """Handle client connection"""
        try:
            while self.running:
                data = client_socket.recv(4096).decode('utf-8').strip()
                if not data:
                    break
                
                print(f"[RelayServer] Received from {address}: {data}")
                
                if data.startswith("REGISTER|"):
                    self.handle_register(client_socket, data)
                elif data == "GET_PCS":
                    self.handle_get_pcs(client_socket)
                elif data.startswith("CONNECT|"):
                    self.handle_connect(client_socket, data)
                else:
                    client_socket.send(b"UNKNOWN_COMMAND\n")
                    
        except Exception as e:
            print(f"[RelayServer] Client error: {e}")
        finally:
            print(f"[RelayServer] Client {address} disconnected")
            client_socket.close()
    
    def handle_register(self, client_socket, data):
        """Handle PC registration"""
        try:
            parts = data.split("|")
            pc_id = parts[1]
            username = parts[2]
            hostname = parts[3]
            
            with self.lock:
                self.connected_pcs[pc_id] = {
                    'socket': client_socket,
                    'username': username,
                    'hostname': hostname,
                    'pc_id': pc_id
                }
            
            print(f"[RelayServer] Registered PC: {hostname} (User: {username}, ID: {pc_id})")
            client_socket.send(b"REGISTERED\n")
            
        except Exception as e:
            print(f"[RelayServer] Registration error: {e}")
            client_socket.send(b"REGISTRATION_FAILED\n")
    
    def handle_get_pcs(self, client_socket):
        """Send list of connected PCs"""
        try:
            with self.lock:
                pc_count = len(self.connected_pcs)
                
                if pc_count == 0:
                    response = "PC_LIST|0\n"
                else:
                    # Create list of PC info dictionaries
                    pc_list = []
                    for pc_id, info in self.connected_pcs.items():
                        pc_list.append({
                            'pc_id': pc_id,
                            'username': info['username'],
                            'hostname': info['hostname']
                        })
                    
                    # Send as JSON
                    response = f"PC_LIST|{pc_count}|{json.dumps(pc_list)}\n"
            
            print(f"[RelayServer] Sent PC list: {pc_count} PCs")
            client_socket.send(response.encode('utf-8'))
            
        except Exception as e:
            print(f"[RelayServer] Get PCs error: {e}")
            client_socket.send(b"ERROR\n")
    
    def handle_connect(self, client_socket, data):
        """Handle mobile app connecting to a PC"""
        try:
            parts = data.split("|")
            target_pc_id = parts[1]
            
            with self.lock:
                if target_pc_id in self.connected_pcs:
                    print(f"[RelayServer] Connecting mobile to PC: {target_pc_id}")
                    client_socket.send(b"CONNECTED\n")
                    # TODO: Implement bidirectional relay
                else:
                    print(f"[RelayServer] PC not found: {target_pc_id}")
                    client_socket.send(b"PC_NOT_FOUND\n")
                    
        except Exception as e:
            print(f"[RelayServer] Connect error: {e}")
            client_socket.send(b"ERROR\n")

if __name__ == "__main__":
    server = RelayServer()
    server.start()
