# Processor Base Class Documentation

The ProcessorInterface base class provides the foundation for all IRMA processors, implementing common functionality and establishing the standard processor lifecycle.

## Overview

All IRMA processors inherit from `ProcessorInterface`, which provides:

- **Standardized Lifecycle**: Common initialization, execution, and cleanup patterns
- **CDA Integration**: Built-in Common Data Area access
- **Control Interface**: Standard command and control mechanisms
- **Synchronization**: Process coordination and timing

## Class Definition

```cpp
class ProcessorInterface {
protected:
    CDA cda;                              // Common Data Area instance
    ControlData* pCtrl;                   // Pointer to control data
    enum_process_areas sem_area;          // Assigned semaphore area
    
public:
    ProcessorInterface();                 // Constructor
    virtual ~ProcessorInterface();       // Virtual destructor
    
    // Lifecycle methods (to be implemented by subclasses)
    virtual int init() = 0;              // Initialize processor
    virtual int step() = 0;              // Process one cycle
    virtual int cleanup() = 0;           // Clean up resources
    
    // Main execution method
    void execute();                      // Main processor loop
};
```

## Processor Lifecycle

### 1. Initialization Phase

```cpp
virtual int init() {
    // Get pointer to processor's shared memory area
    pProcessorData = (ProcessorData*)cda.getMemPtr(PROCESSOR_AREA);
    
    // Set up control pointer for base class
    pCtrl = &(pProcessorData->ctrl);
    sem_area = PROCESSOR_AREA;
    
    // Initialize processor-specific resources
    initializeHardware();
    loadConfiguration();
    setupDataStructures();
    
    return IRMA2_OK;
}
```

### 2. Execution Phase

```cpp
virtual int step() {
    // Lock shared memory area
    cda.lockArea(sem_area);
    
    // Read input data
    readInputData();
    
    // Process data
    processData();
    
    // Write output data
    writeOutputData();
    
    // Unlock shared memory area
    cda.unlockArea(sem_area);
    
    return IRMA2_OK;
}
```

### 3. Cleanup Phase

```cpp
virtual int cleanup() {
    // Clean up processor-specific resources
    shutdownHardware();
    releaseMemory();
    closeFiles();
    
    return IRMA2_OK;
}
```

## Control System

### Control Structure

Every processor has a control structure in its shared memory area:

```cpp
typedef struct {
    int pid;                    // Process ID
    int loop;                   // Loop control command
    // Additional control fields as needed
} ControlData;
```

### Control Commands

```cpp
enum ControlCommands {
    stop_cmd = 0,              // Stop processing (wait)
    run_cmd,                   // Continuous processing
    step_cmd,                  // Single step processing
    halt_cmd                   // Terminate processor
};
```

### Command Processing

The base class handles command processing automatically:

```cpp
void ProcessorInterface::execute() {
    init();
    
    // Register process ID
    cda.lockArea(sem_area, control);
    pCtrl->pid = getpid();
    cda.unlockArea(sem_area, control);
    
    int loop_cmd;
    do {
        // Read loop command (non-blocking)
        loop_cmd = pCtrl->loop;
        
        switch (loop_cmd) {
            case step_cmd:
                // Execute one step then stop
                cda.lockArea(sem_area, control);
                pCtrl->loop = stop_cmd;
                cda.unlockArea(sem_area, control);
                // Fall through to run_cmd
                
            case run_cmd:
                // Execute processing step
                step();
                break;
                
            case halt_cmd:
                // Exit main loop
                break;
                
            case stop_cmd:
            default:
                // Wait and check again
                usleep(10000);  // 10ms sleep
        }
    } while (loop_cmd != halt_cmd);
    
    cleanup();
}
```

## Shared Memory Integration

### Memory Area Assignment

Each processor is assigned a specific memory area:

```cpp
// Example processor initialization
int LaserProcessor::init() {
    // Get assigned memory area
    pCDALaser = (LaserData*)cda.getMemPtr(LASER_AREA);
    
    // Set up base class pointers
    pCtrl = &(pCDALaser->ctrl);
    sem_area = LASER_AREA;
    
    // ... processor-specific initialization
    
    return IRMA2_OK;
}
```

### Data Synchronization

Use the CDA locking mechanism for thread-safe access:

```cpp
int ExampleProcessor::step() {
    // Lock before accessing shared data
    cda.lockArea(sem_area);
    
    // Read shared data
    float inputValue = pData->inputValue;
    
    // Process data
    float result = processValue(inputValue);
    
    // Write result back
    pData->outputValue = result;
    
    // Unlock after data access
    cda.unlockArea(sem_area);
    
    return IRMA2_OK;
}
```

## Implementation Examples

### Minimal Processor Implementation

```cpp
class MinimalProcessor : public ProcessorInterface {
private:
    MinimalData* pData;
    
public:
    virtual int init() {
        pData = (MinimalData*)cda.getMemPtr(MINIMAL_AREA);
        pCtrl = &(pData->ctrl);
        sem_area = MINIMAL_AREA;
        
        printf("MinimalProcessor: Initialized\n");
        return IRMA2_OK;
    }
    
    virtual int step() {
        cda.lockArea(sem_area);
        
        // Simple processing
        pData->counter++;
        pData->timestamp = getCurrentTime();
        
        cda.unlockArea(sem_area);
        
        return IRMA2_OK;
    }
    
    virtual int cleanup() {
        printf("MinimalProcessor: Cleaned up\n");
        return IRMA2_OK;
    }
};
```

