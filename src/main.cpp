/*****************************************************************************\
 *
 * Projet Jeu 3D OpenGL : ZANGUI GHALI 4ETI GROUPE B
 *
\*****************************************************************************/

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

#include "declarations_jeu.hpp"
#include "Signe.hpp"
#include "Decor.hpp"
#include "Minion.hpp"
    
// Declaration Decor
Decor dSol(pt0,pt1,pt2,pt3,n0);
Decor dPlafond(pt4,pt5,pt6,pt7,n1);
Decor dRoute(pt8,pt9,pt10,pt11,n0);
Decor dFond(pt4,pt5,pt1,pt0,n2);
Decor dArriere(pt7,pt6,pt2,pt3,n4);
Decor dGauche(pt7,pt4,pt0,pt3,n5);
Decor dDroite(pt5,pt6,pt2,pt1,n6);
Decor d[]={dRoute,dPlafond,dFond,dArriere,dGauche,dDroite,dSol};

//Declaration Minions
int NbMinions=3;
Minion m0(xM[0],0.0f,zM[0]);
Minion m1(xM[1],0.0f,zM[1]);
Minion m2(xM[2],0.0f,zM[2]);
Minion m[]={m0,m1,m2};
    
//Declaration pierres/feuilles/ciseaux
Pierre pB(0.4f, 0.5f, 0.0f,3.0f);
Feuille fB(0.5f, 0.5f, 0.0f,3.0f);
Ciseau cB(0.6f, 0.5f, 0.0f,3.0f);

Pierre p0(xM[0]+0.5f,0.5f,zM[0],1.0f);
Feuille f0(xM[0]+0.5f,0.5f,zM[0],1.0f);
Ciseau c0(xM[0]+0.5f,0.5f,zM[0],1.0f);

Pierre p1(xM[1]+0.5f,0.5f,zM[1],1.0f);
Feuille f1(xM[1]+0.5f,0.5f,zM[1],1.0f);
Ciseau c1(xM[1]+0.5f,0.5f,zM[1],1.0f);

Pierre p2(xM[2]+0.5f,0.5f,zM[2],1.0f);
Feuille f2(xM[2]+0.5f,0.5f,zM[2],1.0f);
Ciseau c2(xM[2]+0.5f,0.5f,zM[2],1.0f);

Pierre p[]={pB,p0,p1,p2};
Feuille f[]={fB,f0,f1,f2};
Ciseau c[]={cB,c0,c1,c2};

// Fonctions GLUT
static void init();
static void display_callback();
static void keyboard_callback(unsigned char key, int, int);
static void special_callback(int key, int,int);
static void timer_callback(int);
void load_texture(const char* filename,GLuint *texture_id);

// Fonctions Bob
void init_bob();
void afficher_bob();
// Fonctions Texte
void init_texte();
void afficher_score();

//Fonctions Jeu
bool sauter(bool saut);
void deplacement(float x, float z);
void rotation(float angle);
void defilement();
void test(float pos, int n);


int main(int argc, char** argv){
 
    //initialisation GLUT
    glutInit(&argc, argv);

    //Mode d'affichage (couleur, gestion de profondeur, ...)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    //Taille de la fenetre a l'ouverture
    glutInitWindowSize(1366, 768);

    //Titre de la fenetre
    glutCreateWindow("Spongebob vs Minions : Pierre Feuille Ciseaux");

    //Fonction de la boucle d'affichage
    glutDisplayFunc(display_callback);

    //Fonction de gestion du clavier
    glutKeyboardFunc(keyboard_callback);

    //Fonction des touches speciales du clavier (fleches directionnelles)
    glutSpecialFunc(special_callback);

    //Fonction d'appel d'affichage en chaine
    glutTimerFunc(25, timer_callback, 0);

    //Initialisation des fonctions OpenGL
    glewInit();

    //Initialisation des donnees et chargement des shaders
    init();

    //Lancement de la boucle d'affichage de la fenetre
    glutMainLoop();
    
    return 0;
}


