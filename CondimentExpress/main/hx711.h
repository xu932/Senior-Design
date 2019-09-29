typedef struct _hx711 {
    uint64_t clockPin;
    uint64_t dataPin;
    uint8_t gain;
} HX711;

void HX711_Init(const HX711* hx);

bool HX711_IsReady(const HX711* hx);

void HX711_PowerDown(const HX711* hx);

void HX711_PowerUp(const HX711* hx);

uint32_t HX711_Read(const HX711* hx);