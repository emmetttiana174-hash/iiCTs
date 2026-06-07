# Tube Simulation Module

## 1. Overview
The `Tube Simulation` module is a specialized component within the CTISS software suite, dedicated to configuring and managing tube simulation parameters. It enables users to define tube-specific simulation settings (e.g., anode/cathode properties, voltages, material specifications), select template files, edit parameters in both graphical and text modes, and initiate tube simulation processes. This module integrates with project information to ensure consistent parameter management and seamless workflow execution.


## 2. Module Content
The `Tube Simulation` module comprises several key components organized to support comprehensive tube simulation configuration:

### 2.1 View Toggle Buttons
Located in the top-right corner, these buttons switch between different operational views:
- **UI**: Graphical interface for parameter editing (default view).
- **Det (Detailed)**: Text-based editor for direct modification of template files.
- **AI**: AI-assisted panel (`ChatPanel`) for guidance and automation (if available).


### 2.2 Main Operation Controls
- **Simulation Button**: A prominent button (middle-upper section) to initiate the tube simulation using current parameters and selected target files.
- **Target Path Selector**: A dropdown menu (`TargetCombo`) to choose the script file (.gwl) that defines the simulation workflow. Displays user-created and system-provided scripts with labels (`(user)` or `(system)`).


### 2.3 Parameter Configuration Panels
#### 2.3.1 UI View Panel
A structured interface for editing tube parameters, divided into:
- **Parameter Settings**: Input fields for numerical parameters with units:
  - Anode properties: `AnodeTiltAngle` (°), `AnodeRadius` (mm), `AnodeThickness` (mm), `AnodePotential` (KeV).
  - Cathode properties: `CathodeRadius` (mm), `CathodeLength` (mm), `CathodePotential` (KeV).
  - Focusing components: `FocusRadius` (mm), `FocusThickness` (mm), `FocusPotential` (KeV).
  - Other parameters: `CathodeToAnode` (mm), `FocusHoleRadius` (mm).

- **Path Settings**: Fields for file/directory paths:
  - `OutputFilename`: Name of the simulation output file.
  - `AnodeMaterial`, `CathodeMaterial`, `FocusMaterial`: Materials used in the tube (text input).
  - `OutputPath`: Directory for simulation results (with a browse button `...` to select folders).

- **Template File Controls**: A dropdown (`fileUICombo`) to load system/user templates, plus `Save` and `Save As` buttons to store modified parameters.


#### 2.3.2 Detailed View Panel
A text editor for direct modification of template files:
- **Template File Selector**: A dropdown (`fileCombo`) to load templates (same as `fileUICombo` in UI view).
- **Text Editor**: A large input area to edit the raw content of template files (supports syntax for .gwl scripts).
- **Save Controls**: `Save` and `Save As` buttons to update or create new template files.


## 3. Purpose
The `Tube Simulation` module is designed to:
- Centralize configuration of tube-specific simulation parameters (anode, cathode, focusing components).
- Support both graphical (UI) and text-based (Detailed) editing for flexibility.
- Manage template files (system-provided and user-created) to standardize simulation workflows.
- Enable initiation of tube simulations and integration with the terminal for progress monitoring.


## 4. Operation Steps

### 4.1 Switching Views
- **UI View**: Click `UI` to access graphical parameter inputs. Best for modifying individual parameters with unit guidance.
- **Detailed View**: Click `Det` to edit raw template text. Suitable for advanced users adjusting script logic.
- **AI View**: Click `AI` to open the `ChatPanel` for AI-assisted configuration.


### 4.2 Configuring Parameters in UI View
1. **Load a Template**: Use `fileUICombo` to select a base template (e.g., `hot_tube.gwl` for hot cathodes or `cold_tube.gwl` for cold cathodes). Parameters will auto-populate with template values.
2. **Edit Parameters**: Modify values in the input fields. Units are pre-defined (e.g., `AnodeTiltAngle` uses degrees).
3. **Update Paths**:
   - Enter `OutputFilename` (e.g., `my_tube_simulation`).
   - Specify `OutputPath` using the browse button `...` to select a directory.
   - Define materials (e.g., `Tungsten` for `AnodeMaterial`).
4. **Save Changes**:
   - Click `Save` to update the current template (overwrites if it’s a user file; prompts to create a user copy for system files).
   - Click `Save As` to create a new template. Enter a name in the dialog, and the file will be saved to the project’s `tube` folder.


### 4.3 Editing Templates in Detailed View
1. **Load a Template**: Use `fileCombo` to select a template. The text editor will display its content.
2. **Modify Content**: Edit the text directly (e.g., adjust material names or add custom script logic).
3. **Save Changes**: Use `Save` (update existing) or `Save As` (new file) as in UI view.


### 4.4 Running the Simulation
1. **Select Target Script**: Use `TargetCombo` to choose the .gwl file that executes the simulation (matches the template used for parameter configuration).
2. **Initiate Simulation**: Click the `Simulation` button. The terminal will display progress updates (e.g., `CTISS:1`, `CTISS:2`...).


## 5. Notes
- **Template Sources**: System templates are pre-provided (read-only) and stored in `SystemScriptPath/tube/[TubeType]`. User templates are saved in `ProjectPath/[ProjectName]/tube/`.
- **Parameter Consistency**: Ensure values in UI view match the corresponding template in Detailed view. Mismatches may cause simulation errors.
- **File Extensions**: Templates are saved as `.gwl` files. The module auto-appends this extension if omitted in `Save As`.
- **Material Compatibility**: Use materials available in the system’s material library (defined in `SystemMaterialPath` from the ParaHome module).


## 6. Troubleshooting
- **Simulation Fails to Start**: Ensure a `Target Path` is selected in `TargetCombo`. If empty, populate templates by saving a file in UI/Detailed view.
- **Cannot Save Files**: Check write permissions for the project’s `tube` folder (`ProjectPath/[ProjectName]/tube/`).
- **Parameters Not Updating**: Verify changes are saved before running the simulation. Use `Save` after editing.
- **Template Not Loading**: If a template is missing from the dropdown, refresh by saving any file (triggers re-scan of user/system directories).
