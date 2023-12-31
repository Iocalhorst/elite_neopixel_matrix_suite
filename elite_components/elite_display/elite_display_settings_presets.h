


//API though
typedef struct {
  char ctx[32];
  char name_str[32];
  float fgamma;
  float brightness;
  float col_correction_coeffs[3];
}display_settings_t;

//0.040000 gamma: 0.750000

display_settings_t display_settings_default_low={
  .ctx="default",
  .name_str="low",
  .fgamma=0.72f,
  .brightness=0.03f,
  .col_correction_coeffs={1.0f,1.0f,1.0f}
};
display_settings_t display_settings_default_mid={
  .ctx="default",
  .name_str="mid",
  .fgamma=0.75f,
  .brightness=0.09f,
  .col_correction_coeffs={1.0f,1.0f,1.0f}
};
display_settings_t display_settings_default_high={
  .ctx="default",
  .name_str="high",
  .fgamma=0.84f,
  .brightness=0.3f,
  .col_correction_coeffs={1.0f,1.0f,1.0f}
};