### Sensor Processor Template

```cpp
class SensorProcessor : public ProcessorInterface {
private:
    SensorData* pSensorData;
    SensorHardware* hardware;
    
public:
    virtual int init() {
        // CDA setup
        pSensorData = (SensorData*)cda.getMemPtr(SENSOR_AREA);
        pCtrl = &(pSensorData->ctrl);
        sem_area = SENSOR_AREA;
        
        // Hardware initialization
        hardware = new SensorHardware();
        if (!hardware->initialize()) {
            return IRMA2_ERROR;
        }
        
        return IRMA2_OK;
    }
    
    virtual int step() {
        // Read sensor data
        SensorReading reading = hardware->readSensor();
        
        // Update shared memory
        cda.lockArea(sem_area);
        pSensorData->value = reading.value;
        pSensorData->timestamp = reading.timestamp;
        pSensorData->status = reading.status;
        cda.unlockArea(sem_area);
        
        return IRMA2_OK;
    }
    
    virtual int cleanup() {
        if (hardware) {
            hardware->shutdown();
            delete hardware;
        }
        return IRMA2_OK;
    }
};
```

## Error Handling

### Return Codes

Standard return codes for processor methods:

```cpp
#define IRMA2_OK            0      // Success
#define IRMA2_ERROR        -1      // General error
#define IRMA2_INIT_ERROR   -2      // Initialization error
#define IRMA2_HW_ERROR     -3      // Hardware error
#define IRMA2_MEM_ERROR    -4      // Memory error
```

### Error Handling Pattern

```cpp
virtual int step() {
    int result = IRMA2_OK;
    
    // Lock with error checking
    if (cda.lockArea(sem_area) != IRMA2_OK) {
        return IRMA2_MEM_ERROR;
    }
    
    try {
        // Processing logic
        result = processData();
        
    } catch (const std::exception& e) {
        printf("Processor error: %s\n", e.what());
        result = IRMA2_ERROR;
    }
    
    // Always unlock
    cda.unlockArea(sem_area);
    
    return result;
}
```

## Performance Considerations

### Processing Rate

Each processor should maintain consistent timing:

```cpp
virtual int step() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Processing logic here
    performProcessing();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Monitor processing time
    if (duration.count() > MAX_STEP_TIME_US) {
        printf("Warning: Step time exceeded limit: %ld us\n", duration.count());
    }
    
    return IRMA2_OK;
}
```

### Memory Management

```cpp
// Minimize memory allocation in step()
virtual int init() {
    // Allocate all needed memory during initialization
    workBuffer = new float[BUFFER_SIZE];
    tempData = new ProcessingData();
    
    return IRMA2_OK;
}

virtual int step() {
    // Use pre-allocated memory
    // Avoid dynamic allocation during processing
    
    return IRMA2_OK;
}

virtual int cleanup() {
    // Clean up allocated memory
    delete[] workBuffer;
    delete tempData;
    
    return IRMA2_OK;
}
```

## Thread Safety

### CDA Locking Guidelines

1. **Minimize Lock Time**: Keep critical sections as short as possible
2. **Consistent Locking Order**: Always lock areas in the same order to prevent deadlock
3. **Error Recovery**: Always unlock in error paths

```cpp
virtual int step() {
    // Good practice: minimal critical section
    cda.lockArea(sem_area);
    float input = pData->input;
    cda.unlockArea(sem_area);
    
    // Process outside critical section
    float result = expensiveProcessing(input);
    
    // Another minimal critical section
    cda.lockArea(sem_area);
    pData->output = result;
    cda.unlockArea(sem_area);
    
    return IRMA2_OK;
}
```

## Debugging Support

### Debug Output

```cpp
#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) printf("DEBUG [%s]: " fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

virtual int step() {
    DEBUG_PRINT("Processing step started");
    
    // Processing logic
    
    DEBUG_PRINT("Processing step completed");
    return IRMA2_OK;
}
```

### Status Monitoring

```cpp
virtual int step() {
    static int stepCount = 0;
    stepCount++;
    
    // Periodic status output
    if (stepCount % 100 == 0) {
        printf("Processor %s: %d steps completed\n", 
               getProcessorName(), stepCount);
    }
    
    return IRMA2_OK;
}
```

## Integration with IRMA System

### Main Function Template

```cpp
int main() {
    printf("Starting %s processor\n", PROCESSOR_NAME);
    
    try {
        ProcessorImplementation processor;
        processor.execute();
        
    } catch (const std::exception& e) {
        printf("Fatal error in %s: %s\n", PROCESSOR_NAME, e.what());
        return -1;
    }
    
    printf("%s processor terminated\n", PROCESSOR_NAME);
    return 0;
}
```

### System Integration

The processor integrates with the IRMA system through:
1. **Startup Script**: `start_all.sh` launches processors in correct order
2. **CDA**: Shared memory communication with other processors
3. **Control System**: External control through control commands

---

*The ProcessorInterface base class provides the essential foundation for all IRMA processors. Understanding its usage patterns is crucial for implementing new processors or modifying existing ones.*