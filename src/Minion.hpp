class Minion

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
  
    float s,x,y,z, vitesse, vitesseTir;
    mesh m;

  Minion(float a, float b, float c);
  void init_minion();
  void afficher_minion();

};
