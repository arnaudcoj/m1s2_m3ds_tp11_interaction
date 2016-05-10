#include "GLApplication.h"

#include <iostream>

#include "GLTool.h"
#include "Plane.h"

/*!
*
* @file
*
* @brief
* @author F. Aubert
*
*/


using namespace p3d;
using namespace std;

GLApplication::~GLApplication() {

}

GLApplication::GLApplication() {
  _leftPanelMenu << "Manipulation Translation" << "Manipulation Rotation";

  _cow.readInit("cow.obj");
  _cow.computeNormal();

  _triceratops.readInit("triceratops.obj");
  _triceratops.computeNormal();

  _triangle.readInit("triangle.obj");
  _triangle.computeNormal();

  _cow.translate(Vector3(-0.7,0.65,-0.3));
  _cow.rotate(-20,Vector3(0,1,0));
  _triceratops.translate(Vector3(0.5,0.5,1.0));
  _triangle.translate(Vector3(0,1,2));
  _triangle.rotate(30,Vector3(1,1,0));



  _camera.translate(Vector3(0,1,10),Coordinate_Local);


  _meshArray.push_back(&_cow);
  _meshArray.push_back(&_triceratops);
  _meshArray.push_back(&_triangle);

  _selectedIntersection=NULL;

  p3d::lightPosition[0]=Vector4(0,0,10,1);

}

void GLApplication::initialize() {
  glClearColor(1,1,1,1);

  glLineWidth(4);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearDepth(1);

  initGLTool();
  glPointSize(5.0);


  _groundTexture.readMipmap("mosaic_pierre.jpg");
  glEnable(GL_STENCIL_TEST);


  _cow.initDraw();
  _triceratops.initDraw();
  _triangle.initDraw();

}



void GLApplication::drawScene() {

  p3d::modelviewMatrix.push();

  drawGround();
  p3d::ambientColor=Vector4(0,0,0,1);
  drawCow();
  drawTriceratops();
  drawTriangle();

  p3d::modelviewMatrix.pop();

}

void GLApplication::drawCow() {
  // tracé de la vache
    p3d::modelviewMatrix.push();
    p3d::modelviewMatrix*=_cow.worldLocal();
    p3d::diffuseColor=Vector3(0.8,0.0,0.0);
    p3d::shaderLightPhong();
    _cow.draw();
    p3d::modelviewMatrix.pop();
    // fin tracé de la vache
}

void GLApplication::drawTriceratops() {
  // tracé du tricératops
  p3d::modelviewMatrix.push();

  p3d::modelviewMatrix*=_triceratops.worldLocal();
  p3d::diffuseColor=Vector3(0.0,0.6,0.6);
  p3d::shaderLightPhong();
  _triceratops.draw();
  p3d::modelviewMatrix.pop();
  // fin tracé tricératops
}

void GLApplication::drawTriangle() {
  // tracé du triangle
  p3d::modelviewMatrix.push();

  p3d::modelviewMatrix*=_triangle.worldLocal();
  p3d::diffuseColor=Vector3(0.4,0.6,0.9);
  p3d::diffuseBackColor=Vector3(0.8,0.2,0.7);
  p3d::shaderLightPhong();
  _triangle.draw();
  p3d::modelviewMatrix.pop();
  // fin tracé triangle
}


void GLApplication::drawGround() {
  p3d::modelviewMatrix.push();
  p3d::textureMatrix.push();
  p3d::textureMatrix.scale(135,135,135);
  p3d::modelviewMatrix.translate(0,-3,0);
  p3d::modelviewMatrix.scale(10,10,10);
  p3d::ambientColor=Vector4(0,0,0,1);
  p3d::shaderTextureLight();
  p3d::drawGround(_groundTexture);
  p3d::textureMatrix.pop();
  p3d::modelviewMatrix.pop();
}

void GLApplication::drawCursor() {
  p3d::modelviewMatrix.push();
  Vector3 cursor=_pickingRay.point(100);
  p3d::modelviewMatrix.translate(cursor.x(),cursor.y(),cursor.z());
  p3d::modelviewMatrix.scale(0.05,0.05,0.05);
  p3d::diffuseColor=Vector3(0.7,0.0,0.3);
  p3d::ambientColor=Vector4(0,0,0,0);
  p3d::specularColor=Vector3(0,0,0);
  p3d::shaderLightPhong();
  p3d::drawSphere();
  p3d::modelviewMatrix.pop();
}

void GLApplication::testIntersection() {
  _debug.clear();
  cout << "Nombre d'intersections = " << _sceneIntersection.size() << endl;
  for(unsigned int i=0;i<_sceneIntersection.size();i++) {
    _debug.point(_pickingRay.point(_sceneIntersection[i]->lambda()),toString(i),Vector3(1,0,0));
  }
  _debug.segment(Line(_pickingRay.point(0),_pickingRay.direction()*1000),"","",Vector3(0,1,0));
}


/** ***************************************************************** **/
void GLApplication::resize(int width,int height) {
  _camera.viewport(0,0,width,height);
  double ratio=double(width)/height;
  _camera.frustum(-0.01*ratio,0.01*ratio,-0.01,0.01,0.03,100.0);
}


void GLApplication::update() {
  updateCamera();
  selectObject();
  moveSelectedObject();
}


void GLApplication::selectObject() {
  if (mouseRight()) {
    _pickingRay=_camera.pickingRay(mouseX(),mouseY());
  }
  if (mouseRightPressed()) {

    _sceneIntersection.intersect({&_triangle, &_triceratops, &_cow},_pickingRay);

    if (_sceneIntersection.size()>0) {

      testIntersection(); // draws ray and intersections : comment this line once intersection is ok

      _selectedIntersection=_sceneIntersection[0]; // the first intersection is the nearest (from the camera)

      // save the intersection point coordinates (required for interaction) :
      _attachPointWorld=_selectedIntersection->rayWorld().point(_selectedIntersection->lambda());
    }
    else {
      _selectedIntersection=0; // no intersection
    }
  }
}

