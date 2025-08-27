# Executive Processor Documentation

The Executive processor is the central control component of the IRMA system, responsible for coordinating all other processors and managing overall system behavior.

## Overview

The Executive processor acts as the "brain" of the IRMA system, providing:

- **System Coordination**: Controls the execution of other processors
- **Hardware Interface**: Manages low-level hardware communication
- **State Management**: Maintains overall system state
- **Command Processing**: Handles high-level navigation and behavior commands
- **Safety Monitoring**: Ensures safe operation and emergency handling

## Architecture

### Core Responsibilities

1. **Processor Coordination**
   - Start/stop other processors
   - Monitor processor health
   - Manage execution priorities

2. **Hardware Management**
   - Motor control and odometry
   - Sensor data acquisition
   - Safety system monitoring

3. **Navigation Control**
   - High-level path planning
   - Goal management
   - Behavior arbitration

4. **System State**
   - Robot pose tracking
   - Mission status
   - Error handling

## Implementation Structure

### Class Hierarchy

```cpp
class ExecutiveProcessor : public ProcessorInterface {
private:
    // Hardware interfaces
    RobotInterface* robotHW;    // Robot hardware abstraction
    MotorController* motors;    // Motor control
    SensorManager* sensors;     // Sensor interfaces
    
    // System state
    RobotState currentState;    // Current robot state
    Mission currentMission;     // Active mission
    
    // Inter-processor communication
    ExecutiveData* pExecutive;  // Shared memory data
    
public:
    int init();                 // Initialize hardware and interfaces
    int step();                 // Execute one control cycle
    int cleanup();              // Clean up resources
    
    // Control methods
    int processCommands();      // Handle incoming commands
    int updateState();          // Update system state
    int coordinateProcessors(); // Manage other processors
};
```

### Hardware Interfaces

The Executive supports multiple hardware platforms:

#### MSP430 Platform (MSP = true)
- UART-based communication
- Serial driver interface
- Embedded control capabilities

#### AVR Platform (MSP = false) 
- I2C-based communication
- Robostix interface
- Bootloader support

#### PC Platform (PC = true)
- Simulation and testing
- Debug interfaces
- Development support

### Configuration

Build configuration is controlled via Makefile flags:

```makefile
PC = true       # Compile for PC (vs embedded)
MSP = true      # Use MSP430 interface (vs AVR)
```

## Data Structures

### Executive Data Area

```cpp
typedef struct {
    ControlData ctrl;           // Base control structure
    
    // Robot state
    float x, y, theta;          // Robot pose
    float vx, vy, vtheta;       // Robot velocities
    
    // Mission data
    int missionState;           // Current mission state
    Goal currentGoal;           // Active navigation goal
    
    // System status
    int systemHealth;           // Overall system health
    ProcessorStatus procStatus[NUM_PROCESSORS]; // Processor states
    
    // Hardware status
    MotorStatus motors;         // Motor controller status
    SensorStatus sensors;       // Sensor system status
    
    // Safety systems
    bool emergencyStop;         // Emergency stop flag
    float batteryLevel;         // Battery voltage
    
} ExecutiveData;
```

### Command Interface

```cpp
enum ExecutiveCommands {
    CMD_STOP = 0,              // Stop all motion
    CMD_GO_TO_GOAL,            // Navigate to specified goal
    CMD_FOLLOW_PATH,           // Follow predefined path
    CMD_EXPLORE,               // Autonomous exploration
    CMD_EMERGENCY_STOP,        // Emergency stop
    CMD_RESET_SYSTEM,          // System reset
    CMD_CALIBRATE              // Hardware calibration
};

typedef struct {
    int command;               // Command type
    float params[8];           // Command parameters
    long timestamp;            // Command timestamp
} ExecutiveCommand;
```

## Processing Pipeline

### Main Control Loop

```cpp
int ExecutiveProcessor::step() {
    // 1. Process incoming commands
    processCommands();
    
    // 2. Update robot state from sensors
    updateRobotState();
    
    // 3. Execute current mission logic
    executeMission();
    
    // 4. Coordinate other processors
    coordinateProcessors();
    
    // 5. Update hardware outputs
    updateHardware();
    
    // 6. Monitor system health
    monitorSystemHealth();
    
    // 7. Update shared memory
    updateSharedMemory();
    
    return IRMA2_OK;
}
```

