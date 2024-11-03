# Polygonal Mesh Smoothing
오브젝트의 면적을 이루는 Vertex의 위치를 움직여주면서 표면을 부드럽게 만들어주는 기법을 스무딩 기법이라고 한다. 가장 기본적인 방법으로는 `Average Smoothing` 방법이 있다. 이는 Triangle Mesh를 이루는 Vertex의 위치를 평균적으로 변화시켜줌으로써 매끄럽게 만들어주는 방식이다. 그러나 이 방식은 원본 Feature를 많이 무너뜨리는 방식이기 때문에 부드러움의 정도를 많이 원하는 경우 원본 Feature가 상당히 많이 무너진 상태의 오브젝트를 얻을 수가 있다. 이 원본 Feature가 무너지는 현상을 개선하기 위해 그동안 많은 연구가 이루어졌으며, 그 중에서 `Laplcian Smoothing`을 이 프로젝트에서 다루고 저장하려고 한다. <i>개인 저장용 리포지토리 이고 출처를 명확히 밝히는 바이다.</i>

## Average Smoothing
Laplacian Smoothing을 설명하기 이전, Average Smoothing에 대해서 간단하게 설명하고자 한다. 이 프로젝트에서 언급하는 Smoothing 기법의 기본적인 원리는 Triangle Mesh를 이루는 Vertex의 위치 Vector를 이용한다. 

$$(P_i-1 + P_i+1)/2-p_i$$

기준점 `Pi`가 있을 때 인접 Vertex `Pi+1`과 `Pi-1`의 각각의 위치 Vector를 기반으로 벡터 합 연산을 통한 새로운 벡터를 구하고 해당 벡터의 크기를 1/2 연산을 한다. 새로운 벡터가 가리키는 위치는 `Pi`의 새로운 위치가 되고 이 연산의 기댓값은 인접 Vertex와 이루는 Mesh의 내각이 이전보다 부드러워짐으로써 Smoothing이 되는 것이다.
[원본 사진과 과한 Iteration으로 인한 Smoothing]
this method는 object가 부드러워지는 목적은 달성했지만 원본 Feature가 무너지는 현상을 확인할 수가 있다. 극단적으로 많은 Iteration이 실행되었을 때는 원본이 알아보기 힘들정도 무너져있는 것을 확인할 수 있다.

이를 개선하기 위해 Laplacian Smoothing이 제안되었다.

## Laplcian Smoothing
$$P_i <- P_i + \lambda L(P_i)$$
$$P_i^(t_1)=P_i^(t)+\lambda L(P_i^(t))$$
기준점 `Pi`가 있을 때 인접 Vertex `Pi+1`과 `Pi-1`의 각각의 위치 Vector를 기반으로 벡터 합 연산을 이전 Average Smoothing과 동일하게 구한다. Average Smoothing과 다른 점은 평균값에 의한 이동의 격차가 매우 큰 것을 감안해서 인접 Vertex간의 연산 결과인 새로운 벡터가 가리키는 방향의 크기를 1/2 한 것으로 좀 더 원본 Feature를 유지할 수 있게 되었다. 하지만 이 방식도 결국에는 원본 Feature가 무너지게 된다. Average Smoothing과 비교했을 때 덜 무너지는 것은 확인할 수 있다.
3D Mesh는 아래 식을 참고할 수 있다.
$$1/\vert N_i \vert(\displaystyle\sum_{j\in N_i}{P_j}-P_i)$$
이를 개선하기 위해서 기존의 Laplacian Smoothing 방식에서 인접 Vertex 간의 Cotangent 가중치를 사용하였다. 

## Cotangent-Weighted
Cotangent 가중치를 사용한 Laplacian Smoothing의 핵심적인 개념은 인접 Face가 서로 공유하는 변을 포함하지 않는 Triangle의 내각의 각 \ceta 의 Cotangent 값을 이용하여 가중치 평균을 구한 뒤 $P_i가 움직일 새로운 위치의 방향 벡터를 구한다.
Cotangent 가중치를 사용하는 배경은 기존의 Laplacian Smoothing 방식의 무의미한 Vertex 이동을 좀 더 명확하게 하기 위해서 각 변에 수선의 발이 서로 만나는 지점의 평균으로 새로운 벡터를 구한다. 각 변에 수선의 발을 내리는 Face는 Triangle Mesh의 한 변을 공유하는 2개의 인접 Face 중 1개의 Face를 기준으로 한다.
이는 한 변을 공유하는 2개의 인접 Face에서 공유하는 변을 포함하지 않는 내각 \ceta를 의미하고 각 \ceta의 Cotangent 값을 이용하여 기댓값인 인접 Face의 수선의 발이 서로 만나는 지점의 평균으로 새로운 벡터를 구할 수 있다.

$$(L(P_i)=(\displaystyle\sum_{j\in N_i}{W_ij})/1(\displaystyle\sum_{j\in N_i}{W_ij P_j})-P_i)$$

인접 Face를 탐색할 수 있도록 새로운 자료구조 `Edge`를 만들고 필요한 기능을 삽입한다.



## Dependencies



## Load an .obj file

