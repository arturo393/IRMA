# Navigation Processors Documentation

This document covers the navigation processors in the IRMA system: Close Range Navigator, Long Range Navigator, and Feature Navigator.

## Overview

The IRMA system uses a layered navigation approach with three specialized processors:

1. **Close Range Navigator**: Reactive obstacle avoidance
2. **Long Range Navigator**: Global path planning
3. **Feature Navigator**: Landmark-based navigation

## Close Range Navigator

### Purpose
Provides real-time obstacle avoidance and collision prevention for safe robot operation in dynamic environments.

### Key Features
- **Reactive Navigation**: Fast response to immediate obstacles
- **Safety Priority**: Overrides other navigation commands when obstacles detected
- **Sensor Fusion**: Integrates laser and proximity sensor data
- **Dynamic Obstacle Handling**: Adapts to moving obstacles

### Implementation Details

```cpp
class CloseRangeNavigator : public ProcessorInterface {
private:
    CloseNavData* pCloseNav;        // Shared memory data
    LaserData* pLaser;              // Laser sensor data
    float safetyRadius;             // Minimum safe distance
    float maxVelocity;              // Maximum allowed velocity
    
public:
    virtual int init();
    virtual int step();
    virtual int cleanup();
    
private:
    bool detectObstacles();
    VelocityCommand calculateAvoidanceVelocity();
    bool isPathClear(float angle, float distance);
};
```

### Data Structure

```cpp
typedef struct {
    ControlData ctrl;               // Base control structure
    
    // Input (from other processors)
    float targetVx, targetVy;       // Desired velocity from path planner
    float targetVtheta;             // Desired angular velocity
    
    // Output (to motor control)
    float safeVx, safeVy;          // Safe velocity commands
    float safeVtheta;              // Safe angular velocity
    
    // Status
    bool obstacleDetected;         // Obstacle in path
    float obstacleDistance;        // Distance to nearest obstacle
    float obstacleAngle;          // Angle to nearest obstacle
    
} CloseNavData;
```

### Algorithm
1. Read desired velocity from long-range navigator
2. Analyze laser scan data for obstacles
3. Calculate obstacle-free velocity vector
4. Apply safety constraints
5. Output safe velocity commands

## Long Range Navigator

### Purpose
Performs global path planning and high-level navigation to reach distant goals while avoiding known obstacles.

### Key Features
- **Global Path Planning**: A* or similar algorithms for optimal paths
- **Map Integration**: Uses occupancy grid from mapping system
- **Goal Management**: Handles waypoints and complex missions
- **Path Optimization**: Smooths paths for efficient motion

### Implementation Details

```cpp
class LongRangeNavigator : public ProcessorInterface {
private:
    LongNavData* pLongNav;          // Shared memory data
    MapData* pMap;                  // Map data from mapper
    PathPlanner* planner;           // Path planning algorithm
    
public:
    virtual int init();
    virtual int step();
    virtual int cleanup();
    
private:
    Path planPath(Point start, Point goal);
    VelocityCommand followPath();
    bool isGoalReached();
    void updatePath();
};
```

### Data Structure

```cpp
typedef struct {
    ControlData ctrl;               // Base control structure
    
    // Input
    float goalX, goalY;            // Target position
    float goalTheta;               // Target orientation
    bool hasNewGoal;               // New goal flag
    
    // Current state
    float currentX, currentY;      // Current position
    float currentTheta;            // Current orientation
    
    // Path planning
    Path currentPath;              // Active path
    int currentWaypoint;           // Current waypoint index
    
    // Output
    float targetVx, targetVy;      // Velocity commands
    float targetVtheta;            // Angular velocity command
    
    // Status
    int planningStatus;            // Planning algorithm status
    float distanceToGoal;          // Remaining distance
    
} LongNavData;
```

### Path Planning Algorithm
1. Receive new goal coordinates
2. Load current occupancy grid
3. Run path planning algorithm (A*, RRT, etc.)
4. Generate smooth trajectory
5. Execute path following controller
6. Monitor progress and replan if needed

## Feature Navigator

### Purpose
Provides navigation based on environmental landmarks and features for robust localization in GPS-denied environments.

### Key Features
- **Landmark Detection**: Identifies distinctive environmental features
- **Feature Tracking**: Maintains feature database
- **Robust Localization**: Reduces drift in long missions
- **Topological Navigation**: Graph-based navigation between landmarks

### Implementation Details

```cpp
class FeatureNavigator : public ProcessorInterface {
private:
    FeatureNavData* pFeatureNav;    // Shared memory data
    FeatureDatabase* features;      // Known features
    FeatureMatcher* matcher;        // Feature matching
    
public:
    virtual int init();
    virtual int step();
    virtual int cleanup();
    
private:
    std::vector<Feature> detectFeatures();
    void updateFeatureDatabase();
    Position estimatePosition();
    VelocityCommand navigateToFeature();
};
```

### Data Structure

