# Changelog

All notable changes to the IRMA (Intelligent Robot Mobile Architecture) project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Comprehensive README.md documentation
- CHANGELOG.md for version tracking
- Detailed component documentation

### Changed
- Updated repository documentation structure

## [2.0.0] - Historical Release

### Major Features
- **IRMA2 Architecture**: Complete rewrite of the original IRMA system
- **Modular Processor Design**: Independent processors with standardized interfaces
- **Common Data Area (CDA)**: Shared memory system for inter-process communication
- **Real-time SLAM**: Simultaneous Localization and Mapping capabilities

### Processors Implemented

#### Core System
- **executive**: Central control and coordination system
- **monitor**: System monitoring and diagnostics
- **init-cda**: Common Data Area initialization

#### Navigation
- **closeRangeNavigator**: Close-range obstacle avoidance
- **longRangeNavigator**: Long-distance path planning
- **featureNavigator**: Feature-based navigation

#### Sensors
- **laser**: Laser scanner processing with PSM (Polar Scan Matching)
  - Support for Sick LMS 200
  - Support for Hokuyo URG-04LX
  - Support for Hokuyo UTM-30LX
  - Real-time occupancy grid generation
- **cameraBlobber**: Camera-based blob detection and tracking

#### Cognitive Processing
- **objCognitive**: Object recognition and classification
- **numCognitive**: Numerical cognition processing

#### Mapping and Networking
- **mapper**: Advanced mapping functionality
- **network**: Network communication capabilities

### Technical Improvements

#### Common Data Area (CDA)
- Thread-safe shared memory implementation
- Semaphore-based synchronization
- Support for multiple processor areas
- Automatic memory management

#### Laser Processing
- Integration of Polar Scan Matching algorithm (Diosi & Kleeman)
- Real-time scan matching and localization
- EKF (Extended Kalman Filter) implementation
- Occupancy grid mapping
- Multiple visualization modes

#### Build System
- Modular Makefile structure
- Cross-compilation support for ARM platforms
- PC and embedded target support
- Configuration management system

### Hardware Support
- Gumstix embedded platform support
- ARM cross-compilation
- MSP430 microcontroller interface
- I2C and UART communication protocols
- Serial device management

### Development Branches

#### Active Feature Branches
- **CRN**: Cognitive Recognition Network enhancements
- **GridMap**: Grid-based mapping improvements
- **Mapper**: Advanced mapping algorithm development

### Dependencies Added
- **URG Library v0.8.18**: Hokuyo laser scanner interface
- **Polar Matching Library**: PSM implementation by Diosi & Kleeman
- **CImg Library**: Image processing and visualization
- **CVBlobsLib**: Computer vision blob detection
- **Qt Framework**: GUI components (selective use)

### Configuration System
- File-based configuration management
- Hardware-specific parameter files
- Runtime configuration updates
- Calibration data storage

## [1.0.0] - Original IRMA System (Historical)

### Initial Features
- Basic autonomous navigation
- Simple sensor integration
- Prototype mapping capabilities

*Note: Detailed changelog for IRMA v1.0 is not available. IRMA2 represents a complete system redesign.*

## Branch-Specific Changes

### CRN Branch
- Enhanced cognitive recognition algorithms
- Improved pattern matching capabilities
- Memory-based learning systems

### GridMap Branch  
- Grid-based mapping enhancements
- Improved occupancy grid algorithms
- Better memory efficiency for large maps

### Mapper Branch
- Advanced SLAM algorithms
- Map merging capabilities
- Enhanced loop closure detection

## Known Issues and Limitations

### Current Limitations
- Build system requires 32-bit development libraries
- X11 dependency for visualization components
- Hardware-specific calibration required
- Limited documentation for some processors

### Compatibility Notes
- Tested primarily on Fedora Core 11
- ARM cross-compilation for Gumstix platforms
- Requires specific hardware setup for full functionality

## Future Roadmap

### Planned Improvements
- Modernized build system (CMake migration)
- Updated dependencies and library versions
- Enhanced documentation and tutorials
- Docker containerization for easier deployment
- ROS integration possibilities

### Research Integration
- Integration of newer SLAM algorithms
- Machine learning enhancements
- Multi-robot coordination capabilities
- Cloud-based mapping and planning

---

## Acknowledgments

### Research Contributions
- **Polar Scan Matching**: A. Diosi and L. Kleeman (Monash University)
- **SLAM Algorithms**: Various robotics research contributions
- **Computer Vision**: CVBlobsLib contributors

### Development Team
- **Tomás Arredondo**: IRMA2 architecture and core development
- **Pablo Benaprés**: Camera blob detection implementation
- **Multiple Contributors**: Individual processor development

### Licensing
- IRMA Core: GNU Lesser General Public License v3
- Polar Scan Matching: BSD License
- URG Library: Original license terms
- Third-party libraries: Respective licenses

---

*This changelog is maintained to track the evolution of the IRMA system. For technical details about specific changes, refer to commit messages and code documentation.*