# HPC_RayTracer
 HPC-accelerated Mento Carlo ray tracer
# Project Description
Photo-realistic images are of great charm and importance to daily life. It has many applications like special effects in movie, scene design inside room and computer games. However, creating photo-realistic images is time-consuming since we need to simulate the light path of every ray ejected from view point. Meanwhile, there are heavy computation about the intersection point between ray and triangles faces. That's why we consider using OpenMP (and GPU, if possible,) to accelerate this process. The speed-up will boost efficiency of artists to create and adjust scene layout to create images of better quality.
# Simulation Methods and Algorithms
- OpenMP (parallelize trace stepping of different rays)
- GPU (still under consideration)
# Expected Results
1. Simulate the Cornell Box scene and render corresponding images
2. Render models with materials and thousands of faces
3. Reach significant speed-up over sequential computation
4. Explore possibility of using GPU to speed up rendering
# Environment
Windows 10, Intel(R) Core(TM) i7-7700HQ CPU @2.80GHz
RAM: 16.0 GB
Installed: Visual Studio, Microsoft MPI
# Run Program
I suggest downloading the whole project and open the sln file in Visual Studio 2017. 
Make sure you have installed Microsoft MPI and the directory of mpiexec.exe is listed in System Path.
On windows platform: mpiexec.exe -n 8 RayTracer.exe (feel free to choose number of processes)

![image](https://github.com/DevidXu/RayTracer/blob/master/Slides/S1.jpg)