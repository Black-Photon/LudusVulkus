# LudusVulkus
An experimental Vulkan game engine developed to give me more experience in Vulkan, and to experiment with graphical techniques.

## Current Progress
I've been following the tutorial [here](https://vulkan-tutorial.com) to build up the initial engine. There are two scenes so far:

The Vulkan3D scene is coming along well, now featuring support for image samplers! Sample shown below ([image from here](https://pixabay.com/en/statue-sculpture-fig-historically-1275469/)):
<p align="center">
  <img src="https://github.com/Black-Photon/LudusVulkus/assets/28807595/5747baa6-55b3-42f7-ac55-8ac9c6490117" />
</p>

A triangle engine is also included as a seperate scene - build it yourself now to get the true triangular experience! Sample screenshot of the experience shown below:
<p align="center">
  <img src="https://user-images.githubusercontent.com/28807595/222603046-7208e43e-ecec-441a-902e-9a0b63590aa5.png" />
</p>

## Building
Right now the recommended way of building is through Visual Studio. You'll need to install and add these to your Additional Include Directories:
 - VulkanSDK (tested with 1.3.243.0)
 - GLFW (tested with 3.3.8)
 - GLM (tested with 0.9.9.8)
 - STB (stb_image.h) (tested with 2.28)
 - Boost (tested with 1.82)
 
You'll also need these as Additional Dependancies:
 - `vulkan-1.lib`
 - `glfw3.lib`

And you'll need to add the include directories for each of the above libraries.
You'll also need Python to run the shader compilation script, and to update the script with your Vulkan SDK path
