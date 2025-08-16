# OpenUC2 ESP32 Serial Interface Implementation

This document provides a comprehensive overview of the USB Serial interface implementation that replaces the WiFi-based communication system in the OpenUC2 ESP32 controller.

## Overview

The implementation provides:
- **Bidirectional serial communication** between ESP32 and PC/Python
- **JSON-based message protocol** for structured data exchange
- **New hardware controls** including objective slots, snap button, and sample map
- **Python package** for easy integration with host applications
- **Event-driven architecture** with callbacks for real-time updates

## Architecture Changes

### Before (WiFi-Based)
```
PC/Python ←→ WiFi Network ←→ ESP32 (WebSocket/HTTP)
```

### After (Serial-Based)
```
PC/Python ←→ USB Serial ←→ ESP32 (JSON Messages)
```

## File Structure

```
├── src/
│   ├── main.ino                    # Updated main entry point for serial mode
│   ├── serial_controller.h/.cpp    # Serial communication controller
│   ├── SerialApi.h/.cpp           # Serial API (replaces RestApi)
│   ├── uc2ui_objectivepage.h/.cpp # New objective/snap/map UI components
│   └── uc2ui_microscopepage.cpp   # Updated to include objective page
├── python_serial_interface/
│   ├── uc2_serial_controller.py   # Main Python controller class
│   ├── __init__.py                # Package initialization
│   ├── setup.py                   # Package installation script
│   ├── requirements.txt           # Python dependencies
│   ├── example.py                 # Usage example
│   ├── test_controller.py         # Test suite
│   └── README.md                  # Python package documentation
```

## ESP32 Side Changes

### 1. Serial Controller (`serial_controller.h/.cpp`)
Replaces `wifi_controller` with USB serial communication:

```cpp
namespace serial_controller {
    void init();                    // Initialize serial communication
    void loop();                    // Process incoming messages
    void sendMessage(String msg);   // Send JSON message to PC
}
```

**Key Features:**
- Automatic message parsing from serial input
- Buffer management for incoming data
- Connection status tracking

### 2. Serial API (`SerialApi.h/.cpp`)
Replaces `RestApi` with serial-based communication:

```cpp
namespace SerialApi {
    // Core functions
    void init();
    void handleIncomingMessage(const String& message);
    
    // Hardware control
    void updateColors(int r, int g, int b);
    void driveMotorForever(int motor, int speed);
    void setLedOn(bool enable, int r, int g, int b);
    
    // New features
    void setObjectiveSlot(int slot);
    void snapImage();
    void updateSamplePosition(float x, float y);
}
```

**Key Features:**
- JSON message parsing and generation
- Queue-based command processing
- Event-driven UI updates
- FreeRTOS task for message handling

### 3. Objective Page UI (`uc2ui_objectivepage.h/.cpp`)
New UI components for enhanced microscope control:

```cpp
namespace uc2ui_objectivepage {
    void initUI(lv_obj_t* container);           // Initialize UI components
    void setCurrentSlot(int slot);              // Update slot display
    void updateSampleMap(float x, float y);     // Update position indicator
    void setOnSlotChangeListener(callback);     // Register slot change callback
    void setOnSnapButtonListener(callback);     // Register snap button callback
}
```

**UI Components:**
- **Current Slot Display:** Shows active objective slot (1 or 2)
- **Slot Selection Buttons:** Switch between objective slots
- **Snap Button:** Large green button for image capture
- **Sample Map:** Visual representation with red position dot
- **Position Indicator:** Moves based on normalized coordinates (0-1)

## Python Package (`python_serial_interface/`)

### Main Controller Class (`uc2_serial_controller.py`)

```python
class UC2SerialController:
    def __init__(self, port=None, baudrate=115200)
    
    # Connection management
    def connect(port=None) -> bool
    def disconnect()
    
    # Hardware control
    def set_led(enabled, r, g, b)
    def move_motor(motor_id, speed) 
    def move_xy_motors(speed_x, speed_y)
    def set_objective_slot(slot)
    def snap_image()
    def update_sample_position(x, y)
    
    # Event callbacks
    def on_led_update(callback)
    def on_motor_update(callback)
    def on_objective_slot_update(callback)
    def on_sample_position_update(callback)
    def on_image_captured(callback)
```

**Key Features:**
- **Auto Port Detection:** Automatically finds ESP32 serial port
- **Threading:** Background thread for reading serial data
- **State Tracking:** Maintains current state of all hardware
- **Callback System:** Event-driven updates from ESP32
- **Error Handling:** Robust error handling and logging

### Data Structures

