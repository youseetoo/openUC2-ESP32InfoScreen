# OpenUC2 ESP32 Serial Interface

A Python package for communicating with the OpenUC2 ESP32 controller via USB Serial. This package provides bidirectional communication for microscope control including motors, LEDs, objective slots, sample positioning, and laser/PWM control.

## Features

- **Bidirectional Serial Communication**: Send commands to ESP32 and receive status updates
- **Enhanced Device Detection**: Validates ESP32 communication before connecting
- **Motor Control**: Control individual stepper motors and XY stage movements  
- **LED Control**: Set LED colors and brightness
- **Objective Slot Management**: Switch between objective slots (1 and 2)
- **Sample Position Tracking**: Display and update sample position on a map
- **PWM/Laser Control**: Control 4 PWM channels with 0-1024 range for laser intensity
- **Image Capture**: Trigger image capture with snap button
- **Callback System**: Register callbacks for various events (motor updates, LED changes, PWM updates, etc.)
- **Auto Port Detection**: Automatically find and validate ESP32 serial port
- **Clean JSON Protocol**: Proper separation of debug logs and command messages

## Enhanced UI Structure

The ESP32 interface now features individual tabs instead of nested structure:
- **Motor**: Individual motor control and XY stage movements
- **LED**: Color picker and brightness controls  
- **Objective**: Slot switching and snap button
- **Sample Map**: Visual position display with red indicator
- **Lasers**: PWM channels 1-4 with 0-1024 sliders
- **Acquisition**: Image capture settings and controls

## Installation

```bash
# Install from requirements
pip install -r requirements.txt

# Or install the package in development mode
pip install -e .
```

## Quick Start

```python
from uc2_serial_controller import UC2SerialController, find_esp32_port

# Auto-detect ESP32 port
port = find_esp32_port()
controller = UC2SerialController(port)

# Register callbacks
def on_led_update(data):
    print(f"LED changed: {data}")

def on_motor_update(data):
    print(f"Motor moved: {data}")

controller.on_led_update(on_led_update)
controller.on_motor_update(on_motor_update)

# Connect and control
if controller.connect():
    # Control LED
    controller.set_led(True, 255, 0, 0)  # Red
    
    # Move motors
    controller.move_motor(1, 1000)  # Motor 1, speed 1000
    controller.move_xy_motors(500, -500)  # XY movement
    
    # Switch objective slots
    controller.set_objective_slot(2)
    
    # Control PWM/Lasers (NEW)
    controller.set_pwm_value(1, 512)  # Channel 1, half power
    controller.set_pwm_value(2, 1024) # Channel 2, full power
    
    # Capture image
    controller.snap_image()
    
    # Update sample position
    controller.update_sample_position(0.3, 0.7)  # Normalized coordinates
```

## API Reference

### UC2SerialController Class

#### Connection Methods
- `connect(port=None)` - Connect to ESP32
- `disconnect()` - Disconnect from ESP32
- `connected` (property) - Check connection status

#### Control Methods
- `set_led(enabled, r, g, b)` - Control LED state and color
- `move_motor(motor_id, speed)` - Move individual motor
- `move_xy_motors(speed_x, speed_y)` - Move XY motors simultaneously  
- `set_objective_slot(slot)` - Switch objective slot (1 or 2)
- `snap_image()` - Trigger image capture
- `update_sample_position(x, y)` - Update sample position (0-1 coordinates)

#### Callback Registration
- `on_status_update(callback)` - Status change events
- `on_motor_update(callback)` - Motor position updates
- `on_led_update(callback)` - LED state changes
- `on_objective_slot_update(callback)` - Objective slot changes
- `on_sample_position_update(callback)` - Sample position updates
- `on_image_captured(callback)` - Image capture events
- `on_connection_changed(callback)` - Connection status changes

#### State Properties
- `motor_positions` - Current motor positions
- `led_status` - Current LED state
- `current_objective_slot` - Active objective slot
- `current_sample_position` - Current sample position

### Utility Functions
- `find_esp32_port()` - Automatically detect ESP32 serial port

## Message Protocol

The serial communication uses JSON messages for bidirectional communication:

### Commands (Python → ESP32)
```json
// LED control
{"type": "led_command", "data": {"enabled": true, "r": 255, "g": 0, "b": 0}}

// Motor control
{"type": "motor_command", "data": {"motor": 1, "speed": 1000}}
{"type": "motor_xy_command", "data": {"speedX": 500, "speedY": -500}}

// Objective slot
{"type": "objective_slot_command", "data": {"slot": 2}}

// Image capture
{"type": "snap_image_command"}

// Sample position
{"type": "sample_position_command", "data": {"x": 0.3, "y": 0.7}}
```

### Updates (ESP32 → Python)
```json
// Status update
{"type": "status_update", "data": {"connected": true, "modules": {"motor": true, "led": true}}}

// Motor update
{"type": "motor_update", "data": {"positions": {"x": 1000, "y": 2000, "z": 500}}}

// LED update  
{"type": "led_update", "data": {"enabled": true, "r": 255, "g": 0, "b": 0}}

// Objective slot update
{"type": "objective_slot_update", "data": {"current_slot": 2}}

// Sample position update
{"type": "sample_position_update", "data": {"x": 0.3, "y": 0.7}}

// Image captured
{"type": "image_captured", "data": {}}
```

## Hardware Requirements

- OpenUC2 ESP32 with serial interface firmware
- USB connection between PC and ESP32
- Compatible stepper motors and LED array
- Objective lens slots (optional)

## Troubleshooting

### Connection Issues
- Check USB cable connection
- Verify correct serial port (use `find_esp32_port()`)
- Ensure ESP32 is not connected to other applications
- Check baud rate (default: 115200)

### Communication Issues
- Monitor serial output for error messages
- Verify JSON message format
- Check for firmware compatibility
- Ensure proper grounding and connections

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

MIT License - see LICENSE file for details.