Serial port;

class GCodeSender {

  GCodeSender(Serial p) {
    port=p;
  }

  void send(String s) {
    try{
    port.write(s);
    port.write (13);
    }catch(Exception e){
    }
  }
  
  
}