static void init(){

    // Chargement du shader
    shader_program_id = read_shader("shader.vert", "shader.frag");

    //matrice de projection
    projection = matrice_projection(60.0f*M_PI/180.0f,1.0f,0.01f,100.0f);
    glUniformMatrix4fv(get_uni_loc(shader_program_id,"projection"),1,false,pointeur(projection)); PRINT_OPENGL_ERROR();

    //centre de rotation de la 'camera' (les objets sont centres en z=-2)
    transformation_view.rotation_center = vec3(0.0f,0.0f,0.0f);

    //activation de la gestion de la profondeur
    glEnable(GL_DEPTH_TEST); PRINT_OPENGL_ERROR();
    
    //initialisation Bob et score
    init_bob();
    init_texte();
    
    //initialisations decor 
    for (i=0; i<7; i++){
      d[i].init_decor();
      load_texture(texturesDecor[i],&d[i].texture_id_object);
    }
    
    //initialisation Minions
    for (i=0; i<NbMinions; i++){
      m[i].init_minion();
      load_texture("../data/minion.tga",&m[i].texture_id_object);
    }

    //initialisation pierres/feuilles/ciseaux
    for (i=0; i<4; i++){
      
      f[i].init_feuille();
      load_texture("../data/feuille.tga",&f[i].texture_id_object);     
      p[i].init_pierre();
      load_texture("../data/pierre.tga",&p[i].texture_id_object);
      c[i].init_ciseau();
      load_texture("../data/white.tga",&c[i].texture_id_object);
    }
}


//Fonction d'affichage
static void display_callback()
{
    //effacement des couleurs du fond d'ecran
    glClearColor(0.5f, 0.6f, 0.9f, 1.0f);                 PRINT_OPENGL_ERROR();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   PRINT_OPENGL_ERROR();

    // Affecte les parametres uniformes de la vue (identique pour tous les modeles de la scene)
    {
        //envoie de la rotation
        glUniformMatrix4fv(get_uni_loc(shader_program_id,"rotation_view"),1,false,pointeur(transformation_view.rotation)); PRINT_OPENGL_ERROR();

        //envoie du centre de rotation
        vec3 cv = transformation_view.rotation_center;
        glUniform4f(get_uni_loc(shader_program_id,"rotation_center_view") , cv.x,cv.y,cv.z , 0.0f); PRINT_OPENGL_ERROR();

        //envoie de la translation
        vec3 tv = transformation_view.translation;
	tv.y-=1.2f;
        tv.z-=2.8f;
        glUniform4f(get_uni_loc(shader_program_id,"translation_view") , tv.x,tv.y,tv.z , 0.0f); PRINT_OPENGL_ERROR();
    }

    // Affiche Bob et le score
    afficher_bob();
    afficher_score();
    
    // Affiche le decor
    for (i=0; i<7; i++){
      d[i].shader_program_id=shader_program_id;
      d[i].afficher_decor();
    }
    
    // Affiche Minions
    for (i=0; i<NbMinions; i++){     
      m[i].shader_program_id=shader_program_id;
      m[i].afficher_minion();
    }
    
    // Affiche pierre/feuillle/ciseau BOB
    f[bob].shader_program_id=shader_program_id;
    f[bob].afficher_model();
    c[bob].shader_program_id=shader_program_id;
    c[bob].afficher_model();
    p[bob].shader_program_id=shader_program_id;
    p[bob].afficher_model();
    
    // Affiche pierres/feuilles/ciseaux MINIONS
    for (i=1; i<4; i++){

      if(active_pierre[i]==true){
	p[i].shader_program_id=shader_program_id;
	p[i].afficher_model();
      }

      else if(active_feuille[i]==true){
	f[i].shader_program_id=shader_program_id;
	f[i].afficher_model();
      }

      else if(active_ciseau[i]==true){
	c[i].shader_program_id=shader_program_id;
	c[i].afficher_model();
      }
    }
	
    //Changement de buffer d'affichage pour eviter un effet de scintillement
    glutSwapBuffers();
}

