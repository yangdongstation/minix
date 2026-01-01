# MINIX Testing Infrastructure Architecture

This directory contains the comprehensive testing infrastructure for MINIX, covering kernel functionality, system services, device drivers, and architecture-specific features. The testing framework validates system correctness, performance, and reliability across all MINIX components.

## Test Organization

### Architecture-Specific Tests (riscv64/)
RISC-V 64-bit specific testing suite focused on hardware features and architecture compliance:

**Core Architecture Tests:**
- **test_csr.c**: Control and Status Register testing
  - CSR read/write operations validation
  - Privilege level transitions
  - Architecture-specific register access
  - CSR instruction encoding verification

- **test_atomic.c**: Atomic operations testing
  - Load-linked/store-conditional operations
  - Atomic memory operations (AMO)
  - Memory ordering and synchronization
  - Multi-processor atomic behavior

- **test_sbi.c**: Supervisor Binary Interface testing
  - SBI call validation and error handling
  - Firmware interface compliance
  - Timer and interrupt SBI functions
  - Console and shutdown SBI operations

- **test_memory.c**: Memory management testing
  - Virtual memory operations
  - Page table manipulation
  - Memory protection validation
  - Cache and TLB behavior

- **test_trap.c**: Exception and interrupt testing
  - Trap handling mechanisms
  - Exception vector processing
  - Interrupt delivery and handling
  - Trap frame management

- **test_timer.c**: Timer functionality testing
  - Timer interrupt generation
  - Timer accuracy and drift
  - Multiple timer support
  - Timer cascade operations

- **test_ipc.c**: Inter-process communication testing
  - Message passing mechanisms
  - IPC performance validation
  - Cross-process communication
  - Message queue operations

- **test_vm.c**: Virtual memory subsystem testing
  - Address space management
  - Memory mapping operations
  - Page fault handling
  - Memory protection mechanisms

### Remote MIB Testing (rmibtest/)
Testing infrastructure for the Management Information Base remote subtree functionality:

**Test Service Implementation:**
- **rmibtest.c**: Remote MIB test service
  - Test subtree definition and registration
  - Node type validation (int, bool, quad, string, struct)
  - Permission and access control testing
  - Dynamic node creation and destruction
  - Function-based node handlers
  - Remote subtree mounting and unmounting

**Test Configuration:**
- **rmibtest.conf**: Service configuration
  - Service privilege settings
  - Resource allocation parameters
  - System integration requirements

### System Integration Tests
Comprehensive tests for system-level functionality:

**Process and Memory Tests:**
- **testvm.c**: Virtual memory system testing
  - Memory allocation and deallocation
  - Process memory space management
  - Memory protection validation
  - Shared memory operations

- **testvm.conf**: VM test configuration
  - Test environment setup
  - Resource limits and parameters
  - System service dependencies

**File System Tests:**
- **testmfs.sh**: MINIX File System testing
  - File system operations validation
  - Directory management testing
  - File permission and security testing
  - File system recovery and repair

- **testisofs.sh**: ISO file system testing
  - ISO 9660 compliance validation
  - Read-only file system operations
  - Volume descriptor processing
  - Path table navigation

**Device and Driver Tests:**
- **testvnd.sh**: Virtual node device testing
  - Block device emulation
  - Device layering functionality
  - I/O operation validation
  - Error handling verification

