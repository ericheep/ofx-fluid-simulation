# ofx-fluid-simulation
Fluid Simulation in openFrameworks. 

A port of [Sebastian Lague's fluid simulation](https://github.com/SebLague/Fluid-Sim/) in Unity. This work was done on Silicon Mac.

# OpenAPI TBB Install

This project requires openAPI TBB to be installed and added to this XCode project. 

    brew install tbb

The run the following to find where the package installed and the paths you'll need to add to your XCode project.

    brew info tbb

For example on Silicon Mac, tbb installed to the following directory `/opt/homebrew/Cellar/tbb/2021.11.0`

In Xcode you would then set:

`Header Search Paths` to  `/opt/homebrew/Cellar/tbb/2021.11.0/include`

`Library Search Paths` to `/opt/homebrew/Cellar/tbb/2021.11.0/lib`

`Other Linker Flags` to `-ltbb`
