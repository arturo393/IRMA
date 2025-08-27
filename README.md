# IRMA - Intelligent Robot Mobile Architecture

IRMA is a comprehensive autonomous robotics system designed for mobile robot navigation, mapping, and object recognition. The system implements a modular architecture with multiple specialized processors that communicate through a shared memory system.

## System Overview

IRMA is designed as a distributed system where multiple processors run concurrently, each handling specific aspects of robot functionality. The system uses a Common Data Area (CDA) for inter-process communication and synchronization.

### Key Features

- **Autonomous Navigation**: Close-range and long-range navigation capabilities
- **SLAM (Simultaneous Localization and Mapping)**: Real-time mapping and localization using laser sensors
- **Object Recognition**: Computer vision-based object detection and cognitive processing
- **Modular Architecture**: Independent processors for different robot functions
- **Real-time Processing**: Synchronized multi-process architecture with shared memory

## System Architecture

### Core Components

#### 1. Common Data Area (CDA)
- **Location**: `src/common/CDA/`
- **Purpose**: Provides shared memory communication between processors
- **Features**: Thread-safe data sharing, semaphore-based synchronization

#### 2. Processors
All processors inherit from `ProcessorInterface` and implement the following lifecycle:
- `init()`: Initialize processor resources
- `step()`: Execute one processing cycle
- `cleanup()`: Clean up resources

### Processor Modules

#### Navigation Processors

- **closeRangeNavigator** (`src/processors/closeRangeNavigator/`)
  - Handles close-range obstacle avoidance
  - Real-time collision prevention

- **longRangeNavigator** (`src/processors/longRangeNavigator/`)
  - Path planning for long-distance navigation
  - Global route optimization

- **featureNavigator** (`src/processors/featureNavigator/`)
  - Feature-based navigation using environmental landmarks

#### Sensor Processors

- **laser** (`src/processors/laser/`)
  - Laser scanner data processing
  - SLAM implementation using Polar Scan Matching (PSM)
  - Occupancy grid generation
  - Support for Sick LMS 200, Hokuyo URG-04LX, and Hokuyo UTM-30LX

- **cameraBlobber** (`src/processors/cameraBlobber/`)
  - Camera-based blob detection
  - Visual feature extraction
  - Implemented by Pablo Benaprés

#### Cognitive Processors

- **objCognitive** (`src/processors/objCognitive/`)
  - Object recognition and classification
  - Memory-based learning system
  - Multi-threaded pattern matching

- **numCognitive** (`src/processors/numCognitive/`)
  - Numerical cognition and analysis

#### System Management

- **executive** (`src/processors/executive/`)
  - Central control and coordination
  - System state management
  - Hardware interface coordination

- **monitor** (`src/processors/monitor/`)
  - System monitoring and diagnostics
  - Performance tracking

- **mapper** (`src/processors/mapper/`)
  - Map building and management
  - Occupancy grid processing

- **network** (`src/processors/network/`)
  - Network communication
  - Remote monitoring capabilities

## Installation and Setup

### Prerequisites

- Linux-based operating system (tested on Fedora Core 11)
- GCC/G++ compiler with 32-bit support
- X11 development libraries (for visualization)
- Make build system

### Build Instructions

1. **Clone the repository**:
   ```bash
   git clone https://github.com/arturo393/IRMA.git
   cd IRMA
   ```

2. **Build individual processors**:
   ```bash
   cd src/processors/[processor_name]
   make clean
   make
   ```

3. **Launch the system**:
   ```bash
   ./start_all.sh
   ```

### Configuration

- Configuration files are located in the `config/` directory
- Hardware settings can be adjusted in individual processor Makefiles
- Serial port permissions may need adjustment: `sudo chown irma /dev/ttyACM0 /dev/ttyUSB0`

## Usage

### Starting the System

The `start_all.sh` script launches all processors in the correct order:

1. Initialize Common Data Area (`init/init-cda`)
2. Start monitoring system (`monitor/monitor`)
3. Launch navigation processors
4. Start sensor processors
5. Begin executive control

### System Operation

Each processor runs independently and communicates through the CDA. The executive processor coordinates overall system behavior and responds to high-level commands.

## Repository Structure

```
IRMA/
├── src/
│   ├── common/           # Shared libraries and utilities
│   │   ├── CDA/         # Common Data Area implementation
│   │   ├── CogMem/      # Cognitive memory system
│   │   └── configFile/  # Configuration file handling
│   ├── processors/       # Individual processor modules
│   │   ├── base/        # Base processor interface
│   │   ├── laser/       # Laser sensor processing
│   │   ├── cameraBlobber/ # Camera blob detection
│   │   ├── executive/   # Central control
│   │   ├── monitor/     # System monitoring
│   │   └── [others]/    # Additional processors
│   ├── init/            # System initialization
│   └── utils/           # Utility programs
├── config/              # Configuration files
├── start_all.sh         # System startup script
└── README.md           # This file
```

## Branch Information

The repository contains several branches with different features and developments:

- **master**: Main stable branch
- **CRN**: Cognitive recognition network features
- **GridMap**: Grid-based mapping enhancements
- **Mapper**: Advanced mapping functionality
- **copilot/**: Development and fix branches

## Dependencies

### External Libraries

- **Polar Scan Matching**: Laser scan matching implementation by Albert Diosi and Lindsay Kleeman
- **CImg**: Image processing library
- **URG Library**: Hokuyo laser scanner interface (version 0.8.18)
- **CVBlobsLib**: Computer vision blob detection library

### Hardware Support

- **Laser Scanners**: Sick LMS 200, Hokuyo URG-04LX, Hokuyo UTM-30LX
- **Serial Interfaces**: USB and ACM devices
- **Robot Platforms**: Designed for differential drive robots

## Contributing

When contributing to IRMA:

1. Follow the existing coding style
2. Test your changes with hardware if possible
3. Update documentation for any new features
4. Ensure thread safety in shared memory access

## License

This project incorporates code with various licenses:

- IRMA core system: GNU Lesser General Public License version 3
- Polar Scan Matching: BSD License (Copyright 2005-2010, Albert Diosi and Lindsay Kleeman)
- Individual components may have specific license terms

## Authors and Acknowledgments

- **Tomás Arredondo** - IRMA2 system architecture and core development
- **Pablo Benaprés** - Camera blob detection processor
- **Albert Diosi and Lindsay Kleeman** - Polar Scan Matching algorithm
- Various contributors to individual processors and components

## Support

For issues and questions:
- Check existing documentation in individual processor directories
- Review configuration files for hardware-specific settings
- Consult the original research papers for algorithmic details

## Related Research

The IRMA system is based on research in autonomous robotics and incorporates algorithms from:

- A. Diosi and L. Kleeman, "Fast Laser Scan Matching using Polar Coordinates", IJRR 2007
- A. Diosi and L. Kleeman, "Laser Scan Matching in Polar Coordinates with Application to SLAM", IROS 2005

---

*This documentation provides an overview of the IRMA system. For detailed technical information, refer to the individual processor documentation and source code comments.*