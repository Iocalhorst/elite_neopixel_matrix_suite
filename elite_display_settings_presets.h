


//API though
typedef struct {
  char name_str[32];
  float fgamma;
  float brightness;
  float col_correct_coeffs[];
}display_settings_t;

//0.040000 gamma: 0.750000


display_settings_t list_presets[128];
display_settings_t spriteshow_low;
display_settings_t spriteshow_mid;
display_settings_t spriteshow_high;
