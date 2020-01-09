#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usr\include\GL\freeglut.h"
#include "gmath.h"

// 매크로 상수 정의
#define MAX_DEPTH 1
#define H 768
#define W 1024
unsigned char Image[H * W * 3];

std::vector<GSphere> SphereList;
std::vector<GLight> LightList;

// 콜백 함수 선언
void Render();
void Reshape(int w, int h);
void Timer(int id);



// 광선 추적 함수
void CreateImage();
GVec3 RayTrace(GLine ray, int depth);
GVec3 Phong(GPos3 P, GVec3 N, GSphere Obj);
bool intersect_line_sphere(GLine ray, int &sidx, double &t);

int main(int argc, char **argv)
{
	// OpenGL 초기화, 윈도우 크기 설정, 디스플레이 모드 설정
	glutInit(&argc, argv);
	glutInitWindowSize(W, H);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// 윈도우 생성 및 콜백 함수 등록
	glutCreateWindow("RayTracer");
	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	glutTimerFunc(10, Timer, 0);

	// 조명 설정
	GLight Light0;
	Light0.Pos.Set(-500.0, 200.0, 5000.0);
	Light0.Ia.Set(0.7, 0.2, 0.2);
	Light0.Id.Set(1.0, 1.0, 1.0);
	Light0.Is.Set(1.0, 1.0, 1.0);
	LightList.push_back(Light0);

	GLight Light1;
	Light1.Pos.Set(-300.0, 300.0, -1000.0);
	Light1.Ia.Set(0.2, 0.2, 1.0);
	Light1.Id.Set(1.0, 1.0, 1.0);
	Light1.Is.Set(0.8, 0.8, 0.8);
	LightList.push_back(Light1);
	
	GLight Light2; //추가된 라이트
	Light2.Pos.Set(300, 0.0, -500.0);
	Light2.Ia.Set(0.0, 1.0, 0.2);
	Light2.Id.Set(1.0, 1.0, 0.1);
	Light2.Is.Set(1.0, 0.8, 0.8);
	LightList.push_back(Light2);
	



	// 장면에 3개의 구를 배치한다.
	GSphere Sphere0;
	Sphere0.Pos.Set(0, 0, -300.0);
	Sphere0.Rad = 30.0;
	Sphere0.Ka.Set(0.1, 0.1, 0.1);
	Sphere0.Kd.Set(0.8, 0.8, 0.8);
	Sphere0.Ks.Set(0.9, 0.9, 0.9);
	Sphere0.ns = 8.0;
	Sphere0.bTransparent = true;
	SphereList.push_back(Sphere0);

	GSphere Sphere1;
	Sphere1.Pos.Set(0, 0, -500.0);
	Sphere1.Rad = 20.0;
	Sphere1.Ka.Set(0.2, 0.2, 0.2);
	Sphere1.Kd.Set(0.7, 0.0, 0.0);
	Sphere1.Ks.Set(0.8, 0.8, 0.8);
	Sphere1.ns = 8.0;
	SphereList.push_back(Sphere1);

	GSphere Sphere2;
	Sphere2.Pos.Set(-80, -80, -650.0);
	Sphere2.Rad = 20.0;
	Sphere2.Ka.Set(0.2, 0.2, 0.2);
	Sphere2.Kd.Set(0.0, 0.7, 0.0);
	Sphere2.Ks.Set(0.8, 0.8, 0.8);
	Sphere2.ns = 8.0;
	SphereList.push_back(Sphere2);

	GSphere Sphere3;
	Sphere3.Pos.Set(80, -80, -650.0);
	Sphere3.Rad = 20.0;
	Sphere3.Ka.Set(0.2, 0.2, 0.2);
	Sphere3.Kd.Set(0.0, 0.0, 0.7);
	Sphere3.Ks.Set(0.8, 0.8, 0.8);
	Sphere3.ns = 8.0;
	SphereList.push_back(Sphere3);

	GSphere Sphere4;
	Sphere4.Pos.Set(80, 80, -650.0);
	Sphere4.Rad = 20.0;
	Sphere4.Ka.Set(0.2, 0.2, 0.2);
	Sphere4.Kd.Set(0.0, 0.7, 0.7);
	Sphere4.Ks.Set(0.8, 0.8, 0.8);
	Sphere4.ns = 8.0;
	SphereList.push_back(Sphere4);

	GSphere Sphere5;
	Sphere5.Pos.Set(200, 500, -500.0);
	Sphere5.Rad = 20.0;
	Sphere5.Ka.Set(0.2, 0.2, 0.2);
	Sphere5.Kd.Set(0.7, 0.0, 0.7);
	Sphere5.Ks.Set(0.8, 0.8, 0.8);
	Sphere5.ns = 8.0;
	SphereList.push_back(Sphere5);

	// 이미지를 생성
	CreateImage();

	// 이벤트를 처리를 위한 무한 루프로 진입한다.
	glutMainLoop();

	return 0;
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void Render()
{
	// 칼라 버퍼와 깊이 버퍼 지우기
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 칼라 버퍼에 Image 데이터를 직접 그린다.
	glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, Image);

	// 칼라 버퍼 교환한다
	glutSwapBuffers();
}