// touches clavier
static void keyboard_callback(unsigned char key, int, int)
{
    float d_angle=0.1f, z=0.5f*cos(-angle_y_bob);

    switch (key)
    {
      
    case '!' :
	exit(0);
	break;
	
    case 'q':	// rotation bob -
        angle_y_bob -= d_angle;
	rotation(angle_y_bob);
	break;
	
    case 'd':	//rotation bob +
        angle_y_bob += d_angle;
        rotation(angle_y_bob);
	break;

    case 's':	// saut
	if((d[fond].transformation_model.translation.z<40.0f || z<0)	 &&
	   (d[arriere].transformation_model.translation.z>-40.0f || z>0))        jump=true;
	
	break;

    case 'w':	// rotation camera -
	angle_view -= d_angle;
	transformation_view.rotation = matrice_rotation(angle_view , 0.0f,1.0f,0.0f) ;
	break;
	
    case 'x':	// rotation camera +
	angle_view += d_angle;
        transformation_view.rotation = matrice_rotation(angle_view , 0.0f,1.0f,0.0f) ;
	break;
	
    case 'a':	// tirer pierre
        active_pierre[bob]=true;
	break;
	
    case 'z':	// tirer feuille
        active_feuille[bob]=true;
	break;
	
    case 'e':	// tirer ciseau
        active_ciseau[bob]=true;
	break;
    }
}


// fleches clavier
static void special_callback(int key, int,int)
{
    float dL=0.5f, x=0.0f, z=0.0f;
    
    switch (key){
      
    case GLUT_KEY_UP:
      	x=dL*sin(-angle_y_bob);
	z=dL*cos(-angle_y_bob);
	
	//gestion des colisions 
	if((d[fond].transformation_model.translation.z<47.0f || z<0)	 &&
	   (d[arriere].transformation_model.translation.z>-47.0f || z>0) &&
	   (d[gauche].transformation_model.translation.x<15.0f || x<0)  &&
	   (d[droite].transformation_model.translation.x>-15.0f || x>0))
	  deplacement(x,z);
	
	break;
	
    case GLUT_KEY_DOWN:
	x=-dL*sin(-angle_y_bob);
	z=-dL*cos(-angle_y_bob);

	//gestion des colisions 
	if((d[fond].transformation_model.translation.z<47.0f || z<0)	 &&
	   (d[arriere].transformation_model.translation.z>-47.0f || z>0) &&
	   (d[gauche].transformation_model.translation.x>-15.0f || x>0)  &&
	   (d[droite].transformation_model.translation.x<15.0f || x<0))
	  deplacement(x,z);
	
	break;
	
    case GLUT_KEY_LEFT:
	x=dL*cos(angle_y_bob);
	z=dL*sin(angle_y_bob);

	//gestion des colisions 
	if((d[fond].transformation_model.translation.z>-47.0f || z>0)	&&
	   (d[arriere].transformation_model.translation.z<47.0f || z<0) &&
	   (d[gauche].transformation_model.translation.x>-15.0f || x>0) &&
	   (d[droite].transformation_model.translation.x<15.0f || x<0))
	  deplacement(x,z);
	
	break;
	
    case GLUT_KEY_RIGHT:
	x=-dL*cos(angle_y_bob);
	z=-dL*sin(angle_y_bob);
	
	//gestion des colisions 
	if((d[fond].transformation_model.translation.z>-47.0f || z>0)	&&
	   (d[arriere].transformation_model.translation.z<47.0f || z<0) &&
	   (d[gauche].transformation_model.translation.x<15.0f || x<0)  &&
	   (d[droite].transformation_model.translation.x>-15.0f || x>0))
	  deplacement(x,z);
	
	break;
    }
  
    //reactualisation de l'affichage
    glutPostRedisplay();
}


// timer affichage
static void timer_callback(int)
{
    // Saut Bob
    jump=sauter(jump);
    
    // Tirs Bob
    active_feuille[bob]=f[bob].tirer(active_feuille[bob]);
    active_pierre[bob]=p[bob].tirer(active_pierre[bob]);
    active_ciseau[bob]=c[bob].tirer(active_ciseau[bob]);
    
    // Défilement des minions
    defilement();
    
    //demande de rappel de cette fonction dans 50ms
    glutTimerFunc(50, timer_callback, 0);

    //reactualisation de l'affichage
    glutPostRedisplay();
}

