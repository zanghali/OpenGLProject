
//identifiant du shader
GLuint shader_program_id;

//identifiants pour Bob l'eponge
GLuint vbo_object_1=0;
GLuint vboi_object_1=0;
GLuint texture_id_object_1=0;
int nbr_triangle_object_1;

// Les affichages "texte"
GLuint texture_textes[11];
GLuint vbo_text=0;
GLuint vboi_text=0;
GLuint texture_id_text=0;
GLuint shader_text;
int score = 0;

//Matrice de transformation
struct transformation
{
    mat4 rotation;
    vec3 rotation_center;
    vec3 translation;

    transformation():rotation(),rotation_center(),translation(){}
};

//Transformation de Bob
transformation transformation_bob;

//Transformation de la vue (camera)
transformation transformation_view;

//Matrice de projection
mat4 projection;

//angle de deplacement
float angle_y_bob = 0.0f;
float angle_view = 0.0f;

//coordonnees geometriques du decor
vec3 pt0=vec3(-50.0f,-50.0f,-50.0f);
vec3 pt1=vec3( 50.0f,-50.0f,-50.0f);
vec3 pt2=vec3( 50.0f,-50.0f, 50.0f);
vec3 pt3=vec3(-50.0f,-50.0f, 50.0f);
vec3 pt4=vec3(-50.0f,50.0f,-50.0f);
vec3 pt5=vec3( 50.0f,50.0f,-50.0f);
vec3 pt6=vec3( 50.0f,50.0f, 50.0f);
vec3 pt7=vec3(-50.0f,50.0f, 50.0f);
vec3 pt8=vec3(-16.0f,0.0f, 50.0f);
vec3 pt9=vec3(16.0f,0.0f, 50.0f);
vec3 pt10=vec3(16.0f,0.0f, -50.0f);
vec3 pt11=vec3(-16.0f,0.0f, -50.0f);
vec3 n0=vec3( 0.0f, 1.0f,  0.0f);
vec3 n1=vec3(0.0f,-1.0f,0.0f);
vec3 n2=vec3(0.0f,0.0f,1.0f);
vec3 n4=vec3(0.0f,0.0f,-1.0f);
vec3 n5=vec3(1.0f,0.0f,0.0f);
vec3 n6=vec3(-1.0f,0.0f,0.0f);


// Declarations Jeu
float RandomFloat(float a, float b);

enum Perso {bob,mignon1,mignon2,mignon3}perso;
enum Tir {PIERRE, FEUILLE, CISEAU}tir;
enum Mur {route,plafond,fond,arriere,gauche,droite,sol}mur;

bool jump=false;
bool active_pierre[]={false,false,false,false};
bool active_feuille[]={false,false,false,false};
bool active_ciseau[]={false,false,false,false};

int i=0, signeM[] = {rand()%3,rand()%3,rand()%3};

float randX[3];
float xM[]={RandomFloat(-1.0f,1.0f)-3.75f,RandomFloat(-1.0f,1.0f),RandomFloat(-1.0f,1.0f)+3.75f};
float zM[]={-55.0f,-60.0f,-65.0f};
float vitesseTir=0.5f, pos=0.0f, xTir=0.0f, zTir=0.0f;

const char* texturesDecor[] = {"../data/decor/route.tga",
			  "../data/decor/plafond.tga",
			  "../data/decor/fond.tga",
			  "../data/decor/arriere.tga",
			  "../data/decor/gauche.tga",
			  "../data/decor/droite.tga",
			  "../data/decor/sol.tga",};
			  
