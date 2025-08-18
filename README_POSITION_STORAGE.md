# Position Storage Feature - README

## Quick Start

### For Backend Developers

To integrate with the new position storage feature:

1. **Send position updates periodically**:
```python
import json

# Send current motor position to InfoScreen
position_data = {
    "type": "position_update", 
    "data": {
        "x": current_motor_x,
        "y": current_motor_y,
        "z": current_motor_z
    }
}
serial_connection.write(json.dumps(position_data))
```

2. **Handle go-to commands from InfoScreen**:
```python
# In your message handler
if message["type"] == "goto_position_command":
    coords = message["data"]
    move_motors_to_position(coords["x"], coords["y"], coords["z"])
    
    # Send confirmation when movement complete
    send_position_update(coords["x"], coords["y"], coords["z"])
```

### For Users

1. Navigate to **Motor** tab, then **Positions** sub-tab
2. Current position displays real-time coordinates from motors
3. Click **"Save Current"** to store the current position
4. Saved positions appear in scrollable list below
5. Click **▶** (play) icon to navigate to any saved position
6. Click **🗑** (trash) icon to delete individual positions
7. Click **"Delete All"** to clear all saved positions

### Storage Details

- Positions are stored in ESP32 flash memory (persistent across reboots)
- Maximum 20 positions can be stored
- Each position includes X, Y, Z coordinates as floating-point numbers
- Positions are auto-named as "Pos 1", "Pos 2", etc.

## File Summary

**Modified files for this feature:**
- `src/uc2ui_motorpage.h` - Function declarations
- `src/uc2ui_motorpage.cpp` - UI implementation and storage logic
- `src/SerialApi.h` - Communication interface  
- `src/SerialApi.cpp` - Message handlers
- `src/main.ino` - Listener setup

**Total lines added:** ~350 lines
**Dependencies added:** ESP32 Preferences library (standard)