void Timer(int id)
{


	static double theta = 0.0;
	theta += 0.1;
	double x = 50 * cos(theta);
	double y = 50 * sin(theta);
	SphereList[0].Pos[0] = x;
	SphereList[0].Pos[1] = y;
	static double thetaka = 1.0;
	thetaka -= 0.15;
	
	
	double x1 = 70 * cos( theta);
	double y1 = 70 * sin( theta);
	
	double x2 = 70 * cos( theta + (180 / 3.14) * 180);
	double y2 = 70 * sin(theta + (180 / 3.14) * 180);
	//안정
	double x3 = 120 * cos(theta + (180 / 3.14) * 40);
	double y3 = 120 * sin( theta + (180 / 3.14) * 40);

	double x4 = 120 * cos(theta + (180 / 3.14) * 120);
	double y4 = 120 * sin(theta + (180 / 3.14) * 120);

	//Sphere0.Ka.Set(0.1, 0.1, 0.1);
	
	
	SphereList[2].Pos[0] = x1;
	SphereList[2].Pos[1] = y1;


	SphereList[3].Pos[0] = x2;
	SphereList[3].Pos[1] = y2;

	SphereList[4].Pos[0] = x3;
	SphereList[4].Pos[1] = y3;

	SphereList[5].Pos[0] = x4;
	SphereList[5].Pos[1] = y4;
	

	CreateImage();
	glutPostRedisplay();
	glutTimerFunc(1, Timer, 0);
}

void CreateImage()
{
	int x0 = -W / 2;
	int y0 = H / 2 - 1;
	double z = -(H / 2) / tan(M_PI * 15 / 180.0);
	for (int i = 0; i < H; ++i)
	{
		for (int j = 0; j < W; ++j)
		{
			double x = x0 + j;
			double y = y0 - i;
			GLine ray(GPos3(0.0, 0.0, 0.0), GPos3(x, y, z));
			GVec3 Color = RayTrace(ray, 0);

			int idx = ((H - 1 - i) * W + j) * 3;
			unsigned char r = (Color[0] > 1.0) ? 255 : (unsigned int)(Color[0] * 255);
			unsigned char g = (Color[1] > 1.0) ? 255 : (unsigned int)(Color[1] * 255);
			unsigned char b = (Color[2] > 1.0) ? 255 : (unsigned int)(Color[2] * 255);
			Image[idx] = r;
			Image[idx + 1] = g;
			Image[idx + 2] = b;
		}
	}
}

