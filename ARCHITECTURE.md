# IRMA System Architecture

## High-Level System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         IRMA System                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐        │
│  │  Executive  │    │   Monitor   │    │  Network    │        │
│  │ Processor   │    │ Processor   │    │ Processor   │        │
│  │ (Control)   │    │(Diagnostics)│    │(Comm)       │        │
│  └─────────────┘    └─────────────┘    └─────────────┘        │
│         │                   │                   │              │
│         └───────────────────┼───────────────────┘              │
│                             │                                  │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │              Common Data Area (CDA)                     │   │
│  │         (Shared Memory + Semaphores)                    │   │
│  └─────────────────────────────────────────────────────────┘   │
│         │           │           │           │           │       │
│         │           │           │           │           │       │
│  ┌──────▼────┐ ┌────▼────┐ ┌────▼────┐ ┌────▼────┐ ┌────▼────┐ │
│  │   Laser   │ │ Camera  │ │ Close   │ │  Long   │ │ Feature │ │
│  │Processor  │ │ Blob    │ │ Range   │ │ Range   │ │   Nav   │ │
│  │ (SLAM)    │ │Detector │ │   Nav   │ │   Nav   │ │         │ │
│  └───────────┘ └─────────┘ └─────────┘ └─────────┘ └─────────┘ │
│         │           │           │           │           │       │
│         │           │           │           │           │       │
│  ┌──────▼────┐ ┌────▼────┐ ┌────▼──────────▼──────────▼────┐   │
│  │  Mapper   │ │   Obj   │ │         Robot Hardware          │   │
│  │Processor  │ │Cognitive│ │     (Motors, Sensors, I/O)     │   │
│  │           │ │         │ │                                 │   │
│  └───────────┘ └─────────┘ └─────────────────────────────────┘   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## Processor Communication Flow

```
┌─────────────┐    Commands    ┌─────────────┐
│ External    │───────────────→│ Executive   │
│ Interface   │                │ Processor   │
└─────────────┘                └─────────────┘
                                      │
                                      │ Control Commands
                                      ▼
                        ┌─────────────────────────────┐
                        │     Common Data Area        │
                        │   (Shared Memory System)    │
                        └─────────────────────────────┘
                                      │
        ┌─────────────────────────────┼─────────────────────────────┐
        │                             │                             │
        ▼                             ▼                             ▼
┌─────────────┐              ┌─────────────┐              ┌─────────────┐
│  Sensor     │              │ Navigation  │              │ Cognitive   │
│ Processors  │              │ Processors  │              │ Processors  │
├─────────────┤              ├─────────────┤              ├─────────────┤
│ • Laser     │              │ • Close Nav │              │ • Object    │
│ • Camera    │              │ • Long Nav  │              │   Cognitive │
│             │              │ • Feature   │              │ • Numerical │
│             │              │   Navigator │              │   Cognitive │
└─────────────┘              └─────────────┘              └─────────────┘
        │                             │                             │
        │ Sensor Data                 │ Navigation Data             │ Recognition Data
        │                             │                             │
        └─────────────┐               │               ┌─────────────┘
                      │               │               │
                      ▼               ▼               ▼
                        ┌─────────────────────────────┐
                        │        Mapper              │
                        │      Processor             │
                        │   (Map Building)           │
                        └─────────────────────────────┘
```

## Data Flow Architecture

```
Hardware Layer:
┌────────────┐  ┌────────────┐  ┌────────────┐  ┌────────────┐
│Laser       │  │Camera      │  │Motors      │  │Encoders    │
│Scanner     │  │System      │  │(Drive)     │  │(Odometry)  │
└────────────┘  └────────────┘  └────────────┘  └────────────┘
       │               │               ▲               ▲
       │ Scan Data     │ Images        │ Commands      │ Position
       │               │               │               │
       ▼               ▼               │               │
┌────────────┐  ┌────────────┐        │               │
│Laser       │  │Camera      │        │               │
│Processor   │  │Blob        │        │               │
│            │  │Processor   │        │               │
└────────────┘  └────────────┘        │               │
       │               │               │               │
       │ Position      │ Objects       │               │
       │ Map Data      │ Detected      │               │
       │               │               │               │
       ▼               ▼               │               │
┌─────────────────────────────────────────────────────────────┐
│                Common Data Area                             │
│  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐   │
│  │   Laser   │ │  Camera   │ │Navigation │ │ Executive │   │
│  │   Data    │ │   Data    │ │   Data    │ │   Data    │   │
│  └───────────┘ └───────────┘ └───────────┘ └───────────┘   │
└─────────────────────────────────────────────────────────────┘
       │               │               ▲               │
       │ Map/Position  │ Objects       │ Goals         │ Control
       │               │               │               │
       ▼               ▼               │               ▼
┌────────────┐  ┌────────────┐  ┌────────────┐  ┌────────────┐
│Navigation  │  │Object      │  │Path        │  │Executive   │
│Processors  │  │Recognition │  │Planning    │  │Control     │
│            │  │            │  │            │  │            │
└────────────┘  └────────────┘  └────────────┘  └────────────┘
       │               │               │               │
       │ Velocity      │ Decisions     │ Path          │ Commands
       │ Commands      │               │               │
       │               │               │               │
       └───────────────┼───────────────┼───────────────┘
                       │               │
                       ▼               ▼
                ┌────────────────────────────┐
                │     Motor Control          │
                │    (Hardware Layer)        │
                └────────────────────────────┘
```

