# ExtractCppCodeInfo

You can use this repo to extract class declaration/class variable/macro define/included header from a c++ project.  
You need a docker image https://hub.docker.com/repository/docker/leaves2docker/refactor_tool for the running environment.  
   
# usage    
Enter the docker contianer: docker run -v <your_working_dir>:/hx/work -it <image_name> bash.  
python3 cppAna.py <cpp_project_directory>.  
