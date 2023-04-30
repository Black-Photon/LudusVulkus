# LudusVulkus
An experimental Vulkan game engine developed to give me more experience in Vulkan, and to experiment with graphical techniques.

## Current Progress
The Triangle Engine is up and running! Build it yourself now to get the true triangular experience! Sample screenshot of the experience shown below:
<p align="center">
  <img src="https://user-images.githubusercontent.com/28807595/222603046-7208e43e-ecec-441a-902e-9a0b63590aa5.png" />
</p>

## Building
Right now the recommended way of building is through Visual Studio. You'll need to install and add these to your Additional Library Directories:
 - VulkanSDK (tested with 1.3.243.0)
 - GLFW (tested with 3.3.8)
 - GLM (Tested with 0.9.9.8)
 
You'll also need these as Additional Library Dependancies:
 - `vulkan-1.lib`
 - `glfw3.lib`

And you'll need to add the include directories for each of the above libraries.
You'll also need Python to run the shader compilation script, and to update the script with your Vulkan SDK path