void GLApplication::moveSelectedObject() {
  if (mouseRight()) {
    if (_selectedIntersection) { // an intersection has been found (in GLApplication::selectObject() ).
      MeshObject3D *mesh=_selectedIntersection->mesh(); // selected mesh

      double dx=double(deltaMouseX())/100.0,dy=double(deltaMouseY())/100.0; // /100.0 to attenuate mouse motion

      if (_controlMouse==Manipulation_Translation) {
          //e5q2
          Vector3 T = _camera.directionTo(Coordinate_World, Vector3(dx, dy, 0));
          mesh->translate(T ,Coordinate_World);

          //e5q3
          /*
          Vector3 T_Interface;  // <- c'est ça qu'il faut trouver : T_Interface (T' sur le schéma)

          //On devra trouver P'_Interface car T_Interface = [P_Interface,P'_Interface]
          Vector3 Pp_Interface;

          //Pour trouver P'_Interface, il nous faut trouver :
          //le rayon qui part de la caméra et qui passe par P'_Ecran (voir schéma)
          Line ray2;
          //à quelle distance de la caméra se situe le point d'intersection entre le plan Interface et le rayon
          double k;
          //grâce à ces 2 éléments on pourra retrouver P'_Interface et donc T_Interface


          //On connait :
          Vector3 P_Interface = _attachPointWorld;
          Vector3 P_Ecran = _pickingRay.point();
          Vector3 T_Ecran = _camera.directionTo(Coordinate_World, Vector3(dx, dy, 0)); //distance que parcourt le curseur
          Vector3 Eye = _camera.pointTo(Coordinate_World, Vector3(0.,0.,0.)); // Origine de la caméra

          //n = la normale du plan Ecran et donc Interaction (les plans sont parallèles)
          //on le passe dans le repère World pour que tout soit calculé dans ce repère
          Vector3 n = _camera.directionTo(Coordinate_World, Vector3(0., 0., 1.));

          //On calcule P'_Ecran (On translate P_Ecran avec le vecteur T_Ecran)
          Vector3 Pp_Ecran = P_Ecran + T_Ecran;

          //On créé le rayon qui part de la caméra et passe par le point P'_Ecran
          ray2 = _camera.pickingRay(Pp_Ecran.x(), Pp_Ecran.y());

          //Maintenant on cherche k
          //Pour se faire on doit résoudre l'intersection entre ray2 et le plan interaction

          //Le plan interaction est défini par le point P_Interaction et par le vecteur (0,0,1)_Camera
          //(les 2 plans sont parallèles, la normale de écran est (0,0,1)_Ecran, donc la même normale pour interaction
          //(voir schéma tp)

          //La droite pour laquelle on veut trouver l'intersection est ray2,
          //qui part de la caméra et passe par P'_Ecran, et logiquement par P'_Interaction

          //Pour se faire, IL SUFFIT DE résoudre [P_Interface,I].n = 0,
          //sachant qu'on connait P_Interface, que n = (0,0,1)Camera,
          //que I = Eye + k * [ray2.direction()] <- ça j'ai toujours pas compris

          //il faut savoir que Eye est le point "d'origine" de ray2, qui passe par ray2.point()
          //[Eye,ray2.point()] est donc en fait le vecteur ray2.direction()

          //En résolvant l'équation (je n'écris pas tout), on voit que
          k = - (Vector3(P_Interface, Eye).dot(n)) / ray2.direction().dot(n);

          Pp_Interface = Eye + k * ray2.direction(); // WOOT !! \o/

          //Maintenant qu'on a P'_Interface, on peut calculer T_interface
          T_Interface = Vector3(P_Interface, Pp_Interface);
          mesh->translate(T_Interface ,Coordinate_World);
*/
      }
      else if (_controlMouse==Manipulation_Orientation) {

      }
    }
  }

}

void GLApplication::updateCamera() {
    //E2Q1
  if (left()) {
      _camera.translate(-0.5, 0, 0, Coordinate_Local);
  }

  if (right()) {
      _camera.translate(0.5, 0, 0, Coordinate_Local);
  }

  if (forward()) {
      _camera.translate(0, 0, -0.5, Coordinate_Local);
  }

  if (backward()) {
      _camera.translate(0, 0, 0.5, Coordinate_Local);
  }

  if (mouseLeft()) {
    // rotate camera : deltaMouseX(), deltaMouseY() give the mouse motion

      //E2Q2
      _camera.rotate(deltaMouseY(), 1., 0, 0, Coordinate_Local);
      //cameraWorld() * Vector4 => on prend l'axe vertical du monde et on le traduit dans le repère de la caméra
      //(il y a peut être plus simple que _camera.cameraWorld() * Vector4(0., 1., 0., 0.)).xyz() ...
      _camera.rotate(-deltaMouseX(), (_camera.cameraWorld() * Vector4(0., 1., 0., 0.)).xyz(), Coordinate_Local);
  }
}


void GLApplication::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  _camera.applyGL();


  drawCursor();
  drawScene();

  p3d::ambientColor=Vector4(0.2,0.9,0.4,1.0);
  glDepthFunc(GL_ALWAYS);
  _debug.drawPoint();
  glDepthFunc(GL_LESS);
  _debug.drawSegment();

}


void GLApplication::leftPanel(int i,const std::string &s) {
  cout << "GLApplication : button clicked " << i << " " << s << endl;
  switch(i) {
  case 0:_controlMouse=Manipulation_Translation;break;
  case 1:_controlMouse=Manipulation_Orientation;break;
  }
}

