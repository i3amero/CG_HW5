## Computer Graphics HW 5 - 202011378 차현준

Key Implementation Steps
1. Preparing Input Data

sphere_scene.cpp:<br/>
Vertex Buffer → Vertex positions<br/>
Index Arrangement (gIndexBuffer) → Triangle Connection Information<br/>
Spherical mesh is automatically generated, including up/down poles<br/>

2. Conversion Pipeline

model transform:<br/>
Move to unit sphere → (0, 0, -7), scale to radius 2<br/>
view transform:<br/>
Eye point (0, 0, 0), camera positioned in the -z direction<br/>
projection transform:<br/>
Set the perspective projection (frustrum): l=-0.1, r=0.1, b=-0.1, t=0.1, n=0.1, f=1000<br/>
viewport transform:<br/>
Transform to screen coordinates: nx=512, ny=512<br/>

3. vertex transformation

Transform each vertex into a proj * view * model (MVP) matrix<br/>
divide from NDC space to clip space ( clip / = clip.w)<br/>
Conversion to screen coordinate system<br/>

4. Triangular rasterization

Calculate triangle bounding box<br/>
pixel 별로 barycentric weight (w0, w1, w2) 계산<br/>
Check whether the triangle is inside (w0, w1, w2 >= 0)<br/>
When passing the depth test (z < DepthBuffer[idx]) after the z-value interpolation:<br/>
depth buffer update<br/>
Pixel Color Settings<br/>

5. flat shading

Calculate the triangle centroid<br/>
face normal 계산 (cross(e1, e2))<br/>
Calculate Phong lighting in centroid<br/>
Apply centroid calculation color throughout the triangle<br/>

* Output<br/>
Outputting the CPU-calculated pixel data (OutputImage) to OpenGL as glDrawPixels()<br/>

* Final code configuration<br/>
Main_EmptyViewer.cpp → main() run, transform, rasterizer<br/>
sphere_scene.cpp → sphere mesh generate<br/>
<br/>
![441592635-0c6ec7e9-ee1b-4e0b-839c-b913cdaaeaae](https://github.com/user-attachments/assets/f2fa9238-10d9-4228-ad0e-180a40d18f77)

