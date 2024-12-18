# ZenohPubSubForUnity

This repository contains a simple publisher and subscriber for Unity using Zenoh.

## Getting Started

### Prerequisites

- [zenoh-cpp](https://github.com/eclipse-zenoh/zenoh-cpp) (latest version)
- Visual Studio 2022 or newer

### Target platform

- Windows 11 (x64)
- Unity 6 or newer

### Installation from source (not mandatory)

1. Clone the repository:

    ```sh
    git clone https://github.com/comoc/zenoh-pub-sub-for-unity.git
    ```

2. Build and install zenoh-cpp by following its [README](https://github.com/eclipse-zenoh/zenoh-cpp/blob/main/README.md)

3. Build the Unity plugin DLL.

- Open `ZenohPubSubForUnity\ZenohPubSubForUnity.sln` with Visual Studio.
- Set the paths to the required include directories and libraries (= zenohc.dll.lib or zenohcd.dll.lib).
- Build the project.
- Copy the DLLs into `ZenohPubSubTest\Assets\Plugins\`.

### Usage

1. Open `ZenohPubSubTest` with Unity Editor.

- Load `SampleScene` into the Hierarchy.

2. Run the subscriber:

    ```sh
    z_sub.exe
    ```

3. Play the scene on the Unity Editor.

You can see the logs on the console and Unity.

### Contributing

Contributions are welcome! Please open an issue or submit a pull request.

### Acknowledgements

- [Zenoh](https://zenoh.io/) for the publish-subscribe framework.
