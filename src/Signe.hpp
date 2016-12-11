class Signe

{
  public: 
  
    GLuint shader_program_id;
    GLuint vbo_object;
    GLuint vboi_object;
    GLuint texture_id_object;
    int nbr_triangle_object;
   
    struct transformation
    {
      mat4 rotation;
      vec3 rotation_center;
      vec3 translation;

      transformation():rotation(),rotation_center(),translation(){}
    };
    
    transformation transformation_model;
    mat4 rotation;
    vec3 rotation_center;
    vec3 translation;
  
    float s,x,y,z;
    float vitesseTir;
    mesh m;
  
  Signe(float a, float b, float c, float v);
  void init_model();
  void afficher_model();
  bool tirer(bool active);

};

class Feuille : public Signe
{
  
  public:
  
    Feuille(float a, float b, float c, float v);
    void init_feuille();
};

class Pierre : public Signe
{
  
  public:
  
    Pierre(float a, float b, float c, float v);
    void init_pierre();
};

class Ciseau : public Signe
{
  
  public:
  
    Ciseau(float a, float b, float c, float v);
    void init_ciseau();
};