### Command Processing

```cpp
int ExecutiveProcessor::processCommands() {
    // Check for new commands
    if (newCommandAvailable()) {
        ExecutiveCommand cmd = getNextCommand();
        
        switch (cmd.command) {
            case CMD_GO_TO_GOAL:
                setNavigationGoal(cmd.params[0], cmd.params[1], cmd.params[2]);
                break;
                
            case CMD_EMERGENCY_STOP:
                emergencyStop();
                break;
                
            case CMD_EXPLORE:
                startExploration();
                break;
                
            // ... handle other commands
        }
    }
    
    return IRMA2_OK;
}
```

## Hardware Interface Layer

### Motor Control

```cpp
class MotorController {
public:
    int setVelocity(float vx, float vy, float vtheta);
    int getOdometry(float& x, float& y, float& theta);
    int emergencyStop();
    MotorStatus getStatus();
};
```

### Sensor Interface

```cpp
class SensorManager {
public:
    int readSensors();
    SensorData getSensorData(SensorType type);
    bool isSensorHealthy(SensorType type);
};
```

## Inter-Processor Coordination

### Processor Management

The Executive coordinates other processors through the CDA:

```cpp
int ExecutiveProcessor::coordinateProcessors() {
    // Check processor health
    for (int i = 0; i < NUM_PROCESSORS; i++) {
        if (!isProcessorHealthy(i)) {
            restartProcessor(i);
        }
    }
    
    // Update processor commands
    updateNavigationProcessors();
    updateSensorProcessors();
    updateMappingProcessors();
    
    return IRMA2_OK;
}
```

### Data Flow Coordination

```cpp
int ExecutiveProcessor::updateNavigationProcessors() {
    // Update close range navigator
    CloseNavData* pCloseNav = (CloseNavData*)cda.getMemPtr(CLOSE_NAV_AREA);
    cda.lockArea(CLOSE_NAV_AREA);
    pCloseNav->targetVelocity = calculateSafeVelocity();
    cda.unlockArea(CLOSE_NAV_AREA);
    
    // Update long range navigator
    LongNavData* pLongNav = (LongNavData*)cda.getMemPtr(LONG_NAV_AREA);
    cda.lockArea(LONG_NAV_AREA);
    pLongNav->currentGoal = currentMission.goal;
    cda.unlockArea(LONG_NAV_AREA);
    
    return IRMA2_OK;
}
```

## Mission Management

### Mission States

```cpp
enum MissionState {
    MISSION_IDLE = 0,          // No active mission
    MISSION_PLANNING,          // Planning phase
    MISSION_EXECUTING,         // Active execution
    MISSION_PAUSED,            // Temporarily paused
    MISSION_COMPLETED,         // Successfully completed
    MISSION_FAILED,            // Failed execution
    MISSION_ABORTED            // User aborted
};
```

### Mission Execution

```cpp
int ExecutiveProcessor::executeMission() {
    switch (currentMission.state) {
        case MISSION_PLANNING:
            if (planMission()) {
                currentMission.state = MISSION_EXECUTING;
            }
            break;
            
        case MISSION_EXECUTING:
            if (updateMissionProgress()) {
                if (isMissionComplete()) {
                    currentMission.state = MISSION_COMPLETED;
                }
            } else {
                currentMission.state = MISSION_FAILED;
            }
            break;
            
        // ... handle other states
    }
    
    return IRMA2_OK;
}
```

## Safety Systems

### Emergency Stop

```cpp
int ExecutiveProcessor::emergencyStop() {
    // Immediately stop all motors
    motors->emergencyStop();
    
    // Set emergency flag in shared memory
    pExecutive->emergencyStop = true;
    
    // Notify all processors
    broadcastEmergencyStop();
    
    // Log emergency event
    logEmergencyEvent("Emergency stop activated");
    
    return IRMA2_OK;
}
```

### System Health Monitoring

```cpp
int ExecutiveProcessor::monitorSystemHealth() {
    // Check battery level
    if (pExecutive->batteryLevel < BATTERY_LOW_THRESHOLD) {
        handleLowBattery();
    }
    
    // Check processor health
    checkProcessorHealth();
    
    // Check hardware status
    checkHardwareHealth();
    
    // Update overall system health
    updateSystemHealthStatus();
    
    return IRMA2_OK;
}
```

