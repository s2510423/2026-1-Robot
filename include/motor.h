class Motor {
    private:
        struct Pin{char pwm, fore, back;};
        Pin pin;
    public:
        Motor(char p_pwm, char p_fore, char p_back);
        void drive(unsigned char speed, bool dir);
};

class Mobile {
    private:
        Motor fl, fr, bl, br;
    public:
        Mobile(const Motor& p_fl, const Motor& p_fr, const Motor& p_bl, const Motor& p_br);
        void drive(unsigned char speed, bool dir, double time);
        void turn(bool left);
};
