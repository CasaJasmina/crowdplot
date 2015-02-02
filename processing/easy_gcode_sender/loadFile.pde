String gCode;

void newDownload() {
  //println("time");
  if (readyToAct&&!gCodeExecutionStarted&&GO) {
    println("looking for new files...");
    downloadNewFile();
    if (parseGcode()) {
      refreshGrid();
      executeGCode();
    }
  }
}

void downloadNewFile() {
  GetRequest get = new GetRequest("http://23.22.246.190:65035/getData");
  get.send();
  println("Reponse Content: " + get.getContent());
  println("Reponse Content-Length Header: " + get.getHeader("Content-Length"));
  gCode=get.getContent();
}