## Configuration and Calibration

### Hardware Configuration

The Executive reads configuration from files:

```cpp
int ExecutiveProcessor::loadConfiguration() {
    ConfigFile config("../config/executive.conf");
    
    // Load motor parameters
    motorGains.kp = config.getFloat("motor.kp");
    motorGains.ki = config.getFloat("motor.ki");
    motorGains.kd = config.getFloat("motor.kd");
    
    // Load safety parameters
    safetyLimits.maxVelocity = config.getFloat("safety.max_velocity");
    safetyLimits.maxAcceleration = config.getFloat("safety.max_acceleration");
    
    return IRMA2_OK;
}
```

### Calibration Procedures

```cpp
int ExecutiveProcessor::calibrateSystem() {
    // Calibrate motors
    calibrateMotors();
    
    // Calibrate sensors
    calibrateSensors();
    
    // Calibrate odometry
    calibrateOdometry();
    
    // Save calibration data
    saveCalibrationData();
    
    return IRMA2_OK;
}
```

## Build System

### Makefile Configuration

The Executive processor supports multiple build targets:

```makefile
# Platform selection
ifeq ($(PC), true)
    CPPFLAGS += -DPC -m32 -ggdb
    CC = g++ -g
else
    # Embedded target (Gumstix)
    TARGET_ARCH = -Os -march=armv5te -mtune=xscale
    CC = $(CROSS_COMPILE)g++
endif

# Hardware interface selection
ifeq ($(MSP), true)
    CPPFLAGS += -DMSP_
    OBJS += msp.o uart-io.o driverSerial.o
else
    OBJS += robostix.o i2c-io.o i2c-api.o
endif
```

### Dependencies

Required libraries and components:
- Common Data Area (CDA)
- Configuration file system
- Hardware drivers (MSP/AVR specific)
- Logging system

## Integration Points

### With Navigation System

```cpp
// Provide goals to navigation processors
int setNavigationGoal(float x, float y, float theta) {
    LongNavData* pLongNav = (LongNavData*)cda.getMemPtr(LONG_NAV_AREA);
    cda.lockArea(LONG_NAV_AREA);
    pLongNav->goalX = x;
    pLongNav->goalY = y;
    pLongNav->goalTheta = theta;
    pLongNav->hasNewGoal = true;
    cda.unlockArea(LONG_NAV_AREA);
    
    return IRMA2_OK;
}
```

### With Sensor Systems

```cpp
// Get sensor data for decision making
int updateSensorData() {
    LaserData* pLaser = (LaserData*)cda.getMemPtr(LASER_AREA);
    cda.lockArea(LASER_AREA);
    currentPose.x = pLaser->x;
    currentPose.y = pLaser->y;
    currentPose.theta = pLaser->dir;
    cda.unlockArea(LASER_AREA);
    
    return IRMA2_OK;
}
```

## Debugging and Monitoring

### Debug Output

Enable debugging with compiler flags:

```cpp
#ifdef DEBUG
    printf("Executive: Current state = %d\n", currentMission.state);
    printf("Executive: Robot pose = (%.2f, %.2f, %.2f)\n", 
           currentPose.x, currentPose.y, currentPose.theta);
#endif
```

### Log System

```cpp
class Logger {
public:
    void logInfo(const char* message);
    void logWarning(const char* message);
    void logError(const char* message);
    void logEmergency(const char* message);
};
```

### Performance Monitoring

```cpp
int ExecutiveProcessor::monitorPerformance() {
    // Measure cycle time
    static long lastCycleTime = 0;
    long currentTime = getCurrentTime();
    long cycleTime = currentTime - lastCycleTime;
    
    if (cycleTime > MAX_CYCLE_TIME) {
        logWarning("Executive cycle time exceeded threshold");
    }
    
    lastCycleTime = currentTime;
    return IRMA2_OK;
}
```

## Usage Examples

### Basic Operation

```bash
# Build executive processor
cd src/processors/executive
make clean
make

# Run executive (started by system script)
./start_all.sh

# Or run manually
./executive 0
```

### Configuration

Edit configuration files:
```bash
# Hardware configuration
edit config/executive.conf

# Platform selection
edit src/processors/executive/Makefile
```

---

*The Executive processor serves as the central coordinator for the entire IRMA system. Understanding its operation is crucial for system integration and debugging.*