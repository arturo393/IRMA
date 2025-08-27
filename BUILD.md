# IRMA Build and Installation Guide

This guide provides detailed instructions for building and installing the IRMA system on various platforms.

## System Requirements

### Supported Platforms

#### Primary Target Platforms
- **Linux PC** (x86/x64) - Development and simulation
- **Gumstix Overo** (ARM) - Embedded deployment
- **Custom robot platforms** with Linux support

#### Tested Distributions
- **Fedora Core 11** (primary development platform)
- **Ubuntu 18.04/20.04** (community tested)
- **Debian-based** distributions

### Hardware Requirements

#### Minimum PC Requirements
- **CPU**: x86/x64 processor
- **RAM**: 2GB minimum, 4GB recommended
- **Storage**: 1GB for source code and builds
- **Display**: X11-compatible for visualization

#### Embedded Target Requirements
- **ARM processor** (ARMv5TE or newer)
- **RAM**: 256MB minimum, 512MB recommended
- **Storage**: 64MB for binaries and configuration
- **I/O**: Serial ports for sensor communication

### Software Dependencies

#### Required Packages

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install gcc-multilib g++-multilib  # For 32-bit builds
sudo apt-get install libx11-dev                 # For visualization
sudo apt-get install git make

# Fedora/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install glibc-devel.i686              # For 32-bit builds
sudo yum install libX11-devel                   # For visualization
sudo yum install git make

# Additional libraries
sudo apt-get install libjpeg-dev libpng-dev    # Image processing
```

#### Optional Dependencies

```bash
# For cross-compilation (Gumstix)
sudo apt-get install gcc-arm-linux-gnueabi
sudo apt-get install g++-arm-linux-gnueabi

# For development tools
sudo apt-get install gdb valgrind              # Debugging
sudo apt-get install doxygen                   # Documentation
```

## Build System Overview

### Build Architecture

The IRMA build system uses:
- **GNU Make** for build management
- **GCC/G++** for compilation
- **Modular Makefiles** for each processor
- **Cross-compilation support** for embedded targets

### Build Configurations

#### Platform Selection
```makefile
PC = true          # Build for PC (false for embedded)
MSP = true         # Use MSP430 interface (false for AVR)
DEBUG = true       # Enable debug builds
```

#### Target Architectures
- **PC 32-bit**: Development and simulation
- **PC 64-bit**: Modern development platforms  
- **ARM**: Embedded deployment (Gumstix)

## Installation Instructions

### 1. Repository Setup

```bash
# Clone the repository
git clone https://github.com/arturo393/IRMA.git
cd IRMA

# Check available branches
git branch -a

# Switch to master branch (if not already)
git checkout master
```

### 2. Environment Setup

#### Set Environment Variables

```bash
# Add to ~/.bashrc or ~/.profile
export IRMA_HOME=$HOME/IRMA
export PATH=$IRMA_HOME/bin:$PATH

# For cross-compilation (if using Gumstix)
export GUMSTIX_BUILDROOT=/path/to/gumstix-buildroot
export CROSS_COMPILE=arm-linux-gnueabi-
```

#### Create Build Directories

```bash
cd $IRMA_HOME
mkdir -p bin
mkdir -p logs
mkdir -p laser_output
```

### 3. Configure Hardware Permissions

```bash
# Add user to dialout group for serial access
sudo usermod -a -G dialout $USER

# Set device permissions (add to udev rules for permanent)
sudo chown $USER /dev/ttyACM0
sudo chown $USER /dev/ttyUSB0

# Or create udev rule
echo 'SUBSYSTEM=="tty", ATTRS{idVendor}=="1d50", MODE="0666"' | sudo tee /etc/udev/rules.d/99-irma.rules
sudo udevadm control --reload-rules
```

## Building IRMA

### Build Process Overview

1. **Initialize Common Data Area**
2. **Build Core Libraries**
3. **Build Individual Processors**
4. **Create System Scripts**

### Step-by-Step Build

#### 1. Build Common Libraries

```bash
cd src/common/CDA
make clean
make

