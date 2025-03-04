import os
import platform
import shutil
import re
import time

itch_project_path = "cydeamon/babylon-embers"

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


# Publish to itch.io
def publish(platform, version):
    platformFolder = ""

    if platform == "windows":
        platformFolder = "Windows"
    elif platform == "linux":
        platformFolder = "Linux"
    elif platform == "html5":
        platformFolder = "Web"

    if version == "":
        print("Can't find any versions for " + platformFolder + " platform")
    else:
        itch_project = itch_project_path + ":" + platform
        command = "butler push --userversion " + version + " ./Deploy/" + platformFolder + "/" + version + " " + itch_project

        print("Pushing " + platformFolder + " Build " + version + " to " + itch_project)
        print("Command: " + command)
        exit_code = 1
        while exit_code != 0:
            exit_code = os.system(command)
            
            if exit_code == 0:
                print(bcolors.OKGREEN + " Deploy successful" + bcolors.ENDC)
            else:
                print(bcolors.FAIL + " Deploy failed" + bcolors.ENDC)
                time.sleep(1)        


# Find project version in CMakeLists.txt
def find_project_version():
    with open('CMakeLists.txt', 'r') as file:
        content = file.read()
        version_match = re.search(r'project\(.*VERSION\s+(\d+\.\d+\.\d+)\)', content)
        if version_match:
            return version_match.group(1)
        else:
            return None


project_version = find_project_version()


# Find project name frmo CMakeLists.txt
def find_project_name():
    with open('CMakeLists.txt', 'r') as file:
        content = file.read()
        name_match = re.search(r'project\((\w+)', content)
        if name_match:
            return name_match.group(1)
        else:
            return None


project_name = find_project_name()


# Ignore shader source files
def ignore_shader_sources(directory, filenames):
    return [name for name in filenames if name.endswith(".vert") or name.endswith(".frag") or name.endswith(".glsl")]


# Find project executables possible names (PROJECT_NAME + "Something" + "_BUILDTYPE")
def find_executables(project_name):
    search_path = os.getcwd() + "/Bin"
    executablesTypes = [""]
    executables = []
    regex = r'(?:' + project_name + r')(.*)_(?:Release|Debug|MinSizeRel)(.exe)?'

    for file in os.listdir(search_path):
        matches = re.findall(regex, file)

        if len(matches) > 0 and matches[0] not in executablesTypes:
            executablesTypes.append(matches[0][0])

    for type in executablesTypes:
        executable_path = ""

        for file in os.listdir(search_path):
            if file.endswith(project_name + type + "_Release.exe") or file.endswith(project_name + type + "_Release"):
                executable_path = os.path.join(search_path, file)
                break
            elif file.endswith(project_name + type + "_MinSizeRel.exe") or file.endswith(
                    project_name + type + "_MinSizeRel"):
                executable_path = os.path.join(search_path, file)
                break
            elif ((file.endswith(project_name + type + "_Debug.exe") or file.endswith(project_name + type + "_Debug"))
                  and not (executable_path.endswith(project_name + type + "_Release.exe") or executable_path.endswith(project_name + type + "_Release"))
                  and not (executable_path.endswith(project_name + type + "_MinSizeRel.exe") or executable_path.endswith(project_name + type + "_MinSizeRel"))):
                executable_path = os.path.join(search_path, file)

        if len(executable_path) > 0:
            print(bcolors.WARNING + "Executable found: " + executable_path + bcolors.ENDC)
            executables.append(executable_path)

    return executables


##################################################################################
##################################################################################
###################################  MAIN CODE  ##################################


# Find project executable in current directory (Prefer file with "Release" postfix, otherwise "Debug")
executables = find_executables(find_project_name())

if len(executables) == 0:
    print(bcolors.FAIL + "No executable found!" + bcolors.ENDC)
    exit(1)

# Path to the deployment folder
deploy_folder = "./Deploy/" + platform.system() + "/" + project_version + "/"

# Delete folder if exists
if os.path.exists(deploy_folder):
    print(bcolors.WARNING + "Deploy folder exists: " + deploy_folder + ", deleting..." + bcolors.ENDC)
    shutil.rmtree(deploy_folder)

