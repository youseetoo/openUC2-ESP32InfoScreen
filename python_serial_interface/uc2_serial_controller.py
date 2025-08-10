#!/usr/bin/env python3
"""
OpenUC2 ESP32 Serial Interface

A Python package for communicating with the OpenUC2 ESP32 controller via USB Serial.
Provides bidirectional communication for microscope control including motors, LEDs,
objective slots, and sample positioning.
"""

import serial
import json
import threading
import time
import logging
from typing import Callable, Optional, Dict, Any
from dataclasses import dataclass

@dataclass
class MotorPosition:
    """Motor position data"""
    x: float = 0.0
    y: float = 0.0
    z: float = 0.0
    a: float = 0.0

@dataclass
class LEDState:
    """LED state data"""
    enabled: bool = False
    r: int = 0
    g: int = 0
    b: int = 0

@dataclass
class ObjectiveSlot:
    """Objective slot information"""
    current_slot: int = 1
    available_slots: list = None
    
    def __post_init__(self):
        if self.available_slots is None:
            self.available_slots = [1, 2]

@dataclass
class SamplePosition:
    """Sample position on the map"""
    x: float = 0.5  # Normalized coordinates 0-1
    y: float = 0.5

class UC2SerialController:
    """
    Main controller class for ESP32 serial communication
    """
    
    def __init__(self, port: str = None, baudrate: int = 115200):
        self.port = port
        self.baudrate = baudrate
        self.serial_conn: Optional[serial.Serial] = None
        self.is_connected = False
        self.read_thread: Optional[threading.Thread] = None
        self.running = False
        
        # State tracking
        self.motor_position = MotorPosition()
        self.led_state = LEDState()
        self.objective_slot = ObjectiveSlot()
        self.sample_position = SamplePosition()
        
        # Callbacks
        self.callbacks: Dict[str, list] = {
            'status_update': [],
            'motor_update': [],
            'led_update': [],
            'objective_slot_update': [],
            'sample_position_update': [],
            'image_captured': [],
            'connection_changed': []
        }
        
        # Setup logging
        self.logger = logging.getLogger(__name__)
        handler = logging.StreamHandler()
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
        self.logger.setLevel(logging.INFO)
    
    def connect(self, port: str = None) -> bool:
        """
        Connect to ESP32 via serial port
        
        Args:
            port: Serial port name (e.g., 'COM3' on Windows, '/dev/ttyUSB0' on Linux)
            
        Returns:
            bool: True if connection successful
        """
        if port:
            self.port = port
            
        if not self.port:
            self.logger.error("No serial port specified")
            return False
            
        try:
            self.serial_conn = serial.Serial(self.port, self.baudrate, timeout=1)
            time.sleep(2)  # Wait for ESP32 to reset
            
            # Start reading thread
            self.running = True
            self.read_thread = threading.Thread(target=self._read_loop, daemon=True)
            self.read_thread.start()
            
            self.is_connected = True
            self.logger.info(f"Connected to ESP32 on {self.port}")
            
            # Request initial status
            self.request_status()
            
            self._notify_callbacks('connection_changed', {'connected': True})
            return True
            
        except serial.SerialException as e:
            self.logger.error(f"Failed to connect to {self.port}: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from ESP32"""
        self.running = False
        if self.read_thread and self.read_thread.is_alive():
            self.read_thread.join(timeout=1)
            
        if self.serial_conn:
            self.serial_conn.close()
            self.serial_conn = None
            
        self.is_connected = False
        self.logger.info("Disconnected from ESP32")
        self._notify_callbacks('connection_changed', {'connected': False})
    
    def _read_loop(self):
        """Background thread for reading serial messages"""
        buffer = ""
        
        while self.running and self.serial_conn:
            try:
                if self.serial_conn.in_waiting:
                    data = self.serial_conn.read(self.serial_conn.in_waiting).decode('utf-8', errors='ignore')
                    buffer += data
                    
                    # Process complete lines
                    while '\n' in buffer:
                        line, buffer = buffer.split('\n', 1)
                        line = line.strip()
                        if line:
                            self._handle_message(line)
                
                time.sleep(0.01)
                
            except Exception as e:
                self.logger.error(f"Error in read loop: {e}")
                break
    
    def _handle_message(self, message: str):
        """Handle incoming message from ESP32"""
        try:
            data = json.loads(message)
            msg_type = data.get('type', '')
            
            if msg_type == 'status_update':
                self._handle_status_update(data.get('data', {}))
            elif msg_type == 'motor_update':
                self._handle_motor_update(data.get('data', {}))
            elif msg_type == 'led_update':
                self._handle_led_update(data.get('data', {}))
            elif msg_type == 'objective_slot_update':
                self._handle_objective_slot_update(data.get('data', {}))
            elif msg_type == 'sample_position_update':
                self._handle_sample_position_update(data.get('data', {}))
            elif msg_type == 'image_captured':
                self._handle_image_captured(data.get('data', {}))
            else:
                self.logger.debug(f"Unknown message type: {msg_type}")
                
        except json.JSONDecodeError as e:
            self.logger.error(f"Failed to parse JSON message: {message} - {e}")
        except Exception as e:
            self.logger.error(f"Error handling message: {e}")
    
    def _handle_status_update(self, data: Dict[str, Any]):
        """Handle status update from ESP32"""
        self.logger.info("Received status update")
        self._notify_callbacks('status_update', data)
    
    def _handle_motor_update(self, data: Dict[str, Any]):
        """Handle motor position update"""
        positions = data.get('positions', {})
        self.motor_position.x = positions.get('x', self.motor_position.x)
        self.motor_position.y = positions.get('y', self.motor_position.y)
        self.motor_position.z = positions.get('z', self.motor_position.z)
        self.motor_position.a = positions.get('a', self.motor_position.a)
        
        self._notify_callbacks('motor_update', data)
    
    def _handle_led_update(self, data: Dict[str, Any]):
        """Handle LED state update"""
        self.led_state.enabled = data.get('enabled', self.led_state.enabled)
        self.led_state.r = data.get('r', self.led_state.r)
        self.led_state.g = data.get('g', self.led_state.g)
        self.led_state.b = data.get('b', self.led_state.b)
        
        self._notify_callbacks('led_update', data)
    
    def _handle_objective_slot_update(self, data: Dict[str, Any]):
        """Handle objective slot update"""
        self.objective_slot.current_slot = data.get('current_slot', self.objective_slot.current_slot)
        
        self._notify_callbacks('objective_slot_update', data)
    
    def _handle_sample_position_update(self, data: Dict[str, Any]):
        """Handle sample position update"""
        self.sample_position.x = data.get('x', self.sample_position.x)
        self.sample_position.y = data.get('y', self.sample_position.y)
        
        self._notify_callbacks('sample_position_update', data)
    
    def _handle_image_captured(self, data: Dict[str, Any]):
        """Handle image capture notification"""
        self.logger.info("Image captured")
        self._notify_callbacks('image_captured', data)
    
    def _send_message(self, message_dict: Dict[str, Any]):
        """Send message to ESP32"""
        if not self.is_connected or not self.serial_conn:
            self.logger.error("Not connected to ESP32")
            return False
            
        try:
            message = json.dumps(message_dict)
            self.serial_conn.write((message + '\n').encode('utf-8'))
            self.serial_conn.flush()
            return True
        except Exception as e:
            self.logger.error(f"Failed to send message: {e}")
            return False
    
    def _notify_callbacks(self, event_type: str, data: Dict[str, Any]):
        """Notify registered callbacks"""
        for callback in self.callbacks.get(event_type, []):
            try:
                callback(data)
            except Exception as e:
                self.logger.error(f"Error in callback for {event_type}: {e}")
    
    # Callback registration methods
    def on_status_update(self, callback: Callable[[Dict[str, Any]], None]):
        """Register callback for status updates"""
        self.callbacks['status_update'].append(callback)
    
    def on_motor_update(self, callback: Callable[[Dict[str, Any]], None]):
        """Register callback for motor updates"""
        self.callbacks['motor_update'].append(callback)
    
    def on_led_update(self, callback: Callable[[Dict[str, Any]], None]):
        """Register callback for LED updates"""
        self.callbacks['led_update'].append(callback)
    
    def on_objective_slot_update(self, callback: Callable[[Dict[str, Any]], None]):
        """Register callback for objective slot updates"""
        self.callbacks['objective_slot_update'].append(callback)
    
    def on_sample_position_update(self, callback: Callable[[Dict[str, Any]], None]):
        """Register callback for sample position updates"""
        self.callbacks['sample_position_update'].append(callback)
    
    def on_image_captured(self, callback: Callable[[Dict[str, Any]], None]):
        """Register callback for image capture events"""
        self.callbacks['image_captured'].append(callback)
    
    def on_connection_changed(self, callback: Callable[[Dict[str, Any]], None]):
        """Register callback for connection changes"""
        self.callbacks['connection_changed'].append(callback)
    
    # Command methods
    def request_status(self):
        """Request status update from ESP32"""
        return self._send_message({"type": "status_request"})
    
    def set_led(self, enabled: bool, r: int = 0, g: int = 0, b: int = 0):
        """Control LED"""
        return self._send_message({
            "type": "led_command",
            "data": {
                "enabled": enabled,
                "r": r,
                "g": g,
                "b": b
            }
        })
    
    def move_motor(self, motor_id: int, speed: int):
        """Move individual motor"""
        return self._send_message({
            "type": "motor_command",
            "data": {
                "motor": motor_id,
                "speed": speed
            }
        })
    
    def move_xy_motors(self, speed_x: int, speed_y: int):
        """Move X and Y motors simultaneously"""
        return self._send_message({
            "type": "motor_xy_command",
            "data": {
                "speedX": speed_x,
                "speedY": speed_y
            }
        })
    
    def set_objective_slot(self, slot: int):
        """Set objective slot (1 or 2)"""
        return self._send_message({
            "type": "objective_slot_command",
            "data": {
                "slot": slot
            }
        })
    
    def snap_image(self):
        """Trigger image capture"""
        return self._send_message({
            "type": "snap_image_command"
        })
    
    def update_sample_position(self, x: float, y: float):
        """Update sample position on map (normalized coordinates 0-1)"""
        return self._send_message({
            "type": "sample_position_command",
            "data": {
                "x": x,
                "y": y
            }
        })
    
    # Property accessors
    @property
    def connected(self) -> bool:
        """Check if connected to ESP32"""
        return self.is_connected
    
    @property
    def motor_positions(self) -> MotorPosition:
        """Get current motor positions"""
        return self.motor_position
    
    @property  
    def led_status(self) -> LEDState:
        """Get current LED state"""
        return self.led_state
    
    @property
    def current_objective_slot(self) -> int:
        """Get current objective slot"""
        return self.objective_slot.current_slot
    
    @property
    def current_sample_position(self) -> SamplePosition:
        """Get current sample position"""
        return self.sample_position


def find_esp32_port() -> Optional[str]:
    """
    Automatically find ESP32 serial port
    
    Returns:
        str: Port name if found, None otherwise
    """
    import serial.tools.list_ports
    
    # Common ESP32 device identifiers
    esp32_identifiers = [
        "CP210x",  # Silicon Labs CP210x
        "CH340",   # WCH CH340
        "UART",    # Generic UART
        "ESP32",   # ESP32 specific
        "USB Serial Device"  # Generic USB serial
    ]
    
    ports = serial.tools.list_ports.comports()
    
    for port in ports:
        for identifier in esp32_identifiers:
            if identifier.lower() in port.description.lower():
                return port.device
                
    # If no specific match, return first available port
    if ports:
        return ports[0].device
        
    return None


if __name__ == "__main__":
    # Example usage
    def on_led_update(data):
        print(f"LED Update: {data}")
    
    def on_motor_update(data):
        print(f"Motor Update: {data}")
    
    def on_status_update(data):
        print(f"Status Update: {data}")
    
    # Find ESP32 port automatically
    port = find_esp32_port()
    if not port:
        print("No ESP32 device found")
        exit(1)
    
    print(f"Found ESP32 on port: {port}")
    
    # Create controller and connect
    controller = UC2SerialController(port)
    
    # Register callbacks
    controller.on_led_update(on_led_update)
    controller.on_motor_update(on_motor_update)
    controller.on_status_update(on_status_update)
    
    # Connect
    if controller.connect():
        print("Connected successfully!")
        
        try:
            # Test commands
            time.sleep(1)
            controller.set_led(True, 255, 0, 0)  # Red
            time.sleep(1)
            controller.set_led(True, 0, 255, 0)  # Green
            time.sleep(1) 
            controller.set_led(False)  # Off
            
            # Keep running
            while True:
                time.sleep(1)
                
        except KeyboardInterrupt:
            print("Shutting down...")
        finally:
            controller.disconnect()
    else:
        print("Failed to connect!")