void load_texture(const char* filename,GLuint *texture_id)
{
    // Chargement d'une texture (seul les textures tga sont supportes)
    Image  *image = image_load_tga(filename);
    if (image) //verification que l'image est bien chargee
    {

        //Creation d'un identifiant pour la texture
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); PRINT_OPENGL_ERROR();
        glGenTextures(1, texture_id); PRINT_OPENGL_ERROR();

        //Selection de la texture courante a partir de son identifiant
        glBindTexture(GL_TEXTURE_2D, *texture_id); PRINT_OPENGL_ERROR();

        //Parametres de la texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); PRINT_OPENGL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); PRINT_OPENGL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); PRINT_OPENGL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); PRINT_OPENGL_ERROR();

        //Envoie de l'image en memoire video
        if(image->type==IMAGE_TYPE_RGB){ //image RGB
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data); PRINT_OPENGL_ERROR();}
        else if(image->type==IMAGE_TYPE_RGBA){ //image RGBA
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data); PRINT_OPENGL_ERROR();}
        else{
            std::cout<<"Image type not handled"<<std::endl;}

        delete image;
    }
    else
    {
        std::cerr<<"Erreur chargement de l'image, etes-vous dans le bon repertoire?"<<std::endl;
        abort();
    }

    glUniform1i (get_uni_loc(shader_program_id, "texture"), 0); PRINT_OPENGL_ERROR();
}

void init_bob()
{
    // Chargement d'un maillage a partir d'un fichier
    mesh m = load_obj_file("../data/bob.obj");

    // Affecte une transformation sur les sommets du maillage
    float s = 1.0f;
    mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
                          0.0f,    s, 0.0f, 0.0f,
                          0.0f, 0.0f,   s , 0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f);
    apply_deformation(&m,matrice_rotation(M_PI,0.0f,1.0f,0.0f));
    apply_deformation(&m,transform);

    // Centre la rotation du modele 1 autour de son centre de gravite approximatif
    transformation_bob.rotation_center = vec3(0.0f,-0.0f,0.0f);
    // Calcul automatique des normales du maillage
    update_normals(&m);
    // Les sommets sont affectes a une couleur blanche
    fill_color(&m,vec3(1.0f,1.0f,1.0f));
    //attribution d'un buffer de donnees (1 indique la création d'un buffer)
    glGenBuffers(1,&vbo_object_1); PRINT_OPENGL_ERROR();
    //affectation du buffer courant
    glBindBuffer(GL_ARRAY_BUFFER,vbo_object_1); PRINT_OPENGL_ERROR();
    //copie des donnees des sommets sur la carte graphique
    glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); PRINT_OPENGL_ERROR();
    //attribution d'un autre buffer de donnees
    glGenBuffers(1,&vboi_object_1); PRINT_OPENGL_ERROR();
    //affectation du buffer courant (buffer d'indice)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object_1); PRINT_OPENGL_ERROR();
    //copie des indices sur la carte graphique
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); PRINT_OPENGL_ERROR();

    // Nombre de triangles de l'objet 1
    nbr_triangle_object_1 = m.connectivity.size();

    // Chargement de la texture
    load_texture("../data/bob.tga",&texture_id_object_1);

}

void afficher_bob()
{
    //envoie des parametres uniformes
    {
        glUniformMatrix4fv(get_uni_loc(shader_program_id,"rotation_model"),1,false,pointeur(transformation_bob.rotation));    PRINT_OPENGL_ERROR();

        vec3 c = transformation_bob.rotation_center;
        glUniform4f(get_uni_loc(shader_program_id,"rotation_center_model") , c.x,c.y,c.z , 0.0f);                                 PRINT_OPENGL_ERROR();

        vec3 t = transformation_bob.translation;
        glUniform4f(get_uni_loc(shader_program_id,"translation_model") , t.x,t.y,t.z , 0.0f);                                     PRINT_OPENGL_ERROR();
    }

    //placement des VBO
    {
        //selection du VBO courant
        glBindBuffer(GL_ARRAY_BUFFER,vbo_object_1);                                                    PRINT_OPENGL_ERROR();

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
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object_1);                           PRINT_OPENGL_ERROR();
        glBindTexture(GL_TEXTURE_2D, texture_id_object_1);                             PRINT_OPENGL_ERROR();
        glDrawElements(GL_TRIANGLES, 3*nbr_triangle_object_1, GL_UNSIGNED_INT, 0);     PRINT_OPENGL_ERROR();
    }

}