# Create deploy folder
os.makedirs(deploy_folder)
os.makedirs(deploy_folder + "Bin/")

for executable_path in executables:
    # Run ldd on the executable
    ldd_output = os.popen("ldd \"" + executable_path + "\"").read()

    # Parse the ldd output to get the library paths
    library_paths = []
    for line in ldd_output.splitlines():
        if "=>" in line:
            # Add path to library_path if it is not equal to "not found"
            if "not found" not in line:
                matches = re.findall(r"^\s*([\w\.\d+-]+) => ([/\w\.\d+ -]+)(?: \(.*\))?", line)

                if platform.system() == "Windows":
                    current_drive = os.path.splitdrive(os.getcwd())[0][0].lower()
                    library_path = matches[0][1].replace("/c/", "C:/").strip().replace("/" + current_drive + "/", current_drive + ":/")
                elif platform.system() == "Linux":
                    library_path = matches[0][1].strip()

                # Append if the library not from Windows folder or related to Visual C++
                if not library_path.startswith("C:/Windows") or "msvc" in library_path or "VCRUNTIME" in library_path:
                    library_paths.append(library_path)
                    print(bcolors.WARNING + "Library found: " + library_path)
            else:
                print(bcolors.FAIL + "Library not found: " + line.split("=>")[0].strip())

    # Copy the libraries to the deployment folder
    for library_path in library_paths:
        print(
            bcolors.OKGREEN + "Copying library: " + library_path + " to deploy folder: " + deploy_folder + bcolors.ENDC)
        shutil.copy(library_path, deploy_folder + "Bin/")

    # Copy the executable to the deployment folder
    shutil.copy(executable_path, deploy_folder + "Bin/")

# Copy allowed files to deploy folder
allowed_types = ["ini"]

for file in os.listdir(os.getcwd()):
    for allowed_type in allowed_types:
        if file.endswith(allowed_type):
            print(
                bcolors.OKBLUE + "Copying allowed file: " + file + " to deploy folder: " + deploy_folder + bcolors.ENDC)
            shutil.copy(file, deploy_folder)

# Copy assets folder if exists
if os.path.exists("./Assets/"):
    print(bcolors.OKCYAN + "Copying assets folder to deploy folder: " + deploy_folder + bcolors.ENDC)
    shutil.copytree("./Assets/", deploy_folder + "/Assets/")

# Copy levels folder if exists
if os.path.exists("./Levels/"):
    print(bcolors.OKCYAN + "Copying levels folder to deploy folder: " + deploy_folder + bcolors.ENDC)
    shutil.copytree("./Levels/", deploy_folder + "/Levels/")

# Copy shaders binaries files if exists
if os.path.exists("./Shaders/"):
    print(bcolors.OKGREEN + "Copying shaders binaries files to deploy folder: " + deploy_folder + bcolors.ENDC)
    shutil.copytree("./Shaders/", deploy_folder + "/Shaders/")
    # shutil.copytree("./Shaders/", deploy_folder + "/Shaders/", ignore=ignore_shader_sources)

# Archive source code
source_code_ignore_directories = ["Bin", ".cache", "build", ".git", "Deploy"]

for file in os.listdir(os.getcwd()):
    ignore = False
    for ignore_directory in source_code_ignore_directories:
        if file.startswith(ignore_directory):
            ignore = True
            break
    
    if ignore:
        continue

    if os.path.isdir(file):
        shutil.copytree(file, deploy_folder + "SourceFiles/" + file)
    else:
        shutil.copy(file, deploy_folder + "SourceFiles/" + file)

if os.path.exists(deploy_folder + "SourceFiles"):
    shutil.make_archive(deploy_folder + "SourceFiles", 'zip', deploy_folder + "SourceFiles")
    shutil.rmtree(deploy_folder + "SourceFiles")

# Open deploy folder in explorer
if platform.system() == "Windows":
    publish("windows", project_version)
    os.startfile(os.path.abspath("./Deploy/Windows"))
elif platform.system() == "Linux":
    publish("linux", project_version)
    os.system("xdg-open ./Deploy/Linux")

# Archive and delete folder
shutil.make_archive("./Deploy/" + platform.system() + "/" + project_version, 'zip', deploy_folder)
shutil.rmtree(deploy_folder)