## Memory Layout (Common Data Area)

```
Shared Memory Segment:
┌─────────────────────────────────────────────────────────────┐
│  Memory Area 0: Close Range Navigation Data                 │
├─────────────────────────────────────────────────────────────┤
│  Memory Area 1: Long Range Navigation Data                  │
├─────────────────────────────────────────────────────────────┤
│  Memory Area 2: Feature Navigation Data                     │
├─────────────────────────────────────────────────────────────┤
│  Memory Area 3: Executive Control Data                      │
├─────────────────────────────────────────────────────────────┤
│  Memory Area 4: Mapper Data                                 │
├─────────────────────────────────────────────────────────────┤
│  Memory Area 5: Monitor Data                                │
├─────────────────────────────────────────────────────────────┤
│  Memory Area 6: Network Data                                │
├─────────────────────────────────────────────────────────────┤
│  Memory Area 7: Cognitive Processing Data                   │
├─────────────────────────────────────────────────────────────┤
│  Memory Area 8: Numerical Cognition Data                    │
├─────────────────────────────────────────────────────────────┤
│  Memory Area 9: Camera Blob Data                            │
├─────────────────────────────────────────────────────────────┤
│  Memory Area 10: Laser Data (Largest - includes map grid)   │
└─────────────────────────────────────────────────────────────┘

Each Memory Area Contains:
┌─────────────────────────────────────────────────────────────┐
│ Control Section:                                            │
│  • Process ID                                               │
│  • Loop control commands                                    │
│  • Status flags                                             │
├─────────────────────────────────────────────────────────────┤
│ Data Section 1: (Primary data)                             │
│  • Sensor readings                                          │
│  • Position information                                     │
│  • Processing results                                       │
├─────────────────────────────────────────────────────────────┤
│ Data Section 2: (Secondary data)                           │
│  • Configuration parameters                                 │
│  • Historical data                                          │
│  • Debug information                                        │
├─────────────────────────────────────────────────────────────┤
│ Data Section 3: (Auxiliary data)                           │
│  • Temporary buffers                                        │
│  • Inter-processor messages                                 │
│  • Extended status                                          │
└─────────────────────────────────────────────────────────────┘
```

## Process Lifecycle

```
System Startup:
┌─────────────┐
│ start_all.sh│
└─────────────┘
       │
       ▼
┌─────────────┐    1. Create CDA
│ init-cda    │───────────────────┐
└─────────────┘                   │
       │                          ▼
       │ 2. Start Monitor    ┌─────────────┐
       └────────────────────→│   Monitor   │
       │                     └─────────────┘
       │ 3. Start Navigation      │
       ├────────────────────┐     │ Monitor all
       ▼                    ▼     ▼
┌─────────────┐    ┌─────────────┐
│  Close Nav  │    │  Long Nav   │
└─────────────┘    └─────────────┘
       │                    │
       │ 4. Start Executive │
       └────────────────────┼──────┐
                            │      ▼
                            │ ┌─────────────┐
                            │ │ Executive   │
                            │ └─────────────┘
                            │      │
       5. Start Sensors     │      │ 6. Coordinate
       ┌────────────────────┘      │
       ▼                           ▼
┌─────────────┐         ┌─────────────────────┐
│   Laser     │         │ All Processors      │
│ Processor   │         │ Enter Main Loop     │
└─────────────┘         └─────────────────────┘
```

## Key Design Principles

### 1. Modular Architecture
- Each processor is an independent process
- Standardized interfaces through ProcessorInterface base class
- Clean separation of concerns

### 2. Real-time Communication
- Shared memory for high-speed data exchange
- Semaphore-based synchronization
- Minimal latency between processors

### 3. Fault Tolerance
- Independent process failures don't crash entire system
- Executive processor monitors system health
- Graceful degradation possible

### 4. Scalability
- Easy to add new processors
- Memory areas can be resized
- Processor-specific optimizations

### 5. Hardware Abstraction
- Hardware interfaces isolated in specific processors
- Platform-independent core algorithms
- Support for simulation and multiple robot platforms

---

*This architecture provides a robust foundation for autonomous robotics applications while maintaining flexibility for research and development.*