- **blocktest/**: Block device testing framework
  - Raw block device operations
  - I/O performance measurement
  - Device error simulation
  - Multi-device coordination

## Test Execution Framework

### Test Runner Infrastructure
**Automated Test Execution:**
- **run_tests.sh**: Main RISC-V test runner
  - Architecture-specific test coordination
  - Test result aggregation
  - Performance measurement
  - Error reporting and logging

**Test Categories:**
- **kernel**: Core kernel functionality tests
- **user**: User space and system call tests
- **build**: Compilation and linking tests
- **all**: Complete test suite execution

### Test Result Validation
**Success Criteria:**
- Test completion without errors
- Expected result validation
- Performance threshold compliance
- Resource usage verification

**Error Handling:**
- Test failure detection and reporting
- Error categorization and analysis
- Recovery procedure validation
- System state verification

## Test Development Guidelines

### Test Structure and Organization
**File Naming Conventions:**
- **test*.c**: Individual test implementations
- **test*.sh**: Shell script test drivers
- **test*.conf**: Test configuration files
- **test*.h**: Test header and utility files

**Test Function Prototypes:**
```c
int test_function(void);
int test_function_with_params(int param1, char *param2);
void test_cleanup(void);
```

### Test Implementation Patterns
**Standard Test Structure:**
```c
int test_example(void)
{
    int result = OK;
    
    /* Test initialization */
    if (init_test_environment() != OK) {
        return EINVAL;
    }
    
    /* Test execution */
    if (perform_test_operation() != EXPECTED_RESULT) {
        result = EIO;
        goto cleanup;
    }
    
    /* Result validation */
    if (validate_test_results() != TRUE) {
        result = EFAULT;
    }

cleanup:
    /* Cleanup and return */
    cleanup_test_environment();
    return result;
}
```

### Error Reporting and Logging
**Debug Output:**
- Use printf for basic test output
- Include test name and operation description
- Report both success and failure conditions
- Provide meaningful error messages

**Result Reporting:**
- Return standard error codes (OK, EINVAL, EIO, etc.)
- Document expected vs. actual results
- Include performance metrics where applicable
- Support verbose and quiet output modes

## Architecture-Specific Testing

### RISC-V 64-bit Testing Focus
**Hardware Feature Validation:**
- RV64GC instruction set compliance
- Privilege mode transitions (M/S/U modes)
- Control and Status Register functionality
- Memory management unit operations
- Interrupt and exception handling

**Platform Integration:**
- QEMU virt machine compatibility
- OpenSBI firmware interface validation
- Platform-level interrupt controller (PLIC) testing
- Core Local Interruptor (CLINT) functionality
- UART and serial console operations

### Multi-Architecture Support
**Portable Test Design:**
- Conditional compilation for architecture differences
- Abstracted hardware interfaces
- Platform-specific optimization validation
- Cross-architecture compatibility verification

## Performance and Stress Testing

### Load Generation
**Stress Test Categories:**
- Memory allocation stress tests
- Process creation and destruction loops
- File system operation stress testing
- Network and IPC load generation
- Device driver stress validation

**Performance Measurement:**
- Operation timing and latency measurement
- Throughput validation under load
- Resource utilization monitoring
- Scalability testing with increasing load

### Resource Monitoring
**System Resource Tracking:**
- Memory usage validation
- CPU utilization measurement
- I/O operation counting
- Interrupt frequency monitoring
- Error rate tracking

## Integration with Development Workflow

### Continuous Integration
**Automated Testing:**
- Build-time test execution
- Regression test validation
- Performance benchmark comparison
- Multi-configuration testing

### Development Testing
**Developer-Focused Features:**
- Individual test execution
- Debug mode support
- Verbose output options
- Targeted test selection

### Release Validation
**Pre-release Testing:**
- Complete test suite execution
- Performance regression validation
- Stress test completion
- Multi-platform validation

## Best Practices and Guidelines

### Test Design Principles
1. **Isolation**: Tests should be independent and isolated
2. **Repeatability**: Tests should produce consistent results
3. **Comprehensiveness**: Cover both success and failure cases
4. **Performance**: Include timing and resource usage validation
5. **Maintainability**: Write clear, documented test code

### Test Implementation Standards
**Code Quality:**
- Follow MINIX coding conventions
- Include comprehensive comments
- Implement proper error handling
- Use consistent naming patterns

**Test Coverage:**
- Exercise all code paths where possible
- Test boundary conditions
- Validate error handling
- Verify resource cleanup

### Debugging and Troubleshooting
**Test Failure Analysis:**
- Detailed failure reporting
- System state capture on failure
- Debug output and logging
- Reproduction scenario documentation

This comprehensive testing infrastructure ensures MINIX system reliability, performance, and correctness across all supported architectures and configurations while providing developers with robust validation tools for system development and maintenance.