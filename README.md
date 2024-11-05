# Polygonal Mesh Smoothing
<!--<i>개인 저장용 리포지토리 이고 출처를 명확히 밝히는 바이다.</i>-->
Smoothing is one of method to make smooth surface of an object to move positions of vertice that are constructed surface of an object. Most basic method is `Average Smoothing`. This is a method that makes it smooth by moving the positions of the vertices that make up the `Triangle Mesh` on average. However, since this method simply moves the position of the vertex on average, if a high degree of smoothness is desired, an object may be obtained in which the original feature is significantly collapsed. Much research has been done to improve this phenomenon of original feature collapse, and among them, `Laplacian Smoothing` is dealt with and saved in this project.

## Average Smoothing
Before explaining Laplacian Smoothing, will simply explain Average Smoothing in this section. The basic principle of Smoothing metion at this project uses vectors of vertice make up the triangle mesh. 

$$\frac{(P_{i-1} + P_{i+1})}{2}-P_i$$

When there is a reference point $P_i$, a new vector is obtained through a vector addition operation based on the position vectors of each adjacent vertex $P_{i+1}$ and $P_{i-1}$, and the magnitude of the corresponding vector is computed as $\frac{1}{2}$. The position pointed to by the new vector becomes the new position of $P_i$, and the expected value of this operation is smoothing as the internal angle of the mesh formed with the adjacent vertices becomes smoother than before.

$$\frac{1}{\vert N_i \vert}(\displaystyle\sum_{j\in N_i}{P_j})-P_i\quad\text{(in 3D)}$$

This method called Average Smoothing achieve making an object smooth but we can see that the original feature of the object is collapsed. When an extremely large number of iterations are run, you can see that the original is degraded to the point where it is difficult to recognize.

<p align="center">
  <img src="https://github.com/user-attachments/assets/2f583a1b-3084-45be-afc1-d0d5d889e95e">
  <b>Figure 1.</b> A standard object rendering (pictured left) and result of laplacian smoothing (pictured right).
</p>

To address this phenomenon, suggests `Laplacian Smoothing`.

## Laplacian Smoothing
$$P_i \leftarrow P_i + \lambda L(P_i)$$

When there is a reference point $P_i$, the vector addition operation is computed based on the respective position vectors of the adjacent vertices $P_{i+1}$ and $P_{i-1}$ in the same way as the previous method Average Smoothing. The difference from Average Smoothing is where the original feacture can be maintained a little more compared Average Smoothing.

<!--기준점 $P_i$가 있을 때 인접 Vertex $P_{i+1}$과 $P_{i-1}$의 각각의 위치 Vector를 기반으로 벡터 합 연산을 이전 Average Smoothing과 동일하게 구한다. Average Smoothing과 다른 점은 평균값에 의한 이동의 격차가 매우 큰 것을 감안해서 인접 Vertex간의 연산 결과인 새로운 벡터가 가리키는 방향의 크기를 $\frac{1}{2}$ 한 것으로 좀 더 원본 Feature를 유지할 수 있게 되었다. 하지만 이 방식도 결국에는 원본 Feature가 무너지게 된다. Average Smoothing과 비교했을 때 덜 무너지는 것은 확인할 수 있다. 3D Mesh는 아래 식을 참고할 수 있다.--> 

$$P_i^{(t+1)}=P_i^{(t)}+\lambda L(P_i^{(t)})$$

In matrix notation,

$$P = 
 \begin{bmatrix}
  x_{1}, & y_{1}  \\
  x_{2}, & y_{2}  \\
  \vdots & \vdots \\
  x_{n}, & y_{n}
 \end{bmatrix} \in R^{n×2}, \quad
 L = \frac{1}{2}
 \begin{bmatrix}
  2 & -1 & & & & -1 \\
  -1 & 2 & -1 & & & \\
     &   & \vdots & & & \\
  & & & & &  \\
  & & & -1 & -2 & -1 \\
  -1 & & & & -1 & -2 \\
 \end{bmatrix} \in R^{n×n}
 $$

However, Laplacian Smoothing couldn't also avoid the phenomenon of collapse origianl feature of the object. Although this is maintaining feature such as sharp part of the object well when compared Average Smoothing, it's necessary to poignantly maintain more. To improve this, uses a cotangent weight among adjacency vertices in the Laplacian Smoothing.

## Cotangent-Weighted Laplacian Smoothing
The core concept of Laplacian Smoothing using cotangent weights is to obtain the weighted average using the cotangent value of each internal angle $\theta$ of a triangle that does not include the edges shared by adjacent faces, and then obtain the direction vector of the new position where $P_i$ will move. The background of using cotangent weights is to obtain a new vector as the average of the points where the feet of the perpendiculars to each edge meet each other in order to make the meaningless vertex movement of the existing Laplacian Smoothing method more clear. The Face that places the foot of the perpendicular to each edge is based on one of the two adjacent Faces that share one edge of the Triangle Mesh.

$$W_{ij}=\frac{h_{ij}^1+h_{ij}^2}{l_{ij}}=\frac{1}{2}(\cot\alpha_{ij}+\cot\beta_{ij})$$

This means an internal angle $\theta$ that doesn't involve an edge between two adjacency vertices sharing one edge. And, can get new vector by computing average of vertices where perpendiculars of adjacency face meet each other using cotangent $\theta$.

$$L(P_i)=\frac{1}{\sum_{j\in N_i}{W_{ij}}}(\displaystyle\sum_{j\in N_i}{W_{ij} P_j})-P_i$$

We make a data structure `Edge` to search adjacency face and insert functions needed. This project shows an way how to get edge informations using information both vertices and face.

<p align="center">
  <img src="https://github.com/user-attachments/assets/5e73eba3-4931-46ee-9fa9-97b06caff32a">
  <b>Figure 2.</b> A result of cotangent-weighted laplacian smoothing(pictured right). Shows that the origin features are sharply preserved despite a large number of smoothing iterations.
</p>

## Control
You cna control this project using keys as follow:
- `F`: Laplacian Smoothing
- `C`: Cotangent-weighted Laplacian Smoothing
- `R`: Reset an object

The loaded object goes through several steps to draw the mesh. The triangle-based Face and Mesh are constructed through the Vertex information. Therefore, you can check each rendering step by rendering it using keys as follow:
- `P`: Vertex
- `W`: Wireframe
- `S`: Surface - OpenGL Flat
- `A`: Surface - OpenGL Smooth

## Dependencies
**Windows OS**
```
Visual Studio 2019 / Release x86
GLUT
GLEW
```
```
# My Experimental Environment
CPU Intel(R) Core(TM) i5-6400
GPU NVIDIA GeForce GT 730
```

## Load an .obj file
(To be uploaded)
