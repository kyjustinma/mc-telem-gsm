#ifndef sd_card_h
#define sd_card_h

class sd_card_function {
 public:
  bool setup_sd_card();

 private:
  bool setup_spi();
};

#endif