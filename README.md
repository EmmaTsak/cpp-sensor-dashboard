# C++ Sensor Dashboard

A full-stack C++ sensor dashboard that simulates IoT-style sensor readings and exposes them through a lightweight HTTP API.
The project includes a simple web frontend that displays live sensor values, warning statuses, recent history, and basic statistics.

## Overview

This project was built as a beginner-friendly modern C++ learning project.
It combines embedded-style sensor simulation, backend API development, and a small frontend dashboard.

The C++ backend generates simulated readings for sensors such as temperature, humidity, and voltage. These readings are updated in the background using a separate thread. The frontend fetches the data from the backend and displays it in a simple dashboard.

## Features

* Simulated IoT sensor readings
* C++ HTTP backend API
* Frontend dashboard using HTML, CSS, and JavaScript
* Live auto-refreshing sensor cards
* Sensor status detection:

  * `OK`
  * `LOW`
  * `HIGH`
* Recent reading history for each sensor
* Minimum, maximum, and average statistics
* Background sensor updates using multithreading
* Thread-safe access to shared sensor data
* JSON API responses using `nlohmann/json`
* Clean C++ project structure with CMake

## Technologies Used

* C++17
* CMake
* cpp-httplib
* nlohmann/json
* HTML
* CSS
* JavaScript

## C++ Concepts Practiced

This project focuses on several important modern C++ concepts:

* Classes and encapsulation
* Header/source file separation
* Constructors
* Smart pointers with `std::unique_ptr`
* RAII principles
* Move semantics with `std::move`
* STL containers:

  * `std::vector`
  * `std::deque`
* STL algorithms:

  * `std::min_element`
  * `std::max_element`
  * `std::accumulate`
* Multithreading with `std::thread`
* Thread synchronization with `std::mutex`
* Safe locking with `std::lock_guard`
* Atomic variables with `std::atomic`
* Time handling with `std::chrono`
* JSON serialization
* Basic HTTP API design

## Project Structure

```text
cpp-sensor-dashboard/
│
├── CMakeLists.txt
├── main.cpp
│
├── include/
│   ├── Sensor.hpp
│   ├── SensorManager.hpp
│   ├── httplib.h
│   └── json.hpp
│
├── src/
│   ├── Sensor.cpp
│   └── SensorManager.cpp
│
└── frontend/
    └── index.html
```

## How It Works

The application has two main parts:

```text
C++ Backend
↓
HTTP JSON API
↓
HTML/CSS/JavaScript Frontend
```

The backend creates simulated sensors and updates their readings every second in a background thread.

The frontend calls the backend API every few seconds and updates the dashboard in the browser.

## API Endpoints

### Health Check

```text
GET /health
```

Example response:

```json
{
  "status": "ok"
}
```

### Get All Sensors

```text
GET /sensors
```

Example response:

```json
{
  "sensors": [
    {
      "average": 25.67,
      "history": [24.12, 26.45, 27.30],
      "max": 27.30,
      "min": 24.12,
      "name": "Temperature",
      "status": "OK",
      "unit": "C",
      "value": 27.30
    }
  ]
}
```

## Sensor Status Logic

Each sensor has two ranges:

1. A generated value range
2. A safe operating range

Example:

```text
Temperature generated range: 10.0°C to 40.0°C
Temperature safe range:      20.0°C to 30.0°C
```

If the value is below the safe range, the status becomes:

```text
LOW
```

If the value is above the safe range, the status becomes:

```text
HIGH
```

Otherwise, the status is:

```text
OK
```

## How to Build

### Requirements

You need:

* A C++17-compatible compiler
* CMake
* Git
* A browser

On Windows, you can use:

* Visual Studio Community
* Visual Studio Build Tools
* VS Code with the CMake Tools extension

## Build Instructions

From the project root folder:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## How to Run

From the build folder, run the executable.

On Windows with Visual Studio generator:

```powershell
.\Debug\sensor_dashboard.exe
```

Or, depending on your build setup:

```powershell
.\sensor_dashboard.exe
```

When the backend starts, you should see:

```text
Sensor Dashboard backend running at http://localhost:8080
Open http://localhost:8080/health to test the server.
Open http://localhost:8080/sensors to see sensor data.
```

## Test the Backend

Open these URLs in your browser:

```text
http://localhost:8080/health
```

```text
http://localhost:8080/sensors
```

## Run the Frontend

Keep the C++ backend running.

Then open:

```text
frontend/index.html
```

in your browser.

The dashboard should display live sensor cards with:

* Latest value
* Unit
* Status
* Min value
* Max value
* Average value
* Recent readings

## Example Sensors

The current version includes:

* Temperature sensor
* Humidity sensor
* Voltage sensor

## Future Improvements

Possible future upgrades:

* Add a `/sensors/{name}` endpoint
* Add charts using Chart.js
* Add WebSocket live updates
* Save readings to SQLite
* Add configuration from a JSON file
* Add unit tests with GoogleTest
* Add Docker support
* Improve frontend design
* Add timestamps for each reading
* Add sensor enable/disable controls

## Learning Purpose

This project was created to practice modern C++ through a realistic small application.

It is intentionally small enough to complete as a beginner project, but it includes important real-world software concepts such as:

* Backend APIs
* Thread-safe shared data
* Sensor-style simulation
* JSON serialization
* Frontend-backend communication
* Clean project structure

## License

This project is open source and available under the MIT License.
