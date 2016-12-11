#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GL/glut.h>

#include "glutils.hpp"
#include "mat4.hpp"
#include "vec3.hpp"
#include "vec2.hpp"
#include "triangle_index.hpp"
#include "vertex_opengl.hpp"
#include "image.hpp"
#include "mesh.hpp"

#include "Signe.hpp"

using namespace std;

  Signe::Signe(float a, float b, float c, float v){

    vbo_object=0;
    vboi_object=0;
    texture_id_object=0;

    x=a;
    y=b;
    z=c;
    
    vitesseTir=v;
    
  }

  Feuille::Feuille(float a, float b, float c, float v) : Signe(a, b, c, v){}
  Pierre::Pierre(float a, float b, float c, float v) : Signe(a, b, c, v){}
  Ciseau::Ciseau(float a, float b, float c, float v) : Signe(a, b, c, v){}

  void Signe::init_model(){
    

    mat4 transform = mat4(   s, 0.0f, 0.0f,x,
                          0.0f,    s, 0.0f,y,
                          0.0f, 0.0f,   s ,z,
                          0.0f, 0.0f, 0.0f,1.0f);
    apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
    apply_deformation(&m,matrice_rotation(0.0*M_PI/2.0f,1.0f,0.0f,0.0f));
    apply_deformation(&m,transform);

    // Calcul automatique des normales du maillage
    update_normals(&m);
    // Les sommets sont affectes a une couleur blanche
    fill_color(&m,vec3(1.0f,1.0f,1.0f));

    //attribution d'un buffer de donnees (1 indique la création d'un buffer)
    glGenBuffers(1,&vbo_object);                                 PRINT_OPENGL_ERROR();
    //affectation du buffer courant
    glBindBuffer(GL_ARRAY_BUFFER,vbo_object); PRINT_OPENGL_ERROR();
    //copie des donnees des sommets sur la carte graphique
    glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); PRINT_OPENGL_ERROR();


    //attribution d'un autre buffer de donnees
    glGenBuffers(1,&vboi_object); PRINT_OPENGL_ERROR();
    //affectation du buffer courant (buffer d'indice)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object); PRINT_OPENGL_ERROR();
    //copie des indices sur la carte graphique
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); PRINT_OPENGL_ERROR();

    // Nombre de triangles de l'objet 5
    nbr_triangle_object = m.connectivity.size();
  }
  
  void Signe::afficher_model(){
   
        //envoie des parametres uniformes
    {
        glUniformMatrix4fv(get_uni_loc(shader_program_id,"rotation_model"),1,false,pointeur(transformation_model.rotation));    PRINT_OPENGL_ERROR();

        vec3 c = transformation_model.rotation_center;
        glUniform4f(get_uni_loc(shader_program_id,"rotation_center_model") , c.x,c.y,c.z , 0.0f);                                 PRINT_OPENGL_ERROR();

        vec3 t = transformation_model.translation;
        glUniform4f(get_uni_loc(shader_program_id,"translation_model") , t.x,t.y,t.z , 0.0f);                                     PRINT_OPENGL_ERROR();
    }

    //placement des VBO
    {
        //selection du VBO courant
        glBindBuffer(GL_ARRAY_BUFFER,vbo_object);                                                    PRINT_OPENGL_ERROR();

        // mise en place des differents pointeurs
        glEnableClientState(GL_VERTEX_ARRAY);                                                          PRINT_OPENGL_ERROR();
        glVertexPointer(3, GL_FLOAT, sizeof(vertex_opengl), 0);                                        PRINT_OPENGL_ERROR();

        glEnableClientState(GL_NORMAL_ARRAY); PRINT_OPENGL_ERROR();                                    PRINT_OPENGL_ERROR();
        glNormalPointer(GL_FLOAT, sizeof(vertex_opengl), buffer_offset(sizeof(vec3)));                 PRINT_OPENGL_ERROR();

        glEnableClientState(GL_COLOR_ARRAY); PRINT_OPENGL_ERROR();                                     PRINT_OPENGL_ERROR();
        glColorPointer(3,GL_FLOAT, sizeof(vertex_opengl), buffer_offset(2*sizeof(vec3)));              PRINT_OPENGL_ERROR();

        glEnableClientState(GL_TEXTURE_COORD_ARRAY); PRINT_OPENGL_ERROR();                             PRINT_OPENGL_ERROR();
        glTexCoordPointer(2,GL_FLOAT, sizeof(vertex_opengl), buffer_offset(3*sizeof(vec3)));           PRINT_OPENGL_ERROR();

    }

    //affichage
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object);                           PRINT_OPENGL_ERROR();
        glBindTexture(GL_TEXTURE_2D, texture_id_object);                             PRINT_OPENGL_ERROR();
        glDrawElements(GL_TRIANGLES, 3*nbr_triangle_object, GL_UNSIGNED_INT, 0);     PRINT_OPENGL_ERROR();
    }
  }

  bool Signe::tirer(bool active){

    if(active==false || transformation_model.translation.z<-30.0f)	transformation_model.translation.z= 0.0f;
    if(active==true){
       transformation_model.translation.z-=vitesseTir;
       transformation_model.rotation_center.z+=vitesseTir;
       if(transformation_model.translation.z<-30.0f)	active=false;
    }
    
    return active;
  }    

  void Feuille::init_feuille(){
 
      m = load_obj_file("../data/feuille.obj");
      s = 0.05f; 
    
      init_model();
    }
    
    void Pierre::init_pierre(){
 
      m = load_obj_file("../data/pierre.obj");
      s = 0.1f; 
    
      init_model();
    }
    
    void Ciseau::init_ciseau(){
 
      m = load_obj_file("../data/ciseaux.obj");
      s = 0.01f; 
      
      init_model();
    }
