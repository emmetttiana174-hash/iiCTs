# Home Module

## 1. Overview
The `Home` module serves as the primary configuration and management interface for the CTISS simulation software. It provides a centralized workspace for users to create, open, and configure simulation projects, set critical parameters, and initiate simulation processes. This module acts as the entry point for project management, enabling seamless parameter configuration and project workflow control.


## 2. Module Content
The `Home` module consists of the following key components, organized in a user-friendly layout:

### 2.1 View Toggle Buttons
- **UI Button**: Switches to the parameter configuration interface (default view).
- **AI Button**: Switches to the `ChatPanel` for AI-assisted operations (if available).
  *Location: Top-right corner of the module.*


### 2.2 Main Operation Buttons
Three prominent buttons for core project management, located in the upper-middle section:
- **New**: Creates a new simulation project with current parameters.
- **Open**: Loads an existing project from a `.jlab` configuration file.
- **START**: Validates parameters, updates project settings, and initiates the simulation workflow.


### 2.3 Parameter Configuration Panel
A structured panel (visible in "UI" view) for configuring project parameters, divided into two sections:

#### 2.3.1 Parameter Settings
Configurable parameters with input fields or dropdown menus:
- `ProjectName`: Name of the simulation project (text input, default: `sim_v1`).
- `TubeType`: Type of tube used in the simulation (dropdown: `hot` or `cold`, default: `hot`).
- `CTType`: Type of CT scanning mode (dropdown: `helical`, `CBCT`, `linear_static`, `polygon_interleaved`, `polygon_side`; default: `helical`).
- `TubeFolder`: Name of the subfolder for tube-related data (text input, default: `tube`).
- `SpectrumFolder`: Subfolder for X-ray spectrum data (text input, default: `spectrum`).
- `DetectorFolder`: Subfolder for detector data (text input, default: `detector`).
- `CTFolder`: Subfolder for CT scanning data (text input, default: `ct`).
- `ReconFolder`: Subfolder for reconstruction results (text input, default: `recon`).
- `MACFolder`: Subfolder for mac files (text input, default: `mac`).


#### 2.3.2 Path Settings
File paths for project resources and system directories, each with a "Browse" button (denoted by `...`) to select directories via a file dialog:
- `ProjectPath`: Root directory for storing the project (default: `/home/vgate/software_gy/share/CTISS/user`).
- `SystemScriptPath`: Path to system scripts (default: `/home/vgate/software_gy/share/CTISS/CTISS/scripts`).
- `SystemMaterialPath`: Path to system material libraries (default: `/home/vgate/software_gy/share/CTISS/CTISS/materials`).
- `TubeDatabasePath`: Path to the tube database (default: `/home/vgate/software_gy/share/CTISS/database/tube`).
- `SpectrumDatabasePath`: Path to the X-ray spectrum database (default: `/home/vgate/software_gy/share/CTISS/database/XraySpectrum`).


## 3. Purpose
The `Home` module is designed to:
- Centralize project configuration, allowing users to define all critical simulation parameters in one interface.
- Facilitate project creation and management (new projects, loading existing projects).
- Validate parameter consistency and automatically create required project directories.
- Serve as a control hub to initiate simulations and propagate project settings to other modules.


## 4. Operation Steps

### 4.1 Switching Views
- **To access Parameter Configuration (UI View)**: Click the `UI` button (top-right). This displays the parameter input fields and path settings.
- **To access AI Assistant (AI View)**: Click the `AI` button (top-right). This switches to the `ChatPanel` for AI-supported operations (if enabled).


### 4.2 Creating a New Project
1. In the **UI View**, configure the parameters in the "Parameter Settings" and "Path Settings" sections:
   - Ensure `ProjectName` is unique to avoid overwriting existing projects.
   - Select appropriate `TubeType` and `CTType` from the dropdown menus.
   - Adjust folder names or paths as needed (use the `...` button to browse for paths).

2. Click the **New** button.
   - The system will:
     - Create a root project directory at `ProjectPath/ProjectName`.
     - Generate subfolders (e.g., `tube`, `spectrum`) as specified in "Parameter Settings".
     - Save a `.jlab` configuration file (named `ProjectName.jlab`) in the project root, storing all parameters.
   - A success message will confirm project creation. If the project already exists, a warning will appear—modify `ProjectName` to proceed.


### 4.3 Opening an Existing Project
1. Click the **Open** button. A file dialog will appear.
2. Navigate to the location of the existing `.jlab` project file (typically in the project root directory).
3. Select the `.jlab` file and click "Open".
   - The system will load all parameters from the file, populating the input fields and dropdowns with the saved values.
   - A success message will confirm the project is loaded.


### 4.4 Modifying Parameters
- After creating or opening a project, update parameters (e.g., `ProjectName`, paths) directly in the input fields or dropdowns.
- Changes are not saved automatically—use the **START** button to apply and save modifications (see Section 4.5).


### 4.5 Starting the Simulation
1. After configuring or modifying parameters, click the **START** button.
   - The system will:
     - Check for parameter changes (compared to the last saved state).
     - Update the `.jlab` configuration file with new parameters if changes are detected.
     - Create any missing subfolders specified in the parameters.
     - Propagate the updated project information to other modules (e.g., simulation, reconstruction modules).
   - A confirmation message will indicate that parameters are updated and the simulation workflow can proceed. Navigate to other modules via the left-side menu to continue.


## 5. Notes
- **Folder Creation**: The module automatically creates project directories and subfolders. If creation fails (e.g., due to permissions), an error message will specify the problematic path.
- **Configuration Files**: Project settings are stored in `.jlab` files, which are plain text and can be manually edited (advanced users only).
- **Parameter Validation**: Ensure all paths are valid and accessible (use the `...` button to verify paths). Invalid paths will cause errors during project creation or simulation startup.
- **Warning Messages**: Pay attention to warnings (e.g., duplicate project names) to avoid data loss or overwrites.


## 6. Troubleshooting
- **"Failed to create directory" error**: Check file permissions for the `ProjectPath` directory. Ensure the user has write access.
- **Parameters not loading**: Verify the `.jlab` file is not corrupted. Recreate the project if necessary.
- **START button button not responding**: Ensure all required fields (e.g., `ProjectName`, `ProjectPath`) are filled. Missing values may block simulation initiation.
