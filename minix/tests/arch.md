# MINIX Test Suite Architecture

This directory contains comprehensive test suites for validating MINIX system functionality, including kernel features, system calls, drivers, servers, and architecture-specific code. The tests ensure system reliability and correctness across different platforms.

## Test Organization

### Core Test Infrastructure
- **common.c/common.h**: Shared test utilities and definitions
- **common-socket.c/common-socket.h**: Socket testing utilities
- **socklib.c/socklib.h**: Socket library test functions
- **run**: Test execution script
- **magic.h**: Test magic numbers and markers
- **Makefile**: Top-level test build configuration
- **Makefile.inc**: Common test build definitions

### Test Categories

#### System Call Tests (test*.c)
Comprehensive system call validation:
- **test1.c - test10.c**: Basic system calls (fork, exec, exit, wait)
- **test11.c - test20.c**: File operations (open, read, write, close)
- **test21.c - test30.c**: Process management and signals
- **test31.c - test40.c**: Memory management and virtual memory
- **test41.c - test50.c**: Inter-process communication
- **test51.c - test60.c**: Device I/O and drivers
- **test61.c - test70.c**: Network operations and sockets
- **test71.c - test80.c**: File systems and storage
- **test81.c - test90.c**: Security and permissions
- **test91.c - test94.c**: Advanced features and edge cases

#### Specialized Tests
- **testcache.c/testcache.h**: Buffer cache testing
- **testvm.c/testvm.h/testvm.conf**: Virtual memory testing
- **tvnd.c**: Virtual node device testing

### Architecture-Specific Tests

#### RISC-V 64-bit Tests (riscv64/)
Comprehensive RISC-V architecture validation:
- **run_tests.sh**: Test runner script for RISC-V
- **test_csr.c**: Control and Status Register testing
  - Validates CSR read/write operations
  - Tests privilege level transitions
  - Verifies architecture-specific registers

- **test_atomic.c**: Atomic operation testing
  - Load-linked/store-conditional operations
  - Atomic memory operations (AMO)
  - Memory ordering guarantees

- **test_sbi.c**: Supervisor Binary Interface testing
  - SBI call validation
  - Firmware interface testing
  - Timer and IPI functionality

- **test_trap.c**: Exception and trap handling
  - Exception vector testing
  - Trap frame validation
  - Interrupt handling verification

- **test_memory.c**: Memory management testing
  - Virtual memory setup
  - Page table operations
  - Memory protection validation

- **test_vm.c**: Virtual memory subsystem
  - Address space management
  - Memory mapping operations
  - Copy-on-write functionality

- **test_timer.c**: Timer functionality
  - Timer interrupt handling
  - Timer accuracy testing
  - Timer calibration

- **test_ipc.c**: Inter-process communication
  - Message passing validation
  - IPC performance testing
  - Cross-process communication

### Kernel Tests (kernel/)
Low-level kernel functionality testing:
- **run**: Kernel test runner
- **sys_padconf/**: Padding configuration tests
- **sys_vumap/**: Virtual memory mapping tests

### Driver Tests

#### Block Device Tests (blocktest/)
Storage driver validation:
- Block I/O operations
- Performance testing
- Error handling verification

#### Flash Block Device (fbdtest/)
Flash storage specific tests:
- Flash erase operations
- Wear leveling validation
- Bad block management

### Server Tests

#### Data Store Tests (ds/)
Data store server validation:
- Persistent storage operations
- Key-value functionality
- Data integrity testing

#### RMIB Tests (rmibtest/)
Remote Management Information Base:
- SNMP-like protocol testing
- Remote monitoring validation
- Management data retrieval

### DDE Kit Tests (ddekit/)
Driver Development Environment Kit:
- Driver framework testing
- Hardware abstraction validation
- Driver compatibility testing

### Architecture Tests (arch/)
Cross-architecture validation:
- Architecture-neutral tests
- Portability verification
- Common functionality testing

## Test Execution Framework

### Test Runner (run_tests.sh)
The RISC-V test runner provides:
- **Test Categories**: kernel, user, build, all
- **Automated Execution**: Sequential test execution
- **Result Reporting**: Pass/fail status and summaries
- **Error Handling**: Graceful failure handling
- **Performance Metrics**: Execution time reporting

### Test Types
1. **Kernel Tests**: Core kernel functionality
2. **User Tests**: Userspace applications and libraries
3. **Build Tests**: Compilation and linking validation
4. **Integration Tests**: Cross-component functionality

### Test Validation
Each test includes:
- **Positive Testing**: Expected functionality validation
- **Negative Testing**: Error condition handling
- **Boundary Testing**: Edge case validation
- **Performance Testing**: Timing and resource usage
- **Stress Testing**: Load and durability testing

## Shell Script Tests

### File System Testing
- **testisofs.sh**: ISO 9660 file system testing
- **testmfs.sh**: MINIX file system testing
- **testvnd.sh**: Virtual node device testing

### System Integration
- **testinterp.sh**: Interpreter testing
- **testkyua.sh**: Kyua test framework integration
- **testrelpol.sh**: Relative policy testing
- **testrmib.sh**: Remote MIB testing
- **testsh1.sh/testsh2.sh**: Shell functionality testing

## Test Development Guidelines

### Test Structure
- **Initialization**: Test environment setup
- **Execution**: Core test functionality
- **Validation**: Result verification
- **Cleanup**: Resource cleanup
- **Reporting**: Status and output

### Error Handling
- **Graceful Degradation**: Handle missing features
- **Meaningful Errors**: Clear failure messages
- **Recovery**: Attempt recovery when possible
- **Logging**: Detailed test execution logs

### Performance Considerations
- **Efficiency**: Minimize test execution time
- **Resource Usage**: Monitor memory and CPU
- **Scalability**: Handle varying system sizes
- **Timing**: Account for system load variations

## Architecture-Specific Testing

### RISC-V 64-bit Focus Areas
1. **Privilege Levels**: M-mode, S-mode, U-mode transitions
2. **Memory Management**: Sv39 virtual memory implementation
3. **Interrupt Handling**: PLIC and CLINT integration
4. **SBI Interface**: Firmware interaction validation
5. **Atomic Operations**: Multi-core synchronization
6. **Timer Support**: Platform timer accuracy

### Platform Integration
- **QEMU virt Platform**: Virtual platform testing
- **Hardware Platforms**: Real hardware validation
- **Boot Process**: OpenSBI to kernel transition
- **Device Tree**: Platform description parsing

## Continuous Integration

### Automated Testing
- **Build Integration**: Automated test builds
- **Regression Testing**: Prevent functionality loss
- **Performance Monitoring**: Track performance changes
- **Coverage Analysis**: Ensure comprehensive testing

### Test Reporting
- **Pass/Fail Status**: Clear test results
- **Performance Metrics**: Execution statistics
- **Error Analysis**: Failure investigation
- **Trend Analysis**: Historical performance tracking

## Development Support

### Test Creation
- **Template Tests**: Starting points for new tests
- **Framework Libraries**: Common test functions
- **Documentation**: Test development guides
- **Debugging Support**: Test failure investigation

### Test Maintenance
- **Regular Updates**: Keep tests current
- **Platform Updates**: Support new architectures
- **Feature Addition**: Test new functionality
- **Bug Fix Validation**: Verify corrections

This comprehensive test suite ensures MINIX reliability across all supported architectures, with particular emphasis on validating the RISC-V 64-bit port implementation and maintaining system stability through thorough validation of all system components.