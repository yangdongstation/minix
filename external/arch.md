# MINIX 3 External Software (/external)

## Overview

The `/external` directory contains third-party software organized by license type. This directory serves as a centralized location for external software components that are integrated into MINIX but maintain their original licensing terms. The organization by license ensures compliance with various open-source license requirements and provides clear attribution and license information.

## 本仓库实际包含/可选或上游组件

本仓库实际包含：以下为顶层条目示例（完整清单以目录实际内容为准）：`bsd/`, `gpl2/`, `gpl3/`, `mit/`, `public-domain/`, `Makefile`, `README`.
可选或上游组件：上文条目可能包含上游/可选组件示例，当前仓库未必包含全部。

## Directory Structure

The external software is organized into license-based subdirectories:

### License Categories

#### **`bsd/`** - BSD-Licensed Software
- Software with BSD-style licenses
- Permissive licensing allowing commercial use
- Minimal license obligations
- Examples: Various BSD utilities and libraries

#### **`gpl2/`** - GNU General Public License v2
- Software licensed under GPL version 2
- Copyleft licensing requiring source distribution
- Compatible with MINIX distribution
- Examples: GNU utilities, libraries

#### **`gpl3/`** - GNU General Public License v3
- Software licensed under GPL version 3
- Updated copyleft terms
- Patent protection clauses
- Examples: Modern GNU software

#### **`lgpl3/`** - GNU Lesser General Public License v3
- Library-specific GPL variant
- More permissive for library linking
- Allows proprietary software to link
- Examples: System libraries

#### **`mit/`** - MIT License
- Very permissive license
- Minimal restrictions
- Commercial-friendly
- Examples: Modern open-source tools

#### **`public-domain/`** - Public Domain Software
- No copyright restrictions
- Completely free to use
- No license obligations
- Examples: Historical software, algorithms

#### **`historical/`** - Historical/Traditional Licenses
- Software with historical licenses
- Custom license terms
- Special attribution requirements
- Examples: Classic UNIX utilities

## Key Files

### Documentation
- **`README`** - Overview of external software organization
- **`Makefile`** - Build configuration for external components
- License files in each subdirectory
- Attribution and copyright notices

### Build Integration
- **`Makefile`** coordinates building of external software
- Integration with MINIX build system
- Cross-compilation support
- Dependency management

## Integration with MINIX System

### Build Process Integration
External software is integrated into MINIX build system:
- Cross-compilation for target architectures
- License compliance verification
- Proper attribution and documentation
- Quality assurance and testing

### System Integration
External components integrate with MINIX:
- Message-based system calls
- Service-oriented architecture
- MINIX-specific adaptations
- Error handling conventions

### License Compliance
Strict license compliance management:
- License compatibility verification
- Proper attribution maintenance
- Source code availability
- License documentation

## RISC-V 64-bit Considerations

### Cross-Platform Compatibility
External software must support RISC-V:
- Architecture-specific optimizations
- RISC-V assembly code (where applicable)
- Memory model compatibility
- Instruction set support (RV64GC)

### Build Configuration
RISC-V specific build settings:
- Cross-compilation toolchain
- Architecture-specific flags
- Optimization settings
- Platform-specific features

### Testing and Validation
Comprehensive testing on RISC-V:
- Functionality verification
- Performance testing
- Compatibility validation
- Integration testing

## Common External Software Components

### Development Tools
- **Compilers and interpreters**
- **Debuggers and profilers**
- **Build utilities**
- **Text processing tools**

### System Libraries
- **Mathematical libraries**
- **Compression libraries**
- **Cryptographic libraries**
- **Networking libraries**

### Utilities and Applications
- **Text editors**
- **File compression tools**
- **Network utilities**
- **System monitoring tools**

## Development Guidelines

### License Management
- Verify license compatibility
- Maintain proper attribution
- Document license terms
- Ensure source availability

### Integration Standards
- Follow MINIX coding conventions
- Integrate with message-passing architecture
- Implement proper error handling
- Maintain compatibility with MINIX services

### Quality Assurance
- Comprehensive testing
- Code quality verification
- Security review
- Performance evaluation

## Maintenance and Updates

### Version Management
- Track upstream versions
- Security update monitoring
- Feature enhancement evaluation
- Compatibility maintenance

### License Compliance
- Regular license audit
- Attribution verification
- Source code maintenance
- Documentation updates

### Integration Updates
- Build system compatibility
- API changes adaptation
- Service integration updates
- Testing procedure updates

## Directory Organization Examples

### Typical Structure
```
external/
├── bsd/
│   ├── utility1/
│   ├── library1/
│   └── tool1/
├── gpl2/
│   ├── gnu-tool1/
│   ├── application1/
│   └── library2/
├── mit/
│   ├── modern-tool1/
│   └── library3/
└── public-domain/
    ├── classic-tool1/
    └── algorithm1/
```

### Build Integration
Each component typically includes:
- Original source code
- MINIX-specific patches
- Build configuration
- Documentation
- License files

## Quality Control

### Code Review
- Security review process
- Code quality standards
- MINIX integration review
- License compliance verification

### Testing Requirements
- Functional testing
- Integration testing
- Performance testing
- Regression testing

### Documentation Standards
- License documentation
- Integration guides
- Usage documentation
- Attribution requirements

The `/external` directory represents MINIX's commitment to leveraging high-quality third-party software while maintaining strict license compliance and system integration standards. This approach allows MINIX to benefit from the broader open-source ecosystem while preserving its architectural integrity and legal compliance.
