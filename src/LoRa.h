class EmisorLora{
    private:
    
    
    public:
    EmisorLora(void){};
    void inicializa(void);
    uint16_t envia(String mensaje);
    void finaliza(void);
    void reset(void);
};

void initLora(void);