cd ../configFile
make clean
make
```

#### 2. Build System Initialization

```bash
cd ../../init
make clean
make
```

#### 3. Build Individual Processors

```bash
# Build in dependency order
processors=(
    "monitor"
    "executive"
    "closeRangeNavigator"
    "longRangeNavigator"
    "featureNavigator"
    "laser"
    "cameraBlobber"
    "mapper"
    "objCognitive"
    "numCognitive"
    "network"
)

for proc in "${processors[@]}"; do
    echo "Building $proc..."
    cd "src/processors/$proc"
    make clean
    make
    if [ $? -ne 0 ]; then
        echo "Build failed for $proc"
        exit 1
    fi
    cd - > /dev/null
done
```

#### 4. Automated Build Script

Create `build.sh`:

```bash
#!/bin/bash
set -e

echo "Building IRMA System..."

# Build common libraries
echo "Building common libraries..."
cd src/common/CDA && make clean && make && cd - > /dev/null
cd src/common/configFile && make clean && make && cd - > /dev/null

# Build initialization
echo "Building initialization..."
cd src/init && make clean && make && cd - > /dev/null

# Build processors
echo "Building processors..."
for dir in src/processors/*/; do
    if [ -f "$dir/Makefile" ]; then
        proc=$(basename "$dir")
        echo "  Building $proc..."
        cd "$dir" && make clean && make && cd - > /dev/null
    fi
done

echo "Build completed successfully!"
```

Make it executable:
```bash
chmod +x build.sh
./build.sh
```

## Platform-Specific Instructions

### PC Development Build

```bash
# Edit Makefiles to enable PC build
find src -name "Makefile" -exec sed -i 's/PC = false/PC = true/' {} \;

# Build for 32-bit (recommended for compatibility)
export CFLAGS="-m32"
export CXXFLAGS="-m32"
export LDFLAGS="-m32"

./build.sh
```

### Embedded Cross-Compilation

#### Setup Cross-Compiler

```bash
# Install cross-compilation tools
sudo apt-get install gcc-arm-linux-gnueabi g++-arm-linux-gnueabi

# Set environment
export CROSS_COMPILE=arm-linux-gnueabi-
export CC=${CROSS_COMPILE}gcc
export CXX=${CROSS_COMPILE}g++
```

#### Configure for Embedded Build

```bash
# Edit Makefiles for embedded target
find src -name "Makefile" -exec sed -i 's/PC = true/PC = false/' {} \;

# Build
./build.sh
```

## Configuration

### Hardware Configuration

#### Laser Scanner Setup

```bash
# Edit laser configuration
cd config
cp laser.conf.example laser.conf

# Configure scanner type and parameters
# Hokuyo URG-04LX example:
echo "scanner_type = hokuyo_urg" >> laser.conf
echo "device_port = /dev/ttyACM0" >> laser.conf
echo "baud_rate = 115200" >> laser.conf
```

#### Motor Controller Setup

```bash
# Configure motor controller
cp motor.conf.example motor.conf

# MSP430 example:
echo "controller_type = msp430" >> motor.conf
echo "device_port = /dev/ttyUSB0" >> motor.conf
echo "baud_rate = 9600" >> motor.conf
```

### System Configuration

#### Memory Configuration

Edit system limits if needed:

```bash
# Check current limits
ipcs -l

# Increase shared memory if needed
echo "kernel.shmmax = 134217728" | sudo tee -a /etc/sysctl.conf
sudo sysctl -p
```

#### Process Priorities

```bash
# Allow real-time priorities (optional)
echo "$USER - rtprio 99" | sudo tee -a /etc/security/limits.conf
echo "$USER - nice -20" | sudo tee -a /etc/security/limits.conf
```

## Running IRMA

### System Startup

#### Automated Startup

```bash
# Use provided startup script
./start_all.sh
```

#### Manual Startup

```bash
# Start components individually
cd $IRMA_HOME

# 1. Initialize shared memory
xterm -e "src/init/init-cda" &
sleep 1

# 2. Start monitor
xterm -e "src/processors/monitor/monitor" &
sleep 1

# 3. Start navigation
xterm -e "src/processors/closeRangeNavigator/closeRangeNavigator" &
sleep 1

# 4. Start executive
xterm -e "src/processors/executive/executive 0" &
sleep 1

# 5. Start laser processor
cd laser_output
../src/processors/laser/laser
```

### Testing Installation

#### Basic System Test

```bash
# Check shared memory
ipcs -m | grep $(whoami)

# Check processes
ps aux | grep -E "(monitor|executive|laser)"

# Monitor system log
tail -f logs/irma.log
```

#### Hardware Test

```bash
# Test laser scanner
cd src/processors/laser/urg-0.8.18/samples/c
make
./simple_example

# Test motor controller
cd src/processors/executive
./test_motors
```

## Troubleshooting

### Common Build Issues

#### Missing 32-bit Libraries

```bash
# Error: cannot find -lstdc++
sudo apt-get install gcc-multilib g++-multilib

# Error: cannot find -lX11
sudo apt-get install libx11-dev:i386
```

#### Cross-Compilation Issues

```bash
# Error: cross-compiler not found
export PATH=/usr/bin:$PATH
export CROSS_COMPILE=arm-linux-gnueabi-

# Check cross-compiler
${CROSS_COMPILE}gcc --version
```

### Runtime Issues

#### Shared Memory Problems

```bash
# Clean up shared memory
ipcrm -m $(ipcs -m | grep $(whoami) | awk '{print $2}')

# Check system limits
cat /proc/sys/kernel/shmmax
```

#### Permission Issues

```bash
# Serial port access denied
sudo usermod -a -G dialout $USER
# Log out and log back in

# Device permissions
ls -l /dev/ttyACM0
sudo chmod 666 /dev/ttyACM0
```

#### Performance Issues

```bash
# Check system load
top
htop

# Monitor memory usage
free -h
ipcs -m -u
```

## Development Setup

### IDE Configuration

#### Code::Blocks Setup

```bash
# Install Code::Blocks
sudo apt-get install codeblocks

# Create project file
cd $IRMA_HOME
find src -name "*.cpp" -o -name "*.h" > irma.files
```

#### Eclipse CDT Setup

```bash
# Install Eclipse CDT
sudo apt-get install eclipse-cdt

# Import existing makefile project
# File -> Import -> C/C++ -> Existing Code as Makefile Project
```

### Debugging Setup

#### GDB Configuration

```bash
# Build with debug symbols
export CFLAGS="-g -O0"
export CXXFLAGS="-g -O0"
./build.sh

# Debug specific processor
gdb src/processors/laser/laser
```

#### Valgrind Setup

```bash
# Check for memory leaks
valgrind --leak-check=full src/processors/laser/laser
```

## Deployment

### Embedded Deployment

#### File Transfer

```bash
# Create deployment package
tar czf irma-embedded.tar.gz bin/ config/ start_embedded.sh

# Copy to target
scp irma-embedded.tar.gz root@robot:/opt/
```

#### Target Installation

```bash
# On target system
cd /opt
tar xzf irma-embedded.tar.gz
chmod +x start_embedded.sh

# Configure for embedded
./configure_embedded.sh
```

### Service Installation

#### Systemd Service

Create `/etc/systemd/system/irma.service`:

```ini
[Unit]
Description=IRMA Robotics System
After=network.target

[Service]
Type=forking
User=irma
Group=irma
WorkingDirectory=/opt/irma
ExecStart=/opt/irma/start_all.sh
ExecStop=/opt/irma/stop_all.sh
Restart=always

[Install]
WantedBy=multi-user.target
```

Enable service:
```bash
sudo systemctl enable irma
sudo systemctl start irma
```

---

*This build guide should get you started with IRMA development and deployment. For specific hardware configurations, refer to the individual processor documentation.*