#include "SceneIntersection.h"

#include "Plane.h"
#include "MeshObject3D.h"

using namespace p3d;
using namespace std;


SceneIntersection::~SceneIntersection() {
  clean();
}

SceneIntersection::SceneIntersection() {
  _result.clear();
}

void SceneIntersection::clean() {
  if (!_result.empty()) {
    for(unsigned int i=0;i<_result.size();i++) {
      delete _result[i];
    }
    _result.clear();
  }
}




void SceneIntersection::insert(Intersection *i) {
  bool found=false;
  int place=_result.size();
  _result.push_back(NULL);
  while(!found) {
    if (place==0 || _result[place-1]->lambda()<=i->lambda()) {
      found=true;
    }
    else {
      _result[place]=_result[place-1];
      place--;
    }
  }
  _result[place]=i;
}

/**
  l : pickingRay dans le repère local à l'objet. l.point() donne l'origine, l.direction() donne un vecteur directeur.
  s0,s1,s2 : sommets du triangle (repère local).
  Il faut :
  - d'abord déterminer si la droite intersecte le triangle (i.e. l passe du même coté des
    segments [s0s1],[s1s2],[s2s0]
  - si la droite intersecte, calculer le lambda de l'intersection (i.e. intersection entre le plan du triangle, I=l.point()+lambda*l.direction() ).
  Remarque : voir l'appelant intersect(MeshObject3D *mesh) pour les paramètres : traduit le rayon dans le repère local; passe les 3 sommets du triangle; s'il y a intersection, l'insère dans le tableau _result
 */
bool SceneIntersection::intersect(const Line &ray,const Vector3 &s0,const Vector3 &s1,const Vector3 &s2,double *lambdaRes) {
    bool res=false;
    double lambda=0.0;

    //Algo de Möller-Trumbore
    //https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

    Vector3 e1, e2;
    Vector3 P, Q, T;
    float det, inv_det, u, v;
    float t;


    //On sélectionne deux côtés du triangle pour avoir le plan
    e1 = Vector3(s0, s1);
    e2 = Vector3(s0, s2);

    //On vérifie si le rayon directeur est dans le plan
    P = ray.direction().cross(e2);
    det = e1.dot(P);
    if(det == 0 )
        return res;

    //À ce niveau on sait que le rayon est dans le plan
    inv_det = 1.f / det;

    //Calcul de la distance entre s0 et le point d'origine du rayon
    //j'ai pas tout compris voir l'algo sur les internets

    T = Vector3(s0, ray.point());

    u = T.dot(P) * inv_det;

    if(u < 0.f || u > 1.f)
        return res;

    Q = T.cross(e1);

    v = ray.direction().dot(Q) * inv_det;

    if(v < 0.f || u + v > 1.f)
        return res;

    t = e2.dot(Q) * inv_det;

    res = t > 0.;

    if(res)
        lambda = t;

    *lambdaRes=lambda;
    return res;
}


void SceneIntersection::intersect(MeshObject3D *e) {
  Line rayLocal;
  rayLocal.point(e->pointTo(Coordinate_Local,_pickingRay.point()));
  rayLocal.direction(e->directionTo(Coordinate_Local,_pickingRay.direction()));
  double lambda;

  for(unsigned int i=0;i<e->nbFace();i++) {
    bool ok=intersect(rayLocal,e->positionVertexFace(i,0),e->positionVertexFace(i,1),e->positionVertexFace(i,2),&lambda);
    if (ok && lambda>1) {
      Intersection *inter=new Intersection();
      inter->mesh(e);
      inter->lambda(lambda);
      inter->rayWorld(_pickingRay);
      this->insert(inter);
    }
  }
}


void SceneIntersection::intersect(const std::vector<p3d::MeshObject3D *> &allObj, const p3d::Line &pickingRay) {
  _pickingRay=pickingRay;
  MeshObject3D *e;
  clean();
  for(vector<MeshObject3D *>::const_iterator i=allObj.begin();i!=allObj.end();i++) {
    e=*i;
    intersect(e);
  }
}