void init_texte()
{
  // Chargement des textures chiffre et "score"
  load_texture("../data/txt/0.tga", texture_textes+0);
  load_texture("../data/txt/1.tga", texture_textes+1);
  load_texture("../data/txt/2.tga", texture_textes+2);
  load_texture("../data/txt/3.tga", texture_textes+3);
  load_texture("../data/txt/4.tga", texture_textes+4);
  load_texture("../data/txt/5.tga", texture_textes+5);
  load_texture("../data/txt/6.tga", texture_textes+6);
  load_texture("../data/txt/7.tga", texture_textes+7);
  load_texture("../data/txt/8.tga", texture_textes+8);
  load_texture("../data/txt/9.tga", texture_textes+9);
  load_texture("../data/txt/score.tga", texture_textes+10);

  // positions des quadrilatères à l'écran
  vec2 vertices[] ={	vec2(20,5), vec2(0,0),
			vec2(70,5), vec2(1,0),
			vec2(70,30), vec2(1,1),
			vec2(20,30), vec2(0,1),

			vec2(80,5), vec2(0,0),
			vec2(110,5), vec2(1,0),
			vec2(110,30), vec2(1,1),
			vec2(80,30), vec2(0,1),

			vec2(120,5), vec2(0,0),
			vec2(150,5), vec2(1,0),
			vec2(150,30), vec2(1,1),
			vec2(120,30), vec2(0,1),

			vec2(160,5), vec2(0,0),
			vec2(190,5), vec2(1,0),
			vec2(190,30), vec2(1,1),
			vec2(160,30), vec2(0,1)};
			
  GLuint indices[] ={	0,1,2, 0,2,3,  // 6 indices par quadrilatère
			4,5,6, 4,6,7,
			8,9,10, 8,10,11,
			12,13,14, 12,14,15};
			
    //attribution d'un buffer de donnees (1 indique la création d'un buffer)
    glGenBuffers(1,&vbo_text);                                             PRINT_OPENGL_ERROR();
    //affectation du buffer courant
    glBindBuffer(GL_ARRAY_BUFFER,vbo_text);                                PRINT_OPENGL_ERROR();
    //copie des donnees des sommets sur la carte graphique
    glBufferData(GL_ARRAY_BUFFER,sizeof vertices, vertices,GL_STATIC_DRAW);  PRINT_OPENGL_ERROR();
    //attribution d'un autre buffer de donnees
    glGenBuffers(1,&vboi_text);                                            PRINT_OPENGL_ERROR();
    //affectation du buffer courant (buffer d'indice)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_text);                       PRINT_OPENGL_ERROR();
    //copie des indices sur la carte graphique
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);  PRINT_OPENGL_ERROR();

    shader_text = read_shader("shader_text.vert", "shader_text.frag");
    glUseProgram(shader_text);
    glUniform2f(get_uni_loc(shader_text, "size"), 900,900);
    glUseProgram(shader_program_id);
    
}

void afficher_score()
{
  glUseProgram(shader_text);
    //placement des VBO
    {
        //selection du VBO courant
        glBindBuffer(GL_ARRAY_BUFFER,vbo_text);                                                    PRINT_OPENGL_ERROR();

        // mise en place des differents pointeurs
        glEnableClientState(GL_VERTEX_ARRAY);                                                          PRINT_OPENGL_ERROR();
        glVertexPointer(2, GL_FLOAT, 2*sizeof(vec2), 0);                                        PRINT_OPENGL_ERROR();

        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY); PRINT_OPENGL_ERROR();                                     PRINT_OPENGL_ERROR();

        glEnableClientState(GL_TEXTURE_COORD_ARRAY); PRINT_OPENGL_ERROR();                             PRINT_OPENGL_ERROR();
        glTexCoordPointer(2,GL_FLOAT, 2*sizeof(vec2), buffer_offset(sizeof(vec2)));           PRINT_OPENGL_ERROR();
    }

    //affichage
    {
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_text);                           PRINT_OPENGL_ERROR();

	    glEnable(GL_BLEND); // Transparence
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	    // Affichage "score"
	    glBindTexture(GL_TEXTURE_2D, texture_textes[10]);
	    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // O = premier quadrilatère
	    // Affichage chiffre centaine
	    glBindTexture(GL_TEXTURE_2D, texture_textes[(score/100)%10]);
	    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, buffer_offset(1*6*sizeof(GLuint)));
	    // Affichage chiffre dizaine
	    glBindTexture(GL_TEXTURE_2D, texture_textes[(score/10)%10]);
	    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, buffer_offset(2*6*sizeof(GLuint)));
	    // Affichage chiffre unité
	    glBindTexture(GL_TEXTURE_2D, texture_textes[(score/1)%10]);
	    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, buffer_offset(3*6*sizeof(GLuint)));
	    glDisable(GL_BLEND); // Transparence
    }

  glUseProgram(shader_program_id);
}