GVec3 RayTrace(GLine ray, int depth)
{

	GVec3 C;
	if (depth++ > MAX_DEPTH)
		return C;



	int sidx;   // 광선과 교차하는 가장 까가운 구의 인덱스
	double t;   // 교차점에서 광선의 파라미터 t
	if (intersect_line_sphere(ray, sidx, t))
	{

		GPos3 P;
		GVec3 N, R, T1;
		GLine ray_reflect, ray_refract;
		P = ray.Eval(t);//교차점
		N = (P - SphereList[sidx].Pos); // 법선벡터
		N = N.Normalize();//

		GVec3 L = P - ray.GetPt();//입사광선
		L = L.Normalize();

		/////////////////////반사
		R = L - 2 * (N*L)*N; //반사광선
		R = R.Normalize();//





		//////////////////////굴절
		double cos1 = SQRT(1 - (SQR(1 / 1.015)*(1 - SQR(N*(-L)))));
		T1 = (1 / 1.015)*L - (cos1 - (1 / 1.015)*N*(-L))*N;
		T1 = T1.Normalize(); //1차 굴절 방향 확인
		GLine f1 = GLine(P, T1); //1차 굴절 직선 
		double r = SphereList[sidx].Rad;
		GVec3 u = f1.p - SphereList[sidx].Pos;
		GVec3 v = f1.v;
		double t2 = -(u*v) + SQRT(SQR(u*v) - (u*u - r * r)); //1차 굴절 직선과 구의 교점
		GPos3 P2 = f1.Eval(t2); // 2차 굴절이 일어날 포인트 확인
		GVec3 N2 = (SphereList[sidx].Pos - P2).Normalize(); //2차 굴절 법선벡터
		double cos2 = SQRT(1 - (SQR(1 / 1.015)*(1 - SQR(N2*(-T1)))));
		GVec3 T2 = (1 / 1.015)*T1 - (cos2 - (1 / 1.015)*N2*(-T1))*N2;
		T2 = T2.Normalize(); //2차 굴절 방향 확인

		ray_reflect = GLine(P, R);
		ray_refract = GLine(P2, T2);

		C = Phong(P, N, SphereList[sidx])
			+ 0.3 * RayTrace(ray_reflect, depth)   // 반사광선
			+ 0.3 * RayTrace(ray_refract, depth);   // 굴절광선

	}

	return C;
}

bool intersect_line_sphere(GLine ray, int &sidx, double &t)
{
	
	int count = 0;
	for (int i = 0; i <SphereList.size(); i++)
	{
		double distance = dist(ray, SphereList[i].Pos);
		if (distance <= SphereList[i].Rad)
		{
			double r = SphereList[i].Rad;
			GVec3 u = ray.p - SphereList[i].Pos;
			GVec3 v = ray.v;
			double Tc = -(u*v) - SQRT(SQR(u*v) - (u*u - r * r));
			if (Tc <= 0)
				continue;
			sidx = i;
			t = Tc;
			count++;
			break;
		}

	}
	if (count == 0)
		return false;
	else
		return true;

}


GVec3 Phong(GPos3 P, GVec3 N, GSphere Obj)
{

	GVec3 C[3];

	for (size_t i = 0; i < 3; i++)
	{

		GVec3 Ambi = GVec3(LightList[i].Ia[0] * Obj.Ka[0], LightList[i].Ia[1] * Obj.Ka[1], LightList[i].Ia[2] * Obj.Ka[2]);
		GVec3 Diff = GVec3(LightList[i].Id[0] * Obj.Kd[0], LightList[i].Id[1] * Obj.Kd[1], LightList[i].Id[2] * Obj.Kd[2]);
		GVec3 Spec = GVec3(LightList[i].Is[0] * Obj.Ks[0], LightList[i].Is[1] * Obj.Ks[1], LightList[i].Is[2] * Obj.Ks[2]);
		
		GVec3 V = GPos3(0.0, 0.0, 0.0) - P;
		V = V.Normalize();//
		GVec3 L = LightList[i].Pos - P;
		L = L.Normalize();//
		GVec3 R = -L + 2 * (N*L)*N;
		R = R.Normalize();//
		C[i] = Ambi +
			Diff * MAX(0.0, N * L) +
			Spec * pow(MAX(0.0, V * R), Obj.ns);
	}

	return C[0] + C[1];

}