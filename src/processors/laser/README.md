# Laser Processor Documentation

The laser processor is one of the core components of the IRMA system, responsible for laser scanner data processing, SLAM (Simultaneous Localization and Mapping), and occupancy grid generation.

## Overview

The laser processor implements real-time laser scan processing using the Polar Scan Matching (PSM) algorithm developed by Albert Diosi and Lindsay Kleeman. It provides:

- Real-time laser data acquisition
- Scan matching and localization
- Occupancy grid mapping
- Visual feedback and debugging

## Features

### Supported Hardware
- **Sick LMS 200**: Industrial laser scanner
- **Hokuyo URG-04LX**: Compact laser range finder
- **Hokuyo UTM-30LX**: Long-range laser scanner

### Core Capabilities
- **Polar Scan Matching (PSM)**: Fast and accurate laser scan matching
- **Extended Kalman Filter (EKF)**: State estimation and tracking
- **Occupancy Grid Mapping**: Real-time map building
- **Multi-scan Processing**: Handles multiple laser scan comparisons
- **Real-time Visualization**: Live display of laser data and maps

## Implementation Details

### Class Structure
```cpp
class LaserProcessor : public ProcessorInterface {
    // Core processing methods
    int init();      // Initialize laser hardware and data structures
    int step();      // Process one laser scan cycle
    int cleanup();   // Clean up resources
};
```

### Processing Pipeline

1. **Data Acquisition**
   - Connect to laser scanner via URG library
   - Capture scan data with timestamp
   - Store raw data for processing

2. **Preprocessing**
   - Filter invalid readings
   - Apply noise reduction
   - Prepare data for scan matching

3. **Scan Matching**
   - Apply Polar Scan Matching algorithm
   - Calculate robot pose changes
   - Update position estimates

4. **State Estimation**
   - Run Extended Kalman Filter
   - Merge scan matching with odometry
   - Maintain pose uncertainty estimates

5. **Mapping**
   - Update occupancy grid
   - Integrate new scan data
   - Maintain map consistency

6. **Output Generation**
   - Update shared memory (CDA)
   - Generate visualization images
   - Save scan data files

### Memory Management

The processor uses the Common Data Area (CDA) to share data:

```cpp
typedef struct {
    long timestamp;     // Scan timestamp
    float x, y, dir;    // Robot pose
    float dx, dy, ddir; // Pose changes
    bool sensar;        // Sensing flag
    unsigned char map[MAP_WIDTH][MAP_HEIGHT]; // Occupancy grid
    // Additional fields...
} LaserData;
```

### Configuration

Key configuration parameters:
- `LASER_START`: Start index for laser data
- `MAP_WIDTH/MAP_HEIGHT`: Occupancy grid dimensions
- Scan matching parameters (alpha, beta, gamma)
- Visualization settings

## File Structure

```
src/processors/laser/
├── laser.cpp              # Main laser processor implementation
├── laser_old.cpp          # Legacy version (for reference)
├── laser.c                # C version (legacy)
├── laser.h                # Header file
├── Makefile               # Build configuration
├── polar_matching/        # PSM algorithm implementation
│   ├── polar_match.h      # PSM header
│   ├── polar_match.cpp    # PSM implementation
│   ├── README             # PSM documentation
│   └── data/              # Test data files
├── urg-0.8.18/           # URG library for Hokuyo scanners
│   ├── src/               # URG source code
│   ├── samples/           # Example programs
│   └── README.txt         # URG documentation
└── .deps/                # Build dependencies
```

## Usage

### Building
```bash
cd src/processors/laser
make clean
make
```

### Running
The laser processor is typically started by the main system script:
```bash
./start_all.sh
```

Or manually:
```bash
cd laser_output  # Create output directory
../laser/laser
```

### Configuration
Edit the Makefile to configure:
- Target platform (PC vs embedded)
- Hardware interface options
- Compilation flags

### Output Files
The processor generates:
- Timestamped scan files in `laser_output/`
- Debug log files
- Visualization images (if enabled)

## Algorithm Details

### Polar Scan Matching (PSM)
The PSM algorithm works by:
1. Converting Cartesian laser data to polar coordinates
2. Finding correlations between consecutive scans
3. Estimating translation and rotation between scans
4. Optimizing the match using iterative refinement

Benefits of PSM:
- Fast computation (real-time capable)
- Robust to noise and outliers
- Works well in structured environments
- Suitable for differential drive robots

### Extended Kalman Filter (EKF)
The EKF implementation:
- Predicts robot pose based on motion model
- Updates prediction using scan matching results
- Maintains uncertainty estimates
- Handles sensor noise and model errors

## Visualization

The processor provides multiple visualization modes:
- **Raw laser scans**: Display current laser readings
- **Occupancy grid**: Show built map
- **Robot trajectory**: Display path history
- **Scan matching**: Show matched scan overlays

Visualization is handled using the CImg library and X11 display.

## Performance Considerations

### Real-time Requirements
- Target processing rate: ~10-20 Hz (depending on scanner)
- Memory usage: Configurable based on map size
- CPU usage: Optimized for embedded platforms

### Optimization Tips
- Adjust laser data range (LASER_START parameter)
- Optimize occupancy grid size for memory constraints
- Use appropriate scan matching parameters for environment
- Consider disabling visualization for better performance

## Troubleshooting

### Common Issues

1. **Scanner not detected**
   - Check USB/serial connection
   - Verify device permissions: `sudo chown irma /dev/ttyACM0`
   - Test with URG sample programs

2. **Build errors**
   - Install 32-bit development libraries
   - Check Makefile target platform settings
   - Verify dependency libraries are installed

3. **Poor scan matching**
   - Adjust PSM parameters (alpha, beta, gamma)
   - Check laser mounting and calibration
   - Verify environment has sufficient features

4. **Memory issues**
   - Reduce occupancy grid size
   - Check CDA initialization
   - Monitor system memory usage

### Debug Options
Enable debugging by:
- Uncommenting debug output in source
- Using `#ifdef DEBUG` sections
- Running with verbose output

## Research References

1. A. Diosi and L. Kleeman, "Fast Laser Scan Matching using Polar Coordinates", International Journal of Robotics Research (IJRR), 2007

2. A. Diosi and L. Kleeman, "Laser Scan Matching in Polar Coordinates with Application to SLAM", Proceedings of IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS), 2005

## Integration with IRMA

The laser processor integrates with other IRMA components:
- **Executive**: Receives control commands and status updates
- **Navigator**: Provides map data and localization
- **Monitor**: Reports system status and performance
- **Mapper**: Shares occupancy grid data

Data flow through the Common Data Area ensures real-time coordination with other processors.

---

*For more technical details, refer to the source code comments and the original PSM research papers.*