```python
@dataclass
class MotorPosition:
    x: float = 0.0
    y: float = 0.0  
    z: float = 0.0
    a: float = 0.0

@dataclass  
class LEDState:
    enabled: bool = False
    r: int = 0
    g: int = 0
    b: int = 0

@dataclass
class SamplePosition:
    x: float = 0.5  # Normalized coordinates 0-1
    y: float = 0.5
```

## Message Protocol

All communication uses JSON messages over serial (115200 baud).

### Commands (Python → ESP32)

```json
// LED Control
{
    "type": "led_command",
    "data": {
        "enabled": true,
        "r": 255,
        "g": 0, 
        "b": 0
    }
}

// Motor Control
{
    "type": "motor_command",
    "data": {
        "motor": 1,
        "speed": 1000
    }
}

// XY Motor Control  
{
    "type": "motor_xy_command",
    "data": {
        "speedX": 500,
        "speedY": -500
    }
}

// Objective Slot
{
    "type": "objective_slot_command", 
    "data": {
        "slot": 2
    }
}

// Image Capture
{
    "type": "snap_image_command"
}

// Image Display
{
    "type": "display_image_command",
    "data": {
        "tab_name": "Live View",
        "width": 160,
        "height": 120,
        "format": "rgb565",
        "image_data": "base64_encoded_rgb565_image_data..."
    }
}

// Sample Position
{
    "type": "sample_position_command",
    "data": {
        "x": 0.3,
        "y": 0.7
    }
}

// Status Request
{
    "type": "status_request"
}
```

### Updates (ESP32 → Python)

```json
// Status Update
{
    "type": "status_update",
    "data": {
        "connected": true,
        "modules": {
            "motor": true,
            "led": true
        }
    }
}

// LED Update
{
    "type": "led_update", 
    "data": {
        "enabled": true,
        "r": 255,
        "g": 0,
        "b": 0
    }
}

// Motor Update
{
    "type": "motor_update",
    "data": {
        "positions": {
            "x": 1000,
            "y": 2000, 
            "z": 500,
            "a": 0
        }
    }
}

// Objective Slot Update
{
    "type": "objective_slot_update",
    "data": {
        "current_slot": 2
    }
}

// Sample Position Update  
{
    "type": "sample_position_update",
    "data": {
        "x": 0.3,
        "y": 0.7
    }
}

// Image Captured
{
    "type": "image_captured",
    "data": {}
}

// Image Display Result
{
    "type": "image_display_result",
    "data": {
        "tab_name": "Live View",
        "width": 160,
        "height": 120,
        "success": true
    }
}
```

## Installation & Usage

### ESP32 Firmware

1. **Flash Updated Firmware:**
   ```bash
   cd /path/to/openUC2-ESP32InfoScreen
   pio run -t upload -e esp32s3wavesharedisplay
   ```

2. **Connect via USB:** Connect ESP32 to PC via USB cable

3. **Verify Connection:** Check serial monitor for startup messages

### Python Package

1. **Install Package:**
   ```bash
   cd python_serial_interface
   pip install -r requirements.txt
   pip install -e .
   ```

2. **Basic Usage:**
   ```python
   from uc2_serial_controller import UC2SerialController, find_esp32_port
   import numpy as np
   
   # Auto-detect ESP32
   port = find_esp32_port()
   controller = UC2SerialController(port)
   
   # Connect and use
   if controller.connect():
       controller.set_led(True, 255, 0, 0)  # Red LED
       controller.set_objective_slot(2)      # Switch to slot 2
       controller.snap_image()               # Take picture
       
       # Send image to display
       test_image = np.random.randint(0, 255, (100, 100, 3), dtype=np.uint8)
       controller.send_image(test_image, "Test Image")
   ```

3. **Image Display Features:**
   ```python
   # Send numpy array to display
   camera_frame = capture_microscope_image()
   controller.send_image(camera_frame, "Live View")
   
   # Automatic image capture on snap button press
   def get_current_frame():
       return camera.capture()
   
   controller.send_image_on_snap(get_current_frame)
   # Now pressing "CAPTURE IMAGE" button creates new image tabs automatically!
   ```

3. **Run Examples:**
   ```bash
   python example.py                    # Full demonstration
   python test_controller.py           # Test suite
   python simple_image_example.py      # Basic image display
   python base64_encoding_example.py   # Base64 encoding demonstration
   python test_image_protocol.py       # Image protocol integration test
   ```

## Image Display System

### Overview
The ESP32 can receive and display images sent from Python applications as new tabs in the LVGL interface. This enables real-time microscopy image viewing directly on the ESP32 display.