void rotation(float angle){

    // Rotation decor
    for (i=0; i<6; i++)	d[i].transformation_model.rotation = matrice_rotation(angle , 0.0f,1.0f,0.0f);  
    
    // Rotation Minions
    for (i=0; i<NbMinions; i++){
      m[i].transformation_model.rotation = matrice_rotation(angle, 0.0f,1.0f,0.0f) ;
      m[i].transformation_model.rotation = matrice_rotation(angle , 0.0f,1.0f,0.0f) ;
      m[i].transformation_model.rotation = matrice_rotation(angle, 0.0f,1.0f,0.0f) ;
    }
    
    // Rotation pierres/feuilles/ciseaux Minions
    for (i=1; i<4; i++){
      f[i].transformation_model.rotation = matrice_rotation(angle, 0.0f,1.0f,0.0f) ;
      p[i].transformation_model.rotation = matrice_rotation(angle , 0.0f,1.0f,0.0f) ;
      c[i].transformation_model.rotation = matrice_rotation(angle, 0.0f,1.0f,0.0f) ;
    }
    
}

void deplacement(float x, float z){	

	// Deplacement decors
	for (i=0; i<7; i++)	d[i].deplacement(x,z);
	
	// Deplacement Minions
        for (i=0; i<NbMinions; i++){
	  m[i].transformation_model.translation.z += z;
	  p[i+1].transformation_model.translation.z += z;
	  f[i+1].transformation_model.translation.z += z;
	  c[i+1].transformation_model.translation.z += z;
	  
	  m[i].transformation_model.translation.x += x;
	  p[i+1].transformation_model.translation.x += x;
	  f[i+1].transformation_model.translation.x += x;
	  c[i+1].transformation_model.translation.x += x;
	 
	  m[i].transformation_model.rotation_center.z -= z;
	  p[i+1].transformation_model.rotation_center.z -= z;
	  f[i+1].transformation_model.rotation_center.z -= z;
	  c[i+1].transformation_model.rotation_center.z -= z;
	  
	  m[i].transformation_model.rotation_center.x -= x;
	  p[i+1].transformation_model.rotation_center.x -= x;
	  f[i+1].transformation_model.rotation_center.x -= x;
	  c[i+1].transformation_model.rotation_center.x -= x;
	  	  
	}
	
}

