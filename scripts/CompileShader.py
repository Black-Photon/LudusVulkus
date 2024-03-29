import subprocess
from os import listdir
from os.path import isfile, join

shader_path = ".\\assets\\shaders\\"
glsl_path = shader_path + "glsl\\"
spirv_path = shader_path + "spir-v\\"

def is_glsl(file):
	extension = file.rsplit('.', 1)[-1]
	return isfile(join(glsl_path, file)) and extension in {"frag", "vert", "glsl"}

return_code = 0

for file in (f for f in listdir(glsl_path) if is_glsl(f)):
	extension = file.rsplit('.', 1)[-1]
	file_name = file.rsplit('.', 1)[0]
	new_name = file_name + "_" + extension + ".spv"
	command = ["C:\\Libraries\\VulkanSDK\\1.3.243.0\\Bin\\glslc.exe", glsl_path + file, "-o", spirv_path + new_name]

	print("Running '" + ' '.join(command) + "'...")
	if subprocess.call(command):
		return_code = 1

exit(return_code)