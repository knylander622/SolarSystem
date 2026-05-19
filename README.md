# SolarSystem
Solar System Simulator  
  
# Current
Sun, Earth, and Mars  
Visible Orbit Trails  
Random Star Placement Generation  
  
# Compile
Download and run Docker  
https://www.docker.com/products/docker-desktop/
  
Clone into repository and change into SolarSystem directory
  
Build docker image with:  
docker build -t solar-system .

Run docker image with:  
docker run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix solar-system

