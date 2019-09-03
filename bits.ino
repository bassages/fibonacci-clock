void setPixels(byte value, byte offset) {
  switch(value) {
    case 1:
      switch(random(2)) {
        case 0:
          pixels[0]|=offset;
          break;
        case 1:
          pixels[1]|=offset;
          break;
      }
      break;
    case 2:
      switch(random(2)) {
        case 0:
          pixels[2]|=offset;
          break;
        case 1:
          pixels[0]|=offset;
          pixels[1]|=offset;
          break;
      }
      break;
    case 3:
      switch(random(3)) {
        case 0:
          pixels[3]|=offset;
          break;
        case 1:
          pixels[0]|=offset;
          pixels[2]|=offset;
          break;
        case 2:
          pixels[1]|=offset;
          pixels[2]|=offset;
          break;
      }
      break;
    case 4:
      switch(random(3)) {
        case 0:
          pixels[0]|=offset;
          pixels[3]|=offset;
          break;
        case 1:
          pixels[1]|=offset;
          pixels[3]|=offset;
          break;
        case 2:
          pixels[0]|=offset;
          pixels[1]|=offset;
          pixels[2]|=offset;
          break;
      }
      break;
    case 5:
      switch(random(3)) {
        case 0:
          pixels[4]|=offset;
          break;
        case 1:
          pixels[2]|=offset;
          pixels[3]|=offset;
          break;
        case 2:
          pixels[0]|=offset;
          pixels[1]|=offset;
          pixels[3]|=offset;
          break;
      }
      break;
    case 6:
      switch(random(4)) {
        case 0:
          pixels[0]|=offset;
          pixels[4]|=offset;
          break;
        case 1:
          pixels[1]|=offset;
          pixels[4]|=offset;
          break;
        case 2:
          pixels[0]|=offset;
          pixels[2]|=offset;
          pixels[3]|=offset;
          break;
        case 3:
          pixels[1]|=offset;
          pixels[2]|=offset;
          pixels[3]|=offset;
          break;
      }
      break;
    case 7:
      switch(random(3)) {
        case 0:
          pixels[2]|=offset;
          pixels[4]|=offset;
          break;
        case 1:
          pixels[0]|=offset;
          pixels[1]|=offset;
          pixels[4]|=offset;
          break;
        case 2:
          pixels[0]|=offset;
          pixels[1]|=offset;
          pixels[2]|=offset;
          pixels[3]|=offset;
          break;
      }
      break;
    case 8:
      switch(random(3)) {
        case 0:
          pixels[3]|=offset;
          pixels[4]|=offset;
          break;
        case 1:
          pixels[0]|=offset;
          pixels[2]|=offset;
          pixels[4]|=offset;
          break;
        case 2:
          pixels[1]|=offset;
          pixels[2]|=offset;
          pixels[4]|=offset;
          break;
      }      
      break;
    case 9:
      switch(random(2)) {
        case 0:
          pixels[0]|=offset;
          pixels[3]|=offset;
          pixels[4]|=offset;
          break;
        case 1:
          pixels[1]|=offset;
          pixels[3]|=offset;
          pixels[4]|=offset;
          break;
      }      
      break;
    case 10:
      switch(random(2)) {
        case 0:
          pixels[2]|=offset;
          pixels[3]|=offset;
          pixels[4]|=offset;
          break;
        case 1:
          pixels[0]|=offset;
          pixels[1]|=offset;
          pixels[3]|=offset;
          pixels[4]|=offset;
          break;
      }
      break;
    case 11:
      switch(random(2)) {
        case 0:
          pixels[0]|=offset;
          pixels[2]|=offset;
          pixels[3]|=offset;
          pixels[4]|=offset;      
          break;
        case 1:
          pixels[1]|=offset;
          pixels[2]|=offset;
          pixels[3]|=offset;
          pixels[4]|=offset; 
          break;
      }
      break;
    case 12:
      pixels[0]|=offset;
      pixels[1]|=offset;
      pixels[2]|=offset;
      pixels[3]|=offset;
      pixels[4]|=offset;
      break;
  }
}
