#include "Camera2.h"
#include "Object3D.h"

#include "Matrix4.h"
#include <iostream>

using namespace p3d;
using namespace std;

Camera2::~Camera2() {
}

Camera2::Camera2() {
  _worldCamera.setIdentity();
  _projection.setIdentity();
  _viewX=_viewY=_viewWidth=_viewHeight=0;
}

void Camera2::frustum(double left, double right, double bottom, double top, double znear, double zfar) {
  _projection.setFrustum(left,right,bottom,top,znear,zfar);
}

void Camera2::viewport(int x, int y, int width, int height) {
  _viewX=x;_viewY=y;_viewWidth=width,_viewHeight=height;
}


const p3d::Matrix4 &Camera2::worldCamera() const {
  return _worldCamera;
}

p3d::Matrix4 Camera2::cameraWorld() const {
  return _worldCamera.inverse();
}

void Camera2::translate(double x,double y, double z,ECoordinate frame) {
  if (frame==Coordinate_Local) {
    _worldCamera*=Matrix4::fromTranslation(x,y,z);
  }
  else {
    _worldCamera=Matrix4::fromTranslation(x,y,z)*_worldCamera;
  }
}

void Camera2::translate(const Vector3 &t, ECoordinate frame) {
  translate(t.x(),t.y(),t.z(),frame);
}

void Camera2::rotate(double angle, double xAxis,double yAxis, double zAxis, ECoordinate frame) {
  if (frame==Coordinate_Local) {
    _worldCamera*=Matrix4::fromAngleAxis(angle,xAxis,yAxis,zAxis);
  }
  else {
    _worldCamera=Matrix4::fromAngleAxis(angle,xAxis,yAxis,zAxis)*_worldCamera;
  }
}

void Camera2::rotate(double angle, const Vector3 &axis, ECoordinate frame) {
  rotate(angle,axis.x(),axis.y(),axis.z(),frame);
}

p3d::Vector3 Camera2::pointTo(ECoordinate frame,const p3d::Vector3 &p) {
  if (frame==Coordinate_World) {
    return _worldCamera.transformPoint(p);
  }
  else {
    return _worldCamera.inverse().transformPoint(p);
  }
}

p3d::Vector3 Camera2::directionTo(ECoordinate frame,const p3d::Vector3 &u) {
  if (frame==Coordinate_World) {
    return _worldCamera.transformDirection(u);
  }
  else {
    return _worldCamera.inverse().transformDirection(u);
  }
}

void Camera2::applyGL() {
  p3d::projectionMatrix=_projection;
  p3d::modelviewMatrix=cameraWorld();
  glViewport(_viewX,_viewY,_viewWidth,_viewHeight);
}

p3d::Vector3 Camera2::windowToNDC(int x,int y) {
  Vector3 res(0,0,0);

  //E3Q1
  //Attention car x, y, _viewX, etc. sont des int, donc on convertit en double
  //x - viewX = position dans le repère de la view
  // positionX / viewWidth = position dans l'intervalle [0, 1]
  // 2 * position[0,1] - 1 = position dans l'intervalle [-1, 1]
  res.x( 2. * (double) ( x - _viewX) / (double)_viewWidth - 1.);
  res.y( 2. * (double) ( y - _viewY) / (double)_viewHeight - 1.);
  res.z(-1.);
  cout << "window to NDC : " << res << endl;

  return res;
}

p3d::Vector3 Camera2::windowToCamera(int x,int y) {
  Vector3 res(0,0,0);

  //E3Q2
  //on prend les coordonnées en NDC (cad dans le repère [-1, 1])
  res = windowToNDC(x,y);

  //on applique l'inverse de la projection à res
  //cad resCamera = camera->window * resWindow
  //sachant qu'ici : resWindow = (x,y) (en NDC)
  // et _projection = window->camera ( d'où le _projection.inverse() )
  res = _projection.inverse().transformPoint(res);
  cout << "window to Camera : " << res << endl;

  return res;
}

p3d::Vector3 Camera2::windowToWorld(int x,int y) {
  Vector3 res(0,0,0);

  //E3Q2
  //on recupère les coordonnées de (x,y) dans le repère Eye
  res = windowToCamera(x,y);

  // resWorld = world->camera * resCamera
  res = worldCamera().transformPoint(res);

  cout << "window to World : " << res << endl;

  return res;
}


p3d::Line Camera2::pickingRay(int x, int y) {
  Line res;
  // res.point(a), res.direction(u) to set the line (a and u are Vector3)
  //E3Q2

  //L'origine de la droite est située au niveau de la caméra, à son origine (0,0,0)
  //ACamera = (0,0,0)
  //AWorld = world->camera * ACamera
  Vector3 A = worldCamera().transformPoint(Vector3(0.,0.,0.));

  //On récupère le point cliqué dans le repère du World
  Vector3 P = windowToWorld(x,y);

  //le point d'origine est A
  res.point(A);

  //le vecteur de direction est AP
  res.direction(Vector3(A,P));

  cout << "line " << res << endl;
  return res;
}



