class Decor

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
  
    vec3 p0, p1, p2, p3, n0;
    
  Decor(vec3 a, vec3 b, vec3 c, vec3 d, vec3 n);
  void init_decor();
  void afficher_decor();
  void deplacement(float x, float z);
//   bool sauter(bool saut);

};