```cpp
typedef struct {
    ControlData ctrl;               // Base control structure
    
    // Feature data
    int numFeatures;               // Number of detected features
    Feature features[MAX_FEATURES]; // Feature array
    
    // Navigation
    int targetFeatureID;           // Target feature ID
    float featureX, featureY;      // Feature coordinates
    
    // Localization
    float confidence;              // Localization confidence
    float positionError;           // Estimated position error
    
    // Output
    float targetVx, targetVy;      // Velocity commands
    float targetVtheta;            // Angular velocity
    
} FeatureNavData;
```

## Navigation Hierarchy

### Command Priority

1. **Emergency Stop** (highest priority)
   - From executive processor
   - Immediate motor shutdown

2. **Close Range Navigation** 
   - Obstacle avoidance overrides
   - Safety-critical responses

3. **Long Range Navigation**
   - Global path following
   - Goal-directed motion

4. **Feature Navigation**
   - Landmark-based corrections
   - Localization improvements

### Velocity Arbitration

```cpp
// Navigation command fusion
VelocityCommand fuseNavigationCommands() {
    VelocityCommand result;
    
    // Start with long-range command
    result = longRangeCommand;
    
    // Apply feature-based corrections
    result = applyFeatureCorrections(result, featureCommand);
    
    // Apply close-range safety constraints
    result = applyObstacleAvoidance(result, closeRangeCommand);
    
    // Apply executive overrides
    if (executiveOverride) {
        result = executiveCommand;
    }
    
    return result;
}
```

## Integration with IRMA System

### Data Flow

```
Goals/Commands ────→ Executive Processor
                           │
                           ▼
                    Long Range Navigator
                           │
                           ▼ (target velocity)
Map Data ──────────→ Close Range Navigator ←──── Laser Data
                           │
                           ▼ (safe velocity)
                     Motor Controllers
```

### Shared Memory Integration

Each navigator reads and writes specific areas of the CDA:

```cpp
// Long Range Navigator
int LongRangeNavigator::step() {
    // Read goal from executive
    ExecutiveData* pExec = (ExecutiveData*)cda.getMemPtr(EXECUTIVE_AREA);
    cda.lockArea(EXECUTIVE_AREA);
    Goal newGoal = pExec->currentGoal;
    cda.unlockArea(EXECUTIVE_AREA);
    
    // Plan path and calculate velocity
    VelocityCommand cmd = planAndExecute(newGoal);
    
    // Send to close range navigator
    cda.lockArea(CLOSE_NAV_AREA);
    pCloseNav->targetVx = cmd.vx;
    pCloseNav->targetVy = cmd.vy;
    pCloseNav->targetVtheta = cmd.vtheta;
    cda.unlockArea(CLOSE_NAV_AREA);
    
    return IRMA2_OK;
}
```

## Configuration

### Navigation Parameters

Each navigator has configurable parameters:

```cpp
// Close Range Navigator
struct CloseNavConfig {
    float safetyRadius;        // Minimum obstacle distance
    float maxVelocity;         // Maximum safe velocity
    float reactionTime;        // Response time to obstacles
    float avoidanceGain;       // Avoidance controller gain
};

// Long Range Navigator  
struct LongNavConfig {
    float planningResolution;  // Grid resolution for planning
    float pathTolerance;       // Path following tolerance
    float goalTolerance;       // Goal reaching tolerance
    int maxPlanningTime;       // Maximum planning time (ms)
};

// Feature Navigator
struct FeatureNavConfig {
    float featureThreshold;    // Feature detection threshold
    int maxFeatures;           // Maximum tracked features
    float matchingTolerance;   // Feature matching tolerance
    float confidenceThreshold; // Minimum localization confidence
};
```

## Performance Considerations

### Real-time Requirements
- **Close Range**: ~20-50 Hz for safety
- **Long Range**: ~1-10 Hz for planning
- **Feature**: ~5-20 Hz for localization

### Computational Load
- Close range navigation: Low (simple reactive algorithms)
- Long range navigation: High (path planning algorithms)
- Feature navigation: Medium (feature detection and matching)

### Memory Usage
- Path storage for long-range planning
- Feature database for feature navigation
- Temporary buffers for calculations

## Troubleshooting

### Common Issues

1. **Oscillatory Behavior**
   - Check navigation parameter tuning
   - Verify sensor calibration
   - Check for conflicting commands

2. **Poor Path Following**
   - Verify map quality
   - Check localization accuracy
   - Tune path following parameters

3. **Feature Navigation Drift**
   - Check feature detection quality
   - Verify feature database integrity
   - Monitor localization confidence

### Debug Tools

```cpp
// Navigation status monitoring
void printNavigationStatus() {
    printf("Long Nav: Goal(%.2f,%.2f) Dist:%.2f Status:%d\n",
           pLongNav->goalX, pLongNav->goalY, 
           pLongNav->distanceToGoal, pLongNav->planningStatus);
           
    printf("Close Nav: Obstacle:%s Dist:%.2f Safe Vel:(%.2f,%.2f)\n",
           pCloseNav->obstacleDetected ? "YES" : "NO",
           pCloseNav->obstacleDistance,
           pCloseNav->safeVx, pCloseNav->safeVy);
}
```

---

*The navigation processors work together to provide safe and efficient autonomous navigation capabilities. Understanding their interaction is key to successful robot operation.*