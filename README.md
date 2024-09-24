# smarty_utils

This package contains useful tools and scripts for the Smarty Software Stack.

## Intended Usage

Please add here custom messages, tools, and scripts that are useful for all packages in the Smarty Software Stack. What should **not** be included:

- Package-specific tools and scripts
- Package-specific messages (which are only used in one other package)
- Package-specific configurations
- Package-specific launch files
- Package-specific launch configurations
- Package-specific launch scripts
- Package-specific tests
- Garbage
- ...

## Subpackages

- `lane_msgs`: Contains the custom message definitions for the lane detection package.
- `timing`: Contains the timing analysis scripts for the Smarty Software Stack.

## Installation

To install the package, clone the repository into your workspace and build it with colcon:

```bash
cd <your_workspace>/src
git clone https://github.com/DHBW-Smart-Rollerz/smarty_utils.git
git checkout jazzy
cd ..
make build PACKAGE="lane_msgs timing"
source install/setup.bash
```

## Usage

For the usage of the subpackages, please refer to their respective READMEs.