bool sauter(bool saut){
  
  float  z=0.5f*cos(-angle_y_bob);
  
  if (saut==false){

    for (i=0; i<7; i++)	if( d[i].transformation_model.translation.y<0.0f){
      d[i].transformation_model.translation.y+=0.5f*sin(M_PI/4);
      d[i].transformation_model.rotation_center.y-=0.5f*sin(M_PI/4);
      d[i].transformation_model.translation.z+=z*cos(M_PI/4);
      d[i].transformation_model.rotation_center.z-=z*cos(M_PI/4);}
    
    for (i=0; i<NbMinions; i++){
      
      if(m[i].transformation_model.translation.y<0.0f){
	m[i].transformation_model.translation.y+=0.5f*sin(M_PI/4);
	m[i].transformation_model.rotation_center.y-=0.5f*sin(M_PI/4);
	m[i].transformation_model.translation.z+=z*cos(M_PI/4);
	m[i].transformation_model.rotation_center.z-=z*cos(M_PI/4);}
    
      if(p[i+1].transformation_model.translation.y<0.0f){
	p[i+1].transformation_model.translation.y+=0.5f*sin(M_PI/4);
	p[i+1].transformation_model.rotation_center.y-=0.5f*sin(M_PI/4);
	p[i+1].transformation_model.translation.z+=z*cos(M_PI/4);
	p[i+1].transformation_model.rotation_center.z-=z*cos(M_PI/4);}
      
      if(f[i+1].transformation_model.translation.y<0.0f){
	f[i+1].transformation_model.translation.y+=0.5f*sin(M_PI/4);
	f[i+1].transformation_model.rotation_center.y-=0.5f*sin(M_PI/4);
	f[i+1].transformation_model.translation.z+=z*cos(M_PI/4);
	f[i+1].transformation_model.rotation_center.z-=z*cos(M_PI/4);}
      
      if(c[i+1].transformation_model.translation.y<0.0f){
      c[i+1].transformation_model.translation.y+=0.5f*sin(M_PI/4);
      c[i+1].transformation_model.rotation_center.y-=0.5f*sin(M_PI/4);
      c[i+1].transformation_model.translation.z+=z*cos(M_PI/4);
      c[i+1].transformation_model.rotation_center.z-=z*cos(M_PI/4);}
    }
  }
	
  if (saut==true){

    for (i=0; i<7; i++){
      if( d[i].transformation_model.translation.y==0.0f || d[i].transformation_model.translation.y!=-4.0f){
      d[i].transformation_model.translation.y-=0.5f*sin(M_PI/4);
      d[i].transformation_model.rotation_center.y+=0.5f*sin(M_PI/4);
      d[i].transformation_model.translation.z+=z*cos(M_PI/4);
      d[i].transformation_model.rotation_center.z-=z*cos(M_PI/4);}
      if( d[i].transformation_model.translation.y<=-4.0f)	saut=false;
    }
      
    for (i=0; i<NbMinions; i++)	{
      
      if( m[i].transformation_model.translation.y==0.0f || m[i].transformation_model.translation.y!=-4.0f){
	m[i].transformation_model.translation.y-=0.5f*sin(M_PI/4);
	m[i].transformation_model.rotation_center.y+=0.5f*sin(M_PI/4);
	m[i].transformation_model.translation.z+=z*cos(M_PI/4);
	m[i].transformation_model.rotation_center.z-=z*cos(M_PI/4);}
      if( m[i].transformation_model.translation.y<=-4.0f)	saut=false;
    
      if( p[i+1].transformation_model.translation.y==0.0f || p[i+1].transformation_model.translation.y!=-4.0f){
	p[i+1].transformation_model.translation.y-=0.5f*sin(M_PI/4);
	p[i+1].transformation_model.rotation_center.y+=0.5f*sin(M_PI/4);
	p[i+1].transformation_model.translation.z+=z*cos(M_PI/4);
	p[i+1].transformation_model.rotation_center.z-=z*cos(M_PI/4);}
      if( p[i+1].transformation_model.translation.y<=-4.0f)	saut=false;
      
      if( f[i+1].transformation_model.translation.y==.0f || f[i+1].transformation_model.translation.y!=-3.0f){
	f[i+1].transformation_model.translation.y-=0.5f*sin(M_PI/4);
	f[i+1].transformation_model.rotation_center.y+=0.5f*sin(M_PI/4);
	f[i+1].transformation_model.translation.z+=z*cos(M_PI/4);
	f[i+1].transformation_model.rotation_center.z-=z*cos(M_PI/4);}
      if( f[i+1].transformation_model.translation.y<=-4.0f)	saut=false;
      
      if( c[i+1].transformation_model.translation.y==0.0f || c[i+1].transformation_model.translation.y!=-3.0f){
      c[i+1].transformation_model.translation.y-=0.5f*sin(M_PI/4);
      c[i+1].transformation_model.rotation_center.y+=0.5f*sin(M_PI/4);
      c[i+1].transformation_model.translation.z+=z*cos(M_PI/4);
      c[i+1].transformation_model.rotation_center.z-=z*cos(M_PI/4);}
      if( c[i+1].transformation_model.translation.y<=-4.0f)	saut=false;
    }
  }
  
  return saut;
}