### Image Processing Pipeline
1. **Python Side**: 
   - Accepts numpy arrays, image files (JPG, PNG), grayscale, RGBA formats
   - Automatically converts to RGB888 format
   - Resizes images to fit ESP32 memory constraints (240x160 max recommended)
   - Converts RGB888 to RGB565 format for efficiency
   - Base64 encodes for JSON transmission

2. **ESP32 Side**:
   - Receives `display_image_command` via 32KB JSON buffer
   - Base64 decodes image data using improved decoder
   - Allocates memory in SPIRAM if available
   - Creates LVGL image descriptor and tab
   - Displays with close button for memory cleanup

### Supported Formats
- **Input**: Numpy arrays (RGB, RGBA, grayscale), image files
- **Transmission**: Base64-encoded RGB565 
- **Display**: 16-bit RGB565 format optimized for ESP32

### Memory Management
- Automatic SPIRAM allocation for large images
- Fallback to regular heap if SPIRAM unavailable  
- Close button properly frees allocated memory
- Image size validation prevents memory overflow

## Testing

### Python Package Tests
```bash
cd python_serial_interface
python test_controller.py
```

**Test Coverage:**
- ✅ Message protocol validation
- ✅ State management
- ✅ Callback system
- ✅ Port detection
- ✅ JSON serialization/deserialization

### Hardware Testing
1. **LED Control:** Verify color changes and on/off states
2. **Motor Control:** Test individual and XY motor movements
3. **Objective Slots:** Confirm slot switching works
4. **Sample Map:** Verify position updates display correctly
5. **Snap Button:** Ensure image capture triggers correctly

## Benefits Over WiFi Implementation

| Aspect | WiFi-Based | Serial-Based |
|--------|------------|--------------|
| **Connection** | Complex network setup | Simple USB connection |
| **Latency** | Network dependent | Direct USB communication |
| **Reliability** | WiFi interference issues | Stable wired connection |
| **Security** | Network security concerns | Local connection only |
| **Setup** | WiFi configuration needed | Plug-and-play |
| **Debugging** | Network troubleshooting | Direct serial monitoring |
| **Power** | WiFi power consumption | Lower power usage |

## Migration Guide

### For Existing Users

1. **Hardware:** No hardware changes needed - same ESP32 board
2. **Connection:** Replace WiFi with USB cable connection
3. **Software:** Update ESP32 firmware and use new Python package
4. **Configuration:** No network configuration required

### Code Migration

**Old (WiFi):**
```python
# Complex WiFi setup required
microscope.connect_wifi("SSID", "password")
microscope.discover_devices()
microscope.connect_to_device("esp32-device")
```

**New (Serial):**
```python
# Simple USB connection
port = find_esp32_port()
controller = UC2SerialController(port)
controller.connect()
```

## Troubleshooting

### Common Issues

1. **ESP32 Not Found**
   - Check USB cable connection
   - Verify ESP32 is powered on
   - Try different USB port
   - Check device drivers

2. **Permission Denied (Linux/Mac)**
   ```bash
   sudo usermod -a -G dialout $USER  # Add user to dialout group
   logout && login                   # Re-login to apply changes
   ```

3. **Message Not Received**
   - Check baud rate (should be 115200)
   - Verify JSON message format
   - Monitor serial output for errors
   - Check buffer overflow issues

4. **UI Not Updating**
   - Verify callback registration
   - Check message types match
   - Monitor ESP32 serial output
   - Ensure UI task is running

### Debugging Tips

1. **Serial Monitor:** Use Arduino Serial Monitor to see ESP32 output
2. **Python Logging:** Enable debug logging in Python controller
3. **Message Validation:** Use test_controller.py to verify protocol
4. **Port Testing:** Use basic serial terminal to test connection

## Future Enhancements

### Planned Features
- **Multi-well Plate Support:** Enhanced sample map with grid layout
- **Image Metadata:** Include position/settings data with captures
- **Macro Recording:** Record and replay command sequences
- **Configuration Profiles:** Save/load microscope settings
- **Remote Access:** Optional WiFi fallback mode

### Performance Optimizations
- **Message Batching:** Combine multiple commands
- **Compression:** Reduce message size for large data
- **Flow Control:** Implement message acknowledgments
- **Buffer Management:** Optimize memory usage

## Conclusion

The USB Serial interface implementation provides a robust, reliable, and user-friendly alternative to WiFi-based communication. Key advantages include:

- **Simplified Setup:** No network configuration required
- **Enhanced Reliability:** Direct USB connection eliminates network issues  
- **New Features:** Objective slots, snap button, and sample mapping
- **Better Performance:** Lower latency and more stable communication
- **Easy Integration:** Complete Python package with examples

The implementation maintains backward compatibility with existing UI components while adding new functionality for enhanced microscope control.