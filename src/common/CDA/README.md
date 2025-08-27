# Common Data Area (CDA) Documentation

The Common Data Area (CDA) is the core inter-process communication system in IRMA, providing shared memory and synchronization mechanisms for all processors.

## Overview

The CDA implements a shared memory architecture that allows multiple processors to communicate efficiently in real-time. It provides:

- **Shared Memory Segments**: Dedicated memory areas for each processor type
- **Semaphore Synchronization**: Thread-safe access to shared data
- **Process Management**: Tracking of active processors
- **Memory Management**: Automatic allocation and cleanup

## Architecture

### Memory Layout

The CDA divides shared memory into separate areas for different processor types:

```cpp
enum enum_process_areas {
    CLOSE_NAV_AREA = 0,      // Close range navigation data
    LONG_NAV_AREA,           // Long range navigation data
    FEATURE_NAV_AREA,        // Feature navigation data
    EXECUTIVE_AREA,          // Executive control data
    MAPPER_AREA,             // Mapping data
    MONITOR_AREA,            // System monitoring data
    NETWORK_AREA,            // Network communication data
    COGNITIVE_AREA,          // Cognitive processing data
    NUM_COG_AREA,            // Numerical cognition data
    CAM_BLOB_AREA,           // Camera blob detection data
    LASER_AREA,              // Laser sensor data
    NUM_PROCESS_AREAS        // Total number of areas
};
```

### Synchronization Areas

Each process area can have multiple semaphore-controlled sections:

```cpp
enum enum_sem_areas {
    control = 0,             // Control information
    data_1,                  // Primary data section
    data_2,                  // Secondary data section
    data_3,                  // Tertiary data section
    NUM_SEM_AREAS           // Total semaphore areas
};
```

## Class Interface

### CDA Class

```cpp
class CDA {
private:
    bool m_creator;                              // Whether this instance created the shared memory
    int nShmId;                                  // Shared memory segment ID
    char* pShmAddr[NUM_PROCESS_AREAS];          // Pointers to each memory area
    struct shmid_ds pShmDesc;                   // Shared memory descriptor
    Semaphores *sem;                            // Semaphore manager
    key_t key;                                  // Shared memory key

public:
    CDA(bool creator = false);                  // Constructor
    ~CDA();                                     // Destructor
    
    // Memory access
    char *getMemPtr(enum_process_areas i);      // Get pointer to specific area
    
    // Synchronization
    int lockArea(enum_process_areas i, enum_sem_areas area = data_1);
    int unlockArea(enum_process_areas i, enum_sem_areas area = data_1);
    
    // Process management
    unsigned short getAttchdProcesses();        // Get number of attached processes
};
```

## Usage Patterns

### Basic Usage

1. **Initialize CDA**:
   ```cpp
   CDA cda(true);  // true = creator (typically init process)
   // or
   CDA cda(false); // false = client (typical processors)
   ```

2. **Get Memory Pointer**:
   ```cpp
   LaserData* pLaser = (LaserData*)cda.getMemPtr(LASER_AREA);
   ```

3. **Safe Data Access**:
   ```cpp
   cda.lockArea(LASER_AREA);
   // Read/write shared data
   pLaser->x = newX;
   pLaser->y = newY;
   cda.unlockArea(LASER_AREA);
   ```

### Processor Integration

All processors follow this pattern:

```cpp
class ProcessorExample : public ProcessorInterface {
private:
    ExampleData* pData;     // Pointer to shared data
    
public:
    int init() {
        // Get pointer to this processor's memory area
        pData = (ExampleData*)cda.getMemPtr(EXAMPLE_AREA);
        
        // Set up control pointer and semaphore area
        pCtrl = &(pData->ctrl);
        sem_area = EXAMPLE_AREA;
        
        return IRMA2_OK;
    }
    
    int step() {
        // Lock memory area
        cda.lockArea(EXAMPLE_AREA);
        
        // Process data
        // ... processing logic ...
        
        // Update shared data
        pData->result = processedValue;
        
        // Unlock memory area
        cda.unlockArea(EXAMPLE_AREA);
        
        return IRMA2_OK;
    }
};
```

## Data Structures

### Control Structure

Every processor area includes a control structure:

```cpp
typedef struct {
    int pid;                    // Process ID
    int loop;                   // Loop control command
    // Additional control fields...
} ControlData;
```

### Loop Commands

```cpp
enum {
    stop_cmd = 0,              // Stop processing
    run_cmd,                   // Continuous processing
    step_cmd,                  // Single step processing
    halt_cmd                   // Terminate processor
};
```

## Memory Areas