void defilement(){
  
//   i=0;
  
   for (i=0; i<NbMinions; i++){

    // Game Over
    if(	m[i].transformation_model.translation.z>2.0f*(-zM[i])		||	// Minion arrivé
       ((abs(m[i].transformation_model.translation.x+xM[i])<0.5f)	&&	// collision Bob-Minion x
	(abs(m[i].transformation_model.translation.z+zM[i])<0.5f))	){	// Collision Bob-Minion z

	load_texture("../data/txt/gameover.tga", texture_textes+10);
	glutMainLoop ();
    }
    
    // Tests en cas de tir
    if	   (active_pierre[bob]==true)	test(-p[bob].transformation_model.translation.z,i);
    else if(active_feuille[bob]==true)	test(-f[bob].transformation_model.translation.z,i);
    else if(active_ciseau[bob]==true)	test(-c[bob].transformation_model.translation.z,i);
    
    // S'il n'y a pas de tir
    else{
    
	randX[i]=RandomFloat(-0.05f,0.05f);
	m[i].transformation_model.translation.z+=m[i].vitesse;
	m[i].transformation_model.rotation_center.z-=m[i].vitesse;
	m[i].transformation_model.translation.x-=randX[i];
	m[i].transformation_model.rotation_center.x+=randX[i];
	  
	if(signeM[i]==PIERRE){
	  
	  active_pierre[i+1]=true;
	  active_feuille[i+1]=false;
	  active_ciseau[i+1]=false;
	    
	  p[i+1].transformation_model.translation.z+=m[i].vitesse;
	  p[i+1].transformation_model.rotation_center.z-=m[i].vitesse;
	  p[i+1].transformation_model.translation.x-=randX[i];
	  p[i+1].transformation_model.rotation_center.x+=randX[i];      
	}

	if(signeM[i]==FEUILLE){
	
	  active_pierre[i+1]=false;
	  active_feuille[i+1]=true;
	  active_ciseau[i+1]=false;
	  
	  f[i+1].transformation_model.translation.z+=m[i].vitesse;
	  f[i+1].transformation_model.rotation_center.z-=m[i].vitesse;
	  f[i+1].transformation_model.translation.x-=randX[i];
	  f[i+1].transformation_model.rotation_center.x+=randX[i];
	}

	if(signeM[i]==CISEAU){
	
	  active_pierre[i+1]=false;
	  active_feuille[i+1]=false;
	  active_ciseau[i+1]=true;
	  
	  c[i+1].transformation_model.translation.z+=m[i].vitesse;
	  c[i+1].transformation_model.rotation_center.z-=m[i].vitesse;
	  c[i+1].transformation_model.translation.x-=randX[i];
	  c[i+1].transformation_model.rotation_center.x+=randX[i];
	}
     }
   }
}

void test(float pos, int n){
  
  xTir=abs(m[n].transformation_model.translation.x+xM[n]);
  zTir=abs(m[n].transformation_model.translation.z+zM[n]);

// Minion touché
  if( (abs(pos-sqrt(xTir*xTir+zTir*zTir))<0.5f)	&&
      (abs(angle_y_bob-atan(xTir/zTir))<0.5f)	&&
      pos<30.0f					){
    
      // Bob perd
	if( (active_pierre[bob]==true && active_feuille[n+1]==true) ||
	      (active_feuille[bob]==true && active_ciseau[n+1]==true) ||
	      (active_ciseau[bob]==true && active_pierre[n+1]==true)  ){
	
	load_texture("../data/txt/gameover.tga", texture_textes+10);
	glutMainLoop ();

	}
	
      //Bob gagne    
	else if( (active_pierre[bob]==true && active_ciseau[n+1]==true)  ||
	    (active_feuille[bob]==true && active_pierre[n+1]==true) ||
	    (active_ciseau[bob]==true && active_feuille[n+1]==true) ){
	  
	  xM[n]=RandomFloat(-1.0f,1.0f);
	  
	  m[n].transformation_model.translation.z=0.0f;
	  m[n].transformation_model.rotation_center.z=0.0f;
	  m[n].transformation_model.translation.x=xM[n];
	  m[n].transformation_model.rotation_center.x=xM[n];
	  
	  f[n+1].transformation_model.translation.z=0.0f;
	  f[n+1].transformation_model.rotation_center.z=0.0f;
	  f[n+1].transformation_model.translation.x=xM[n];
	  f[n+1].transformation_model.rotation_center.x=xM[n];
	    
	  p[n+1].transformation_model.translation.z=0.0f;
	  p[n+1].transformation_model.rotation_center.z=0.0f;
	  p[n+1].transformation_model.translation.x=xM[n];
	  p[n+1].transformation_model.rotation_center.x=xM[n];
	    
	  c[n+1].transformation_model.translation.z=0.0f;
	  c[n+1].transformation_model.rotation_center.z=0.0f;
	  c[n+1].transformation_model.translation.x=xM[n];
	  c[n+1].transformation_model.rotation_center.x=xM[n];
		    
	  score++;
	  
	  active_pierre[bob]=false;
	  active_feuille[bob]=false;
	  active_ciseau[bob]=false;
		  
	  signeM[n]=rand()%3;
	  m[n].vitesse+=0.01f;
	}
	
      // Si egalité on ne fait rien
      else return;

  }
  
  //Le minion n'est pas le bon
  else return;

}