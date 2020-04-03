// place for the graphics
// see https://github.com/olikraus/u8g2/wiki/u8g2reference

void draw(uint8_t a) {
  u8g2_prepare();
  u8g2.drawStr(5, 0, bufferCT1);  // display variable from char bufferCT1
  u8g2.drawStr(5,16, bufferCT2);
  u8g2.drawStr(5,32, bufferCT3);
  u8g2.drawStr(5,52, bufferVrms);
  // fancy graphics
      
 switch(draw_state ) {  // house symbol
    case 0: u8g2.drawGlyph(48, 2, 0x2616) ; break;
    case 1: u8g2.drawGlyph(48, 2, 0x2617) ; break;
    case 2: u8g2.drawGlyph(48, 2, 0x2616) ; break;
    case 3: u8g2.drawGlyph(48, 2, 0x2617) ; break;
    case 4: u8g2.drawGlyph(48, 2, 0x2616) ; break;
    case 5: u8g2.drawGlyph(48, 2, 0x2617) ; break;
    case 6: u8g2.drawGlyph(48, 2, 0x2616) ; break;
    case 7: u8g2.drawGlyph(48, 2, 0x2617) ; break;
     }
    if (P2i < 1) { u8g2.drawGlyph(68, 54, 0x2614) ; } ;// parapluie
    if ((P2i > 1) and (P2i < (MaxSolar / 10))) {u8g2.drawGlyph(68, 52, 0x2591) ;}// fog1
    if ((P2i > (MaxSolar / 10)) and (P2i < (MaxSolar / 4))) {u8g2.drawGlyph(68, 52, 0x2592) ;}// fog2
    if (P2i > (MaxSolar / 4))  {u8g2.drawGlyph(68, 52, 0x2600) ;}// soleil
         
  switch(draw_state ) {  // rotate symbols 
    case 0: u8g2.drawGlyph(58, 52, 0x25f3) ; break;
    case 1: u8g2.drawGlyph(58, 52, 0x25f2) ; break;
    case 2: u8g2.drawGlyph(58, 52, 0x25f1) ; break;
    case 3: u8g2.drawGlyph(58, 52, 0x25f0) ; break;
    case 4: u8g2.drawGlyph(58, 52, 0x25f3) ; break;
    case 5: u8g2.drawGlyph(58, 52, 0x25f2) ; break;
    case 6: u8g2.drawGlyph(58, 52, 0x25f1) ; break;
    case 7: u8g2.drawGlyph(58, 52, 0x25f0) ; break;
    }
      
   // export bars symbol 
if (Duty_Cyc > 2) {   // if > 1% solar produced 
 switch(draw_state ) {  // rising bars, solar power
    case 0: u8g2.drawGlyph(84, 52, 0x2581) ; break;
    case 1: u8g2.drawGlyph(84, 52, 0x2582) ; break;
    case 2: u8g2.drawGlyph(84, 52, 0x2583) ; break;
    case 3: u8g2.drawGlyph(84, 52, 0x2584) ; break;
    case 4: u8g2.drawGlyph(84, 52, 0x2585) ; break;
    case 5: u8g2.drawGlyph(84, 52, 0x2586) ; break;
    case 6: u8g2.drawGlyph(84, 52, 0x2587) ; break;
    case 7: u8g2.drawGlyph(84, 52, 0x2588) ;break;
     }
  }
}
