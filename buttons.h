#ifndef BUTTONS_H
#define BUTTONS_H

String getButtonName(unsigned long code) {
  switch (code) {
    case 0xFF6897: return "0";
    case 0xFF30CF: return "1";
    case 0xFF18E7: return "2";
    case 0xFF7A85: return "3";
    case 0xFF10EF: return "4";
    case 0xFF38C7: return "5";
    case 0xFF5AA5: return "6";
    case 0xFF42BD: return "7";
    case 0xFF4AB5: return "8";
    case 0xFF52AD: return "9";
    case 0xFFA857: return "VOL-";
    case 0xFF629D: return "VOL+";
    case 0xFF02FD: return "PLAY";
    case 0xFFA25D: return "PWR";
    case 0xFFE21D: return "STOP";
    case 0xFFE01F: return "DOWN";
    case 0xFF906F: return "UP";
    case 0xFF9867: return "EQ";
    case 0xFFC23D: return "FWD";
    case 0xFF22DD: return "BACK";
    case 0xFFB04F: return "REPT";
    default:       return "UNKNOWN";
  }
}

#endif