### Laser Area (Example)
```cpp
typedef struct {
    ControlData ctrl;          // Control structure
    long timestamp;            // Last measurement timestamp
    float x, y, dir;          // Robot pose
    float dx, dy, ddir;       // Pose changes
    bool sensar;              // Sensing active flag
    int vr, vl;               // Wheel velocities
    unsigned char map[MAP_WIDTH][MAP_HEIGHT]; // Occupancy grid
} LaserData;
```

### Camera Blob Area (Example)
```cpp
typedef struct {
    ControlData ctrl;          // Control structure
    int nBlobs;               // Number of detected blobs
    BlobData blobs[MAX_BLOBS]; // Blob information array
    unsigned char image[IMAGE_SIZE]; // Processed image data
} CamBlobData;
```

## Initialization Process

### System Startup

1. **Init Process** (creator):
   ```cpp
   CDA cda(true);  // Creates shared memory segments
   // Initialize all memory areas
   ```

2. **Processor Launch**:
   ```cpp
   CDA cda(false); // Attaches to existing shared memory
   // Access specific memory areas
   ```

### Memory Layout Initialization

The CDA constructor sets up memory pointers:

```cpp
CDA::CDA(bool creator) {
    // Create or attach to shared memory
    pShmAddr[0] = (char*)shmat(nShmId, NULL, 0);
    
    // Set up pointers for each area
    pShmAddr[LONG_NAV_AREA] = pShmAddr[0] + sizeof(CloseNavData);
    pShmAddr[FEATURE_NAV_AREA] = pShmAddr[LONG_NAV_AREA] + sizeof(LongNavData);
    // ... continue for all areas
}
```

## Error Handling

### Error Codes

```cpp
// Common error codes
#define IRMA2_OK                    0
#define IRMA2_CDA_ERR_SHMGET       -1
#define IRMA2_CDA_ERR_SHMAT        -2
#define IRMA2_CDA_ERR_SHMDT        -3
```

### Error Scenarios

1. **Shared Memory Creation Failure**:
   - Check system limits: `cat /proc/sys/kernel/shmmax`
   - Verify permissions
   - Ensure no existing segments conflict

2. **Semaphore Lock Timeout**:
   - Check for deadlocks
   - Verify proper unlock calls
   - Monitor process status

3. **Memory Corruption**:
   - Always use lock/unlock pairs
   - Validate data before writing
   - Check array bounds

## Performance Considerations

### Memory Size

Total shared memory size calculation:
```cpp
size_t totalSize = sizeof(CloseNavData) + 
                   sizeof(LongNavData) + 
                   sizeof(FeatureNavData) + 
                   // ... all other data structures
```

### Lock Granularity

- **Fine-grained**: Lock specific data sections
- **Coarse-grained**: Lock entire processor area
- **Read-only access**: Often doesn't require locking

### Best Practices

1. **Minimize Lock Time**: Keep critical sections short
2. **Consistent Locking Order**: Prevent deadlocks
3. **Error Recovery**: Always unlock in error paths
4. **Data Validation**: Check data integrity

## Debugging and Monitoring

### Debug Information

```cpp
// Check attached processes
unsigned short processes = cda.getAttchdProcesses();
printf("Attached processes: %d\n", processes);

// Monitor memory usage
struct shmid_ds shmInfo;
shmctl(nShmId, IPC_STAT, &shmInfo);
printf("Memory size: %ld bytes\n", shmInfo.shm_segsz);
```

### Common Debug Commands

```bash
# List shared memory segments
ipcs -m

# Remove shared memory segment
ipcrm -m [shmid]

# Monitor semaphores
ipcs -s
```

## Integration with Processors

### ProcessorInterface Base Class

All processors inherit common CDA functionality:

```cpp
class ProcessorInterface {
protected:
    CDA cda;                      // CDA instance
    ControlData* pCtrl;           // Pointer to control data
    enum_process_areas sem_area;   // Assigned semaphore area
    
public:
    void execute() {
        init();
        
        // Main processing loop
        int loop_cmd;
        do {
            loop_cmd = pCtrl->loop;
            switch (loop_cmd) {
                case step_cmd:
                case run_cmd:
                    step();
                    break;
                case halt_cmd:
                    break;
                default:
                    usleep(10000);
            }
        } while (loop_cmd != halt_cmd);
        
        cleanup();
    }
};
```

## System Requirements

### Operating System
- Linux with System V IPC support
- Shared memory and semaphore support
- Sufficient system limits for shared memory

### Memory Requirements
- Minimum: ~1-2 MB shared memory
- Typical: 4-8 MB for full system
- Maximum: Limited by system configuration

### Permissions
- Processes must run with appropriate IPC permissions
- Shared memory creator needs sufficient privileges

---

*The CDA provides the foundation for all inter-process communication in IRMA. Proper understanding of its usage patterns is essential for developing new processors or modifying existing ones.*