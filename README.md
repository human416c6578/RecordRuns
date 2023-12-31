# Record Runs Plugin

## Description
The **Record Runs** plugin for AMX Mod X is designed to record and playback player movements in Counter-Strike 1.6. It allows server administrators to capture and review player runs, which can be useful for analyzing gameplay, creating tutorials, or showcasing exceptional performances.

**Note:** This plugin is not intended for standalone use. It requires integration with other plugins and can be controlled using native functions.

## Features
- Record and playback player movements.
- Adjustable bot play speed using the `/bot` command.
- Ability to choose a replay file, pause, and stop the replay.


## Configuration
- Use native functions to save, load, reset, and manipulate replay files.

## Natives

1. `open_bot_menu(id)`
   - Opens the bot control menu for the specified player.

2. `reset_record(id)`
   - Resets the recorded frames for the specified player, keeping the last 20 frames.

3. `save_record(id, demo_time, demo_name, demo_path)`
   - Saves the recorded frames for the specified player to a replay file.

4. `load_record(path)`
   - Loads a replay file from the specified path.

## Usage
- Players can use the `/bot` command to adjust the bot play speed, choose a replay file, pause the replay, or stop it.

## Record Runs Plugin Integration

To seamlessly integrate the **Record Runs** plugin with your existing server setup, an example plugin has been provided. This integration example is encapsulated in the file named `timer_records.sma`.

### Overview

1. **Configuration Setup:**
   - The plugin defines essential configuration parameters, such as storage type and URL, using registered cvars (`g_cStorageType` and `g_cStorageUrl`).

2. **Initialization:**
   - The `plugin_init` function initializes the plugin by registering necessary cvars for storage configuration.

3. **Recording Setup:**
   - The plugin includes timer functions (`timer_player_record` and `timer_player_started`) that handle the recording of player runs based on specific events.

4. **Directory Creation:**
   - The `plugin_cfg` function creates the required directory structure for storing recordings. It ensures the existence of folders based on the map name and recording categories.

5. **Record Loading:**
   - The `timer_db_loaded` function initiates the loading of recorded runs once the database is loaded. It determines whether to load records from a local directory or a web server based on the configured storage type.

6. **Web Server Interaction:**
   - Functions like `load_records_webserver`, `get_file`, and `http_get_file_complete` manage interactions with a web server to fetch recorded run files.

7. **File Upload System:**
   - **Note:** The example assumes the existence of a separate system for uploading files to the web server. In the `utils` folder, you'll find a Python script (`upload_files.py`) designed to run periodically. This script handles the task of uploading recorded run files to the configured web server.

### Example Plugin File

- The integration example is contained in the file named `timer_records.sma`.

Follow this example to seamlessly incorporate the **Record Runs** functionality into your existing server environment. Ensure to deploy the Python script in the `utils` folder to handle the periodic uploading of recorded run files to the web server. Adjust the configuration parameters and event triggers according to your specific server requirements.