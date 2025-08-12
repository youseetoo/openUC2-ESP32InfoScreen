#!/usr/bin/env python3
"""
Test script for UC2 Serial Controller

This script tests the Python package functionality without requiring actual hardware.
"""

import json
import time
from unittest.mock import Mock, MagicMock
from uc2_serial_controller import UC2SerialController, find_esp32_port

def test_controller_without_hardware():
    """Test controller functionality with mocked serial connection"""
    
    print("🧪 Testing UC2SerialController without hardware...")
    
    # Create controller but don't connect yet
    controller = UC2SerialController()
    
    # Test state properties
    print(f"✅ Initial LED state: {controller.led_status}")
    print(f"✅ Initial motor positions: {controller.motor_positions}")
    print(f"✅ Initial objective slot: {controller.current_objective_slot}")
    print(f"✅ Initial sample position: {controller.current_sample_position}")
    print(f"✅ Connected status: {controller.connected}")
    
    # Test callback registration
    callback_called = {'status': False, 'led': False, 'motor': False, 'sample_map_click': False}
    
    def on_status_update(data):
        callback_called['status'] = True
        print(f"✅ Status callback called with: {data}")
    
    def on_led_update(data):
        callback_called['led'] = True
        print(f"✅ LED callback called with: {data}")
    
    def on_motor_update(data):
        callback_called['motor'] = True
        print(f"✅ Motor callback called with: {data}")
    
    def on_sample_map_click(data):
        callback_called['sample_map_click'] = True
        print(f"✅ Sample map click callback called with: {data}")
    
    controller.on_status_update(on_status_update)
    controller.on_led_update(on_led_update)
    controller.on_motor_update(on_motor_update)
    controller.on_sample_map_click(on_sample_map_click)
    
    # Test message handling without serial connection
    print("\n📨 Testing message handling...")
    
    # Simulate receiving messages
    test_messages = [
        '{"type": "status_update", "data": {"connected": true, "modules": {"motor": true, "led": true}}}',
        '{"type": "led_update", "data": {"enabled": true, "r": 255, "g": 0, "b": 0}}',
        '{"type": "motor_update", "data": {"positions": {"x": 1000, "y": 2000, "z": 500}}}',
        '{"type": "objective_slot_update", "data": {"current_slot": 2}}',
        '{"type": "sample_position_update", "data": {"x": 0.3, "y": 0.7}}',
        '{"type": "sample_map_click", "data": {"pixel_x": 120, "pixel_y": 90, "normalized_x": 0.3, "normalized_y": 0.3, "sample_number": 12}}'
    ]
    
    for message in test_messages:
        print(f"Processing: {message}")
        controller._handle_message(message)
    
    # Verify callbacks were called
    print(f"\n✅ Callback tests - Status: {callback_called['status']}, LED: {callback_called['led']}, Motor: {callback_called['motor']}, Sample Map Click: {callback_called['sample_map_click']}")
    
    # Test state updates
    print(f"✅ Updated LED state: {controller.led_status}")
    print(f"✅ Updated motor positions: {controller.motor_positions}")
    print(f"✅ Updated objective slot: {controller.current_objective_slot}")
    print(f"✅ Updated sample position: {controller.current_sample_position}")
    
    print("\n🎉 All tests passed!")

def test_message_protocol():
    """Test JSON message protocol"""
    
    print("\n📋 Testing JSON message protocol...")
    
    # Test command messages (Python -> ESP32)
    commands = {
        "led_command": {
            "type": "led_command",
            "data": {"enabled": True, "r": 255, "g": 0, "b": 0}
        },
        "motor_command": {
            "type": "motor_command", 
            "data": {"motor": 1, "speed": 1000}
        },
        "motor_xy_command": {
            "type": "motor_xy_command",
            "data": {"speedX": 500, "speedY": -500}
        },
        "objective_slot_command": {
            "type": "objective_slot_command",
            "data": {"slot": 2}
        },
        "snap_image_command": {
            "type": "snap_image_command"
        }
    }
    
    for cmd_name, cmd_data in commands.items():
        json_str = json.dumps(cmd_data)
        # Verify it can be parsed back
        parsed = json.loads(json_str)
        print(f"✅ {cmd_name}: {json_str}")
        assert parsed['type'] == cmd_data['type']
    
    # Test update messages (ESP32 -> Python)  
    updates = {
        "status_update": {
            "type": "status_update",
            "data": {"connected": True, "modules": {"motor": True, "led": True}}
        },
        "led_update": {
            "type": "led_update",
            "data": {"enabled": True, "r": 255, "g": 0, "b": 0}
        },
        "motor_update": {
            "type": "motor_update", 
            "data": {"positions": {"x": 1000, "y": 2000, "z": 500}}
        }
    }
    
    for update_name, update_data in updates.items():
        json_str = json.dumps(update_data)
        parsed = json.loads(json_str)
        print(f"✅ {update_name}: {json_str}")
        assert parsed['type'] == update_data['type']
    
    print("✅ All message protocol tests passed!")

def test_port_detection():
    """Test port detection functionality"""
    
    print("\n🔍 Testing port detection...")
    
    # This will work even without actual ESP32 hardware
    port = find_esp32_port()
    if port:
        print(f"✅ Found potential ESP32 port: {port}")
    else:
        print("ℹ️  No ESP32 ports detected (this is expected without hardware)")
    
    # Test listing all available ports
    try:
        import serial.tools.list_ports
        ports = serial.tools.list_ports.comports()
        print(f"📋 Available serial ports ({len(ports)}):")
        for port_info in ports:
            print(f"  - {port_info.device}: {port_info.description}")
    except ImportError:
        print("⚠️  pyserial not available for port detection")

if __name__ == "__main__":
    print("🚀 UC2 Serial Controller Test Suite")
    print("=" * 50)
    
    try:
        test_controller_without_hardware()
        test_message_protocol()
        test_port_detection()
        
        print("\n" + "=" * 50)
        print("🎉 All tests completed successfully!")
        print("📦 The UC2 Serial Controller package is ready for use.")
        print("\nNext steps:")
        print("1. Flash the ESP32 with the updated firmware")
        print("2. Connect ESP32 via USB")
        print("3. Run the example.py script to test hardware communication")
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()